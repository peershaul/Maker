#ifndef HANDMADE_UTILS
#define HANDMADE_UTILS

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


bool handmade_cmpstr(const char* s1, const char* s2);
bool handmade_str_ends_with(const char* parent, const char* child);

int64_t handmade_str_contains(const char* parent, const char* child);
int64_t handmade_str_contains_ch(const char* parent, const char ch);

void handmade_strcpy(char* destination, const char* location, uint64_t destination_length);
void handmade_strappend(char* destination, const char* location, uint64_t destination_length);

uint64_t handmade_strlen(const char* str);
int64_t handmade_char_last_index_of(const char* target, char item);
int64_t handmade_str_index_of(const char* target, const char* param, uint64_t offset);
int64_t handmade_str_index_of_with_length(const char* target, const char* param, uint64_t offset, uint64_t target_length);

void handmade_dumpstr_from_arr(char* arr, char* dump_loc, uint64_t start_loc, uint64_t end_loc, uint64_t arr_length, uint64_t dump_loc_length);

#endif
