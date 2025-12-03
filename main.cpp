#include "chip8.hpp"

int main(int argc, char const *argv[])
{   
    chip8 emulate;

    while (true)
    {
        emulate.load(argv[2]);
        emulate.emulate_cycle();
        emulate.draw_to_console();
    }
    

    
    return 0;
}
