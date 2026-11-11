#include <nt.h>

VOID VrGetDomainName(VOID);
VOID VrGetLogonServer(VOID);
VOID VrNetGetDCName(VOID);
VOID VrNetWkstaSetInfo(VOID);
VOID VrReturnAssignMode(VOID);
VOID VrSetAssignMode(VOID);

static USHORT fixture_ax;
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

static int
expect_original_not_supported(VOID (*provider)(VOID))
{
    fixture_ax = 0u;
    fixture_carry = 0u;
    provider();
    return fixture_ax == ERROR_NOT_SUPPORTED && fixture_carry == 1u;
}

int main(void)
{
    if (!expect_original_not_supported(VrGetDomainName))
        return 1;
    if (!expect_original_not_supported(VrGetLogonServer))
        return 2;
    if (!expect_original_not_supported(VrNetGetDCName))
        return 3;
    if (!expect_original_not_supported(VrNetWkstaSetInfo))
        return 4;

    fixture_ax = 0x2468u;
    fixture_carry = 1u;
    VrReturnAssignMode();
    if (fixture_ax != 0x2468u || fixture_carry != 1u)
        return 5;
    VrSetAssignMode();
    if (fixture_ax != 0x2468u || fixture_carry != 1u)
        return 6;
    return 0;
}
