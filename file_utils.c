//
// Created by rodde on 27.7.2025.
//
#include "data_structures.h"
#include "file_utils.h"
#include "path_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

void tag_file_entry_list_t_append_tag_entry(tag_file_entry_list_t* list,
                                            tag_file_entry_t* e) {
    if (list->len == 0) {
        list->p_head = e;
        list->p_tail = e;
        e->p_next = NULL;
        e->p_prev = NULL;
        list->len = 1;
    } else {
        list->p_tail->p_next = e;
        e->p_prev = list->p_tail;
        list->p_tail = e;
        e->p_next = NULL;
        list->len++;
    }
}

int tag_file_entry_list_t_try_update_tag_entry(tag_file_entry_list_t* list,
                                               char* tag,
                                               char* dir) {
    if (!list) {
        fprintf(stderr, "[ERROR] The input list is null.\n");
        abort();
    }

    tag_file_entry_t* e = list->p_head;

    while (e) {
        if (strcmp(tag, e->dir_tag) == 0) {
            char* updated_dir = malloc(MAX_PATH_LENGTH);
            strcpy(updated_dir, dir);
            free(e->dir_path);
            e->dir_path = updated_dir;
            return TRUE;
        }

        e = e->p_next;
    }

    return FALSE;
}

void free_tag_file_entry_list(tag_file_entry_list_t* list,
                              int free_entry_data) {
    if (!list) {
        return;
    }

    tag_file_entry_t* e = list->p_head;

    while (e) {
        tag_file_entry_t* next_e = e->p_next;

        if (free_entry_data) {
            free(e->dir_tag);
            free(e->dir_path);
        }

        free(e);
        e = next_e;
    }
}

static char* get_string_format_for_output(size_t maximum_tag_length) {
    char* buffer = malloc(40);

    if (!buffer) {
        fprintf(stderr,
                "[ERROR] Could not allocate the format string.\n");
        return NULL;
    }

    sprintf(buffer,
            "%%%zus %%s\n",
            maximum_tag_length);

    return buffer;
}

static char* get_string_format_for_file_reading() {
    char* buffer = malloc(40);

    if (!buffer) {
        fprintf(stderr,
                "[ERROR] Could not allocate the format string.\n");
        return NULL;
    }

    sprintf(buffer,
            "%%%zus %%%zus",
            (size_t) MAX_TAG_NAME_LENGTH,
            (size_t) MAX_PATH_LENGTH);

    return buffer;
}

tag_file_entry_list_t* read_tag_file_entry_list(char* file_path) {
    tag_file_entry_list_t* tag_file_entry_list = malloc(sizeof(tag_file_entry_list_t));

    if (!tag_file_entry_list) {
        fprintf(stderr, "[ERROR] Memory allocation failed for tag file entry list.\n");
        return NULL;
    }

    FILE* f = fopen(file_path, "r");

    if (!f) {
        fprintf(stderr,
                "[ERROR] Could not open tag file: %s\n",
                file_path);

        free(tag_file_entry_list);
        return NULL;
    }

    char* fmt = get_string_format_for_file_reading();

    if (!fmt) {
        fprintf(stderr, "[ERROR] Could not get the format string.");
        fclose(f);
        free(tag_file_entry_list);
        return NULL;
    }

    size_t line_number = 0;

    while (1) {
        if (ferror(f)) {
            fclose(f);
            free(fmt);
            fprintf(stderr, "[ERROR] File error.\n");
            return NULL;
        }

        ++line_number;

        tag_file_entry_t* e = malloc(sizeof *e);
        e->dir_tag  = malloc(MAX_TAG_NAME_LENGTH);
        e->dir_path = malloc(MAX_PATH_LENGTH);

        int read_count = fscanf(f,
                                "%s %s",
                                e->dir_tag,
                                e->dir_path);

        if (ferror(f)) {
            free_tag_file_entry_list(tag_file_entry_list,
                                     1);
            fclose(f);
            free(e);
            return NULL;
        }

        if (feof(f)) {
            fclose(f);
            free(e);
            free_tag_file_entry_list(tag_file_entry_list,
            						 1);

            return tag_file_entry_list;
        }

        if (read_count == 2) {
            if (!tag_file_entry_list->p_head) {
                tag_file_entry_list->p_head = e;
                tag_file_entry_list->p_tail = e;
                tag_file_entry_list->len = 1;
            } else {
                tag_file_entry_list->p_tail->p_next = e;
                e->p_prev = tag_file_entry_list->p_tail;
                tag_file_entry_list->p_tail = e;
                tag_file_entry_list->len++;
            }
        } else {
            fclose(f);
            free(e);
            free_tag_file_entry_list(tag_file_entry_list,
                                     1);

            return tag_file_entry_list;
        }
        
        fclose(f);
        free(e);	
        return tag_file_entry_list;
    }
}

