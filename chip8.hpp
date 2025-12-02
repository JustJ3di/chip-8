#pragma once

#include <map>
#include <functional>
#include <string>
#include <cstring>
#include <array>

#define RAM_SIZE 4096
#define STACK_SIZE 16
#define NVREG  16
#define KEYPAD_SIZE 16
#define DISPLAY_SIZE 2048 //16*32

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

    u_int32_t video[DISPLAY_SIZE]{}; //video display
    u_int8_t keypad[KEYPAD_SIZE]{};
    
    std::array<opcode,16>operations;

    void handle_category_8(uint16_t opc);

public:


    chip8();
    ~chip8();

    bool load(std::string);
    void emulate_cycle(); //fetch decode execute

    void hil()//hardware in the loop


};
