#ifndef WOW_USER_THUNK_SCOPE_H
#define WOW_USER_THUNK_SCOPE_H
/* Include after original WOW32 declarations. Keep its LPFNW32/PVDMFRAME ABI,
 * including the selected profile's FASTCALL definition, without a cast. */
ULONG WINAPI wow_user_invoke_thunk(LPFNW32, PVDMFRAME);
#endif
