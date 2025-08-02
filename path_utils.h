//
// Created by rodde on 26.7.2025.
//

#ifndef IO_GITHUB_CODERODDE_DSV2_PATH_UTILS_H
#define IO_GITHUB_CODERODDE_DSV2_PATH_UTILS_H

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef _WIN32
extern const char* const TAGS_FILE_NAME;
extern const char* const COMMAND_FILE_NAME;
#include <windows.h>
#define MAX_PATH_NAME MAX_PATH
#else
extern const char* const TAGS_FILE_NAME;
extern const char* const COMMAND_FILE_NAME;
#include <limits.h>
#define MAX_PATH_LENGTH PATH_MAX
#endif

char* get_home_directory();
char* get_tag_file_path();
char* get_tag_file_path();
char* get_current_working_directory();
char* get_command_file_path();

#endif // IO_GITHUB_CODERODDE_DSV2_PATH_UTILS_H
