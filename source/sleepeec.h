#ifndef SLEEPEEC_H
# define SLEEPEEC_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdatomic.h>
#include <pthread.h>
#include <direct.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "toml.h"

/*

[TARGET]
cc = "path"
lib = true
dynamic = true
name = ""
flag = "flag"
source = ["dir/src/\**"]
ignore = ["src/test/\*.c"]
std = "std=c99"

if enforce these flag are use :

-Wall -Wextra -Wpedantic -Wshadow -Wundef -Werror -Wno-unused-parameter -fstrict-aliasing -fno-strict-overflow -fwrapv

*/

#define MAX_PATH_LENGTH 0 //for security reason could set it to MAX_PATH
#define MAX_SOURCES 128
#define MAX_TARGET 64


typedef enum {
	invalid_stub,
	invalid_directory,
	invalid_argument,
	invalid_file_missing,
	invalid_thrd_count,
	invalid_file_bad,
}	error_enum;

typedef struct {
	bool	static_library;
	bool	dynamic_library;
	bool	strict_mod;
	char*	name;
	char*	compiler_path;
	char*	cflag;
	char*	lflag;
	char**	sources;
	size_t	source_size;
}	target_t;

typedef struct {
	bool		print_help;
	int		thrd_count;
	char*		directory;
	char*		path;
	error_enum	error;
}	context;

typedef struct {
	target_t*	data;
	size_t		size;
}	config_t;

#endif
