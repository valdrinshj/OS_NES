#include "demo.h"
#include "cpu.h"
#include <raylib.h>

static Cpu6502 *cpu = {0};
static Ppu2C02 *ppu = {0};
static bool emulationRun = false;
static float residualTime = 0.0f;
static int gamepad = 0;

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
                        DrawPixel(x + (i * scale) + is, y + (j * scale) + js, SpriteGetPixel(sprite, fx, fy));
        }
    }
    else {
        fx = fxs;
        for (int32_t i = 0; i < sprite->width; i++, fx += fxm) {
            fy = fys;
            for (int32_t j = 0; j < sprite->height; j++, fy += fym)
                DrawPixel(x + i, y + j, SpriteGetPixel(sprite, fx, fy));
        }
    }
}

void SetupDemo() {
    cpu = CpuGet();
    ppu = PpuGet();
    Cartridge *cartridge = CartridgeCreate("C:\\Users\\Startklar\\CLionProjects\\OS_NES\\Alter_Ego.nes");
    NesInsertCartridge(cpu->bus, cartridge);
    // Reset
    NesReset(cpu->bus);
}

void UpdateDemo() {
    cpu->bus->controller[0] = 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_X) ? 0x80 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_Z) ? 0x40 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_A) ? 0x20 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_S) ? 0x10 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_UP) ? 0x08 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_DOWN) ? 0x04 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_LEFT) ? 0x02 : 0x00;
    cpu->bus->controller[0] |= IsKeyDown(KEY_RIGHT) ? 0x01 : 0x00;

    if (IsGamepadAvailable(gamepad)) {
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) ? 0x08 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ? 0x04 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ? 0x02 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ? 0x01 : 0x00;

        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ? 0x10 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_MIDDLE_LEFT) ? 0x20 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ? 0x40 : 0x00;
        cpu->bus->controller[0] |= IsGamepadButtonDown(gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ? 0x80 : 0x00;

    }

    float elapsedTime = GetFrameTime();
    if (emulationRun) {
        if (residualTime > 0.0f)
            residualTime -= elapsedTime;
        else {
            residualTime += (1.0f / 60.0f) - elapsedTime;
            do { NesClock(cpu->bus); } while (!ppu->frameCompleted);
            ppu->frameCompleted = false;
        }
    }
    if (IsKeyPressed(KEY_SPACE)) emulationRun = !emulationRun;
    if (IsKeyPressed(KEY_R)) NesReset(cpu->bus);
}

void StartDemo() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(255, 240, "GAME WINDOW");

    SetupDemo();
    InitAudioDevice();
    Sound FxWav = LoadSound("C:\\Users\\Startklar\\CLionProjects\\OS_NES\\NES MUSIC.wav");
    while (!WindowShouldClose()) {
        UpdateDemo();
        BeginDrawing();
        ClearBackground(BLACK);
        if (IsSoundPlaying(FxWav)) {

        }else {
            PlaySound(FxWav);
        }

        DrawSprite(ppu->spriteScreen, 0, 0, 1);

        EndDrawing();
    }
}