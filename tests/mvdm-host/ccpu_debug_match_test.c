/* Match-boundary unit test linked with the formal original c_debug.obj.
 * This does not replace instruction, exception-frame or real guest tests. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t CCPU_DR[8];
extern int debug_exception_pending;
extern uint32_t nr_inst_break, nr_data_break;
extern void setup_breakpoints(void);
extern void check_for_data_exception(uint32_t, int32_t, int32_t);
extern void check_for_inst_exception(uint32_t);

#define CHECK(x) do { if (!(x)) { printf("FAIL line=%d\n", __LINE__); return 1; } } while (0)

int main(void)
{
    memset(CCPU_DR, 0, sizeof(CCPU_DR));
    CCPU_DR[0] = 0x1000;
    CCPU_DR[7] = 0x00010001; /* byte write */
    setup_breakpoints();
    CHECK(nr_data_break == 1 && nr_inst_break == 0);
    check_for_data_exception(0x1000, 0, 0); /* read: no match */
    CHECK(!debug_exception_pending && CCPU_DR[6] == 0);
    check_for_data_exception(0x1001, 1, 0); /* adjacent: no match */
    CHECK(!debug_exception_pending);
    check_for_data_exception(0x1000, 1, 0);
    CHECK(debug_exception_pending && (CCPU_DR[6] & 1));
    debug_exception_pending = 0; /* model the consumer, retain status */
    check_for_data_exception(0x2000, 1, 0);
    CHECK(!debug_exception_pending && (CCPU_DR[6] & 1));
    check_for_data_exception(0x1000, 1, 0);
    CHECK(debug_exception_pending); /* same bit, genuinely new hit */

    debug_exception_pending = 0;
    CCPU_DR[7] = 0;
    setup_breakpoints();
    check_for_data_exception(0x1000, 1, 0);
    CHECK(!debug_exception_pending && (CCPU_DR[6] & 1));

    CCPU_DR[7] = 0x000F0001; /* dword read/write */
    setup_breakpoints();
    check_for_data_exception(0x0FFF, 0, 1); /* crossing into first byte */
    CHECK(debug_exception_pending);
    debug_exception_pending = 0;
    check_for_data_exception(0x1004, 1, 3);
    CHECK(!debug_exception_pending);

    CCPU_DR[7] = 1; /* instruction */
    setup_breakpoints();
    CHECK(nr_inst_break == 1 && nr_data_break == 0);
    check_for_inst_exception(0x1001);
    CHECK(!debug_exception_pending && (CCPU_DR[6] & 1));
    check_for_inst_exception(0x1000);
    CHECK(debug_exception_pending && (CCPU_DR[6] & 1));
    puts("S38_CCPU_DEBUG_MATCH_STATUS_SEPARATION_OK");
    return 0;
}
