/* Test-only INT21 resource boundary. Never linked into a product build.
 * Only the probe's impossible drive token BX=00FE is intercepted. */
#define host_simulate s38_ioctl_simulate
#define DpmiMapAndCopyBuffer s38_ioctl_map
#define DpmiSwitchToRealMode s38_ioctl_enter_real
#define DpmiSimulateIretCF s38_ioctl_return
#include "../../src/mvdm/dpmi32/int21map.c"
#undef DpmiMapAndCopyBuffer
#undef host_simulate
#undef DpmiSwitchToRealMode
#undef DpmiSimulateIretCF
#include <stdio.h>

VOID host_simulate(VOID);
VOID DpmiSwitchToRealMode(VOID);
VOID DpmiSimulateIretCF(VOID);
PUCHAR DpmiMapAndCopyBuffer(PUCHAR buffer, USHORT length);
static BOOL parameter_pending;
static USHORT next_sector;
static BOOL track_active, saved_small;
static ULONG saved_large;
static BOOL control_request(VOID)
{
    return getBX() == 0xfe && getAX() >= 0x4402 && getAX() <= 0x4405;
}

static void ioctl_record(const char *event, unsigned value)
{
    DWORD error = GetLastError();
    FILE *log = fopen(DPMI_TRACE_LOG, "a");
    if (log) {
        fprintf(log, "%s value=%u AX=%04X BX=%04X CX=%04X\n",
            event, value, getAX(), getBX(), getCX());
        fclose(log);
    }
    SetLastError(error);
}

VOID s38_ioctl_enter_real(VOID)
{
    if (control_request() || (getAX() == 0x440d && getBX() == 0xfe &&
        (getCX() == 0x841 || getCX() == 0x861 || getCX() == 0x842 ||
         getCX() == 0x846 || getCX() == 0x866 ||
         getCX() == 0x847 || getCX() == 0x867 || getCX() == 0x840 ||
         getCX() == 0x860 || getCX() == 0x862 || getCX() == 0x868))) {
        if (track_active) ExitProcess(91);
        track_active = TRUE;
        saved_small = SmallBufferInUse;
        saved_large = LargeBufferInUseCount;
    }
    DpmiSwitchToRealMode();
}

VOID s38_ioctl_return(VOID)
{
    if (track_active) {
        if (SmallBufferInUse != saved_small ||
            LargeBufferInUseCount != saved_large) {
            ioctl_record("S38_FAIL_IOCTL_BUFFER_BALANCE", LargeBufferInUseCount);
            ExitProcess(92);
        }
        ioctl_record("S38_IOCTL_BUFFER_BALANCED", LargeBufferInUseCount);
        track_active = FALSE;
    }
    DpmiSimulateIretCF();
}

PUCHAR s38_ioctl_map(PUCHAR buffer, USHORT length)
{
    if (control_request() && length != 16) ExitProcess(99);
    if (getAX() == 0x440d && getBX() == 0xfe &&
        (getCX() == 0x840 || getCX() == 0x860 ||
         getCX() == 0x862 || getCX() == 0x868)) {
        USHORT expected = getCL() == 0x40 ? 48 :
            getCL() == 0x60 ? 38 : getCL() == 0x62 ? 5 : 4;
        ioctl_record("S38_IOCTL_DEVICE_PARAMETER_BYTES", length);
        if (length != expected) ExitProcess(97);
    }
    if (getAX() == 0x440d && getBX() == 0xfe &&
        (getCX() == 0x846 || getCX() == 0x866 ||
         getCX() == 0x847 || getCX() == 0x867)) {
        USHORT expected = (getCL() == 0x46 || getCL() == 0x66) ? 25 : 2;
        ioctl_record("S38_IOCTL_METADATA_PARAMETER_BYTES", length);
        if (length != expected) ExitProcess(95);
    }
    if (getAX() == 0x440d && getBX() == 0xfe && getCX() == 0x842) {
        USHORT expected = (buffer[0] & 2) ? 7 : 5;
        ioctl_record("S38_IOCTL_FORMAT_PARAMETER_BYTES", length);
        if (length != expected) {
            ioctl_record("S38_FAIL_IOCTL_FORMAT_PARAMETER_BYTES", expected);
            ExitProcess(93);
        }
    }
    if (parameter_pending) {
        parameter_pending = FALSE;
        /* The real-mode DOS wire packet is 13 bytes even for a 32-bit
         * client. Its wider selector must be read from the client owner. */
        ioctl_record("S38_IOCTL_TRACK_PARAMETER_BYTES", length);
        if (length != 13) {
            ioctl_record("S38_FAIL_IOCTL_DOS_PACKET_BYTES", 13);
            ExitProcess(87);
        }
    }
    return DpmiMapAndCopyBuffer(buffer, length);
}

