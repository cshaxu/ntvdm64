#include <stdint.h>
#include <string.h>

#include <windows.h>

#include "nt.h"
#include "wow_hard_error_dialog.h"

#define MVDM_WOW_HARD_ERROR_OVERRIDE UINT32_C(0x10000000)
#define MVDM_WOW_HARD_ERROR_UNICODE_MASK ((1u << 2) | (1u << 3))
#define MVDM_WOW_HARD_ERROR_PARAMETERS 4u
#define MVDM_WOW_HARD_ERROR_BUTTON_BASE 41000u
#define MVDM_WOW_HARD_ERROR_MESSAGE_ID 40999u
#define MVDM_WOW_HARD_ERROR_TEMPLATE_BYTES 2048u
#define MVDM_SEB_DEFAULT UINT16_C(0x8000)

typedef struct mvdm_wow_hard_error_dialog_data {
    const WCHAR *title;
    const WCHAR *message;
    const WCHAR *labels[3];
    UINT result_values[3];
    UINT button_count;
    UINT default_result;
} mvdm_wow_hard_error_dialog_data;

static BYTE *mvdm_wow_hard_error_align_dword(BYTE *cursor)
{
    return (BYTE *)(((uintptr_t)cursor + 3u) & ~(uintptr_t)3u);
}

static BYTE *mvdm_wow_hard_error_write_word(BYTE *cursor, WORD value)
{
    cursor[0] = (BYTE)(value & 0xffu);
    cursor[1] = (BYTE)(value >> 8);
    return cursor + sizeof(WORD);
}

static BYTE *mvdm_wow_hard_error_write_text(BYTE *cursor, const WCHAR *text)
{
    do {
        cursor = mvdm_wow_hard_error_write_word(cursor, *text);
    } while (*text++ != L'\0');
    return cursor;
}

static const WCHAR *mvdm_wow_hard_error_button_text(WORD descriptor)
{
    switch (descriptor & ~MVDM_SEB_DEFAULT) {
    case 1u: return L"OK";
    case 2u: return L"Cancel";
    case 3u: return L"Yes";
    case 4u: return L"No";
    case 5u: return L"Retry";
    case 6u: return L"Abort";
    case 7u: return L"Ignore";
    case 8u: return L"Close";
    default: return NULL;
    }
}

static int mvdm_wow_hard_error_copy_unicode(const UNICODE_STRING *source,
    WCHAR **copy)
{
    SIZE_T bytes;
    WCHAR *result;

    *copy = NULL;
    if (source == NULL || (source->Length & 1u) != 0u ||
        source->MaximumLength < source->Length ||
        (source->Length != 0u && source->Buffer == NULL)) return 0;
    bytes = (SIZE_T)source->Length + sizeof(WCHAR);
    result = (WCHAR *)HeapAlloc(GetProcessHeap(), 0u, bytes);
    if (result == NULL) return 0;
    if (source->Length != 0u)
        memcpy(result, source->Buffer, source->Length);
    result[source->Length / sizeof(WCHAR)] = L'\0';
    *copy = result;
    return 1;
}

static INT_PTR CALLBACK mvdm_wow_hard_error_dialog_proc(HWND dialog,
    UINT message, WPARAM wparam, LPARAM lparam)
{
    mvdm_wow_hard_error_dialog_data *data;

    if (message == WM_INITDIALOG) {
        UINT index;
        data = (mvdm_wow_hard_error_dialog_data *)lparam;
        SetWindowLongPtr(dialog, DWLP_USER, (LONG_PTR)data);
        SetWindowTextW(dialog, data->title);
        SetDlgItemTextW(dialog, MVDM_WOW_HARD_ERROR_MESSAGE_ID,
            data->message);
        for (index = 0u; index < data->button_count; ++index) {
            UINT id = MVDM_WOW_HARD_ERROR_BUTTON_BASE +
                data->result_values[index];
            SetDlgItemTextW(dialog, id, data->labels[index]);
            if (data->result_values[index] == data->default_result) {
                SendMessageW(dialog, DM_SETDEFID, id, 0);
                SetFocus(GetDlgItem(dialog, id));
                return FALSE;
            }
        }
        return TRUE;
    }
    data = (mvdm_wow_hard_error_dialog_data *)GetWindowLongPtr(dialog,
        DWLP_USER);
    if (message == WM_COMMAND && data != NULL) {
        UINT id = LOWORD(wparam);
        if (id > MVDM_WOW_HARD_ERROR_BUTTON_BASE &&
            id <= MVDM_WOW_HARD_ERROR_BUTTON_BASE + 3u) {
            EndDialog(dialog, (INT_PTR)(id - MVDM_WOW_HARD_ERROR_BUTTON_BASE));
            return TRUE;
        }
    }
    /* There is no source-proven implicit close response. Keep the dialog
     * modal until an offered source-defined button is selected. */
    if (message == WM_CLOSE) return TRUE;
    return FALSE;
}

