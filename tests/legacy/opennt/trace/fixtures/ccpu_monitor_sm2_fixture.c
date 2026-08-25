/* SM-2: CCPU BOP reaches the original BIOS[] selector table. */
#include <stdint.h>
#include <string.h>

extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void c_cpu_simulate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern unsigned long runner_ccpu_sm0_unexpected_calls(void);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern void setCX(uint16_t value);

int main(void) {
    static const uint8_t code[] = {
        0xb0, 0x01,             /* AL = BASE_MODULE */
        0xc4, 0xc4, 0x21,       /* historical BIOS BOP 21: Get_build_id */
        0xc4, 0xc4, 0xfe        /* CCPU unsimulate */
    };
    uint8_t *ram;

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) return 1;
    memcpy(ram, code, sizeof(code));
    setCS(0); setDS(0); setCX(0x4000); setIP(0);
    c_cpu_simulate();
    if (memcmp(ram + 0x4000, "Base$", 5) != 0) return 2;
    if (runner_ccpu_sm0_unexpected_calls() != 0) return 3;
    c_cpu_terminate(); sas_term();
    return 0;
}
