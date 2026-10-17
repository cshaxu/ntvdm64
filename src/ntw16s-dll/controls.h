#ifndef WIN16_STYLE_CONTROLS_H
#define WIN16_STYLE_CONTROLS_H
#include "win16_style.h"
/* Same-thread presentation attachment. Standard push/check/radio/group BUTTON
 * and text/box STATIC forms, ordinary string LISTBOX client rows;
 * plain EDIT border presentation retains native
 * editing. Native data and window style remain intact. SCROLLBAR mouse/key
 * handling uses the shared Win3.1 tracker and native application notifications;
 * other controls retain native input. System accessibility geometry for the
 * built-in SCROLLBAR is not yet adapted to its fixed-thumb presentation.
 * Unsupported families fail explicitly until implemented. */
NTW16S_API BOOL win16_style_control_install(HWND window,const win16_style_palette *palette);
/* Tab positions are private native state with no public getter. The creator
 * must supply the current PIXEL positions, not dialog units: count0 means
 * default tabs, count1 a repeated interval, count>1 explicit positions.
 * Attachment never sets/resets native tabs. Subsequent LB_SETTABSTOPS is
 * forwarded and its successful set-time conversion retained for drawing.
 * Unknown pre-existing tab state is not supported by generic install. */
NTW16S_API BOOL win16_style_tabbed_list_install(HWND window,const win16_style_palette *palette,
    int count,const int *pixel_positions);
/* EDIT consumes WS_BORDER during creation. The creator/creation observer must
 * supply the original bit; current GWL_STYLE cannot recover it reliably. */
NTW16S_API BOOL win16_style_edit_install(HWND window,BOOL original_border,const win16_style_palette *palette);
NTW16S_API BOOL win16_style_control_remove(HWND window);
/* Private terminal leaf: release owned state without gesture/font notices. */
BOOL win16_style_control_terminal(HWND window);
NTW16S_API BOOL win16_style_frame_scrollbars_install(HWND window,const win16_style_palette *palette);
NTW16S_API void win16_style_frame_scrollbars_remove(HWND window);
NTW16S_API void win16_style_frame_scrollbars_palette(HWND window,const win16_style_palette *palette);
/* Trial opt-in for a styled LISTBOX/ComboLBox's bars. Multicolumn lists use
 * column positions; ordinary horizontal ranges above65535 remain native.
 * The creator owns disabling/restoring its per-window theme. This function
 * never changes a theme association; ordinary late attachment stays unchanged.
 * Native list range/selection/storage remain owned by the list. */
NTW16S_API BOOL win16_style_list_scrollbar_enable(HWND window);
/* Same caller-owned theme contract, for an already styled multiline EDIT's
 * bars. Wide ranges that cannot use native thumb messages remain native. */
NTW16S_API BOOL win16_style_edit_scrollbars_enable(HWND window);
/* Shared nonclient presentation for frames and native controls. The supplied
 * DC, if any, is window-relative. Native USER owns ranges and bar rectangles;
 * opted-in controls and styled frames use our shared fixed-thumb geometry. */
NTW16S_API void win16_style_scrollbars_paint(HWND window,HDC target,const win16_style_palette *palette);
typedef struct win16_style_control_scope win16_style_control_scope;
typedef struct win16_style_control_counts {unsigned installed,unsupported,failed;} win16_style_control_counts;
/* Same-thread creation scope for future descendants of root. Newest matching
 * scope wins. End stops observation, leaving independent HWND attachments
 * intact; destroy/remove them normally. Root destruction disables matching.
 * Existing EDITs still require their original border bit via explicit API. */
NTW16S_API win16_style_control_scope *win16_style_controls_begin(HWND root,const win16_style_palette *palette);
NTW16S_API BOOL win16_style_controls_end(win16_style_control_scope *scope,win16_style_control_counts *counts);
#endif
