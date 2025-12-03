#include "chip8.hpp"


int main()
{
    chip8 emulator;
    // ... (Caricamento ROM, ecc.)

    while (true) // Loop principale
    {
        // 1. Emulazione di più cicli
        // Esegui la logica del CHIP-8 (es. 10-15 istruzioni)
        emulator.emulate_cycle();
        // ...

        // 2. Rendering
        // Chiama la funzione di disegno nella console
        emulator.draw_to_console();

        // 3. Temporizzazione
        // È FONDAMENTALE ritardare il loop per non sovraccaricare la CPU
        // e per rendere il disegno visibile (es. 16ms per 60 FPS)
        // Per questo potresti usare std::this_thread::sleep_for (richiede <thread> e <chrono>)
        // std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    return 0;
}