VOID s38_ioctl_simulate(VOID)
{
    PWORD16 frame;
    PUCHAR packet;
    USHORT ip, cs, flags, error = 0, result = 0;
    if (!control_request() && (getAX() != 0x440d || getBX() != 0xfe)) {
        host_simulate();
        return;
    }
    /* DPMI_EXEC_INT already pushed its real-mode interrupt frame.
     * Supply only this controlled service's result, then emulate its IRET.
     * No physical disk provider, DOS private data or original media changes. */
    frame = (PWORD16)Sim32GetVDMPointer(
        ((ULONG)getSS() << 16) | getSP(), 6, FALSE);
    ip = frame[0]; cs = frame[1]; flags = frame[2];
    packet = Sim32GetVDMPointer(
        ((ULONG)getDS() << 16) | getDX(), 38, FALSE);
    if (control_request()) {
        unsigned mode = packet[0], i;
        if (getCX() != 16 || mode > 2) ExitProcess(100);
        for (i = 1; i < 16; ++i) if (packet[i] != 0x5a) ExitProcess(101);
        if (mode == 2) error = 5;
        else {
            result = mode == 1 ? 8 : 16;
            if (!(getAL() & 1)) FillMemory(packet, result, 0xa6);
        }
        ioctl_record("S38_IOCTL_CONTROL_RESULT", error ? error : result);
    } else if (getCX() == 0x860 && packet[0] == 0x5a) {
        FillMemory(packet, 38, 0xa6);
        ioctl_record("S38_IOCTL_DEVICE_QUERY", 38);
    } else if (getCX() == 0x840 || getCX() == 0x862 || getCX() == 0x868) {
        unsigned length = getCL() == 0x40 ? 48 : getCL() == 0x62 ? 5 : 4;
        unsigned i;
        for (i = 0; i < length; ++i) {
            BYTE expected = getCL() == 0x40 && i == 38 ? 2 :
                getCL() == 0x40 && i == 39 ? 0 : 0x5a;
            if (packet[i] != expected) ExitProcess(98);
        }
        if (getCL() == 0x68) error = 1;
        ioctl_record("S38_IOCTL_DEVICE_SET_VERIFY_REFUSE", getCX());
    } else if (getCX() == 0x860) {
        ZeroMemory(packet, 38);
        *(PWORD16)(packet + 7) = 512;
        parameter_pending = TRUE;
        next_sector = 0;
        ioctl_record("S38_IOCTL_MOCK_GEOMETRY", 512);
    } else if (getCX() == 0x846 || getCX() == 0x866 ||
               getCX() == 0x847 || getCX() == 0x867) {
        unsigned length = (getCL() == 0x46 || getCL() == 0x66) ? 25 : 2;
        unsigned i;
        for (i = 0; i < length; ++i) if (packet[i] != 0x5a) ExitProcess(96);
        if (getCL() & 0x20) FillMemory(packet, length, 0xa6);
        ioctl_record("S38_IOCTL_MOCK_METADATA_PACKET", getCX());
    } else if (getCX() == 0x842) {
        if (*(PWORD16)(packet + 1) != 1 ||
            *(PWORD16)(packet + 3) != 2 ||
            ((packet[0] & 2) && *(PWORD16)(packet + 5) != 3)) ExitProcess(94);
        ioctl_record("S38_IOCTL_MOCK_FORMAT_PACKET", (packet[0] & 2) ? 7 : 5);
        packet[0] = 0;
    } else if (getCX() == 0x861 || getCX() == 0x841) {
        USHORT sectors = *(PWORD16)(packet + 7);
        USHORT first = *(PWORD16)(packet + 5);
        unsigned length = sectors * 512, i;
        PUCHAR data = Sim32GetVDMPointer(
            ((ULONG)*(PWORD16)(packet + 11) << 16) |
            *(PWORD16)(packet + 9), length, FALSE);
        if (!sectors || sectors > 8 || first != next_sector) ExitProcess(88);
        if ((packet[0] & 0x80) && first == 8) {
            error = 5;
            ioctl_record("S38_IOCTL_MOCK_REFUSED", first);
        } else {
            if (getCX() == 0x861) {
                FillMemory(data, length, 0x5a);
                ioctl_record("S38_IOCTL_MOCK_READ", length);
            } else {
                for (i = 0; i < length; ++i) if (data[i] != 0x5a) ExitProcess(90);
                ioctl_record("S38_IOCTL_MOCK_WRITE", length);
            }
            next_sector += sectors;
        }
    } else {
        ioctl_record("S38_FAIL_IOCTL_UNEXPECTED_REQUEST", getCX());
        ExitProcess(89);
    }
    setAX(error ? error : result);
    setSTATUS((flags & ~1) | (error != 0));
    setCS(cs);
    setIP(ip);
    setSP(getSP() + 6);
}