static int mvdm_wow_hard_error_build_template(BYTE *storage,
    SIZE_T storage_bytes, const mvdm_wow_hard_error_dialog_data *data)
{
    BYTE *cursor = storage;
    BYTE *limit = storage + storage_bytes;
    DLGTEMPLATE *dialog;
    DLGITEMTEMPLATE *item;
    UINT index;

#define MVDM_TEMPLATE_REQUIRE(bytes) \
    do { if ((SIZE_T)(limit - cursor) < (bytes)) return 0; } while (0)
    MVDM_TEMPLATE_REQUIRE(sizeof(*dialog));
    dialog = (DLGTEMPLATE *)cursor;
    dialog->style = WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_SETFONT;
    dialog->dwExtendedStyle = 0u;
    dialog->cdit = (WORD)(data->button_count + 1u);
    dialog->x = 10;
    dialog->y = 10;
    dialog->cx = 220;
    dialog->cy = 90;
    cursor += sizeof(*dialog);
    MVDM_TEMPLATE_REQUIRE(3u * sizeof(WORD) + sizeof(WORD) + 26u * sizeof(WCHAR));
    cursor = mvdm_wow_hard_error_write_word(cursor, 0u);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0u);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0u);
    cursor = mvdm_wow_hard_error_write_word(cursor, 8u);
    cursor = mvdm_wow_hard_error_write_text(cursor, L"MS Shell Dlg");
    cursor = mvdm_wow_hard_error_align_dword(cursor);

    MVDM_TEMPLATE_REQUIRE(sizeof(*item) + 5u * sizeof(WORD));
    item = (DLGITEMTEMPLATE *)cursor;
    item->style = WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX;
    item->dwExtendedStyle = 0u;
    item->x = 8;
    item->y = 8;
    item->cx = 204;
    item->cy = 42;
    item->id = MVDM_WOW_HARD_ERROR_MESSAGE_ID;
    cursor += sizeof(*item);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0xffffu);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0x0082u);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0u);
    cursor = mvdm_wow_hard_error_write_word(cursor, 0u);

    for (index = 0u; index < data->button_count; ++index) {
        SIZE_T label_bytes = (lstrlenW(data->labels[index]) + 1u) * sizeof(WCHAR);
        cursor = mvdm_wow_hard_error_align_dword(cursor);
        MVDM_TEMPLATE_REQUIRE(sizeof(*item) + 5u * sizeof(WORD) + label_bytes);
        item = (DLGITEMTEMPLATE *)cursor;
        item->style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;
        if (data->result_values[index] == data->default_result)
            item->style |= BS_DEFPUSHBUTTON;
        item->dwExtendedStyle = 0u;
        item->x = (short)(12 + index * 68u);
        item->y = 62;
        item->cx = 60;
        item->cy = 16;
        item->id = (WORD)(MVDM_WOW_HARD_ERROR_BUTTON_BASE +
            data->result_values[index]);
        cursor += sizeof(*item);
        cursor = mvdm_wow_hard_error_write_word(cursor, 0xffffu);
        cursor = mvdm_wow_hard_error_write_word(cursor, 0x0080u);
        cursor = mvdm_wow_hard_error_write_text(cursor, data->labels[index]);
        cursor = mvdm_wow_hard_error_write_word(cursor, 0u);
    }
