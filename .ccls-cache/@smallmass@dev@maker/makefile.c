#include "makefile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "makefile.h"

#define STR_SIZE 65536

typedef struct CharMap {
  char variable[NAME_MAX_LENGTH];
  char default_value[NAME_MAX_LENGTH];
} CharMap;

// Returns a list of all the c files mentioned under unique portion of the
// makefile.maker file

char *
extruct_name(const char *path) {
  uint64_t slash_pos = handmade_char_last_index_of(path, '/');
  uint64_t length = strlen(path);

  uint64_t name_length = length - slash_pos - 1;

  char* name = (char*) malloc(name_length * sizeof(char));

  for(uint64_t i = slash_pos; i < length - 1; i++)
    name[i - slash_pos] = path[i + 1];
  
  return name;
}

void look_for_irelevent(CArray *irelevant, CArray *interesting_files,
                        const char *uniques) {
  uint64_t uniques_length = strlen(uniques);
  char word[LINE_MAX_LENGTH];
  bool large_quit = false;
  bool small_quit = false;
  uint64_t index = 0;

  while (!large_quit) {
    uint16_t word_index = 0;
    small_quit = 0;

    while (!small_quit && !large_quit) {
      if (uniques[index] != ' ' && uniques[index] != '\n')
        word[word_index] = uniques[index];
      else
        small_quit = true;

      if (index == uniques_length - 1)
        large_quit = true;

      index++;
      word_index++;
    }

    char path_dumper[LINE_MAX_LENGTH] = {0};

    strcat(path_dumper, "/");
    strcat(path_dumper, word);

    for (uint16_t i = 0; i < interesting_files->length; i++)
      if (handmade_str_ends_with(interesting_files->elements[i].path,
                                 path_dumper)) {
        irelevant->elements[irelevant->length] = interesting_files->elements[i];
        irelevant->length++;
        break;
      }

    memset(word, 0x00, LINE_MAX_LENGTH);
  }
}

// Reading the makefile
void read_makefile(FileOrDirectory *makefile_obj, CArray *c_files,
                   CArray *relevant, char** variables, char** uniques, CArray** irelevent) {
  FILE *file = fopen(makefile_obj->path, "r");

  if (file == NULL) {
    fprintf(stderr, "Cannot open \"%s\"\n", makefile_obj->path);
    return;
  }

  printf("Reading \"%s\"....\n", makefile_obj->path);

  char line[LINE_MAX_LENGTH] = {0};
  bool unique_flag = false;

  *variables = (char *)malloc(STR_SIZE * sizeof(char));
  *uniques = (char *)malloc(STR_SIZE * sizeof(char));

  memset(*variables, 0x00, STR_SIZE);
  memset(*uniques, 0x00, STR_SIZE);

  
  
  *irelevent = (CArray*) malloc(sizeof(CArray));
  CArray* irelevent_ptr = *irelevent;

    
  irelevent_ptr->elements =
    (FileOrDirectory *)malloc(C_ARR_LIMIT * sizeof(FileOrDirectory));
  irelevent_ptr->length = 0;


  // Loops over every line of the makerfile and determaining if it belongs to
  // unique variable or ignore categories
  while (fgets(line, LINE_MAX_LENGTH, file)) {
    if (handmade_str_contains(line, "#begin_unique") != -1 && !unique_flag) {
      unique_flag = true;
      strcat(*uniques, line);
      continue;
    }

    if (handmade_str_contains(line, "#end_unique") != -1 && unique_flag) {
      unique_flag = false;
      strcat(*uniques, line);
      continue;
    }

    if(handmade_str_contains(line, "#") != -1) continue;

    if (unique_flag) {
      strcat(*uniques, line);
      continue;
    }

    if (handmade_str_contains(line, "=") != -1)
      strcat(*variables, line);
  }

  // Looking for c files mentioned under unique
  look_for_irelevent(irelevent_ptr, c_files, *uniques);

  // adding to the relevant list the c files who arent mentions under the unqiue
  // string
  for (uint16_t i = 0; i < c_files->length; i++) {
    bool found = false;
    for (uint16_t j = 0; j < irelevent_ptr->length; j++)
      if (handmade_cmpstr(irelevent_ptr->elements[i].path,
                          c_files->elements[i].path)) {
        found = true;
        break;
      }

if (!found) {
      relevant->elements[relevant->length] = c_files->elements[i];
      relevant->length++;
    }
  }
  fclose(file);
}

