#include "wow_standard_class_bindings.h"
#include "wow_user_capture_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "opennt-host/windows/core/ntuser/inc/user.h"
#include <stddef.h>

/* Exact private STDCLASS data ABI from original wow32/walias.c. The object
 * and classifier below are linked from that original translation unit,
 * not a duplicate name/ordinal/procedure table. Selected provider is x86. */
typedef struct _stdclass {
    LPSTR lpszClassName;
    ATOM aClassAtom;
    WNDPROC lpfnWndProc;
    INT iOrdinal;
    DWORD vpfnWndProc;
} wow_original_standard_class;
extern wow_original_standard_class stdClasses[];
INT WINAPI GetStdClassNumber(LPSTR);
C_ASSERT(sizeof(wow_original_standard_class) == 20);
C_ASSERT(offsetof(wow_original_standard_class, aClassAtom) == 4);
C_ASSERT(offsetof(wow_original_standard_class, lpfnWndProc) == 8);
C_ASSERT(offsetof(wow_original_standard_class, iOrdinal) == 12);
C_ASSERT(offsetof(wow_original_standard_class, vpfnWndProc) == 16);

/* WALIAS.H WOWCLASS order -> clinit.c / init.c registration FNIDs.
 * No class names/procedures or separate classification policy here. */
static const WORD fnids[] = {
    0, 0, FNID_BUTTON, FNID_COMBOBOX, FNID_EDIT, FNID_LISTBOX,
    FNID_MDICLIENT, FNID_SCROLLBAR, FNID_STATIC, FNID_DESKTOP,
    FNID_DIALOG, FNID_ICONTITLE, FNID_MENU, FNID_SWITCH,
    FNID_COMBOLISTBOX
};

/* Original WALIAS.H order to cleanup.c's ICLS_* order, not a new class-name
 * or dispatch table. Edit's restore target is intentionally DefWindowProc. */
static const BYTE cleanup_indices[] = {
    0, 0, 0, 5, 1, 3, 11, 4, 2, 6, 7, 10, 8, 9, 12
};

static BOOL process_control(WORD fnid)
{
    return fnid != FNID_DESKTOP && fnid != FNID_MENU &&
        fnid != FNID_SWITCH && fnid != FNID_ICONTITLE;
}

BOOL WINAPI wow_standard_classes_bind_cleanup(wow_cleanup_context *context)
{
    wow_cleanup_shared shared = {0};
    PROC restore = (PROC)wow_class_call_original_native;
    unsigned i;
    if (!context || !context->shared) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    for (i = 2; i < ARRAYSIZE(fnids); ++i) {
        wow_standard_class_view view;
        if (!process_control(fnids[i])) continue;
        if (!wow_standard_class_describe(stdClasses[i].lpszClassName, &view))
            return FALSE;
        shared.atomSysClass[cleanup_indices[i]] = view.atom;
    }
    shared.apfnClientA.pfnDefWindowProc = (PROC)DefWindowProcA;
    shared.apfnClientW.pfnDefWindowProc = (PROC)DefWindowProcW;
    shared.apfnClientW.pfnButtonWndProc = (PROC)wow_control_client_procedure(FNID_BUTTON, TRUE);
    shared.apfnClientW.pfnStaticWndProc = restore;
    shared.apfnClientW.pfnListBoxWndProc = (PROC)wow_control_client_procedure(FNID_LISTBOX, TRUE);
    shared.apfnClientW.pfnComboBoxWndProc = (PROC)wow_control_client_procedure(FNID_COMBOBOX, TRUE);
    /* Recovered dialog state must keep its canonical original owner after
     * a guest module unload; modern USER has no corresponding PDLG. */
    shared.apfnClientW.pfnDialogWndProc = (PROC)wow_dialog_window_procW;
    shared.apfnClientW.pfnMDIClientWndProc = restore;
    shared.apfnClientW.pfnComboListBoxProc = (PROC)wow_control_client_procedure(FNID_COMBOLISTBOX, TRUE);
    *context->shared = shared;
    context->scroll_proc = restore;
    return TRUE;
}

