#ifndef WOW_DIALOG_CONTEXT_BINDINGS_H
#define WOW_DIALOG_CONTEXT_BINDINGS_H
#include "wow_edit_entry_bindings.h"
#include "wow_default_window_bindings.h"
struct wow_dialog_monitor_view;
typedef struct wow_dialog_context {
    wow_edit_context server;
    wow_default_context geometry; /* Original creator's GetWindowBorders input. */
    struct wow_dialog_monitor_view *monitors;
} wow_dialog_context;
extern __declspec(thread) wow_dialog_context *wow_dialog_current;
/* Capture transfers its font only on success; failure leaves output intact.
 * Creation needs a message-font source; ordinary dialog messages do not. */
BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *, BOOL);
VOID WINAPI wow_dialog_release_context(wow_dialog_context *);
/* Original PMONITOR identity is stable through one dialog entry. Native
 * handles select immutable snapshots; nested entries own separate lists. */
MONITORINFO *WINAPI wow_dialog_monitor(HMONITOR);
MONITORINFO *WINAPI wow_dialog_primary_monitor(void);
#ifdef WOW_ORIGINAL_DIALOG_CLIENT
#undef gpsi
#undef oemInfo
#define gpsi (&wow_dialog_current->server)
#define rcScreen (wow_dialog_current->server.screen_rect)
#endif
#endif
