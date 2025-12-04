#pragma once

#include <map>
#include <functional>
#include <string>
#include <cstring>
#include <array>
#include <cstdlib> 
#include <ctime>
#include <random>
#include <fstream>
#include <iostream>
#include <SDL2/SDL.h> 

#define RAM_SIZE 4096
#define STACK_SIZE 16
#define NVREG  16
#define KEYPAD_SIZE 16

#define DISPLAY_WIDTH 64 
#define DISPLAY_HEIGHT 32 
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT) 

using opcode = std::function<void(u_int16_t opcode)>;

class chip8
{
private:
    
    
    u_int8_t ram[RAM_SIZE]{}; //ram memory
    
    u_int16_t index{}; //index register
    u_int16_t pc{}; //program counter

    u_int16_t stack[STACK_SIZE]{}; //stack
    u_int8_t sp{}; //stack pointer
    
    u_int8_t dl{}; //delay timer
    u_int8_t st{}; //sound timer
    u_int8_t V[NVREG]{};//register
    u_int8_t rgi{};
    u_int16_t opc; //opcode
    u_int8_t keypad[KEYPAD_SIZE]{};
    
    std::array<opcode,16>operations;

    unsigned char chip8_fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };

    //  SDL
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    const int WINDOW_SCALE = 10; 
    const int WINDOW_WIDTH = DISPLAY_WIDTH * WINDOW_SCALE;
    const int WINDOW_HEIGHT = DISPLAY_HEIGHT * WINDOW_SCALE;
    
    void handle_category_0(u_int16_t opc);
    void handle_category_8(u_int16_t opc);
    void handle_category_E(u_int16_t opc);
    void handle_category_F(u_int16_t opc);
    void op_1NNN_JP(uint16_t opc);
    void op_2NNN_CALL(u_int16_t opc);
    void op_3XKK_SE(uint16_t opc);
    void op_6XKK_LD(u_int16_t opc); 
    void op_4XKK_SNE(uint16_t opc);
    void op_5XY0_SE(uint16_t opc);
    void op_7XKK_ADD(uint16_t opc);
    void op_9XY0_SNE(uint16_t opc);
    void op_ANNN_LD_I(uint16_t opc);
    void op_BNNN_JP_V0(uint16_t opc);
    void op_CXKK_RND(uint16_t opc);
    void op_DXYN_DRW(uint16_t opc);

public:
    u_int32_t video[DISPLAY_SIZE]{}; //video display

    chip8();
    ~chip8(); 

    bool load(std::string filename);
    void emulate_cycle(); //fetch decode execute
    
    //  SDL
    bool setup_sdl();
    void cleanup_sdl();
    void render_sdl();

    //  KEYPAD
    void key_down(u_int8_t key_index);
    void key_up(u_int8_t key_index);


};
