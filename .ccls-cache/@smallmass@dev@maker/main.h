
#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>

#include "utils.h"

#define LINE_MAX_LENGTH 2048
#define PATH_MAX_LENGTH 512
#define NAME_MAX_LENGTH 256

#define C_ARR_LIMIT 512

typedef struct MainSetting{
    char base_path[PATH_MAX_LENGTH];
    bool has_config_file;
} MainSetting;

typedef struct FileOrDirectory{
    char path[PATH_MAX_LENGTH];
    bool is_directory;
    uint16_t item_count;
    struct FileOrDirectory* children;
    struct FileOrDirectory* parent;

} FileOrDirectory;

typedef struct ConfigFile{
    FileOrDirectory* object;
    char* contents;
    uint64_t contents_length;
} ConfigFile;

typedef struct CArray{
    FileOrDirectory* elements;
    uint16_t length;
} CArray;

typedef struct LinkedPaths{
    char* path;
    struct LinkedPaths* next;
} LinkedPaths;

#endif
