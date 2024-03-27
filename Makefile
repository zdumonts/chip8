myChip8: main.o chip8.o 
	gcc -std=c17 -Wall -lSDL2 -g -o myChip8 main.o chip8.o

main.o: main.c chip8.h
	gcc -std=c17 -Wall -g -c main.c

chip8.o: chip8.c chip8.h
	gcc -std=c17 -Wall -g -c chip8.c

clean:
	rm -f myChip8 main.o chip8.o
