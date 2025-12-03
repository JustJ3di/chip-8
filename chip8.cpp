#include "chip8.hpp"

/**
 *array di 16 posizione per i livelli di operazione,  dispatch table
 */

chip8::chip8()
{

    pc = 0;
    opc = 0;
    sp = 0;
    index = 0;

    memset(V,0,NVREG);
    memset(stack,0,STACK_SIZE);
    memset(ram, 0,RAM_SIZE);

    operations[0x0] = std::bind(&chip8::handle_category_0, this, std::placeholders::_1); 

    operations[0x1] = std::bind(&chip8::op_1NNN_JP, this, std::placeholders::_1); 
    
    // Categoria 2: CALL addr (Chiamata a sottoprogramma) - Opcode semplice
    operations[0x2] = std::bind(&chip8::op_2NNN_CALL, this, std::placeholders::_1);
    
    // Categoria 3: SE Vx, byte (Salto se uguale) - Opcode semplice
    operations[0x3] = std::bind(&chip8::op_3XKK_SE, this, std::placeholders::_1);
    
    // Categoria 4: SNE Vx, byte (Salto se NON uguale) - Opcode semplice
    operations[0x4] = std::bind(&chip8::op_4XKK_SNE, this, std::placeholders::_1);
    
    // Categoria 5: SE Vx, Vy (Salto se registri uguali) - Opcode semplice
    operations[0x5] = std::bind(&chip8::op_5XY0_SE, this, std::placeholders::_1);
    
    // Categoria 6: LD Vx, byte (Carica un valore) - Opcode semplice
    operations[0x6] = std::bind(&chip8::op_6XKK_LD, this, std::placeholders::_1);
    
    // Categoria 7: ADD Vx, byte (Addizione) - Opcode semplice
    operations[0x7] = std::bind(&chip8::op_7XKK_ADD, this, std::placeholders::_1);

    // Categoria 8: Opcode complessi (Operazioni logiche/aritmetiche su registri)
    operations[0x8] = std::bind(&chip8::handle_category_8, this, std::placeholders::_1);

    // Categoria 9: SNE Vx, Vy (Salto se registri NON uguali) - Opcode semplice
    operations[0x9] = std::bind(&chip8::op_9XY0_SNE, this, std::placeholders::_1);

    // Categoria A (10): LD I, addr (Carica indirizzo) - Opcode semplice
    operations[0xA] = std::bind(&chip8::op_ANNN_LD_I, this, std::placeholders::_1);

    // Categoria B (11): JP V0, addr (Salto con offset) - Opcode semplice
    operations[0xB] = std::bind(&chip8::op_BNNN_JP_V0, this, std::placeholders::_1);

    // Categoria C (12): RND Vx, byte (Genera numero casuale) - Opcode semplice
    operations[0xC] = std::bind(&chip8::op_CXKK_RND, this, std::placeholders::_1);
    
    // Categoria D (13): DRW Vx, Vy, nibble (Disegna sprite) - Opcode semplice/complesso
    operations[0xD] = std::bind(&chip8::op_DXYN_DRW, this, std::placeholders::_1);

    // Categoria E (14): Opcode complessi (Input/tastiera)
    operations[0xE] = std::bind(&chip8::handle_category_E, this, std::placeholders::_1);

    // Categoria F (15): Opcode complessi (Varie)
    operations[0xF] = std::bind(&chip8::handle_category_F, this, std::placeholders::_1);
    
    
}
chip8::~chip8()
{
}


bool chip8::load(std::string rom)
{
    return false;
}
void chip8::handle_category_0(u_int16_t opc)
{
}

void chip8::handle_category_8(uint16_t opc)
{
        // Estrai i parametri comuni
    u_int8_t X = (opc >> 8) & 0xF; // Registro di destinazione/sorgente Vx
    u_int8_t Y = (opc >> 4) & 0xF; // Registro di sorgente Vy
    uint8_t F = 15;

    // Estrai la SOTTOCLASSE (l'ultimo nibble N)
    u_int8_t sub_opcode = opc & 0xF; 

    // Esegui il dispatch (Livello 2)
    switch (sub_opcode) {
        case 0x0: // 8XY0: LD Vx, Vy
            V[X] = V[Y];
            break;
        case 0x1: // 8XY1: OR Vx, Vy
            V[X] |= V[Y];
            break;
        case 0x2: // 8XY2: AND Vx, Vy
            V[X] &= V[Y];
            break;
        case 0x3: // 8XY3: XOR Vx, Vy
            V[X] ^= V[Y];
            break;
        case 0x4: {
            u_int16_t sum = V[X] + V[Y]; 
                if (sum > 255) {
                    V[F] = 1; // Overflow
                } else {
                    V[F] = 0; 
                }
                V[X] = sum & 0xFF; //trunked
            }
            break;
        case 0x5:{
                if (V[X] >= V[Y]) {
                        V[0xF] = 1; // No Borrow
                } else {
                    V[0xF] = 0; // Borrow
                }
                V[X] = V[X] - V[Y];
            }
            break;
        case 0x6: 
            V[0xF] = V[X] & 0x1; 
            V[X] >>= 1;
            break;
        case 0x7://dual of 0x6
        {
            if (V[Y] >= V[X]) {
                V[0xF] = 1; // No Borrow
            } else {
                V[0xF] = 0; // Borrow
            }
            V[X] = V[Y] - V[X]; 
            break;
        }
        case 0x8:
        break;
        case 0x9:
        break;
    
        // ... (tutti gli altri opcode 8XYN: SUB, SHR, SUBN, SHL)
        default:
            // Gestione di opcode non validi (opzionale)
            break;
    }
}

