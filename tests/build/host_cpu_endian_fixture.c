/* Exercise the actual host register union used by original word-port I/O. */
typedef unsigned short word;
typedef unsigned char half_word;
#include "../../src/mvdm-host/softpc.new/host/inc/host_cpu.h"

int main(void)
{
    reg value;
    value.X = 0x1234;
    return value.byte.low == 0x34 && value.byte.high == 0x12 ? 0 : 1;
}
