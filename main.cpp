#include "chip8.hpp"


int main()
{
    chip8 emulator;

    while (true) //HIL
    {

        emulator.emulate_cycle();


        emulator.draw_to_console();

    }
    return 0;
}