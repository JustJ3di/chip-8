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
}

void chip8::handle_category_F(u_int16_t opc)
{
}

void chip8::op_1NNN_JP(uint16_t opc)
{
}

void chip8::op_2NNN_CALL(u_int16_t opc)
{
}

void chip8::op_3XKK_SE(uint16_t opc)
{
}

void chip8::op_6XKK_LD(u_int16_t opc)
{
}

void chip8::op_4XKK_SNE(uint16_t opc)
{
}

void chip8::op_5XY0_SE(uint16_t opc)
{
}

void chip8::op_7XKK_ADD(uint16_t opc)
{
}

void chip8::op_9XY0_SNE(uint16_t opc)
{
}

void chip8::op_ANNN_LD_I(uint16_t opc)
{
}

void chip8::op_BNNN_JP_V0(uint16_t opc)
{
}

void chip8::op_CXKK_RND(uint16_t opc)
{
}

void chip8::op_DXYN_DRW(uint16_t opc)
{
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