#undef MVDM_TEMPLATE_REQUIRE
    return cursor <= limit;
}

NTSTATUS mvdm_wow_hard_error_present(NTSTATUS error_status,
    ULONG number_of_parameters, ULONG unicode_string_parameter_mask,
    PULONG_PTR parameters, ULONG valid_response_options, PULONG response,
    int *recognized)
{
    const UNICODE_STRING *title;
    const UNICODE_STRING *message;
    WORD descriptors[3];
    WCHAR *title_copy = NULL;
    WCHAR *message_copy = NULL;
    mvdm_wow_hard_error_dialog_data data;
    BYTE template_storage[MVDM_WOW_HARD_ERROR_TEMPLATE_BYTES];
    INT_PTR result;
    UINT index;

    if (recognized != NULL) *recognized = 0;
    if (response != NULL) *response = 0u;
    if (error_status != (NTSTATUS)(STATUS_VDM_HARD_ERROR |
            MVDM_WOW_HARD_ERROR_OVERRIDE) ||
        number_of_parameters != MVDM_WOW_HARD_ERROR_PARAMETERS ||
        unicode_string_parameter_mask != MVDM_WOW_HARD_ERROR_UNICODE_MASK ||
        parameters == NULL || valid_response_options != 0u || response == NULL)
        return STATUS_NOT_IMPLEMENTED;

    if (recognized != NULL) *recognized = 1;
    if (HIWORD(parameters[0]) != TRUE) return STATUS_INVALID_PARAMETER;
    descriptors[0] = LOWORD(parameters[0]);
    descriptors[1] = HIWORD(parameters[1]);
    descriptors[2] = LOWORD(parameters[1]);
    title = (const UNICODE_STRING *)(ULONG_PTR)parameters[2];
    message = (const UNICODE_STRING *)(ULONG_PTR)parameters[3];

    ZeroMemory(&data, sizeof(data));
    for (index = 0u; index < 3u; ++index) {
        const WCHAR *label;
        if (descriptors[index] == 0u) continue;
        label = mvdm_wow_hard_error_button_text(descriptors[index]);
        if (label == NULL) return STATUS_INVALID_PARAMETER;
        data.labels[data.button_count] = label;
        data.result_values[data.button_count] = index + 1u;
        if ((descriptors[index] & MVDM_SEB_DEFAULT) != 0u) {
            if (data.default_result != 0u) return STATUS_INVALID_PARAMETER;
            data.default_result = index + 1u;
        }
        ++data.button_count;
    }
    if (data.button_count == 0u || data.default_result == 0u)
        return STATUS_INVALID_PARAMETER;
    if (!mvdm_wow_hard_error_copy_unicode(title, &title_copy) ||
        !mvdm_wow_hard_error_copy_unicode(message, &message_copy)) {
        if (title_copy != NULL) HeapFree(GetProcessHeap(), 0u, title_copy);
        if (message_copy != NULL) HeapFree(GetProcessHeap(), 0u, message_copy);
        return STATUS_NO_MEMORY;
    }
    data.title = title_copy;
    data.message = message_copy;
    if (!mvdm_wow_hard_error_build_template(template_storage,
        sizeof(template_storage), &data)) {
        HeapFree(GetProcessHeap(), 0u, message_copy);
        HeapFree(GetProcessHeap(), 0u, title_copy);
        return STATUS_INVALID_PARAMETER;
    }
    result = DialogBoxIndirectParamW(GetModuleHandleW(NULL),
        (DLGTEMPLATE *)template_storage, GetDesktopWindow(),
        mvdm_wow_hard_error_dialog_proc, (LPARAM)&data);
    HeapFree(GetProcessHeap(), 0u, message_copy);
    HeapFree(GetProcessHeap(), 0u, title_copy);
    if (result < 1 || result > 3) return STATUS_UNSUCCESSFUL;
    *response = (ULONG)result;
    return STATUS_SUCCESS;
}
