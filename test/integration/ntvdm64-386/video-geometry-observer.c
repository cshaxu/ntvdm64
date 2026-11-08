/* Diagnostic link only: retain the selected original BIOS calculation and
 * observe its actual display/BDA inputs. No guest or device state is changed. */
#define c_sas_store observe_font_store
#include "mvdm-host/softpc.new/base/video/ega_vide.c"
#undef c_sas_store
extern void c_sas_store(IU32, IU8);
extern void test_video_font_store(unsigned, unsigned, unsigned);
extern void test_video_font_mapping(unsigned, unsigned, unsigned);
extern IU32 phy_r32(IU32);
extern unsigned test_sas_data_selector(void);
/* CCPU binds sas_store directly, not through the SasVector. Preserve that
 * real write and observe only the first byte of each requested font bank. */
void observe_font_store(IU32 address, IU8 value)
{
    if ((getAX() == 0x1100 || getAX() == 0x1110) &&
        (address == 0xa0000 || address == 0xa4000) && getPG()) {
        IU32 pde = phy_r32((getCR3() & 0xfffff000) + ((address >> 22) * 4));
        IU32 pte = (pde & 1) ? phy_r32((pde & 0xfffff000) + (((address >> 12) & 1023) * 4)) : 0;
        half_word si, gi, s2, s4, g4, g5, g6;
        inb(0x3c4, &si); inb(0x3ce, &gi);
        outb(0x3c4, 2); inb(0x3c5, &s2);
        outb(0x3c4, 4); inb(0x3c5, &s4);
        outb(0x3ce, 4); inb(0x3cf, &g4);
        outb(0x3ce, 5); inb(0x3cf, &g5);
        outb(0x3ce, 6); inb(0x3cf, &g6);
        outb(0x3c4, si); outb(0x3ce, gi);
        test_video_font_mapping(address + 2, s2 | (s4 << 8),
            g4 | (g5 << 8) | (g6 << 16));
        test_video_font_mapping(address, pte, getCPL() | (getVM() << 8) |
            ((unsigned)sas_memory_type(pte & 0xfffff000) << 16));
    }
    if (address == 0xa4000 && (getAX() == 0x1100 || getAX() == 0x1110))
        test_video_font_mapping(address + 1, test_sas_data_selector(), getDS());
#ifdef FONT_READ_PLANE_CONTROL
    /* Explicit diagnostic intervention, not ordinary read-only observation:
     * select the font plane for reads as well as writes before the bank. */
    if ((getAX() == 0x1100 || getAX() == 0x1110) &&
        (address == 0xa0000 || address == 0xa4000)) {
        half_word index;
        inb(0x3ce, &index);
        outb(0x3ce, 4); outb(0x3cf, 2); outb(0x3ce, index);
    }
#endif
#ifdef FONT_SEQ_MODE_CONTROL
    if ((getAX() == 0x1100 || getAX() == 0x1110) &&
        (address == 0xa0000 || address == 0xa4000)) {
        half_word index;
        inb(0x3c4, &index);
        outb(0x3c4, 4); outb(0x3c5, 7); outb(0x3c4, index);
    }
#endif
#ifdef FONT_BANK_SELECT_CONTROL
    if ((getAX() == 0x1100 || getAX() == 0x1110) &&
        (address == 0xa0000 || address == 0xa4000)) {
        half_word index;
        inb(0x3c4, &index);
        outb(0x3c4, 3); outb(0x3c5, address == 0xa4000 ? 5 : 0);
        outb(0x3c4, index);
    }
#endif
    c_sas_store(address, value);
    if ((getAX() == 0x1100 || getAX() == 0x1110) &&
        (address == 0xa0000 || address == 0xa4000))
        test_video_font_store(address, value, sas_hw_at(address));
}
extern void test_video_geometry(unsigned, unsigned, unsigned, unsigned);
void test_video_geometry_snapshot(unsigned after, unsigned function)
{
    half_word index, low, overflow, maximum;
    io_addr port;
    test_video_geometry(after, function, get_screen_height() + 1,
        sas_hw_at_no_check(vd_rows_on_screen) + 1);
    if (function != 0x1112) return;
    port = sas_w_at_no_check(0x463);
    inb(port, &index);
    outb(port, 0x12); inb(port + 1, &low);
    outb(port, 7); inb(port + 1, &overflow);
    outb(port, 9); inb(port + 1, &maximum);
    outb(port, index);
    test_video_geometry(after + 2, function,
        low | ((overflow & 2) << 7) | ((overflow & 0x40) << 3), maximum);
}
