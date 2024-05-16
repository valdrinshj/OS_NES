#include "demo.h"
#include "cpu.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_CHARS "0123456789ABCDEF"

static char* mapAsm[0xFFFF] = {0};
static Cpu *cpu = {0};
static Ppu *ppu = {0};
static bool emulationRun = false;
static float residualTime = 0.0f;
static uint8_t selectedPalette = 0x00;

static char* hex(uint32_t n, uint8_t d, char *dst) {
    memset(dst, 0, sizeof dst);
    int i;
    dst[d] = 0;
    for (i = d - 1; i >= 0; i--, n >>= 4) {
        dst[i] = HEX_CHARS[n & 0xF];
    }
    return dst;
}

void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns) {
    int nRamX = x, nRamY = y;
    char hex_aux[16];
    char sOffSet[1024];
    for (int row = 0; row < nRows; row++) {
        memset(sOffSet, 0, sizeof sOffSet);
        strcat(sOffSet, "$");
        strcat(sOffSet, hex(nAddr, 4, hex_aux));
        strcat(sOffSet, ":");
        for (int col = 0; col < nColumns; col++) {
            strcat(sOffSet, " ");
            strcat(sOffSet, hex(cpu_read(nAddr), 2, hex_aux));
            nAddr += 1;
        }
        DrawText(sOffSet, nRamX, nRamY, 4, BLACK);
        nRamY += 10;
    }
}

void DrawCpu(int x, int y) {
    char hex_aux[16];
    DrawText("STATUS:", x , y , 4, BLACK);
    DrawText("N", x  + 64, y,  4, cpu->status & N ? GREEN : RED);
    DrawText("V",x  + 80, y ,  4, cpu->status & V ? GREEN : RED);
    DrawText("-",x  + 96, y ,  4, cpu->status & U ? GREEN : RED);
    DrawText("B",x  + 112, y ,  4, cpu->status & B ? GREEN : RED);
    DrawText("D",x  + 128, y ,  4, cpu->status & D ? GREEN : RED);
    DrawText("I",x  + 144, y ,  4, cpu->status & I ? GREEN : RED);
    DrawText("Z",x  + 160, y ,  4, cpu->status & Z ? GREEN : RED);
    DrawText("C",x  + 178, y ,  4, cpu->status & C ? GREEN : RED);
    char pc[1024];
    char a[1024];
    char xr[1024];
    char yr[1024];
    char sp[1024];

    strcpy(pc, "");
    strcpy(a, "");
    strcpy(xr, "");
    strcpy(yr, "");
    strcpy(sp, "");

    strcat(pc, "PC: $");
    strcat(pc, hex(cpu->PC, 4, hex_aux));

    strcat(a, "A: $");
    strcat(a, hex(cpu->A, 2, hex_aux));
    strcat(a, "  [");
    char *a_str [256];
    sprintf(a_str,"%d", cpu->A);
    strcat(a, a_str);
    strcat(a, "]");

    strcat(xr, "X: $");
    strcat(xr, hex(cpu->X, 2, hex_aux));
    strcat(xr, "  [");
    char *x_str [256];
    sprintf(x_str,"%d", cpu->X);
    strcat(xr, x_str);
    strcat(xr, "]");

    strcat(yr, "Y: $");
    strcat(yr, hex(cpu->Y, 2, hex_aux));
    strcat(yr, "  [");
    char *y_str [256];
    sprintf(y_str,"%d", cpu->Y);
    strcat(yr, y_str);
    strcat(yr, "]");

    strcat(sp, "SP: $");
    strcat(sp, hex(cpu->SP, 4, hex_aux));


    DrawText(pc, x , y + 10, 4, BLACK);
    DrawText(a, x , y + 20, 4, BLACK);
    DrawText(xr, x , y + 30, 4, BLACK);
    DrawText(yr, x , y + 40, 4, BLACK);
    DrawText(sp, x , y + 50, 4, BLACK);
}

