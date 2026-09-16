#include "opennt-abi/host-compat/include/nt.h"
#include "sim32.h"

int monitor_declaration_fixture(void)
{
    return (MAXSIZE == 1024 && GOOD == 1 && BAD == 0 && BADSIZE == 2) ? 0 : 1;
}
