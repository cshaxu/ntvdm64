#ifndef WOW_USER_THUNK_SCOPE_H
#define WOW_USER_THUNK_SCOPE_H
/* Include after original WOW32 declarations. Keep its LPFNW32/PVDMFRAME ABI,
 * including the selected profile's FASTCALL definition, without a cast. */
ULONG WINAPI wow_user_invoke_thunk(LPFNW32, PVDMFRAME);
/* ADAPTER-WOW-099: original policy, known original API return identity. */
INT WINAPI wow_user_dispatch_exception(DWORD, PEXCEPTION_POINTERS, PTD,
    VPVOID, BOOL);
/* Original service contract; the worker remains presentation owner. */
ULONG __cdecl wow_user_sys_error_box(LPSTR, LPSTR, USHORT, USHORT, USHORT);
VOID WINAPI wow_user_trace_file_open_failure(LPCSTR, LPCSTR, WORD, DWORD);
VOID WINAPI wow_user_trace_wndproc_enter(UINT, DWORD, DWORD);
VOID WINAPI wow_user_trace_wndproc_return(UINT, DWORD, BOOL, LONG);
#endif
