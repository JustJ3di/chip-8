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
    
    operations[0x2] = std::bind(&chip8::op_2NNN_CALL, this, std::placeholders::_1);
    
    operations[0x3] = std::bind(&chip8::op_3XKK_SE, this, std::placeholders::_1);

    operations[0x4] = std::bind(&chip8::op_4XKK_SNE, this, std::placeholders::_1);
    
    operations[0x5] = std::bind(&chip8::op_5XY0_SE, this, std::placeholders::_1);
    
    operations[0x6] = std::bind(&chip8::op_6XKK_LD, this, std::placeholders::_1);
    
    operations[0x7] = std::bind(&chip8::op_7XKK_ADD, this, std::placeholders::_1);

    operations[0x8] = std::bind(&chip8::handle_category_8, this, std::placeholders::_1);

    operations[0x9] = std::bind(&chip8::op_9XY0_SNE, this, std::placeholders::_1);

    operations[0xA] = std::bind(&chip8::op_ANNN_LD_I, this, std::placeholders::_1);

    operations[0xB] = std::bind(&chip8::op_BNNN_JP_V0, this, std::placeholders::_1);

    operations[0xC] = std::bind(&chip8::op_CXKK_RND, this, std::placeholders::_1);
    
    operations[0xD] = std::bind(&chip8::op_DXYN_DRW, this, std::placeholders::_1);

    operations[0xE] = std::bind(&chip8::handle_category_E, this, std::placeholders::_1);

    operations[0xF] = std::bind(&chip8::handle_category_F, this, std::placeholders::_1);
    
   
}



bool chip8::load(std::string rom)
{
    // 1. Apri il file ROM in modalità binaria e posiziona il cursore alla fine (ate)
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        // Ottieni la dimensione del file in byte
        std::streampos size = file.tellg();
        
        // Verifica che la ROM non sia troppo grande (max 3584 byte, da 0x200 a 0xFFF)
        if (size > (RAM_SIZE - 0x200)) {
            std::cerr << "ERRORE: La ROM è troppo grande per la memoria CHIP-8." << std::endl;
            file.close();
            return false;
        }

        // Ritorna all'inizio del file
        file.seekg(0, std::ios::beg);

        // 2. Leggi i dati nel buffer RAM a partire da 0x200
        // La RAM[0x200] punta all'inizio dell'area del programma
        if (file.read((char*)&ram[0x200], size))
        {
            // 3. Successo: Imposta il Program Counter (pc)
            pc = 0x200; 
            
            file.close();
            std::cout << "ROM '" << filename << "' caricata con successo (" << size << " byte)." << std::endl;
            return true;
        }
        else
        {
            std::cerr << "ERRORE: Impossibile leggere il file ROM." << std::endl;
            file.close();
            return false;
        }
    }
    else
    {
        std::cerr << "ERRORE: Impossibile aprire il file ROM '" << filename << "'." << std::endl;
        return false;
    }
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
    
        default:
            break;
    }
}

void chip8::handle_category_E(u_int16_t opc)
{
    u_int8_t X = (opc >> 8) & 0xF; 
        u_int8_t sub_opcode = opc & 0xFF; 


    u_int8_t key_index = V[X];

    switch (sub_opcode)
    {
        case 0x9E: 

            if (keypad[key_index] == 1) 
            {
                pc += 2;
            }
            break;

        case 0xA1: 

            if (keypad[key_index] == 0) 
            {
                pc += 2;
            }
            break;

        default:

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

void chip8::draw_to_console()
{
    // Pulisci il terminale prima di disegnare (dipendente dal sistema operativo)
    // Su Linux/macOS:
    system("clear"); 

    // Oppure stampare molte righe vuote.

    std::cout << "------------------------------------------------------------------" << std::endl;

    for (int y = 0; y < 32; y++) // 32 righe
    {
        std::cout << "|";
        for (int x = 0; x < 64; x++) // 64 colonne
        {
            // Indice nel buffer 1D: video[y * 64 + x]
            int index = y * 64 + x;

            if (video[index] == 1) 
            {
                // Pixel attivo (usa un carattere pieno o '█')
                std::cout << "█"; 
            }
            else 
            {
                // Pixel inattivo (usa uno spazio o '.')
                std::cout << " "; 
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "------------------------------------------------------------------" << std::endl;
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

