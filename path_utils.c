//
// Created by rodde on 26.7.2025.
//

#include "path_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char* const TAGS_FILE_NAME    = "/.dsv2/tags";
const char* const COMMAND_FILE_NAME = "/.dsv2/ds_command";

char* get_home_directory() {
    return getenv("HOME");
}

char* get_tag_file_path() {
    const char* home = get_home_directory();

    if (!home) {
        fprintf(stderr, "[ERROR] Home directory not found.\n");
        return NULL;
    }

    char* tag_file_path = malloc(MAX_PATH_LENGTH);

    snprintf(tag_file_path,
             MAX_PATH_LENGTH,
             "%s%s",	
             home,
             TAGS_FILE_NAME);

    return tag_file_path;
}

char* get_command_file_path() {
    const char* home = get_home_directory();

    if (!home) {
        fprintf(stderr, "[ERROR] Home directory not found.\n");
        return NULL;
    }

    char* cmd_file_path = malloc(MAX_PATH_LENGTH);

    snprintf(cmd_file_path,
             MAX_PATH_LENGTH,
             "%s%s",
             home,
             COMMAND_FILE_NAME);

    return cmd_file_path;

}

char* get_current_working_directory() {
#ifdef _WIN32
    char* buffer = malloc(MAX_PATH_LENGTH);
    if (!buffer) {
        fprintf(stderr, "[ERROR] Memory allocation failed for current directory buffer.\n");
        return NULL;
    }

    // Get the current working directory
    // using the Windows GetCurrentDirectoryA function.
    // This function will return the current working directory
    // in the buffer provided.
    // If it fails, it will return 0.
    // The buffer must be large enough to hold the path.
    if (!GetCurrentDirectoryA(MAX_PATH_LENGTH, buffer)) {
        fprintf(stderr, "[ERROR] Could not get current directory.\n");
        free(buffer);
        return NULL;
    }
    return buffer;
#else
    char* buffer = malloc(MAX_PATH_LENGTH);

    if (!buffer) {
        fprintf(stderr, "[ERROR] Memory allocation failed for current directory buffer.\n");
        return NULL;
    }

    // Get the current working directory
    // using the POSIX getcwd function.
    // This function will return the current working directory
    // in the buffer provided.
    // If it fails, it will return NULL.
    // The buffer must be large enough to hold the path.
    // The maximum length of the path is defined by MAX_PATH_LENGTH.
    // If the buffer is NULL, it will return NULL.
    // If the current working directory cannot be retrieved,
    // it will return NULL and set errno to indicate the error.
    // The buffer must be freed by the caller.
    if (!getcwd(buffer, MAX_PATH_LENGTH)) {
        fprintf(stderr, "[ERROR] Could not get current directory.\n");
        free(buffer);
        return NULL;
    }

    return buffer;
#endif
}
