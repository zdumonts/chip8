// TODO: add last couple of opcodes, fix clock (stuttering), add sound

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

void process_input(Chip8 chip);
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
		process_input(chip);
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

// TODO: fix massive switch

void process_input(Chip8 chip) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
                	case SDLK_ESCAPE:
                    	running = false;
                		break;
					case SDLK_1:
						setKey(chip, 0x1, true);
						break;
					case SDLK_2:
						setKey(chip, 0x2, true);
						break;
					case SDLK_3:
						setKey(chip, 0x3, true);
						break;
					case SDLK_4:
						setKey(chip, 0xC, true);
						break;
					case SDLK_q:
						setKey(chip, 0x4, true);
						break;
					case SDLK_w:
						setKey(chip, 0x5, true);
						break;
					case SDLK_e:
						setKey(chip, 0x6, true);
						break;
					case SDLK_r:
						setKey(chip, 0xD, true);
						break;
					case SDLK_a:
						setKey(chip, 0x7, true);
						break;
					case SDLK_s:
						setKey(chip, 0x8, true);
						break;
					case SDLK_d:
						setKey(chip, 0x9, true);
						break;
					case SDLK_f:
						setKey(chip, 0xE, true);
						break;
					case SDLK_z:
						setKey(chip, 0xA, true);
						break;
					case SDLK_x:
						setKey(chip, 0x0, true);
						break;
					case SDLK_c:
						setKey(chip, 0xB, true);
						break;
					case SDLK_v:
						setKey(chip, 0xF, true);
					default:
						running = false;
				}
				break;
            case SDL_KEYUP:
				switch (event.key.keysym.sym) {
                	case SDLK_ESCAPE:
                    	running = false;
                		break;
					case SDLK_1:
						setKey(chip, 0x1, false);
						break;
					case SDLK_2:
						setKey(chip, 0x2, false);
						break;
					case SDLK_3:
						setKey(chip, 0x3, false);
						break;
					case SDLK_4:
						setKey(chip, 0xC, false);
						break;
					case SDLK_q:
						setKey(chip, 0x4, false);
						break;
					case SDLK_w:
						setKey(chip, 0x5, false);
						break;
					case SDLK_e:
						setKey(chip, 0x6, false);
						break;
					case SDLK_r:
						setKey(chip, 0xD, false);
						break;
					case SDLK_a:
						setKey(chip, 0x7, false);
						break;
					case SDLK_s:
						setKey(chip, 0x8, false);
						break;
					case SDLK_d:
						setKey(chip, 0x9, false);
						break;
					case SDLK_f:
						setKey(chip, 0xE, false);
						break;
					case SDLK_z:
						setKey(chip, 0xA, false);
						break;
					case SDLK_x:
						setKey(chip, 0x0, false);
						break;
					case SDLK_c:
						setKey(chip, 0xB, false);
						break;
					case SDLK_v:
						setKey(chip, 0xF, false);
					default:
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