int write_tag_file_entry_list(tag_file_entry_list_t* list) {
    char* file_path = get_tag_file_path();

    if (!file_path) {
        fprintf(stderr, "[ERROR] The file path is NULL.");
        return EXIT_FAILURE;
    }

    FILE* f = fopen(file_path, "w+");

    for (tag_file_entry_t* e = list->p_head; e; e = e->p_next) {
        fprintf(f,
                "%s %s\n",
                e->dir_tag,
                e->dir_path);
    }

    fclose(f);
    return EXIT_SUCCESS;
}

int print_tags(tag_file_entry_list_t* list) {
    if (!list) {
        fprintf(stderr, "[ERROR] The input tag file list is NULL.");
        return EXIT_FAILURE;
    }

    tag_file_entry_t* e = list->p_head;

    while (e) {
        printf("%s\n", e->dir_tag);
        e = e->p_next;
    }

    return EXIT_SUCCESS;
}

static size_t get_maximum_tag_length(tag_file_entry_list_t* list) {
    size_t maximum_tag_length = 0;
    tag_file_entry_t* e = list->p_head;

    while (e) {
        size_t len = strlen(e->dir_tag);

        if (maximum_tag_length < len) {
            maximum_tag_length = len;
        }

        e = e->p_next;
    }

    return maximum_tag_length;
}

int print_tags_and_directories(tag_file_entry_list_t* list) {
    if (!list) {
        fprintf(stderr, "[ERROR] The input tag file list is NULL.");
        return EXIT_FAILURE;
    }

    size_t maximum_tag_length = get_maximum_tag_length(list);
    char* fmt = get_string_format_for_output(maximum_tag_length);

    tag_file_entry_t* e = list->p_head;

    while (e) {
        printf(fmt,
               e->dir_tag,
               e->dir_path);

        e = e->p_next;
    }

    return EXIT_SUCCESS;
}


static char* get_directory_print_format(tag_file_entry_list_t* list) {
    size_t max_path_length = 0;
    tag_file_entry_t* e = list->p_head;

    while (e) {
        size_t path_len = strlen(e->dir_path);

        if (max_path_length < path_len) {
            max_path_length = path_len;
        }

        e = e->p_next;
    }

    char* buffer = malloc(FORMAT_BUFFER_LENGTH);

    if (!buffer) {
        fprintf(stderr, "[ERROR] Could not allocate the format buffer.\n");
        return NULL;
    }

    sprintf(buffer, "%%%zus %%s\n", max_path_length);
    return buffer;
}

static char* get_tag_and_directory_print_format(tag_file_entry_list_t* list) {
    size_t max_tag_length = 0;
    tag_file_entry_t* e = list->p_head;

    while (e) {
        size_t tag_len = strlen(e->dir_tag);

        if (max_tag_length < tag_len) {
            max_tag_length = tag_len;
        }

        e = e->p_next;
    }

    char* buffer = malloc(FORMAT_BUFFER_LENGTH);

    if (!buffer) {
        fprintf(stderr, "[ERROR] Could not allocate the format buffer.\n");
        return NULL;
    }

    sprintf(buffer, "%%%zus %%s\n", max_tag_length);
    return buffer;
}

int print_directories(tag_file_entry_list_t* list) {
    if (!list) {
        fprintf(stderr, "[ERROR] The input tag file list is NULL.");
        return EXIT_FAILURE;
    }

    if (list->len == 0) {
        // OK, print nothing:
        return EXIT_SUCCESS;
    }

    tag_file_entry_t* e = list->p_head;
    char* fmt = get_directory_print_format(list);

    while (e) {
        printf(fmt,
               e->dir_path,
               e->dir_tag);

        e = e->p_next;
    }

    return EXIT_SUCCESS;
}

