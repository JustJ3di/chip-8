#include "chip8.hpp"

/**
 *array di 16 posizione per i livelli di operazione,  dispatch table
 */

chip8::chip8()
{

    //pc = 0;
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
chip8::~chip8() 
{
    cleanup_sdl(); 
}

// ----------------------  SDL ----------------------

bool chip8::setup_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL non si è inizializzato! Errore: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr) {
        std::cerr << "La finestra non è stata creata! Errore: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Il Renderer non è stato creato! Errore: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING,
        DISPLAY_WIDTH, DISPLAY_HEIGHT
    );
    if (texture == nullptr) {
        std::cerr << "La Texture non è stata creata! Errore: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    return true;
}

void chip8::cleanup_sdl()
{
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void chip8::render_sdl()
{
    u_int32_t pixels[DISPLAY_SIZE]; 
    
    for (int i = 0; i < DISPLAY_SIZE; ++i) {
        //mapping
        if (video[i] == 1) {
             pixels[i] = 0xFFFFFFFF; 
        } else {
             pixels[i] = 0xFF000000; 
        }
    }

    SDL_UpdateTexture(
        texture, 
        NULL, 
        pixels, 
        DISPLAY_WIDTH * sizeof(u_int32_t) 
    );
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}



void chip8::key_down(u_int8_t key_index)
{
    if (key_index < KEYPAD_SIZE) { 
        keypad[key_index] = 1; // 1 = Tasto premuto
    }
}

void chip8::key_up(u_int8_t key_index)
{
    if (key_index < KEYPAD_SIZE) {
        keypad[key_index] = 0; // 0 = Tasto rilasciato
    }
}


//load file
bool chip8::load(std::string rom)
{

    std::ifstream file(rom, std::ios::binary | std::ios::ate); 

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        

        if (size > (RAM_SIZE - 0x200)) {
            std::cerr << "ERRORE: La ROM è troppo grande per la memoria CHIP-8." << std::endl;
            file.close();
            return false;
        }
        file.seekg(0, std::ios::beg);

        if (file.read((char*)&ram[0x200], size))
        {
            pc = 0x200; 
            
            file.close();
            std::cout << "ROM '" << rom << "' caricata con successo (" << size << " byte)." << std::endl; 
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
        std::cerr << "ERRORE: Impossibile aprire il file ROM '" << rom << "'." << std::endl; 
        return false;
    }
}
void chip8::handle_category_0(u_int16_t opc)
{
 
    u_int16_t sub_opcode = opc & 0x0FFF;
    switch (sub_opcode)
    {
        case 0x0E0:

            for (int i = 0; i < DISPLAY_SIZE; ++i) {
                video[i] = 0;
            }

            break;

        case 0x0EE: 

            pc = stack[sp];
            

            sp--;
            break;

        default:

            break;
    }
}

void chip8::handle_category_8(uint16_t opc)
{
    u_int8_t X = (opc >> 8) & 0xF; 
    u_int8_t Y = (opc >> 4) & 0xF; 
    uint8_t F = 15;

    
    u_int8_t sub_opcode = opc & 0xF; 

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
    u_int8_t X = (opc >> 8) & 0xF; 
    u_int8_t sub_opcode = opc & 0xFF; // Estrae gli ultimi due byte (KK)

    switch (sub_opcode)
    {
        case 0x07: // FX07: LD Vx, DT (Set Vx = delay timer value.)
            V[X] = dl;
            break;

        case 0x0A: // FX0A: LD Vx, K (Wait for a key press, store the value of the key in Vx.)
//TODO
            break; 

        case 0x15: // FX15: LD DT, Vx (Set delay timer = Vx.)
            dl = V[X];
            break;

        case 0x18: // FX18: LD ST, Vx (Set sound timer = Vx.)
            st = V[X];
            break;

        case 0x1E: 
            index += V[X];
            break;

        case 0x29: // FX29: LD F, Vx (Set I = location of sprite for digit Vx.)
            index = V[X] * 5;
            break;

        case 0x33: // FX33: LD B, Vx (Store BCD representation of Vx in memory locations I, I+1, and I+2.)
            ram[index]     = V[X] / 100;        
            ram[index + 1] = (V[X] / 10) % 10;  
            ram[index + 2] = V[X] % 10;         
            break;

        case 0x55: // FX55: LD [I], Vx (Store registers V0 through Vx in memory starting at address I.)
            for (u_int8_t i = 0; i <= X; ++i) {
                ram[index + i] = V[i];
            }
            break;

        case 0x65: // FX65: LD Vx, [I] (Read registers V0 through Vx from memory starting at address I.)
            for (u_int8_t i = 0; i <= X; ++i) {
                V[i] = ram[index + i];
            }
            // NOTA: I vecchi interpreti incrementavano I = I + X + 1 dopo questa operazione.
            break;

        default:
            // Opcode sconosciuto (puoi aggiungere un messaggio di errore qui)
            break;
    }
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

// justj3di/chip-8/chip-8-main/chip8.cpp

void chip8::op_DXYN_DRW(u_int16_t opc)
{
 
    u_int8_t X = (opc >> 8) & 0x0F;       
    u_int8_t Y = (opc >> 4) & 0x0F;       
    u_int8_t height = opc & 0x0F;         

    // Collision flag
    V[0xF] = 0;
    u_int8_t start_x = V[X]; 
    u_int8_t start_y = V[Y];

  
    for (int y_line = 0; y_line < height; y_line++)
    {

        u_int8_t sprite_byte = ram[index + y_line];
        

        u_int8_t current_y = (start_y + y_line) % DISPLAY_HEIGHT;
        


        for (int x_bit = 0; x_bit < 8; x_bit++)
        {
            u_int8_t sprite_pixel = (sprite_byte >> (7 - x_bit)) & 0x1;
            
            u_int8_t current_x = (start_x + x_bit) % DISPLAY_WIDTH;
            
            
            int video_index = current_y * DISPLAY_WIDTH + current_x;
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

