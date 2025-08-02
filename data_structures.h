//
// Created by rodde on 27.7.2025.
//
#ifndef IO_GITHUB_CODERODDE_DSV2_DATA_STRUCTURES_H
#define IO_GITHUB_CODERODDE_DSV2_DATA_STRUCTURES_H
#include <stdlib.h>

// The tag file entry struct. Basically a mapping from
// a directory tag to the directory name.
typedef struct tag_file_entry_t {
    char* dir_tag;                   // The tag name.
    char* dir_path;                  // The file path associated with the tag.
    struct tag_file_entry_t* p_next; // The next tag file entry in the list.
    struct tag_file_entry_t* p_prev; // The previous tag file entry in the list.
}
tag_file_entry_t;

// The actual list of tag file entries.
typedef struct tag_file_entry_list_t {
    size_t len;               // The number of tag file entries in this list.
    tag_file_entry_t* p_head; // The pointer to the very first tag file entry.
    tag_file_entry_t* p_tail; // The pointer to the very last tag file entry.
}
tag_file_entry_list_t;

#endif // IO_GITHUB_CODERODDE_DSV2_DATA_STRUCTURES_H