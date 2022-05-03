#ifndef ERROR_H
#define ERROR_H

/**
 * About two years ago, I tried to make a game engine from scratch. These
 * nice error printing macros are ripped from there. This is not the whole 
 * thing, but just what is required for this project.
 * Dalton Caron
 */

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ERROR_LOG(_f, ...) fprintf(stderr, ANSI_COLOR_RED "[Error]: " \
    ANSI_COLOR_RESET _f "\n", ##__VA_ARGS__);

#define ERROR_LOGV(_f, ...) fprintf(stderr, ANSI_COLOR_RED "[Error]: " \
    ANSI_COLOR_RESET _f " at line %d in %s\n", ##__VA_ARGS__, __LINE__, __FILE__);

#define WARNING_LOG(_f, ...) fprintf(stderr, ANSI_COLOR_YELLOW "[Warning]: " \
    ANSI_COLOR_RESET _f "\n", ##__VA_ARGS__);

#endif