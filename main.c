//
// Created by rodde on 26.7.2025.
//
#include "commands.h"
#include "data_structures.h"
#include "file_utils.h"
#include "path_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (empty_command_line() != EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    }

    if (argc == 1) {
        return command_switch_to_previous();
    } else if (argc == 2) {
        char* arg = argv[1];

        if (strcmp(arg, "-l") == 0) {
            command_show_tags();
        } else if (strcmp(arg, "-L") == 0) {
            command_show_tags_and_directories();
        } else if (strcmp(arg, "-s") == 0) {
            command_show_sorted_tags();
        } else if (strcmp(arg, "-S") == 0) {
            command_show_sorted_tags_and_directories();
        } else if (strcmp(arg, "-d") == 0) {
            command_show_sorted_directories();
        } else {
            return command_try_switch(arg);
        }

        return EXIT_SUCCESS;
    }

    if ((argc == 4) && ((strcmp(argv[1], "-a") == 0) || (strcmp(argv[1], "--add-tag") == 0))) {
        char* tag = argv[2];
        char* dir = argv[3];
        tag_file_entry_t* e = malloc(sizeof *e);

        if (!e) {
            fprintf(stderr,
                    "[ERROR] Could not allocate the tag file entry.\n");

            return EXIT_FAILURE;
        }

        tag_file_entry_list_t* list =
                read_tag_file_entry_list(get_tag_file_path());

        if (!list) {
            free(e);
            fprintf(stderr, "[ERROR[ Could not read the tag file.\n");
            return EXIT_FAILURE;
        }

        e->dir_tag  = malloc(MAX_TAG_NAME_LENGTH);
        e->dir_path = malloc(MAX_PATH_LENGTH);

        strcpy(e->dir_tag, tag);
        strcpy(e->dir_path, dir);

        if (!tag_file_entry_list_t_try_update_tag_entry(list, tag, dir)) {
            tag_file_entry_list_t_append_tag_entry(list, e);
        }

        write_tag_file_entry_list(list);
        free_tag_file_entry_list(list, 1);
        return EXIT_SUCCESS;
    }

    if ((argc > 1) && ((strcmp(argv[1], "-r") == 0) || (strcmp(argv[1], "--remove-tags") == 0))) {
        return command_remove_tags(argc, argv);
    }

    fprintf(stderr,
            "[ERROR] Unrecognized command line: %s %s ...\n",
            argv[0],
            argv[1]);
    
    return EXIT_FAILURE;
}
