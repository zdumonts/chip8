myChip8: main.o 
	gcc -std=c17 -Wall -lSDL2 -g -o myChip8 main.o

main.o: main.c
	gcc -std=c17 -Wall -g -c main.c

clean:
	rm -f myChip8 main.o