BOOL WINAPI wow_standard_class_describe(LPCSTR name,
    wow_standard_class_view *output)
{
    wow_standard_class_view result = {0};
    WNDCLASSEXA by_atom = {sizeof(by_atom)};
    wow_original_standard_class *entry;
    INT index;
    if (!name || !output) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    index = GetStdClassNumber((LPSTR)name);
    if (index <= 1 || (UINT)index >= ARRAYSIZE(fnids)) {
        SetLastError(ERROR_CLASS_DOES_NOT_EXIST); return FALSE;
    }
    entry = &stdClasses[index];
    result.native.cbSize = sizeof(result.native);
    if (!GetClassInfoExA(NULL, entry->lpszClassName, &result.native))
        return FALSE;
    /* Same producer/cache as GetStdClassNumber's integer path. Predefined
     * #327xx atoms are already in the original table. No Base atom table. */
    if (!entry->aClassAtom)
        entry->aClassAtom = (ATOM)RegisterWindowMessageA(entry->lpszClassName);
    if (!entry->aClassAtom) return FALSE;
    if (!GetClassInfoExA(NULL, MAKEINTATOM(entry->aClassAtom), &by_atom))
        return FALSE;
    if (result.native.lpfnWndProc != by_atom.lpfnWndProc ||
            result.native.hInstance != by_atom.hInstance ||
            result.native.style != by_atom.style ||
            result.native.cbClsExtra != by_atom.cbClsExtra ||
            result.native.cbWndExtra != by_atom.cbWndExtra) {
        SetLastError(ERROR_INVALID_DATA); return FALSE;
    }
    result.atom = entry->aClassAtom;
    result.fnid = fnids[index];
    *output = result;
    return TRUE;
}

BOOL WINAPI wow_standard_classes_publish(wow_class_lookup_context *classes)
{
    wow_class_lookup_entry *entry;
    if (!classes || !classes->client_module) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    for (entry = classes->pclsPublicList; entry; entry = entry->next) {
        wow_standard_class_view view;
        if (!wow_class_words_native_registration((wow_class_words_binding *)entry) ||
                entry->guest_server) continue;
        if (!wow_standard_class_describe(MAKEINTATOM(entry->atomClassName), &view) ||
                !wow_class_words_publish_guest(entry, &view.native)) return FALSE;
    }
    /* Successful members remain owned by the process on failure. No task
     * enters guest code until the entire group is available; retry fills
     * only missing members, and normal process disposal releases all of them. */
    return TRUE;
}

BOOL WINAPI wow_standard_classes_prepare(wow_class_lookup_context *classes,
    wow_window_callback callback)
{
    unsigned i;
    DWORD error;
    if (!classes || !classes->client_module || classes->pclsPrivateList ||
            classes->pclsPublicList || classes->system_classes) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    for (i = 2; i < ARRAYSIZE(fnids); ++i) {
        wow_standard_class_view view;
        IN_STRING name = {0};
        BOOL imported;
        /* These four are original LW_RegisterWindows desktop/menu/switch/
         * icon-title classes, not this process-control registration group. */
        if (!process_control(fnids[i])) continue;
        if (!wow_standard_class_describe(stdClasses[i].lpszClassName, &view))
            goto fail;
        if (!RtlCaptureAnsiString(&name, stdClasses[i].lpszClassName, TRUE))
            goto fail;
        imported = wow_class_words_import_native(classes, &view.native,
            name.pstr, view.atom, view.fnid, callback);
        error = GetLastError();
        if (name.fAllocated)
            RtlFreeHeap(RtlProcessHeap(), 0, name.strCapture.Buffer);
        SetLastError(error);
        if (!imported) goto fail;
    }
    return TRUE;
fail:
    error = GetLastError();
    /* No callbacks, HWND associations or guest pages are created here;
     * retirement cannot unregister the borrowed native registrations. */
    (void)wow_class_words_forget_native(classes);
    SetLastError(error);
    return FALSE;
}