static int compare_by_tags(const void* a, const void* b) {
    tag_file_entry_t** e1 = (tag_file_entry_t**) a;
    tag_file_entry_t** e2 = (tag_file_entry_t**) b;

    return strcmp((*e1)->dir_tag,
                  (*e2)->dir_tag);
}

static int compare_by_directories(const void* a, const void* b) {
    tag_file_entry_t** e1 = (tag_file_entry_t**) a;
    tag_file_entry_t** e2 = (tag_file_entry_t**) b;

    return strcmp((*e1)->dir_path,
                  (*e2)->dir_path);
}

static tag_file_entry_t* tag_file_entry_t_copy(tag_file_entry_t* e) {
    tag_file_entry_t* e_copy = malloc(sizeof *e);

    if (!e_copy) {
        fprintf(stderr, "[ERROR] Could not allocate a copy of a tag file entry.\n");
        return NULL;
    }

    e_copy->dir_tag  = malloc(MAX_TAG_NAME_LENGTH);
    e_copy->dir_path = malloc(MAX_PATH_LENGTH);

    strcpy(e_copy->dir_tag,
           e->dir_tag);

    strcpy(e_copy->dir_path,
           e->dir_path);

    return e_copy;
}

static void tag_file_entry_t_free(tag_file_entry_t* e) {
    free(e->dir_tag);
    free(e->dir_path);
    e->dir_tag = NULL;
    e->dir_path = NULL;
    e->p_next = NULL;
    e->p_prev = NULL;
}

static tag_file_entry_list_t*
sort_tag_file_entry_list_impl(
        tag_file_entry_list_t* list,
        int(*cmp)(const void*, const void*)) {

    tag_file_entry_t* e = list->p_head;

    if (!e) {
        fprintf(stderr, "[ERROR] The input tag file entry list is NULL.\n");
        return NULL;
    }

    tag_file_entry_t** arr = malloc(list->len * sizeof(tag_file_entry_t*));
    size_t arr_index = 0;

    while (e) {
        arr[arr_index++] = tag_file_entry_t_copy(e);
        e = e->p_next;
    }

    qsort(arr,
          list->len,
          sizeof(tag_file_entry_t*),
          cmp);

    tag_file_entry_list_t* sorted_list = malloc(sizeof *sorted_list);

    if (!sorted_list) {
        fprintf(stderr,
                "[ERROR] Could not allocate the space for "
                "the sorted tag file entry list.\n");

        for (arr_index = 0; arr_index < list->len; ++arr_index) {
            tag_file_entry_t_free(arr[arr_index]);
        }

        free(sorted_list);
        return NULL;
    }

    sorted_list->len = 0;

    for (arr_index = 0; arr_index < list->len; ++arr_index) {
        tag_file_entry_t* e_copy = tag_file_entry_t_copy(arr[arr_index]);

        tag_file_entry_list_t_append_tag_entry(sorted_list,
                                               e_copy);
    }

    sorted_list->p_tail->p_next = NULL;
    return sorted_list;
}

tag_file_entry_list_t* sort_tag_file_entry_list_by_tags(tag_file_entry_list_t* list) {
   return sort_tag_file_entry_list_impl(list,
                                        compare_by_tags);
}

tag_file_entry_list_t* sort_tag_file_entry_list_by_directories(tag_file_entry_list_t* list) {
    return sort_tag_file_entry_list_impl(list,
                                         compare_by_directories);
}

int write_command_file(char* command) {
    if (!command) {
        fprintf(stderr, "[ERROR] Input command is NULL.");
        return EXIT_FAILURE;
    }

    FILE* f = fopen(get_command_file_path(), "w");

    if (!f) {
        free(command);
        fprintf(stderr, "[ERROR] Could not open the command file.\n");
        return EXIT_FAILURE;
    }

    fputs(command, f);
    fclose(f);
    return EXIT_SUCCESS;
}

int empty_command_line() {
    FILE* f = fopen(get_command_file_path(), "w+");

    if (!f) {
        fprintf(stderr, "[ERROR] Could not open the command file.\n");
        return EXIT_FAILURE;
    }

    fclose(f);
    return EXIT_SUCCESS;
}
