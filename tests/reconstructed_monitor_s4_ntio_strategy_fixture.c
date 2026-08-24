#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reconstructed_monitor_s4.h"

enum {
    ntio_image_bytes = 33792u,
    real_mode_ram_bytes = 0x100000u,
    ntio_load_physical = 0x0700u,
    ntio_segment = 0x0070u,
    ntio_strategy_offset = 0x01ecu,
    ntio_ptrsav_offset = 0x0018u
};

static int read_exact_image(const char *path, uint8_t *out_image)
{
    FILE *file;
    size_t bytes;
    int trailing;

    file = fopen(path, "rb");
    if (file == NULL) return 0;
    bytes = fread(out_image, 1u, ntio_image_bytes, file);
    trailing = fgetc(file);
    fclose(file);
    return bytes == ntio_image_bytes && trailing == EOF;
}

int main(int argc, char **argv)
{
    uint8_t *ram;
    app_s4_monitor monitor;
    app_s4_state state;
    app_s4_stop_event stop;
    app_s4_run_result run_result;
    uint32_t ptrsav;
    int result = 1;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <source-built-NTIO.SYS>\n", argv[0]);
        return 2;
    }
    ram = calloc(real_mode_ram_bytes, 1u);
    if (ram == NULL) return 1;
    if (!read_exact_image(argv[1], ram + ntio_load_physical)) {
        fprintf(stderr, "expected an NTIO.SYS image of exactly %u bytes\n", ntio_image_bytes);
        goto done;
    }
    if (ram[ntio_load_physical + ntio_strategy_offset] != 0x2eu ||
        ram[ntio_load_physical + ntio_strategy_offset + 10u] != 0xcbu) {
        fprintf(stderr, "NTIO strategy byte signature does not match the fixed source-built image\n");
        goto done;
    }
    if (app_s4_monitor_initialize(&monitor, ram, real_mode_ram_bytes, NULL, NULL) !=
        APP_S4_BUDGET_EXHAUSTED ||
        app_s4_monitor_get_state(&monitor, &state) != APP_S4_BUDGET_EXHAUSTED) {
        goto done;
    }
    state.eax = state.ecx = state.edx = state.esi = state.edi = state.ebp = 0u;
    state.ebx = 0x1234u;
    state.esp = 0x0600u;
    state.cs = ntio_segment;
    state.ds = 0x2222u;
    state.es = 0x3456u;
    state.ss = 0u;
    state.ip = ntio_strategy_offset;
    state.flags = 0x0002u;
    if (app_s4_monitor_set_state(&monitor, &state) != APP_S4_BUDGET_EXHAUSTED) {
        fprintf(stderr, "could not set the strategy entry state\n");
        goto done;
    }
    run_result = app_s4_monitor_run(&monitor, 2u);
    if (run_result != APP_S4_BUDGET_EXHAUSTED ||
        app_s4_monitor_get_last_stop(&monitor, &stop) != APP_S4_NO_STOP_EVENT) {
        fprintf(stderr, "strategy run stopped with result %d\n", (int)run_result);
        goto done;
    }
    ptrsav = ntio_load_physical + ntio_ptrsav_offset;
    if (ram[ptrsav] != 0x34u || ram[ptrsav + 1u] != 0x12u ||
        ram[ptrsav + 2u] != 0x56u || ram[ptrsav + 3u] != 0x34u ||
        monitor.state.cs != ntio_segment || monitor.state.ip != (uint16_t)(ntio_strategy_offset + 10u)) {
        fprintf(stderr, "unexpected strategy boundary: ptrsav=%02X%02X:%02X%02X state=%04X:%04X\n",
            ram[ptrsav + 3u], ram[ptrsav + 2u], ram[ptrsav + 1u], ram[ptrsav],
            monitor.state.cs, monitor.state.ip);
        goto done;
    }
    result = 0;

done:
    free(ram);
    return result;
}
