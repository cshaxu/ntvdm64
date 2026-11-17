# Extracted function provenance

Original paths are relative to OpenNT revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. Lines are inclusive and cover the signature through the closing brace. Destination files are relative to `src/compat`.

| Batch | Original relative path and file | Lines | Original signature | Destination | Contract |
| --- | --- | --- | --- | --- | --- |
| P19 | `windows/core/ntuser/kernel/ntstubs.c` | 6170–6188 (19) | `BOOL NtUserSetWindowFNID( IN HWND hwnd, IN WORD fnid)` | `wow_control_fnid.c` | Existing adapter name and HWND binding; BOOL(HWND, WORD) retained. Destination `BOOL WINAPI wow_edit_set_window_fnid(HWND hwnd, WORD fnid)`. |
| P21 | `windows/core/ntuser/client/wow.c` | 251–265 (15) | `PWND _GetDlgItem( PWND pwnd, int id)` | `wow_dialog_lookup.c` | Original signature retained. |
| P22 | `windows/core/ntuser/kernel/input.c` | 5015–5042 (28) | `void AddAttachment( PTHREADINFO pti, PQ pqAttach, LPBOOL pfChanged)` | `wow_input_membership.c` | Original signature retained. |
| P22 | `windows/core/ntuser/kernel/input.c` | 5044–5107 (64) | `void Recalc2( PQ pqAttach)` | `wow_input_membership.c` | Existing adapter takes PTHREADINFO ptiCurrent explicitly; original signature takes only PQ. Destination `void Recalc2( PQ pqAttach, PTHREADINFO ptiCurrent)`. |
| P24 | `windows/core/ntuser/client/client.c` | 2181–2194 (14) | `ATOM WINAPI RegisterClassWOWA( WNDCLASSA *lpWndClass, LPDWORD pdwWOWstuff)` | `wow_class_client.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2203–2253 (51) | `WORD WowGetDefWindowProcBits( PBYTE pDefWindowProcBits, WORD cbDefWindowProcBits)` | `wow_default_window_bits.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2326–2333 (8) | `DWORD WOWDlgInit( HWND hwndDlg, LONG lParam)` | `wow_dialog_memory.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2345–2350 (6) | `HANDLE GetEditDS()` | `wow_dialog_memory.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2362–2368 (7) | `VOID ReleaseEditDS( HANDLE h)` | `wow_dialog_memory.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2383–2406 (24) | `LONG DispatchClientMessage( PWND pwnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD pfn)` | `wow_client_dispatch.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2826–2832 (7) | `void SetWindowState( PWND pwnd, UINT flags)` | `wow_control_state.c` | Original signature retained. |
| P24 | `windows/core/ntuser/client/client.c` | 2834–2840 (7) | `void ClearWindowState( PWND pwnd, UINT flags)` | `wow_control_state.c` | Original signature retained. |
| P27 | `windows/core/ntuser/rtl/rect.c` | 46–56 (11) | `BOOL APIENTRY CopyInflateRect( LPRECT prcDst, CONST RECT *prcSrc, int cx, int cy)` | `wow_control_rect.c` | Original signature retained. |
| P28 | `windows/core/ntuser/kernel/miscutil.c` | 24–37 (14) | `VOID SetDialogPointer(PWND pwnd, LONG lPtr)` | `wow_dialog_pointer.c` | Original x86 /Gz calling convention retained. Destination `VOID SetDialogPointer(PWND pwnd, LONG lPtr)`. |
