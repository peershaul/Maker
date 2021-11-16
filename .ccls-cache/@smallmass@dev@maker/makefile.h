#ifndef MAKEFILE_H
#define MAKEFILE_H

#include "main.h"

void read_makefile(FileOrDirectory* makefile_obj, CArray* c_files, CArray* relevant,
		   char** variables, char** uniques, CArray** irelevent);

void write_to_makefile(FileOrDirectory* root, FileOrDirectory* maker_file, CArray* relevent,
		       char* variables, char* uniques, CArray* irelevent);

#endif