void check_for_important_variables(FILE *file, char *variables) {

  const uint8_t imp_vars_length = 3;
  
  CharMap important_variables[] =
  { {"CC", "gcc"},
    {"LIBS", ""},
    {"CFLAGS", "-g -Wall"} };

  uint8_t declared_vars[imp_vars_length];
  uint8_t declared_vars_length = 0;

  uint64_t index = 0;
  uint64_t variables_length = strlen(variables);
  char first_word[NAME_MAX_LENGTH] = "";
  bool small_quit;
  bool large_quit = false;

  while (!large_quit) {
    uint16_t word_index = 0;
    small_quit = false;

    while (!small_quit && !large_quit) {
      if (variables[index] != ' ' && variables[index] != '\n') {
        first_word[word_index] = variables[index];
	first_word[word_index + 1] = '\0';
      } else
        small_quit = true;

      if(index == variables_length - 1)
	large_quit = true;

      index++;
      word_index++;
    }
    
    for(uint8_t i = 0; i < imp_vars_length && declared_vars_length < imp_vars_length; i++)
      if (handmade_cmpstr(first_word, important_variables[i].variable)) {
        declared_vars[declared_vars_length++] = i;
        break;
      }

    if(declared_vars_length == imp_vars_length) break;

    char ch;
    while (ch != '\n' && ch != '\0' && index < variables_length - 1) {
      ch = variables[index];
      if (ch == '\0')
        large_quit = true;

      index++;
    }
  }

  if (declared_vars_length != imp_vars_length) {
    fprintf(file, "// Default variables, added by Maker\n");

    for(uint8_t i = 0; i < imp_vars_length; i++){
      bool found = false;
      for(uint8_t j = 0; j < declared_vars_length; j++)
        if (declared_vars[j] == i) {
          found = true;
          break;
        }

      if (!found)
        fprintf(file, "%s = \"%s\"\n", important_variables[i].variable,
                important_variables[i].default_value);
    }
  }
}

void write_to_makefile(FileOrDirectory* root, FileOrDirectory* maker_file,
		       CArray* relevant, char* variables, char* uniques,
		       CArray* irelevent){


  char makefile_path[PATH_MAX_LENGTH] = "";
  strcat(makefile_path, root->path);
  strcat(makefile_path, "/makefile");

  FILE* makefile = fopen(makefile_path, "w");
  rewind(makefile);

  check_for_important_variables(makefile, variables);

  fprintf(makefile, "\n%s\n\n%s\n", variables, uniques);
  
  fprintf(makefile, "// Maker added code!\n");

  uint16_t names_offset;
  
  char** names =
    (char**) malloc((relevant->length + relevant->length) * sizeof(char*));

  for (uint16_t i = 0; i < relevant->length; i++) {
    names[i] = extruct_name(relevant->elements[i].path); 

    char object_path[PATH_MAX_LENGTH];
    uint16_t index = 1;

    strcat(object_path, "bulid/objects/");
    uint16_t name_ptr = strlen("bulid/objects/");

    while (names[i][index] != '\0') {
      object_path[name_ptr + index - 1] = names[i][index - 1];
      index++;
    }

    object_path[name_ptr + index - 1] = 'o';
    object_path[name_ptr + index] = '\0';
    names[i][strlen(names[i]) - 1] = 'o';

    fprintf(makefile, "%s: %s\n", names[i], relevant->elements[i].path);
    fprintf(makefile, "\t${CC} ${CLFAGS} %s -o %s ${LIBS}\n\n",
	    relevant->elements[i].path, object_path);

    names_offset = i;
  }

  uint16_t names_length = names_offset + 1;
  
  for (uint16_t i = 0; i < irelevent->length; i++) {
    char* name = extruct_name(irelevent->elements[i].path);
    name[strlen(name) - 1] = 'o';

    bool found = false;
    for(uint64_t j = 0; j <= names_offset + i; j++)
      if(handmade_cmpstr(name, names[j])){
	found = true;
	break;
      }

    if (!found) {
      names[names_length] = name;
      names_length++;
    };
  }

  fprintf(makefile, "all: ");

  for(uint16_t i = 0; i < names_length; i++)
    fprintf(makefile, "%s ", names[i]);

  fprintf(makefile, "\n");

  // Finish off the linking

  fclose(makefile);
}


