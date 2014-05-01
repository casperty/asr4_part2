CC=gcc
CFLAGS = -Wall -Werror -g -pedantic
main: main.o shell.o ligne_commande.o
	 $(CC) -o $@ $^
clear: 
	 rm main *.o