#include "chip8.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

uint8_t fontset[80] = 
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct Chip8Obj {

	uint8_t memory[4096];
	uint8_t V[16];
	uint16_t index;
	uint16_t pc;
	uint16_t stack[16];
	uint8_t sp;
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t keypad[16];
	uint32_t screen[64 * 32];
	uint16_t opcode;
	bool drawFlag;

} Chip8Obj;

uint32_t getScreen(Chip8 chip, int x) {
	return chip->screen[x];
}

bool getFlag(Chip8 chip) {
	return chip->drawFlag;
}

void setFlag(Chip8 chip) {
	chip->drawFlag = false;
}

void setKey(Chip8 chip, int x, bool down) {
	if (down)
		chip->keypad[x] = 1;
	else 
		chip->keypad[x] = 0;
}

void debugRender(Chip8 chip) {
	for(int y = 0; y < 32; ++y) {
		for(int x = 0; x < 64; ++x) { 
			if(chip->screen[(y*64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

Chip8 newChip() {

	Chip8 chip = calloc(1, sizeof(Chip8Obj));
	chip->opcode = 0;
	chip->pc = 0x200;
	chip->index = 0;
	chip->sp = 0;
	chip->drawFlag = false;

	for (int i = 0; i < 64 * 32; i++) {
		chip->screen[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
		chip->stack[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
		chip->V[i] = 0;
	}
	for (int i = 0; i < 4096; i++) {
		chip->memory[i] = 0;
	}
	for (int i = 0; i < 80; i++) {
		chip->memory [i] = fontset[i];
	}

	return chip;

}

void freeChip(Chip8 *chip) {
	if (chip != NULL && *chip != NULL) {
		free(*chip);
		*chip = NULL;
	}
}

void loadROM(Chip8 chip, char* filename) {

	printf("Loading ROM...\n");

	FILE* in = fopen(filename, "rb");
	if (in == NULL) {
		printf("Failed opening %s\n", filename);
		exit(1);
	}

	fseek(in, 0, SEEK_END);
	int file_size = ftell(in);
	uint8_t* buffer = malloc(file_size);
	rewind(in);
	size_t res = fread(buffer, 1, file_size, in);
	assert((int)res == file_size);


	for (int i = 0; i < file_size; ++i) {
		chip->memory[i + 512] = buffer[i];
		// fprintf(stderr, "%d ", buffer[i]);
	}

	free(buffer);
	printf("Loaded ROM successfully!\n");

}

void emulateCycle(Chip8 chip) {
	chip->opcode = (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
	uint8_t X = (chip->opcode & 0x0F00) >> 8;
	uint8_t Y = (chip->opcode & 0x00F0) >> 4;
	uint8_t N = chip->opcode & 0x000F;
	uint8_t NN = chip->opcode & 0x00FF;
	uint16_t NNN = chip->opcode & 0x0FFF;

	if ((chip->opcode & 0xF000) != 0x1000)
		printf("opcode called: 0x%X\n", chip->opcode);
 	
	switch(chip->opcode & 0xF000) {
		case 0x0000:
			switch (N) {
				case 0x0000:
					for(int i = 0; i < 2048; ++i) {
						chip->screen[i] = 0;
					}
					chip->drawFlag = true;
					chip->pc += 2;
					break;
				case 0x000E:
					--chip->sp;
					chip->pc = chip->stack[chip->sp];
					chip->pc += 2;
					break;

				default:
					printf ("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);   
					chip->pc += 2;
			}
			break;
		case 0x1000:
			chip->pc = NNN;
			break;
		case 0x2000:
			chip->stack[chip->sp] = chip->pc;
			++chip->sp;
			chip->pc = NNN;
			break;
		case 0x3000:
			if (chip->V[X] == NN) 
				chip->pc += 2;
			chip->pc += 2;
			break;
		case 0x4000:
			if (chip->V[X] != NN)
				chip->pc += 2;
			chip->pc += 2;
		case 0x5000:
			if (chip->V[X] == chip->V[Y]) 
				chip->pc += 2;
			chip->pc += 2;
		case 0x6000:
			chip->V[X] = NN;
			chip->pc += 2;
			break;
		case 0x7000:
			chip->V[X] += NN;
			chip->pc += 2;
			break;
		case 0x8000:
			switch (N) {
				case 0x0000:
					chip->V[X] = chip->V[Y];
					chip->pc += 2;
					break;
				case 0x0001:
					chip->V[X] |= chip->V[Y];
					chip->pc += 2;
					break;
				case 0x0002:
					chip->V[X] &= chip->V[Y];
					chip ->pc += 2;
					break;
				case 0x0003:
					chip->V[X] ^= chip->V[Y];
					chip->pc += 2;
					break;
				case 0x0004:       
					if (chip->V[Y] > (0xFF - chip->V[X]))
						chip->V[0xF] = 1;
 	 				else
						chip->V[0xF] = 0;
					chip->V[X] += chip->V[Y];
 	 				chip->pc += 2;          
					break;
				case 0x0005:
					if (chip->V[Y] > chip->V[X]) 
						chip->V[0xF] = 1;
					else 
						chip->V[0xF] = 0;
					chip->V[X] -= chip->V[Y];
					chip->pc += 2;
					break;
				case 0x0006:
					chip->V[0xF] = chip->V[X] & 0x1;
					chip->V[X] >>= 1;
					chip->pc += 2;
					break;
				case 0x0007:
					if (chip->V[X] > chip->V[Y]) 
						chip->V[0xF] = 1;
					else 
						chip->V[0xF] = 0;
					chip->V[X] = chip->V[Y] - chip->V[X];
					chip->pc += 2;
					break;
				case 0x000E:
					chip->V[0xF] = chip->V[X] >> 7;
					chip->V[X] <<= 1;
					chip->pc += 2;
					break;
				default:
					printf ("Unknown opcode [0x0000]: 0x%X\n", chip->opcode);
					chip->pc += 2;
			}
			break;
		case 0x9000:
			if (chip->V[X] != chip->V[Y])
				chip->pc += 2;
			chip->pc += 2;
		case 0xA000:
			chip->index = NNN;
			chip->pc += 2;
			break;
		case 0xD000:
		{
			uint8_t x = (chip->V[X]);
			uint8_t y = (chip->V[Y]);
			uint8_t height = N;
			uint8_t pixel;

			// printf("x: %d y: %d height: %d\n", x, y, height);

			chip->V[0xF] = 0;
			for (int yline = 0; yline < height; yline++) {
				pixel = chip->memory[chip->index + yline];
				for (int xline = 0; xline < 8; xline++) {
					if (((pixel & (0x80 >> xline))) != 0) {
						if (chip->screen[(x + xline + ((y + yline) * 64))] == 1)
							chip->V[0xF] = 1;
						chip->screen[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
			chip->drawFlag = true;
			chip->pc += 2;
			break;
		}
		case 0xE000:
			switch(NN) {
				case 0x009E:
					if (chip->keypad[chip->V[X]] == 0)
						chip->pc += 2;
					chip->pc += 2;
					break;
				case 0x00A1:
					if (chip->keypad[chip->V[X]] != 0)
						chip->pc += 2;
					chip->pc += 2;

					break;
				default:
					chip->pc += 2;
					printf ("Unknown opcode: 0x%X\n", chip->opcode);	
			}
			break;
		case 0xF000:
			switch (NN) {
				case 0x0007:
					chip->V[X] = chip->delay_timer;
					chip->pc += 2;
					break;
				case 0x0033:
					chip->memory[chip->index] = chip->V[X] / 100;
					chip->memory[chip->index + 1] = (chip->V[X] / 10) % 10;
					chip->memory[chip->index + 2] = (chip->V[X] % 100) % 10;
					chip->pc += 2;
					break;
				case 0x001A: {
					bool pressed = false;
					for (int i = 0; i < 16; i++) {
						if (chip->keypad[i] != 0) {
							chip->V[X] = i;
							pressed = true;
						}
					}
					if (!pressed)
						return;
					chip->pc += 2;
					break;
					}
				case 0x001E:
					chip->index += chip->V[X];
					chip->pc += 2;
					break;
				case 0x0015:
					chip->delay_timer = chip->V[X];
					chip->pc += 2;
					break;
				case 0x0018:
					chip->sound_timer = chip->V[X];
					chip->pc += 2;
					break;
				default:
					chip->pc += 2;
					printf ("Unknown opcode: 0x%X\n", chip->opcode);	
			}
			break;
		default:
			chip->pc += 2;
			printf ("Unknown opcode: 0x%X\n", chip->opcode);	
	}    
	if (chip->delay_timer > 0)
		--chip->delay_timer;

	if(chip->sound_timer > 0) {
		if(chip->sound_timer == 1)
			printf("BEEP!\n");
		--chip->sound_timer;
	}
}
