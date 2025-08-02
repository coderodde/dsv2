//
// Created by rodde on 28.7.2025.
//

#include "data_structures.h"
#include "file_utils.h"
#include "path_utils.h"
#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define PREV_DIR_TAG "previous_dir"
#define COMMAND_LENGTH (100 + MAX_PATH_LENGTH)

int command_show_tags() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        return EXIT_FAILURE;
    }

    int ret = print_tags(list);
    free_tag_file_entry_list(list, 1);
    return ret;
}

int command_show_sorted_tags() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        return EXIT_FAILURE;
    }

    tag_file_entry_list_t* sorted_list = sort_tag_file_entry_list_by_tags(list);

    if (!sorted_list) {
        free_tag_file_entry_list(list, 1);
        return EXIT_FAILURE;
    }

    int ret = print_tags(sorted_list);
    free_tag_file_entry_list(list, 1);
    free_tag_file_entry_list(sorted_list, 1);
    return ret;
}

int command_show_sorted_directories() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        return EXIT_FAILURE;
    }

    tag_file_entry_list_t* sorted_list = sort_tag_file_entry_list_by_directories(list);

    if (!sorted_list) {
        free_tag_file_entry_list(list, 1);
        return EXIT_FAILURE;
    }

    int ret = print_directories(sorted_list);
    free_tag_file_entry_list(list, 1);
    free_tag_file_entry_list(sorted_list, 1);
    return ret;
}   

int command_show_tags_and_directories() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        return EXIT_FAILURE;
    }

    int ret = print_tags_and_directories(list);
    free_tag_file_entry_list(list, 1);
    return ret;
}

int command_show_sorted_tags_and_directories() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        return EXIT_FAILURE;
    }

    tag_file_entry_list_t* sorted_list = sort_tag_file_entry_list_by_tags(list);

    if (!sorted_list) {
        free_tag_file_entry_list(list, 1);
        return EXIT_FAILURE;
    }

    int ret = print_tags_and_directories(sorted_list);
    free_tag_file_entry_list(list, 1);
    free_tag_file_entry_list(sorted_list, 1);
    return ret;
}

static size_t compute_levenshtein_distance(char* str1, char* str2) {
    const size_t num_rows = strlen(str1) + 1;
    const size_t num_cols = strlen(str2) + 1;
    size_t** matrix = malloc(num_rows * sizeof (size_t));

    for (size_t row_index = 0; row_index < num_rows; ++row_index) {
        matrix[row_index] = malloc(num_cols * sizeof(size_t));
    }

    for (size_t row_index = 0; row_index < num_rows; ++row_index) {
        matrix[row_index][0] = row_index;
    }

    for (size_t col_index = 1; col_index < num_cols; ++col_index) {
        matrix[0][col_index] = col_index;
    }

    for (size_t col_index = 1; col_index < num_cols; ++col_index) {
        for (size_t row_index = 1; row_index < num_rows; ++row_index) {
            size_t substitution_cost;

            if (str1[row_index - 1] == str2[col_index - 1]) {
                substitution_cost = 0;
            } else {
                substitution_cost = 1;
            }

            size_t a = matrix[row_index - 1][col_index] + 1;
            size_t b = matrix[row_index][col_index - 1] + 1;
            size_t c = matrix[row_index - 1][col_index - 1] + substitution_cost;
            size_t min_abc = MIN(a, MIN(b, c));

            matrix[row_index][col_index] = min_abc;
        }
    }

    return matrix[num_rows - 1][num_cols - 1];
}

static tag_file_entry_t* get_previous_tag_file_entry(tag_file_entry_list_t* list) {
    tag_file_entry_t* e = list->p_head;

    while (e) {
        if (strcmp(e->dir_tag, PREV_DIR_TAG) == 0) {
            return e;
        }

        e = e->p_next;
    }

    return NULL;
}

int command_switch_to_previous() {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());
    tag_file_entry_t* prev_entry = get_previous_tag_file_entry(list);
    char* cwd = get_current_working_directory();

    if (!prev_entry) {
        prev_entry = malloc(sizeof(*prev_entry));

        char* command = malloc(10 + PATH_MAX);
        sprintf(command, "cd %s\n", cwd);

        char* tag = malloc(MAX_TAG_NAME_LENGTH);
        strcpy(tag, PREV_DIR_TAG);

        prev_entry->dir_tag  = tag;
        prev_entry->dir_path = cwd;
        tag_file_entry_list_t_append_tag_entry(list, prev_entry);
        puts("[INFO] No previous tag in the tag file. Created one.");

        write_command_file(command);
        write_tag_file_entry_list(list);
        free_tag_file_entry_list(list, 1);
        return EXIT_SUCCESS;
    }

    char* prev_dir = prev_entry->dir_path;
    char* command = malloc(COMMAND_LENGTH);
    sprintf(command, "cd %s", prev_dir);
    prev_entry->dir_path = cwd;
    write_command_file(command);
    write_tag_file_entry_list(list);
    return EXIT_SUCCESS;
}

int command_try_switch(char* tag) {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        fprintf(stderr,
                "[ERROR] Could not read the tag file.\n");
        return EXIT_FAILURE;
    }

    tag_file_entry_t* e = list->p_head;

    if (!e) {
        fprintf(stderr, "[ERROR] The tag file is empty.\n");
        free_tag_file_entry_list(list, 1);
        return EXIT_FAILURE;
    }

    size_t best_dist =  SIZE_MAX;
    char* best_path = NULL;

    while (e) {
        size_t dist = compute_levenshtein_distance(tag,
                                                   e->dir_tag);

        if (dist == 0) {
            char* command = malloc(100 + PATH_MAX);
            sprintf(command, "cd %s", e->dir_path);
            write_command_file(command);
            free_tag_file_entry_list(list, 1);
            return EXIT_SUCCESS;
        }

        if (best_dist > dist) {
            best_dist = dist;
            best_path = e->dir_path;
        }

        e = e->p_next;
    }

    char* command = malloc(COMMAND_LENGTH);
    sprintf(command, "cd %s\n", best_path);
    write_command_file(command);
    free_tag_file_entry_list(list, 1);
    return EXIT_SUCCESS;
}

static void tag_file_entry_list_t_unlink(tag_file_entry_list_t* list,
                                         tag_file_entry_t* e) {
    if (list->len == 1) {
        list->p_head = NULL;
        list->p_tail = NULL;
        list->len = 0;
    } else {
        if (e->p_prev) {
            e->p_prev->p_next = e->p_next;
        } else {
            list->p_head = e->p_next;
            list->p_head->p_prev = NULL;
        }

        if (e->p_next) {
            e->p_next->p_prev = e->p_prev;
        } else {
            list->p_tail = e->p_prev;
            list->p_tail->p_next = NULL;
        }

        list->len--;
    }
}

int command_remove_tags(int argc, char* argv[]) {
    tag_file_entry_list_t* list = read_tag_file_entry_list(get_tag_file_path());

    if (!list) {
        fputs("[ERROR] Could not read the tag file entry list.",
              stderr);

        return EXIT_FAILURE;
    }

    if (list->len == 0) {
        return EXIT_SUCCESS;
    }

    for (int index = 2; index < argc; ++index) {
        char* target_tag = argv[index];
        tag_file_entry_t* e = list->p_head;

        while (e) {
            if (strcmp(target_tag, e->dir_tag) == 0) {
                tag_file_entry_list_t_unlink(list, e);
            }

            e = e->p_next;
        }
    }

    return write_tag_file_entry_list(list);
}
