#ifndef MVDM_WOW_HARD_ERROR_DIALOG_H
#define MVDM_WOW_HARD_ERROR_DIALOG_H

#include <windows.h>

/* This is deliberately the one reached WOW private hard-error form, not a
 * public NtRaiseHardError replacement. `recognized` is zero when the caller
 * must retain the source-shaped unavailable boundary. */
NTSTATUS mvdm_wow_hard_error_present(NTSTATUS error_status,
    ULONG number_of_parameters, ULONG unicode_string_parameter_mask,
    PULONG_PTR parameters, ULONG valid_response_options, PULONG response,
    int *recognized);

#endif
