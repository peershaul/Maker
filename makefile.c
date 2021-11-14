#include "makefile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makefile.h"

#include <string.h>

#define STR_SIZE 65536

void look_for_irelevent(CArray* irelevant, CArray* interesting_files, const char* uniques){
  uint64_t uniques_length = strlen(uniques);
  char word[LINE_MAX_LENGTH];
  bool large_quit = false;
  bool small_quit = false;
  uint64_t index = 0;

  while(!large_quit){
    uint16_t word_index = 0;
    small_quit = false;
    
    while(!small_quit && !large_quit){
      if(uniques[index] != ' ' && uniques[index] != '\n')
	word[word_index] = uniques[index];
      else small_quit = true;

      if(index == uniques_length - 1) large_quit = true;
      
      index++;
      word_index++;
    }

    char path_dumper[LINE_MAX_LENGTH] = {0};

    strcat(path_dumper, "/");
    strcat(path_dumper, word);
    
    for(uint16_t i = 0; i < interesting_files->length; i++)
      if(handmade_str_ends_with(interesting_files->elements[i].path, path_dumper)){
	irelevant->elements[irelevant->length] = interesting_files->elements[i];
	irelevant->length++;
	break;
      }
    
    memset(word, 0x00, LINE_MAX_LENGTH);
  }
}

void read_makefile(FileOrDirectory* makefile_obj, CArray* c_files, CArray* relevant){
    FILE* file = fopen(makefile_obj->path, "r");

    if(file == NULL){
        fprintf(stderr, "Cannot open \"%s\"\n", makefile_obj->path);
        return;
    }

    printf("Reading \"%s\"....\n", makefile_obj->path);

    char line[LINE_MAX_LENGTH] = {0};
    bool unique_flag = false;

    char* variables = (char*) malloc(STR_SIZE * sizeof(char));
    char* uniques = (char*) malloc(STR_SIZE * sizeof(char));

    memset(variables, 0x00, STR_SIZE);
    memset(uniques, 0x00, STR_SIZE);

    CArray irelevent = {(FileOrDirectory*) malloc(C_ARR_LIMIT * sizeof(FileOrDirectory)), 0};

    while(fgets(line, LINE_MAX_LENGTH, file)){
        if(handmade_str_contains(line, "#begin_unique") != -1 && !unique_flag){
            unique_flag = true;
            strcat(uniques, line);
            continue;
        }

        if(handmade_str_contains(line, "#end_unique") != -1 && unique_flag){
            unique_flag = false;
            strcat(uniques, line);
            continue;
        }

        if(unique_flag){
            strcat(uniques, line);
            continue;
        }

        if(handmade_str_contains(line, "=") != -1)
            strcat(variables, line);
    }

    look_for_irelevent(&irelevent, c_files, uniques);

    for(uint16_t i = 0; i < c_files->length; i++){
      bool found = false;
      for(uint16_t j = 0; j < irelevent.length; j++)
	if(handmade_cmpstr(irelevent.elements[i].path, c_files->elements[i].path)){
	    found = true;
	    break;
	  }

      if(!found) {
	relevant->elements[relevant->length] = c_files->elements[i];
	relevant->length++;
      }
      
    } 
    fclose(file);
}
