all: main.c
	gcc -Wall -ansi -pedantic -o mmn14-compiler *.c
clean:
	rm -f mmn14-compiler