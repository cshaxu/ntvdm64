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

uint16_t getCX(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_cx16(&value);
    return value;
}

uint16_t getDX(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_dx16(&value);
    return value;
}

uint16_t getBP(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_bp16(&value);
    return value;
}

uint16_t getSP(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_sp16(&value);
    return value;
}

uint16_t getDI(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_di16(&value);
    return value;
}

uint16_t getIP(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_ip16(&value);
    return value;
}

uint16_t getSI(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_si16(&value);
    return value;
}

uint16_t getCS(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_cs16(&value);
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

uint16_t getSS(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_ss16(&value);
    return value;
}

uint16_t getFS(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_fs16(&value);
    return value;
}

uint16_t getGS(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_gs16(&value);
    return value;
}

uint16_t getMSW(void)
{
    uint16_t value = 0u;
    (void)machine_facade_copy_msw16(&value);
    return value;
}

uint32_t getPE(void)
{
    return (uint32_t)(getMSW() & 1u);
}

#define MVDM_COMMAND_GET_REGISTER32(public_name, facade_name) \
uint32_t get##public_name(void) \
{ \
    uint32_t value = 0u; \
    (void)machine_facade_copy_##facade_name##32(&value); \
    return value; \
}

MVDM_COMMAND_GET_REGISTER32(EAX, eax)
MVDM_COMMAND_GET_REGISTER32(EBX, ebx)
MVDM_COMMAND_GET_REGISTER32(ECX, ecx)
MVDM_COMMAND_GET_REGISTER32(EDX, edx)
MVDM_COMMAND_GET_REGISTER32(ESI, esi)
MVDM_COMMAND_GET_REGISTER32(EDI, edi)
MVDM_COMMAND_GET_REGISTER32(EBP, ebp)
MVDM_COMMAND_GET_REGISTER32(ESP, esp)
MVDM_COMMAND_GET_REGISTER32(EIP, eip)

void setAL(uint8_t value)
{
    (void)machine_facade_set_al8(value);
}

void setBL(uint8_t value)
{
    (void)machine_facade_set_bl8(value);
}

void setAX(uint16_t value)
{
    (void)machine_facade_set_ax16(value);
}

void setBX(uint16_t value)
{
    (void)machine_facade_set_bx16(value);
}

void setDX(uint16_t value)
{
    (void)machine_facade_set_dx16(value);
}

void setCX(uint16_t value)
{
    (void)machine_facade_set_cx16(value);
}

void setSP(uint16_t value)
{
    (void)machine_facade_set_sp16(value);
}

void setSI(uint16_t value)
{
    (void)machine_facade_set_si16(value);
}

void setDI(uint16_t value)
{
    (void)machine_facade_set_di16(value);
}

void setIP(uint16_t value)
{
    (void)machine_facade_set_ip16(value);
}

#define MVDM_COMMAND_SET_REGISTER32(public_name, facade_name) \
void set##public_name(uint32_t value) \
{ \
    (void)machine_facade_set_##facade_name##32(value); \
}

MVDM_COMMAND_SET_REGISTER32(EAX, eax)
MVDM_COMMAND_SET_REGISTER32(EBX, ebx)
MVDM_COMMAND_SET_REGISTER32(ECX, ecx)
MVDM_COMMAND_SET_REGISTER32(EDX, edx)
MVDM_COMMAND_SET_REGISTER32(ESI, esi)
MVDM_COMMAND_SET_REGISTER32(EDI, edi)
MVDM_COMMAND_SET_REGISTER32(EBP, ebp)
MVDM_COMMAND_SET_REGISTER32(ESP, esp)
MVDM_COMMAND_SET_REGISTER32(EIP, eip)

void setCF(uint32_t value)
{
    (void)machine_facade_set_carry(value);
}
