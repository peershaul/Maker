#include "utils.h"

#include <stdio.h>

bool handmade_cmpstr(const char* s1, const char* s2){

    uint64_t length = 0;

    while(1){
        if(s1[length] != s2[length])
            return false;
        if(s1[length] == '\0')
            return true;
        length++;
    }
}

void handmade_strcpy(char* destination, const char* location, uint64_t destination_length){
    uint64_t ptr = 0;
    while(ptr < destination_length){
        destination[ptr] = location[ptr];
        if(location[ptr] == '\0') return;
        ptr++;
    }

    fprintf(stderr, "[handmade_strcpy]: destination overflowed during string copy");
}

uint64_t handmade_strlen(const char* str){
    uint64_t length = 0;
    while(str[length++] != '\0');
    return length - 1;
}

void handmade_strappend(char* destination, const char* location, uint64_t destination_max_length){
    uint64_t dest_len = handmade_strlen(destination);

    uint64_t index = 0;

    while(location[index] != '\0'){
        if(dest_len + index == destination_max_length) return;
        destination[dest_len + index] = location[index];
        index++;
    }

}

int64_t handmade_char_last_index_of(const char* target, char item){
    uint64_t last_index = handmade_strlen(target) - 1;

    for(uint64_t i = last_index; i >= 0; i--)
        if(target[i] == item)
            return i;

    return -1;
}

bool handmade_str_ends_with(const char* parent, const char* child){
    uint64_t child_length = handmade_strlen(child);
    uint64_t parent_length = handmade_strlen(parent);

    if(child_length > parent_length) return false;

    for(uint64_t i = 1; i <= child_length; i++)
        if(parent[parent_length - i] != child[child_length - i])
            return false;

    return true;
}

int64_t handmade_str_contains(const char* parent, const char* child){
    uint64_t child_length = handmade_strlen(child);
    uint64_t parent_length = handmade_strlen(parent);

    uint64_t cmp_index = 0;
    for(uint64_t i = 0; i < parent_length; i++){
        if(parent[i] == child[cmp_index]){
            if(cmp_index != child_length - 1) cmp_index++;
            else return i;
        }
        else if(parent[i] == child[0]) cmp_index++;
    }

    return -1;
}

int64_t handmade_str_contains_ch(const char* parent, const char ch){
    uint64_t parent_length = handmade_strlen(parent);

    for(uint64_t i = 0; i < parent_length; i++)
        if(parent[i] == ch)
            return i;

    return -1;
}

int64_t handmade_str_index_of_with_length(const char* target, const char* param, uint64_t offset, uint64_t target_length){
    uint64_t param_length = handmade_strlen(param);
    uint64_t length_with_offset = target_length - offset;
    uint64_t param_pointer = 0;

    for(uint64_t i = 0; i < length_with_offset; i++)
        if(param[param_pointer] == target[i + offset]){
            if(param_pointer == param_length - 1)
                return i;
            else param_pointer++;
        }

    return -1;
}

int64_t handmade_str_index_of(const char* target, const char* param, uint64_t offset){
    uint64_t target_length = handmade_strlen(target);
    int64_t result = handmade_str_index_of_with_length(target, param, offset, target_length);
    return result;
}

void handmade_dumpstr_from_arr(char* arr, char* dump_loc, uint64_t start_loc, uint64_t end_loc, uint64_t arr_length, uint64_t dump_loc_length){
    if(end_loc >= arr_length) return;
    uint64_t index = 0;
    for(uint64_t i = start_loc; (i <= end_loc && index < dump_loc_length); i++){
        dump_loc[index] = arr[i];
        index++;
    }
}
