/* Public WINMM feasibility witness for the selected WOW32 multimedia bridge.
 *
 * This is not a playback, capture, callback, or guest test. Format queries
 * never open a device; explicit invalid device ids test rejection only.
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
    WAVEFORMATEX format = {WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0};
    HWAVEOUT wave_handle = NULL;
    HMIDIOUT midi_handle = NULL;
    UINT query = waveOutOpen(&wave_handle, WAVE_MAPPER, &format, 0, 0, WAVE_FORMAT_QUERY);
    UINT bad_wave = waveOutOpen(&wave_handle, 0x7fff, &format, 0, 0, WAVE_FORMAT_QUERY);
    UINT bad_midi = midiOutOpen(&midi_handle, 0x7fff, 0, 0, 0);
    MCIERROR bad_mci = mciSendCommandA(0xfffe, MCI_CLOSE, 0, 0);
    UINT wave_out = waveOutGetNumDevs();
    UINT wave_in = waveInGetNumDevs();
    UINT midi_out = midiOutGetNumDevs();
    UINT midi_in = midiInGetNumDevs();
    UINT aux = auxGetNumDevs();
    UINT mixer = mixerGetNumDevs();
    UINT joystick = joyGetNumDevs();

    if (wave_handle || midi_handle || bad_wave != MMSYSERR_BADDEVICEID ||
        bad_midi != MMSYSERR_BADDEVICEID || bad_mci != MCIERR_INVALID_DEVICE_ID) {
        fprintf(stderr, "Unexpected native invalid-device ownership/result\n");
        if (wave_handle) waveOutClose(wave_handle);
        if (midi_handle) midiOutClose(midi_handle);
        return 1;
    }
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
    printf("WOW_MULTIMEDIA_QUERY query=%u bad_wave=%u bad_midi=%u bad_mci=%lu\n",
           query, bad_wave, bad_midi, bad_mci);
    return 0;
}
