#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Chip8Obj* Chip8;

Chip8 newChip();
void freeChip(Chip8 *chip);
void loadROM(Chip8 chip, char* filename);
void emulateCycle(Chip8 chip);
uint32_t getScreen(Chip8 chip, int x);
bool getFlag(Chip8 chip);
void setFlag(Chip8 chip);

#endif
