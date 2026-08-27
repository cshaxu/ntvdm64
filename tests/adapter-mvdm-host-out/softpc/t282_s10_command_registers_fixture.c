#include <stdint.h>
#include <stdio.h>

#include "mvdm_command_registers.h"

static uint16_t ax_value = 0x1234u;
static uint16_t bx_value = 0x5678u;
static uint16_t dx_value = 0x9abcu;
static uint16_t si_value = 0xdef0u;
static uint16_t ds_value = 0x1357u;
static uint16_t es_value = 0x2468u;
static uint8_t al_value;
static uint32_t carry_value;

int machine_facade_copy_ax16(uint16_t *value) { *value = ax_value; return 1; }
int machine_facade_copy_bx16(uint16_t *value) { *value = bx_value; return 1; }
int machine_facade_copy_dx16(uint16_t *value) { *value = dx_value; return 1; }
int machine_facade_copy_si16(uint16_t *value) { *value = si_value; return 1; }
int machine_facade_copy_ds16(uint16_t *value) { *value = ds_value; return 1; }
int machine_facade_copy_es16(uint16_t *value) { *value = es_value; return 1; }
int machine_facade_copy_al8(uint8_t *value) { *value = (uint8_t)ax_value; return 1; }
int machine_facade_set_al8(uint8_t value) { al_value = value; return 1; }
int machine_facade_set_ax16(uint16_t value) { ax_value = value; return 1; }
int machine_facade_set_bx16(uint16_t value) { bx_value = value; return 1; }
int machine_facade_set_carry(uint32_t value)
{ if (value > 1u) return 0; carry_value = value; return 1; }

int main(void)
{
    if (getAX() != ax_value || getBX() != bx_value || getDX() != dx_value ||
        getSI() != si_value || getDS() != ds_value || getES() != es_value ||
        getAL() != (uint8_t)ax_value)
        return 1;
    setAL(0x5au);
    setAX(0x0f0fu);
    setBX(0xaaaau);
    setCF(1u);
    if (al_value != 0x5au || ax_value != 0x0f0fu || bx_value != 0xaaaau || carry_value != 1u)
        return 2;
    setCF(2u);
    if (carry_value != 1u) return 3;
    puts("PASS: COMMAND source-shaped register boundary");
    return 0;
}
