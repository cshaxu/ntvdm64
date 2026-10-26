/* Public WINMM feasibility witness for the selected WOW32 multimedia bridge.
 *
 * This is deliberately not a device-open, buffer, callback, or guest test.
 * It establishes only that the modern x86 public endpoints needed by the
 * original MMCallProc32 bridge load and report a coherent no-device state.
 * A zero device count is a valid host result, never a fabricated capability.
 */
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

int __cdecl main(void)
{
    TIMECAPS caps;
    CHAR error_text[128];
    UINT wave_out = waveOutGetNumDevs();
    UINT wave_in = waveInGetNumDevs();
    UINT midi_out = midiOutGetNumDevs();
    UINT midi_in = midiInGetNumDevs();
    UINT aux = auxGetNumDevs();
    UINT mixer = mixerGetNumDevs();
    UINT joystick = joyGetNumDevs();

    if (timeGetDevCaps(&caps, sizeof(caps)) != TIMERR_NOERROR ||
        !mciGetErrorStringA(MCIERR_INVALID_DEVICE_ID, error_text,
                            sizeof(error_text)) ||
        !error_text[0]) {
        fprintf(stderr, "WOW_MULTIMEDIA_FEASIBILITY failed error=%lu\n",
                GetLastError());
        return 1;
    }

    printf("WOW_MULTIMEDIA_FEASIBILITY wave_out=%u wave_in=%u midi_out=%u "
           "midi_in=%u aux=%u mixer=%u joystick=%u timer_min=%u timer_max=%u\n",
           wave_out, wave_in, midi_out, midi_in, aux, mixer, joystick,
           caps.wPeriodMin, caps.wPeriodMax);
    return 0;
}
