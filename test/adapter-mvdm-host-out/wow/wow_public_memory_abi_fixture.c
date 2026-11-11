#include "precomp.h"

/* Compile with /Gd and calling-convention/type mismatches as errors. Neither
 * prototype may accidentally depend on the product's default /Gz. This
 * object is a declaration check, not a replacement provider or runtime test. */
LPVOID (WINAPI *wow_pointer_public_abi)(DWORD, DWORD, BOOL) = WOWGetVDMPointer;
LPVOID (APIENTRY *wow_resolve_public_abi)(VPVOID) = WOW32ResolveMemory;
C_ASSERT(sizeof(void *) == 4);
