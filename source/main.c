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
//#include "toml.h"

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

typedef struct {
	char**	argument;
	uint8_t	size;
	uint8_t	max_thread;
}	cmds;

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

void	open_log(char* logname) {
	FILE* file = 0x00;

	file = fopen("logname", "w");
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
	printf(" dir: %s\n log: %s\n path: %s\n thrd:%i\n help: %i\n v: %i\n error: %i\n", ctx->directory, ctx->logfile,  ctx->path, ctx->thrd_count, ctx->print_help, ctx->verbose, ctx->error);
}

void	error_handler(error_enum ctx_error) {

}

void*	cmd_thread(void* arg) {
	system(arg);
	return (0x00);
}

void	cmds_run(cmds cmd) {
	pthread_t	*threads;
	uint32_t	current_nthread = 0;
	threads = malloc(sizeof(pthread_t) * cmd.max_thread);
	assert(threads);

	for (int i = 0; i < cmd.size;) {
		if (current_nthread < cmd.max_thread) {
			pthread_create(&threads[i], 0x00, cmd_thread, cmd.argument[i]);
			current_nthread++;
			i++;
		} else {
			uint32_t joined = 0;
			for (int k = 0; k < cmd.max_thread; k++) {
				pthread_join(threads[k], 0x00);
			}
			current_nthread = 0;
		}
	}
}

void validate_context(context* ctx) {
	if (!ctx->path) {
		//check if paht is set else set to default build file then execture in current working directory
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

int	main(int argc, char** argv) {
	context* ctx = 0x00;
	int errfd = STDERR_FILENO;
	int outfd = STDOUT_FILENO;
	int stream = 0;
	FILE* logfile = 0x00;
	FILE* null_err = 0x00;
	FILE* null_out = 0x00;

	if (argc == 1) {
		return (0);
	}
	ctx = initialize_context(argc, argv);
	validate_context(ctx);
	if (ctx->print_help || ctx->error) {
		print_helper();
		int err = ctx->error;
		return (err);
	}

	if (ctx->logfile) {
		//create a logfile named logfile in working directory
		logfile = fopen(ctx->logfile, "w+");
		if (errno) {
			perror("couldn't use logfile");
		}
		assert(logfile);
		stream = fileno(logfile);
		assert(stream);
		errfd = dup2(STDERR_FILENO, stream);
		outfd = dup2(STDOUT_FILENO, stream);
	} else if (!ctx->verbose) {
			//set current stdout and stderr to no opened
#ifdef _WIN32
		null_err = freopen("NUL", "w", stderr);
		if (errno) {
			perror("error");
		}
		assert(null_err);
		null_out = freopen("NUL", "w", stdout);
		if (errno) {
			perror("error");
			fclose(null_err);
		}
		assert(null_out);
#else
	#ifdef _unix_
		null_err = freopen("/dev/null", "w", stderr);
		if (errno) {
			perror("error");
		}
		assert(null_err);
		null_out = freopen("/dev/null", "w", stdout);
		if (errno) {
			perror("error");
			fclose(null_err);
		}
		assert(null_out);
	#endif
#endif
	}



	FILE* file = 0x00;
	file = fopen(ctx->path, "r");
	if (errno) {
		perror("Error: file could not be opened");
	}//maybe create dummy file
	assert(file);

	//now parse file and create cmd struct

	fclose(file);
	if (ctx->logfile) {
		dup2(STDERR_FILENO, errfd);
		dup2(STDOUT_FILENO, outfd);
		close(stream);
		fclose(logfile);
	} else if (!ctx->verbose) {
		fclose(null_err);
		fclose(null_out);
	}
	//destroy ctx stucture memeber and cmd structure
	return (0);
}
