CC=gcc 
CFLAGS=-Wall -Wextra -Werror

simgrep: main.o
	gcc -o simgrep main.o

main.o: main.c 
	gcc -c main.c