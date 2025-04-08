//#include "toml.hpp"
#include <cassert>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <direct.h>
#include <fstream>
#include <cstring>

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

void	*load_targets() {
	std::fstream file;
	void* smthg = 0x00;

	char* dir = getcwd(0x00, MAX_PATH_LENGTH);
	assert(dir);

	file = std::fstream("/SleepeeBuild.toml");

	if (file.bad()) {
		//dir"/SleepeeBuild.toml"
		return (0x00);

	}
	free(dir);
	return (smthg);
}

void	print_helper() {
	printf("-h show this output\n"\
	"-v print verbos\n"\
	"-l [filename] log output to file\n"\
	"-t [value] number of thread used (can slow down if too high for cpu)\n"\
	"-d [directory_name] look for sleepeebuild.toml in directory\n"\
	"-p [path] look for sleepeebuild.toml at path and execute in working directory.\n");
}

void	create_template(char* filename) {
	std::fstream file;

	//file = std::fstream(filename);
	file.open(filename, std::fstream::out);
	assert(file.good());


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
uint8_t	cstridx(char* str, char c) {
	char* tmp = 0x00;

	tmp = strchr(&str[0], c);
	if (!tmp) {
		return (0);
	}
	uint8_t idx = tmp - &str[0];
	return (idx + 1);
}

int	main(int ac, char **av) {
	std::thread::hardware_concurrency();

	if (ac > 1) {

		switch (cstridx("dhlptv", 'h')) {
			case (1): {
				break;
			}
			case (3): {
				break;
			}
			case (4): {
				break;
			}
			case (5): {
				break;
			}
			case (6): {
				break;
			}
			default: {
				break;
				goto help;
			}
		}
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
