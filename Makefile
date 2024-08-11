all: main.c
	gcc -Wall -ansi -pedantic -o mmn14 *.c
clean:
	rm -f mmn14