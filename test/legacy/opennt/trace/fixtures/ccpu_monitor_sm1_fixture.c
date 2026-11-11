/* SM-1: bounded guest-instruction evidence for historical CCPU SAS. */
#include <stdint.h>

enum { SAS_ROM = 2 };

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void c_cpu_simulate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern void c_sas_connect_memory(uint32_t low, uint32_t high, int type);
extern void c_sas_enable_20_bit_wrapping(void);
extern void c_sas_disable_20_bit_wrapping(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint8_t getAL(void);

static int run_at_zero(uint8_t *ram, const uint8_t *code, uint32_t size,
                       uint16_t data_segment, uint8_t expected_al) {
    uint32_t index;

    for (index = 0; index < size; ++index) {
        ram[index] = code[index];
    }
    setCS(0);
    setDS(data_segment);
    setIP(0);
    c_cpu_simulate();
    return getAL() == expected_al ? 0 : 1;
}

int main(void) {
    static const uint8_t ram_code[] = {
        0xc6, 0x06, 0x00, 0x20, 0x5a, /* mov byte ptr [2000], 5a */
        0xa0, 0x00, 0x20,             /* mov al, [2000] */
        0xc4, 0xc4, 0xfe              /* BOP FE */
    };
    static const uint8_t rom_code[] = {
        0xc6, 0x06, 0x00, 0x30, 0x5a, /* mov byte ptr [3000], 5a */
        0xa0, 0x00, 0x30,             /* mov al, [3000] */
        0xc4, 0xc4, 0xfe
    };
    static const uint8_t a20_code[] = {
        0xc6, 0x06, 0x10, 0x00, 0x7a, /* mov byte ptr [0010], 7a */
        0xa0, 0x10, 0x00,             /* mov al, [0010] */
        0xc4, 0xc4, 0xfe
    };
    uint8_t *ram;
    int result = 0;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        return 1;
    }

    result |= run_at_zero(ram, ram_code, sizeof(ram_code), 0, 0x5a) << 0;

    /* Mapping is page based; the historical write is rejected, not faulted. */
    c_sas_connect_memory(0x3000u, 0x3fffu, SAS_ROM);
    ram[0x3000] = 0;
    result |= run_at_zero(ram, rom_code, sizeof(rom_code), 0, 0) << 1;

    c_sas_enable_20_bit_wrapping();
    result |= run_at_zero(ram, a20_code, sizeof(a20_code), 0xffffu, 0x7a) << 2;
    c_sas_disable_20_bit_wrapping();

    if (runner_ccpu_sm0_unexpected_calls() != 0) {
        result |= 8;
    }
    c_cpu_terminate();
    sas_term();
    return result == 0 ? 0 : result + 10;
}
