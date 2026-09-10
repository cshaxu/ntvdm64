# M0 T310 S8 P4 - DEM host-HANDLE x86/x64 ABI closure

## Scope

This record covers the source-wide DEM convention that carried native NT4 x86 Win32 HANDLE values through two 16-bit guest register words. It is limited to the existing session host-resource mapping manager and selected DEM open/create/close consumers.

## Finding

The original GETHANDLE macro converted a 32-bit guest value directly to HANDLE. On x64 this emitted C4312 at every demhndl.c and demlock.c consumer, while demfile.c and demfcb.c truncated newly created handles when returning their original register pairs. This is a true host/guest boundary, not private pointer arithmetic.

## Resolution

- Reused the one existing session host_resource_mappings instance through mvdm_host_identity; no second handle table was introduced.
- Added adapter-owned word-pair publish/resolve helpers. The mirror retains the original AX:BP and AX:SI two-word ABI.
- DEM open/create paths publish the native handle before registering success; allocation failure closes the just-created handle and takes the existing client-error path.
- DEM close/FCB-close paths release the published identity only after the original CloseHandle succeeds.
- Invalid nonzero identities resolve to INVALID_HANDLE_VALUE, preserving normal Win32 failure behavior instead of treating them as an empty handle.
- The separate FOT pathname prefix computation now performs direct bounded native pointer subtraction, then converts its at-most-path-length count.
- The two SetFilePointer calls now pass their zero high offset through the exact public LONG pointer type.
- The NTDOS loader now uses INVALID_HANDLE_VALUE and direct private PBYTE cursor arithmetic.

## Verification

After regenerating the formal T310 S8 Ninja graphs, both x64 and x86 builds completed for mvdm_host_identity and DEM demfile, demfcb, demhndl, and demlock objects.

Both builds emitted no former DEM DWORD-to-HANDLE, HANDLE-to-register-word, pointer-arithmetic, or SetFilePointer call-contract diagnostics. Remaining warnings are deliberately visible and outside this width-boundary closure.
