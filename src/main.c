#include <stdio.h>
#include "cpu.h"
#include "demo.h"
#include "cartridge.h"

static Bus bus = {0};

int main() {
    bus.ppu = ppu_get();
    cpu_init();
    ppuInit();
    cpu_connect_bus(&bus);
    StartDemo();
}