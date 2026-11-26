/* Exercise the production request gate and template/response path without UI. */
#include <stdio.h>
#include <windows.h>
#include "nt.h"

static unsigned modal_calls;
static INT_PTR modal_result;
static int modal_valid;
static INT_PTR WINAPI fixture_dialog(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
#define DialogBoxIndirectParamW fixture_dialog
#include "ntvdm-exe/win32/wow_hard_error_dialog.c"
#undef DialogBoxIndirectParamW

static INT_PTR WINAPI fixture_dialog(HINSTANCE instance, LPCDLGTEMPLATEW form,
    HWND parent, DLGPROC procedure, LPARAM parameter)
{
    const mvdm_wow_hard_error_dialog_data *data = (const void *)parameter;
    (void)instance; (void)parent;
    ++modal_calls;
    modal_valid = form && form->cdit == 3 && procedure &&
        !lstrcmpW(data->title, L"File Error") &&
        !lstrcmpW(data->message, L"Missing test driver") &&
        data->button_count == 2 && data->result_values[0] == 1 &&
        data->result_values[1] == 3 && data->default_result == 3;
    return modal_result;
}

#define CHECK(x) do { if (!(x)) { printf("FAIL hard-error request line=%d\n", __LINE__); return 0; } } while (0)
int check_hard_error_request(void)
{
    WCHAR title_text[] = L"File Error", message_text[] = L"Missing test driver";
    UNICODE_STRING title = {sizeof(title_text)-sizeof(WCHAR), sizeof(title_text), title_text};
    UNICODE_STRING message = {sizeof(message_text)-sizeof(WCHAR), sizeof(message_text), message_text};
    ULONG_PTR parameters[MAXIMUM_HARDERROR_PARAMETERS] = {0};
    const NTSTATUS request = STATUS_VDM_HARD_ERROR | 0x10000000;
    ULONG response, count;
    NTSTATUS status;
    int recognized;
    parameters[0] = MAKELONG(2, TRUE);
    parameters[1] = MAKELONG(5 | 0x8000, 0);
    parameters[2] = (ULONG_PTR)&title;
    parameters[3] = (ULONG_PTR)&message;
    for (count = 0; count < 2; ++count) {
        parameters[HARDERROR_PARAMETERS_FLAGSPOS] = count ? HARDERROR_FLAGS_DEFDESKTOPONLY : 0;
        modal_calls = 0; modal_result = 3; modal_valid = 0;
        status = mvdm_wow_hard_error_present(request, MAXIMUM_HARDERROR_PARAMETERS,
            12, parameters, 0, &response, &recognized);
        CHECK(status == STATUS_SUCCESS && recognized == 1 && response == 3 && modal_calls == 1 && modal_valid);
    }
    modal_calls = 0; modal_result = -1;
    status = mvdm_wow_hard_error_present(request, MAXIMUM_HARDERROR_PARAMETERS,
        12, parameters, 0, &response, &recognized);
    CHECK(status == STATUS_UNSUCCESSFUL && recognized == 1 && response == 0 && modal_calls == 1);
    modal_calls = 0;
    for (count = 0; count <= MAXIMUM_HARDERROR_PARAMETERS + 1; ++count) {
        if (count == MAXIMUM_HARDERROR_PARAMETERS) continue;
        status = mvdm_wow_hard_error_present(request, count, 12, parameters, 0, &response, &recognized);
        CHECK(status == STATUS_NOT_IMPLEMENTED && !recognized && !response && !modal_calls);
    }
    CHECK(mvdm_wow_hard_error_present(request, 5, 0, parameters, 0, &response, &recognized) == STATUS_NOT_IMPLEMENTED && !recognized);
    CHECK(mvdm_wow_hard_error_present(request, 5, 12, NULL, 0, &response, &recognized) == STATUS_NOT_IMPLEMENTED && !recognized);
    CHECK(mvdm_wow_hard_error_present(request, 5, 12, parameters, 0, NULL, &recognized) == STATUS_NOT_IMPLEMENTED && !recognized);
    CHECK(mvdm_wow_hard_error_present(request, 5, 12, parameters, 1, &response, &recognized) == STATUS_NOT_IMPLEMENTED && !recognized);
    CHECK(mvdm_wow_hard_error_present(STATUS_UNSUCCESSFUL, 5, 12, parameters, 0, &response, &recognized) == STATUS_NOT_IMPLEMENTED && !recognized);
    CHECK(!modal_calls);
    puts("PASS XP five-parameter request, response and rejection contract");
    return 1;
}
