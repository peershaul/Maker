#include "main.h"
#include "makefile.h"

#include <unistd.h>
#include <ctype.h>

#include <sys/stat.h>

#include "utils.c"
#include "makefile.c"



// Checking if a path is a directory or a file
bool is_directory(const char* path){
    struct stat stats;
    stat(path, &stats);

    if((stats.st_mode & S_IFDIR) != 0) return true;
    return false;
}


// Initializing the arguments and seeing what we can understand from them
bool read_arguments(int arg_length, char* args[], MainSetting* settings){
    for(int i = 1; i < arg_length; i++){
        bool answered = false;
        if(handmade_cmpstr(settings->base_path, "null")){
            char buff[PATH_MAX_LENGTH] = "";
            realpath(args[i], buff);

            if(is_directory(buff)){
                handmade_strcpy(settings->base_path, buff, PATH_MAX_LENGTH);
                answered = true;
            }
        }
        if(!answered){
            fprintf(stderr, "We cant understand what \"%s\" means\n", args[i]);
            return false;
        }
    }

    return true;
}

// look at a certain directory and fills out the subdirectories
void look_at_directory(FileOrDirectory* obj){
    obj->is_directory = is_directory(obj->path);

    struct dirent* entry;
    DIR* dir = opendir(obj->path);

    uint16_t item_amount = 0;

    if(dir == NULL) return;
    while((entry = readdir(dir)) != NULL) {
        if(handmade_cmpstr("..", entry->d_name) || handmade_cmpstr(".", entry->d_name)) continue;
        item_amount++;
    }

    closedir(dir);

    dir = opendir(obj->path);

    if(dir == NULL) return;

    obj->item_count = item_amount;

    FileOrDirectory* children = (FileOrDirectory*) malloc(item_amount * sizeof(FileOrDirectory));

    uint16_t index = 0;

    while((entry = readdir(dir)) != NULL){
        if(handmade_cmpstr("..", entry->d_name) || handmade_cmpstr(".", entry->d_name)) continue;

        children[index].parent = obj;

        handmade_strcpy(children[index].path, obj->path, PATH_MAX_LENGTH);
        handmade_strappend(children[index].path, "/", PATH_MAX_LENGTH);
        handmade_strappend(children[index].path, entry->d_name, PATH_MAX_LENGTH);

        struct stat stats;
        stat(children[index].path, &stats);

        children[index].is_directory = is_directory(children[index].path);
        index++;
    }

    for(uint16_t i = 0; i < obj->item_count; i++)
        if(children[i].is_directory)
            look_at_directory(&children[i]);

    obj->children = children;

}

void push_c_file(FileOrDirectory* file, CArray* arr){
    arr->elements[arr->length] = *file;
    arr->length++;
}

FileOrDirectory* look_for_ignore(FileOrDirectory* root){
    for(uint16_t i = 0; i < root->item_count; i++)
        if(handmade_str_ends_with(root->children[i].path, "/makerignore"))
            return &root->children[i];

    return NULL;
}

void read_ignore(const char* root_path, FileOrDirectory* ignore, LinkedPaths* linked_path){
    FILE* file = fopen(ignore->path, "r");
    bool end_of_file = false;
    LinkedPaths* prev = NULL;
    LinkedPaths* pprev = NULL;
    LinkedPaths* curr = linked_path;

    if(file == NULL){
        fprintf(stderr, "Failed to open \"%s\"", ignore->path);
        return;
    }

    while(!end_of_file){
        char name_buff[NAME_MAX_LENGTH] = {};
        uint8_t index = 0;
        bool new_line = false;

        while(!new_line){
            name_buff[index] = fgetc(file);
            if(name_buff[index] == '\n') new_line = true;
            else if(name_buff[index] == EOF) {
                new_line = true;
                end_of_file = true;
            }
            index++;
        }

        bool is_readable = true;
        for(uint8_t i = 0; i < index - 1; i++)
            if(!isalnum(name_buff[i]) && !ispunct(name_buff[i]) && !isspace(name_buff[i])){
                is_readable = false;
                break;
            }

        if(is_readable){

            char path_buff[PATH_MAX_LENGTH] = {};
            handmade_strappend(path_buff, root_path, PATH_MAX_LENGTH);
            handmade_strappend(path_buff, "/", PATH_MAX_LENGTH);
            handmade_strappend(path_buff, name_buff, PATH_MAX_LENGTH);

            uint64_t path_length = handmade_strlen(path_buff);
            curr->path = (char*) malloc((path_length + 1) * sizeof(char));
            handmade_strcpy(curr->path, path_buff, (path_length + 1));
            curr->path[handmade_strlen(curr->path) - 1] = 0;
            curr->next = (LinkedPaths*) malloc(sizeof(LinkedPaths));
            pprev = prev;
            prev = curr;
            curr = prev->next;
        }
    }

    pprev->next = NULL;
    free(prev);
    free(curr);
    fclose(file);
}

