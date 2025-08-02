//
// Created by rodde on 28.7.2025.
//

#ifndef IO_GITHUB_CODERODDE_DSV2_COMMANDS_H
#define IO_GITHUB_CODERODDE_DSV2_COMMANDS_H

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int command_show_tags();
int command_show_sorted_tags();
int command_show_sorted_directories();
int command_show_tags_and_directories();
int command_show_sorted_tags_and_directories();
int command_try_switch(char* tag);
int command_switch_to_previous();
int command_remove_tags(int argc, char* argv[]);

#endif // IO_GITHUB_CODERODDE_DSV2_COMMANDS_H
