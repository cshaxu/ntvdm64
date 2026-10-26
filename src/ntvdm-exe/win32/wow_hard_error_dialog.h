#ifndef MVDM_WOW_HARD_ERROR_DIALOG_H
#define MVDM_WOW_HARD_ERROR_DIALOG_H

#include <windows.h>

/* `DisplayVDMHardError` compacts visible controls but returns original source
 * slots.  Keep this data independent of the modal public-USER surface so the
 * decoder can be verified without creating a second response policy. */
#define MVDM_WOW_HARD_ERROR_MAX_BUTTONS 3u

typedef struct mvdm_wow_hard_error_buttons {
    WORD descriptors[MVDM_WOW_HARD_ERROR_MAX_BUTTONS];
    UINT source_slots[MVDM_WOW_HARD_ERROR_MAX_BUTTONS];
    UINT count;
    UINT default_slot;
    UINT cancel_slot;
} mvdm_wow_hard_error_buttons;

BOOL mvdm_wow_hard_error_decode_buttons(
    const WORD descriptors[MVDM_WOW_HARD_ERROR_MAX_BUTTONS],
    mvdm_wow_hard_error_buttons *buttons);

/* This is deliberately the one reached WOW private hard-error form, not a
 * public NtRaiseHardError replacement. `recognized` is zero when the caller
 * must retain the source-shaped unavailable boundary. */
NTSTATUS mvdm_wow_hard_error_present(NTSTATUS error_status,
    ULONG number_of_parameters, ULONG unicode_string_parameter_mask,
    PULONG_PTR parameters, ULONG valid_response_options, PULONG response,
    int *recognized);

#endif
