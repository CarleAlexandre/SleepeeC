//#include "toml.hpp"
#include <assert.h>
#include <stdlib.h>
#include <direct.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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

-Wall -Wextra -Wpedantic -Wshadow -Wundef -Werror -Wno-unused-parameter
-fstrict-aliasing -fno-strict-overflow
-fwrapv

*/

#define MAX_PATH_LENGTH 0 //for security reason could set it to MAX_PATH

typedef enum {
	invalid_stub,
	invalid_directory,
	invalid_argument,
	invalid_file_missing,
	invalid_thrd_count,
	invalid_file_bad,
}	error_enum;

typedef struct {
	bool	library;
	bool	dynamic;
	bool	enforce_secure;
	char*	target_name;
	char*	compiler_path;
	char*	calling_name;
	char*	flag;
	char*	source;
	char*	ignore;
}	target;

typedef struct {
	bool		print_help;
	bool		verbose;
	char*		logfile;
	int		thrd_count;
	char*		directory;
	char*		path;
	error_enum	error;
}	context;

void	print_helper() {
	printf("-h show this output\n"\
	"-v print verbos\n"\
	"-l [filename] log output to file\n"\
	"-t [value] number of thread used (can slow down if too high for cpu)\n"\
	"-d [directory_name] exec sleepeebuild.toml in directory\n"\
	"-p [path] look for sleepeebuild.toml at path and execute in working directory.\n");
}

/**
 * @brief
 *
 * 	if 0 is returned then c is not an option
 * 	else it return idx from 1 to sizeof(str) + 1;
 *
 * @param str char*
 * @param c char
 * @return uint8_t index of char c in array str
 */
uint8_t	cstridx(const char* str, char c) {
	char* tmp = 0x00;

	tmp = strchr(str, c);
	if (!tmp) {
		return (0);
	}
	uint8_t idx = tmp - str;
	return (idx + 1);
}

context*	initialize_context(int argc, char** argv) {
	context*	ctx = 0x00;
	ctx = (context*)calloc(sizeof(context), 1);
	assert(ctx);

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			int idx = 0;
			if (!argv[i][1]) {
				goto context_error;
			}
			idx = cstridx("hdlptv", argv[i][1]);
			if (idx > 1 && idx < 6) {
				if (i + 1 > argc) {
					goto context_error;
				}
				i++;
			}
			switch (idx) {
				case (2): {
					ctx->directory = strdup(argv[i]);
					break;
				}
				case (3): {
					ctx->logfile = strdup(argv[i]);
					break;
				}
				case (4): {
					ctx->path = strdup(argv[i]);
					break;
				}
				case (5): {
					ctx->thrd_count = atoi(argv[i]);
					if (ctx->thrd_count < 1) {
						goto context_error;
						ctx->error = invalid_thrd_count;
					}
					break;
				}
				case (6): {
					ctx->verbose = true;
					break;
				}
				case (1): {
					ctx->print_help = true;
					return (ctx);
					break;
				}
				default: {
					context_error:
					ctx->print_help = true;
					if (!ctx->error) {
						ctx->error = invalid_argument;
					}
					return (ctx);
				}
			}
		}
	}
	return (ctx);
}

int	main(int argc, char** argv) {
	context* ctx = 0x00;
	ctx = initialize_context(argc, argv);

	printf(" dir: %s\n log: %s\n path: %s\n thrd:%i\n help: %i\n v: %i\n error: %i\n", ctx->directory, ctx->logfile,  ctx->path, ctx->thrd_count, ctx->print_help, ctx->verbose, ctx->error);

	if (ctx->print_help) {
		goto help;
	}

	//get arg if after - {char} != "hvltdp" print -h
	//if -h present then skip all other argument
	//else look into working directory
	//if no file then create template file

	//then load file into value structure for each target

	return (0);

help:

	print_helper();

	return (0);
}
