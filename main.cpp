#include "chip8.hpp"
#include <SDL2/SDL.h> 

u_int8_t map_sdl_key_to_chip8(SDL_Keycode key)
{
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC; //  C
        
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD; 
        
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE; //  E
        
        case SDLK_z: return 0xA; //  A
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB; //  B
        case SDLK_v: return 0xF; //  F
        
        default: return 0xFF; 
    }
}

int main(int argc, char const *argv[])
{   
    if (argc < 2) {
        std::cerr << "Utilizzo: " << argv[0] << " <nome_ROM_chip8>" << std::endl;
        return 1;
    }
    
    chip8 emulate;


    if (!emulate.setup_sdl()) {
        return 1;
    }


    if (!emulate.load(argv[1])) { 
        std::cerr << "Errore nel caricamento della ROM. Uscita." << std::endl;
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    u_int8_t chip8_key;
    
    while (!quit)
    {
      
        while (SDL_PollEvent(&e) != 0) {
            
            if (e.type == SDL_QUIT) {
                quit = true; 
            }
            
  
            if (e.type == SDL_KEYDOWN) {
                chip8_key = map_sdl_key_to_chip8(e.key.keysym.sym);
                
                if (chip8_key != 0xFF) {
                    emulate.key_down(chip8_key);
                }
            }
            

            if (e.type == SDL_KEYUP) {
                chip8_key = map_sdl_key_to_chip8(e.key.keysym.sym);
                
                if (chip8_key != 0xFF) {
                    emulate.key_up(chip8_key);
                }
            }
        }


        emulate.emulate_cycle();
        
  
        emulate.render_sdl();
        
        // test
        SDL_Delay(2); 
    }
    
    return 0;
}