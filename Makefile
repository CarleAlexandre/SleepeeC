all:
	clang -std=c11 -ggdb source/*.c -pthread -o scc
