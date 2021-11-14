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
    printf("%s\n", word); 

    // TODO: (peershaul) - add a loop that searches for a file that is in the list of interesting files
    
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
    fclose(file);
}
