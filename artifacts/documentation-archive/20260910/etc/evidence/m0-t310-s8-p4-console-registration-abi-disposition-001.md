# M0 T310 S8 P4: Console Registration ABI Disposition

`nt_det.c` and `nt_fulsc.c` call the original
`RegisterConsoleVDM(DWORD, HANDLE, HANDLE, LPWSTR, DWORD, LPDWORD, PVOID *,
LPWSTR, DWORD, COORD, PVOID *) -> BOOL` interface.

The current adapter-owned `conapi.h` preserves that full pointer-bearing
declaration, so x86/x64 formal compilation of both original callers has no
implicit-call or calling-contract diagnostic.

The declaration is deliberately **not** a provider: original NT4 Console
Server owned VDM registration, event duplication and state/text section
mapping; current public conhost exposes no same-semantic service. The runtime
disposition remains source-shaped unavailable. This P4 record closes the ABI
diagnostic only and does not claim console/full-screen execution closure.
