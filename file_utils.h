//
// Created by rodde on 27.7.2025.
//
#ifndef IO_GITHUB_CODERODDE_DSV2_FILE_UTILS_H
#define IO_GITHUB_CODERODDE_DSV2_FILE_UTILS_H
#include "data_structures.h"

#define MAX_TAG_NAME_LENGTH 20
#define FORMAT_BUFFER_LENGTH 40

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

void tag_file_entry_list_t_append_tag_entry(tag_file_entry_list_t* list,
                                            tag_file_entry_t* e);

int tag_file_entry_list_t_try_update_tag_entry(tag_file_entry_list_t* list,
                                               char* tag,
                                               char* dir);

tag_file_entry_list_t* read_tag_file_entry_list(char* file_path);
int write_tag_file_entry_list(tag_file_entry_list_t* list);
tag_file_entry_list_t* sort_tag_file_entry_list_by_tags(tag_file_entry_list_t* list);
int print_tags(tag_file_entry_list_t* list);
int print_directories(tag_file_entry_list_t* list);
int print_tags_and_directories(tag_file_entry_list_t* list);
tag_file_entry_list_t* sort_tag_file_entry_list_by_directories(tag_file_entry_list_t* list);

void free_tag_file_entry_list(tag_file_entry_list_t* list,
                              int free_entry_data);

int write_command_file(char* cmd);
int empty_command_line();
#endif // IO_GITHUB_CODERODDE_DSV2_FILE_UTILS_H
