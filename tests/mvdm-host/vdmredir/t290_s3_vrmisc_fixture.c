#include <nt.h>

VOID VrTerminateDosProcess(VOID);
VOID VrUnsupportedFunction(VOID);

static USHORT fixture_ax;
static USHORT fixture_mailslot_pdb;
static USHORT fixture_pipe_pdb;
static ULONG fixture_carry;

USHORT getAX(VOID)
{
    return fixture_ax;
}

VOID setAX(USHORT value)
{
    fixture_ax = value;
}

VOID setCF(ULONG value)
{
    fixture_carry = value;
}

VOID VrTerminateMailslots(WORD pdb)
{
    fixture_mailslot_pdb = pdb;
}

VOID VrTerminateNamedPipes(WORD pdb)
{
    fixture_pipe_pdb = pdb;
}

int main(void)
{
    fixture_ax = 0x3172u;
    VrTerminateDosProcess();
    if (fixture_mailslot_pdb != 0x3172u || fixture_pipe_pdb != 0x3172u)
        return 1;
    fixture_ax = 0u;
    fixture_carry = 0u;
    VrUnsupportedFunction();
    if (fixture_ax != ERROR_NOT_SUPPORTED || fixture_carry != 1u)
        return 2;
    return 0;
}
