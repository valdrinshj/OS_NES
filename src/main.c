#include <stdio.h>
#include "cpu.h"
#include "demo.h"
#include "cartridge.h"

static Bus bus = {0};

int main() {
    cpu_init();
    cpu_connect_bus(&bus);
    StartDemo();
    // Cartridge * cart = CartridgeCreate("cpu_dummy_reads.nes");

}