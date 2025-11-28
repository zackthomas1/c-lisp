CXXFLAGS = -ggdb -std=c99 -Wall
main: main.c
	gcc $(CXXFLAGS) main.c mpc.c -o main