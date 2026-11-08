/* Diagnostic-only original SAS translation unit. Observe the original BIOS
 * helper before and after its real nested CPU execution; never replace it. */
#define host_simulate observe_sas_simulate
#include "mvdm-host/softpc.new/base/ccpu386/ccpusas4.c"
#undef host_simulate
extern void host_simulate(void);
extern void test_video_font_mapping(unsigned, unsigned, unsigned);
unsigned test_sas_data_selector(void) { return data_sel; }
void observe_sas_simulate(void)
{
    IU32 address = getEDX();
    int watch = getCS() == BIOS_VIRTUALISE_SEGMENT &&
        getEIP() == BIOS_WRTB_OFFSET &&
        (address == 0xa0000 || address == 0xa4000);
    if (watch)
        test_video_font_mapping(address + 3, getDS(),
            c_sas_dw_at(0xf0000 + BIOS_WRTB_OFFSET));
    host_simulate();
    if (watch)
        test_video_font_mapping(address + 4, getEAX(), getEIP());
}