void DrawCode(int x, int y, int nLines) {
    int nLineY = (nLines >> 1) * 10 + y;
    uint16_t pc = cpu->PC;
    char *pcLine = mapAsm[pc++];
    DrawText(pcLine, x, nLineY, 4, BLUE);
    while (nLineY < (nLines * 10) + y) {
        pcLine = mapAsm[pc++];
        if (pcLine != NULL) {
            nLineY += 10;
            DrawText(pcLine, x, nLineY, 4, BLACK);
        }
    }
    pc = cpu->PC;
    nLineY = (nLines >> 1) * 10 + y;
    while (nLineY > y) {
        pcLine = mapAsm[--pc];
        if (pcLine != NULL) {
            nLineY -= 10;
            DrawText(pcLine, x, nLineY, 4, BLACK);
        }
    }
}
void DrawSprite(Sprite *sprite, uint16_t x, uint16_t y, int32_t scale) {
    if (sprite == NULL) return;

    int32_t fxs = 0, fxm = 1, fx = 0;
    int32_t fys = 0, fym = 1, fy = 0;

    if (scale > 1) {
        fx = fxs;
        for (int32_t i = 0; i < sprite->width; i++, fx += fxm) {
            fy = fys;
            for (int32_t j = 0; j < sprite->height; j++, fy += fym)
                for (int32_t is = 0; is < scale; is++)
                    for (int32_t js = 0; js < scale; js++)
                        DrawPixel(x + (i * scale) + is, y + (j * scale) + js, spriteGetPixel(sprite, fx, fy));
        }
    }
    else {
        fx = fxs;
        for (int32_t i = 0; i < sprite->width; i++, fx += fxm) {
            fy = fys;
            for (int32_t j = 0; j < sprite->height; j++, fy += fym)
                DrawPixel(x + i, y + j,spriteGetPixel(sprite, fx, fy));
        }
    }
}


void SetupDemo() {
    cpu = cpu_get();
    ppu = ppu_get();
    Cartridge *cartridge = CartridgeCreate("cpu_dummy_reads.nes");
    NesInsertCartridge(cpu->bus, cartridge);

    NesReset(cpu->bus);
}

void updateDemo() {
    float elapsedTime = GetFrameTime();
    if (emulationRun) {
        if (residualTime > 0.0f)
            residualTime -= elapsedTime;
        else {
            residualTime += (1.0f / 60.0f) - elapsedTime;
            do { NesClock(cpu->bus); } while (!ppu->frame_complete);
            ppu->frame_complete = false;
        }
    }
    else {
        // Emulate code step-by-step
        if (IsKeyPressed(KEY_C)) {
            // Clock enough times to execute a whole CPU instruction
            do { NesClock(cpu->bus); } while (!CpuComplete());
            // CPU clock runs slower than system clock, so it may be
            // complete for additional system clock cycles. Drain
            // those out
            do { NesClock(cpu->bus); } while (!CpuComplete());
        }
        // Emulate one whole frame
        if (IsKeyPressed(KEY_F)) {
            // Clock enough times to draw a single frame
            do { NesClock(cpu->bus); } while (!ppu->frame_complete);
            // Use residual clock cycles to complete current instruction
            do { NesClock(cpu->bus); } while (!CpuComplete());
            // Reset frame completion flag
            ppu->frame_complete = false;
        }
    }
    if(IsKeyPressed(KEY_SPACE)) emulationRun = !emulationRun;
    if (IsKeyPressed(KEY_R)) NesReset(cpu->bus);
    if(IsKeyPressed(KEY_P)) selectedPalette = (selectedPalette + 1) % 8;
}


void StartDemo() {
    printf("Hello, Demo!\n");
    InitWindow(780, 480, "NES Instructions Demo");

    SetupDemo();
    while (!WindowShouldClose()) {
        updateDemo();
        BeginDrawing();
        ClearBackground(WHITE);
        DrawCpu(516, 2);
        DrawCode(516, 72, 26);

        DrawSprite(ppu->sprScreen, 0, 0, 2);

        EndDrawing();
    }
}