void chip8::handle_category_E(u_int16_t opc)
{
// Estrae l'indice del registro X (Vx)
    u_int8_t X = (opc >> 8) & 0xF; 
    
    // Estrae il byte della sottoclasse (KK o N)
    u_int8_t sub_opcode = opc & 0xFF; 

    // Il registro Vx contiene l'indice del tasto da controllare (0x0 a 0xF)
    u_int8_t key_index = V[X];

    switch (sub_opcode)
    {
        case 0x9E: // EX9E: SKP Vx (Skip if Key Pressed)
            // L'istruzione salta l'istruzione successiva (pc += 2) 
            // SE il tasto il cui indice è memorizzato in Vx è PREMUTO.
            
            // L'array keypad[KEYPAD_SIZE] contiene lo stato dei tasti (1 = premuto, 0 = rilasciato)
            if (keypad[key_index] == 1) 
            {
                pc += 2;
            }
            break;

        case 0xA1: // EXA1: SKNP Vx (Skip if Key NOT Pressed)
            // L'istruzione salta l'istruzione successiva (pc += 2) 
            // SE il tasto il cui indice è memorizzato in Vx NON è premuto.

            if (keypad[key_index] == 0) 
            {
                pc += 2;
            }
            break;

        default:
            // Gestione di opcode non validi nella categoria E
            // std::cerr << "Opcode EXXX sconosciuto: " << std::hex << opc << std::endl;
            break;
    }
}

void chip8::handle_category_F(u_int16_t opc)
{
}

void chip8::op_1NNN_JP(uint16_t opc)
{
    u_int16_t NNN = opc & 0x0FFF;
    pc = NNN;
}

void chip8::op_2NNN_CALL(u_int16_t opc)
{
    u_int16_t NNN = opc & 0x0FFF;
    sp++; 
    stack[sp] = pc; 
    pc = NNN;
}

void chip8::op_3XKK_SE(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 
    u_int8_t KK = opc & 0xFF;
    if (V[X] == KK)
    {
        pc += 2;
    }

}

void chip8::op_6XKK_LD(u_int16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F;     
    u_int8_t KK = opc & 0xFF;
    V[X] = KK;
}

void chip8::op_4XKK_SNE(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 

    u_int8_t KK = opc & 0xFF;
    if (V[X] != KK)
    {
        pc += 2;
    }
}

void chip8::op_5XY0_SE(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 
    u_int8_t Y = (opc >> 4) & 0x0F;
    if (V[X] == V[Y])
    {
        pc += 2;
    }
}

void chip8::op_7XKK_ADD(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 
    u_int8_t KK = opc & 0xFF;
    V[X] += KK;
}

void chip8::op_9XY0_SNE(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 
    u_int8_t Y = (opc >> 4) & 0x0F;
    if (V[X] != V[Y])
    {
        pc += 2;
    }
}

void chip8::op_ANNN_LD_I(uint16_t opc)
{
    u_int16_t NNN = opc & 0x0FFF;
    index = NNN;
}

void chip8::op_BNNN_JP_V0(uint16_t opc)
{
    u_int16_t NNN = opc & 0x0FFF;
    pc = NNN + V[0];
}

void chip8::op_CXKK_RND(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0x0F; 

    u_int8_t KK = opc & 0xFF;
    u_int8_t random_byte = (u_int8_t)(std::rand() % 256);

    V[X] = random_byte & KK;
}

void chip8::op_DXYN_DRW(u_int16_t opc)
{
 
    u_int8_t X = (opc >> 8) & 0x0F;       
    u_int8_t Y = (opc >> 4) & 0x0F;       
    u_int8_t height = opc & 0x0F;         

    //collision flag
    V[0xF] = 0;

    u_int8_t start_x = V[X] % 64; 
    u_int8_t start_y = V[Y] % 32;

  
    for (int y_line = 0; y_line < height; y_line++)
    {

        u_int8_t sprite_byte = ram[index + y_line];
        u_int8_t current_y = (start_y + y_line) % 32;
        

        if (current_y >= 32) break;


        for (int x_bit = 0; x_bit < 8; x_bit++)
        {
            u_int8_t sprite_pixel = (sprite_byte >> (7 - x_bit)) & 0x1;
            u_int8_t current_x = (start_x + x_bit) % 64;
            if (current_x >= 64) continue;

            
            int video_index = current_y * 64 + current_x;
            u_int32_t current_screen_pixel = video[video_index];


            if (sprite_pixel == 1)
            {
                if (current_screen_pixel == 1)
                {
                    V[0xF] = 1; 
                }

                video[video_index] ^= 1; 
                
            }
        }
    }
}


void chip8::emulate_cycle()
{

    // 1. FETCH
    this->opc = (ram[pc] << 8) | ram[pc + 1];

    // 2. INCREMENT PC 
    this->pc += 2; 

    // 3. DECODE & EXECUTE
    u_int8_t category = (opc >> 12) & 0xF; 

    operations[category](opc); 
    
    
    if(dl > 0) dl--;
    if (st > 0) st--;
}

