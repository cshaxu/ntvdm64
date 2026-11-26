/* Link the actual product FM object and unavailable-service provider. */
#include <windows.h>
#include <stdio.h>
#include "nt_sb.h"
#include "sndblst.h"

extern BOOL FMActive;
extern int Position;

WORD BasePort = 0x220;
static unsigned devices, opens, resets, closes, volumes;
static BOOL fail_open, fail_caps;

static UINT WINAPI device_count(void) { return devices; }
static MMRESULT WINAPI device_caps(UINT id, LPMIDIOUTCAPSA caps, UINT size)
{
    (void)id;
    if (fail_caps) return MMSYSERR_ERROR;
    ZeroMemory(caps, size);
    caps->wTechnology = MOD_FMSYNTH;
    return MMSYSERR_NOERROR;
}
static MMRESULT WINAPI device_open(LPHMIDIOUT out, UINT id, DWORD cb,
                                  DWORD instance, DWORD flags)
{
    (void)id; (void)cb; (void)instance; (void)flags;
    ++opens;
    if (fail_open) return MMSYSERR_ERROR;
    *out = (HMIDIOUT)(ULONG_PTR)0x1234;
    return MMSYSERR_NOERROR;
}
static MMRESULT WINAPI device_reset(HMIDIOUT handle)
{
    if (handle != (HMIDIOUT)(ULONG_PTR)0x1234) return MMSYSERR_ERROR;
    ++resets;
    return MMSYSERR_NOERROR;
}
static MMRESULT WINAPI device_close(HMIDIOUT handle)
{
    if (handle != (HMIDIOUT)(ULONG_PTR)0x1234) return MMSYSERR_ERROR;
    ++closes;
    return MMSYSERR_NOERROR;
}
MIDIGETNUMDEVSPROC MidiGetNumDevsProc = device_count;
MIDIGETDEVCAPSPROC MidiGetDevCapsProc = device_caps;
MIDIOPENPROC MidiOpenProc = device_open;
MIDIRESETPROC MidiResetProc = device_reset;
MIDICLOSEPROC MidiCloseProc = device_close;

VOID MixerSetMidiVolume(BYTE level)
{
    if (level == 8) ++volumes;
}

int main(void)
{
    unsigned scenario, cycle;
    for (scenario = 0; scenario != 4; ++scenario) {
        devices = scenario != 0;
        fail_caps = scenario == 1;
        fail_open = scenario == 2;
        opens = resets = closes = volumes = 0;
        for (cycle = 0; cycle != 2; ++cycle) {
            BYTE status;
            ResetFM();
            if (!OpenFMDevice() || FMActive) return 1;
            if ((HFM != NULL) != (scenario == 3)) return 2;
            FMRegisterSelect(AD_MASK);
            FMDataWrite(0x80);
            FMStatusRead(&status);
            if (status != 0 || Position != 0) return 3;
            FMDataWrite(1);
            FMStatusRead(&status);
            if ((status & 0xc0) != 0xc0 || Position != 0) return 4;
            CloseFMDevice();
            CloseFMDevice();
            if (HFM || FMActive) return 5;
        }
        if (opens != (scenario >= 2 ? 2u : 0u) ||
            resets != (scenario == 3 ? 2u : 0u) ||
            closes != resets || volumes != resets) return 6;
    }
    puts("PASS product FM: unavailable service, absent/failed/open MIDI, timer writes and repeated cleanup");
    return 0;
}
