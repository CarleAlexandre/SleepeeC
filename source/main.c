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

-Wall -Wextra -Wpedantic -Wshadow -Wundef -Werror -Wno-unused-parameter
-fstrict-aliasing -fno-strict-overflow
-fwrapv

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
	char*	target_name;
	char*	compiler_path;
	char*	calling_name;
	char*	flag;
	char*	sources[MAX_SOURCES];
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
	size_t		size
}	config_t;

void	print_helper() {
	printf("-h show this output\n"\
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
			idx = cstridx("hdpt", argv[i][1]);
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
					ctx->path = strdup(argv[i]);
					break;
				}
				case (4): {
					ctx->thrd_count = atoi(argv[i]);
					if (ctx->thrd_count < 1) {
						goto context_error;
						ctx->error = invalid_thrd_count;
					}
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

char*	file_load(char* filename) {
	assert(filename);
	FILE *file = 0x00;

	file = fopen(filename, "r");
	assert(!ferror(file));

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* text = 0x00;
	text = malloc(size);
	assert(text);

	unsigned int count = fread(text, sizeof(char), size, file);
	if (count < size) {
		text = realloc(text, count + 1);
		assert(text);
	}
	text[count] = '\0';
	fclose(file);
	return (text);
}

int	directory_ok(char* dirname) {
	if (!dirname) {
		return (0);
	}

	DIR* dir = 0x00;
	dir = opendir(dirname);
	if (!dir) {
		perror("couldn't open dir");
		return (-1);
	}
	return (1);
}

void	print_ctx(context* ctx) {
	printf(" dir: %s\n path: %s\n thrd:%i\n help: %i\n error: %i\n", ctx->directory,  ctx->path, ctx->thrd_count, ctx->print_help, ctx->error);
}

void*	cmd_thread(void* arg) {
	system(arg);
	return (0x00);
}

void	cmds_run(char** cmds, size_t ncmd) {
	const uint16_t	max_concurrency = 8;
	pthread_t	threads[max_concurrency];
	uint32_t	current_nthread = 0;
	assert(threads);

	for (int i = 0; i < ncmd;) {
		if (current_nthread < max_concurrency) {
			pthread_create(&threads[i], 0x00, cmd_thread, cmds[i]);
			current_nthread++;
			i++;
		} else {
			uint32_t joined = 0;
			for (int k = 0; k < max_concurrency; k++) {
				pthread_join(threads[k], 0x00);
			}
			current_nthread = 0;
		}
	}
}

void validate_context(context* ctx) {
	if (!ctx->path) {
		//check if path is set else set to default build file then execture in current working directory
		ctx->path = strdup("./sleepeec.toml");
	}
	switch (directory_ok(ctx->directory)) {
		case (-1): {
			ctx->error = invalid_directory;
			break;
		}
		case (1): {
			chdir(ctx->directory);
		}
		default:break;
	}
	if (ctx->thrd_count) {
		if (ctx->thrd_count >= PTHREAD_THREADS_MAX) {
			printf("Thread count is highet than maximum thread count");
			ctx->error = invalid_thrd_count;
		}
	} else {
		ctx->thrd_count = 4;
	}
}

config_t	lexer(char* data) {
	unsigned int table_size = 0;
	toml_table_t *root = toml_parse(data, 0x00, 0);
	assert(root);
	table_size = toml_table_ntab(root);
	//need to return the config to then execute and clear it
	target_t* config = 0x00;
	config = malloc(sizeof(target_t) * table_size);

	assert(config);

	for (int i = 0; i < table_size; i++) {
		const char* key = toml_key_in(root, i);
		if (!key) {
			break;
		}
		// name = key
		config[i].target_name = strdup(key);
		toml_table_t *target = toml_table_in(root, key);
		toml_datum_t cc = toml_string_in(target, "compiler");
		if (cc.ok) {
			config[i].compiler_path = cc.u.s;
		}
		toml_datum_t flag = toml_string_in(target, "flags");
		if (flag.ok) {
			config[i].flag = flag.u.s;
		}
		toml_datum_t static_lib = toml_bool_in(target, "static_lib");
		if (static_lib.ok) {
			config[i].static_library = static_lib.u.b;
		}
		toml_datum_t dynamic_lib = toml_bool_in(target, "dynamic_lib");
		if (dynamic_lib.ok) {
			config[i].dynamic_library = dynamic_lib.u.b;
		}
		toml_datum_t strict_mod = toml_bool_in(target, "strict_mod");
		if (strict_mod.ok) {
			config[i].strict_mod = strict_mod.u.b;
		}
		toml_array_t *sources = toml_array_in(target, "sources");
		assert(sources);
		for (int k = 0; ;k++) {
			toml_datum_t src_file = toml_string_at(sources, k);
			if (!src_file.ok) {
				break;
			}
			//add source path to sources
		}
		toml_array_t *excludes = toml_array_in(target, "excludes");
		assert(excludes);
		for (int k = 0; ;k++) {
			toml_datum_t exclude_file = toml_string_at(excludes, k);
			if (!exclude_file.ok) {
				break;
			}
			//exclude path from sources
		}
	}
	config_t ret = {
		.data = config,
		.size = table_size,
	};
	toml_free(root);
	return (ret);
}

int	main(int argc, char** argv) {
	context*	ctx = 0x00;

	if (argc == 1) {
		return (1);
	}
	ctx = initialize_context(argc, argv);
	validate_context(ctx);
	if (ctx->print_help || ctx->error) {
		print_helper();
		int err = ctx->error;
		return (err);
	}
	char* data = 0x00;
	data = file_load(ctx->path);
	assert(data);
	config_t	config = lexer(data);
	char**		cmds = 0x00;
	// need to make cmd for all cmd and if a cmd need other to finish before executing so i need to rethink this part
	for (int i = 0; i < config.size; i++) {
		//append cmd to cmds;
	}
	cmds_run(cmds, 0);
	free(config.data);
	//destroy ctx stucture member and cmd structure
	return (0);
}
