#include "mvdm_command_registers.h"

#include "adapter-bochs/machine_facade.h"

uint16_t getAX(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_ax16(&value);
    return value;
}

uint8_t getAL(void)
{
    uint8_t value = 0u;
    (void)machine_facade_copy_al8(&value);
    return value;
}

uint16_t getBX(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_bx16(&value);
    return value;
}

uint16_t getDX(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_dx16(&value);
    return value;
}

uint16_t getSI(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_si16(&value);
    return value;
}

uint16_t getDS(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_ds16(&value);
    return value;
}

uint16_t getES(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_es16(&value);
    return value;
}

void setAL(uint8_t value)
{
    (void)machine_facade_set_al8(value);
}

void setAX(uint16_t value)
{
    (void)machine_facade_set_ax16(value);
}

void setBX(uint16_t value)
{
    (void)machine_facade_set_bx16(value);
}

void setCF(uint32_t value)
{
    (void)machine_facade_set_carry(value);
}
