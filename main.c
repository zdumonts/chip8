#include "SDL2/SDL.h"
#include "chip8.h"
#include <stdio.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const int SCALE = 10;
int running = true;

SDL_Window* window;
SDL_Renderer* renderer;

void process_input();
void render(Chip8 chip);

#define DEBUG

int main(int argv, char** argc) {

	if (argv != 2) {
		printf("Usage: ./myChip8 <ROM file>\n");
		exit(1);
	}

	// Init SDL

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"Chip8", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		0
	);

	if(!window){
		printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if(!renderer){
		printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
		return 1;
	}

	running = true;

	Chip8 chip = newChip();

	loadROM(chip, argc[1]);

	while (running) {
		emulateCycle(chip);
		process_input();
		if (getFlag(chip)) 
			render(chip);
		SDL_Delay(1);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	freeChip(&chip);
	return 0;
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                break;
        }
    }
}

void render(Chip8 chip) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

	// Draw stuff

	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 32; y++) {
			if (getScreen(chip, y*64 + x) == 0) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			SDL_Rect rect;
			rect.x = x * SCALE;
			rect.y = y * SCALE;
			rect.w = SCALE;
			rect.h = SCALE;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	setFlag(chip);

	//ifndef DEBUG
	//debugRender(chip);
	//endif

	// Swap buffer

    SDL_RenderPresent(renderer);
}