void tree_print_everything(FileOrDirectory* root){
    printf("%s\n", root->path);

    if(!root->is_directory) return;

    for(uint16_t i = 0; i < root->item_count; i++)
        tree_print_everything(&root->children[i]);
}


void search_for_c_files(CArray* arr, FileOrDirectory* root, LinkedPaths* ignore_paths, MainSetting* settings){
     if(!root->is_directory){
        if(handmade_str_ends_with(root->path, ".c")){
            if(settings->has_config_file){
                bool not_ignored = true;
                LinkedPaths* current_ignore_path = ignore_paths;

                while(current_ignore_path != NULL){
                    if(handmade_str_contains(root->path, current_ignore_path->path) != -1){
                        not_ignored = false;
                        break;
                    }
                    current_ignore_path = current_ignore_path->next;
                }

                if(not_ignored) push_c_file(root, arr);
            }
            else push_c_file(root, arr);
        }
        return;
    }

    for(uint16_t i = 0; i < root->item_count; i++)
        search_for_c_files(arr, &root->children[i], ignore_paths, settings);
}

FileOrDirectory* look_for_makefile(FileOrDirectory* root){
    for(uint16_t i = 0; i < root->item_count; i++)
        if(handmade_str_ends_with(root->children[i].path, "/makefile.maker"))
            return &root->children[i];
    return NULL;
}


int main(int argc, char* argv[]){
    // Initializing the setting object
    MainSetting settings = {"null", false};

    // Reading the provided arguments
    if(!read_arguments(argc, argv, &settings)) return -1;
    printf("base path: \"%s\"\n", settings.base_path);

    // Searching for files and storing them
    printf("caching files....\n");

    FileOrDirectory* main_obj = (FileOrDirectory*) malloc(sizeof(FileOrDirectory));
    main_obj->parent = NULL;
    handmade_strcpy(main_obj->path, settings.base_path, PATH_MAX_LENGTH);

    // Making a tree of all the files under the path provided by the user
    look_at_directory(main_obj);


    // Look for the ignore file .
    FileOrDirectory* ignore_file = look_for_ignore(main_obj);

    LinkedPaths* ignore_paths_start = (LinkedPaths*) malloc(sizeof(LinkedPaths));

    if(ignore_file != NULL){
        settings.has_config_file = true;
        read_ignore(main_obj->path, ignore_file, ignore_paths_start);

        LinkedPaths* curr_ignore_path = ignore_paths_start;
        while(curr_ignore_path != NULL){
            printf("%s\n", curr_ignore_path->path);
            curr_ignore_path = curr_ignore_path->next;
        }
    }

    // Allocate memory for the array of c files
    CArray c_files = {(FileOrDirectory*) malloc(C_ARR_LIMIT * sizeof(FileOrDirectory)), 0};
    CArray relevent = {(FileOrDirectory*) malloc(C_ARR_LIMIT * sizeof(FileOrDirectory)), 0};

    search_for_c_files(&c_files, main_obj, ignore_paths_start, &settings);

    printf("\nfound those c files:\n");
    for(uint16_t i = 0; i < c_files.length; i++)
        printf("\t%s\n", c_files.elements[i].path);

    FileOrDirectory* makefile_obj = look_for_makefile(main_obj);
    if(makefile_obj != NULL){
        printf("\nFound makefile.maker: \"%s\"\n", makefile_obj->path);
    }
    else printf("\nHavent found a makefile.maker\n");

    read_makefile(makefile_obj, &c_files, &relevent);

    // Finishing up
    free(main_obj);
    free(c_files.elements);
}
