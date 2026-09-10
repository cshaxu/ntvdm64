# M0 T310 S8 P4 — original COMMAND pipe-worker callback closure

## Scope

This closure covers both original `cmdredir.c` pipe-worker launches:
`cmdPipeOutThread` and `cmdPipeInThread`.

## Source contract

Both bodies are original cdecl `void(LPVOID)` workers.  The historical source
force-casts them to `LPTHREAD_START_ROUTINE` at `CreateThread`; x64 happens to
share a calling convention, while x86 does not.  The direct cast also omits
the session binding required for the original host-resource identities that
the pipe records use.

The existing `cmd.h` declarations match both K&R definitions and callers.
Their C4131 diagnostics are retained as original source style; the callback
cast is the actual cross-architecture ABI boundary.

## Resolution

`MVDM-HOST-DIV-120` reuses the existing named adapter Win32 thread boundary.
It adds only the `void(LPVOID)` cdecl variant needed by the original pipe
workers.  The thunk captures the creator session, binds it around the original
worker body, then returns `DWORD 0` only to the public WINAPI callback ABI.
The original worker bodies, `CreateThread` argument order, pipe state, and
cleanup remain in `cmdredir.c`.

## Verification

Forced formal Ninja `/MT` object rebuilds completed on both hosts:

- x64: `build/M0-T310/S8/p1-machine-source/x64/command-redirection-thread-abi.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/command-redirection-thread-abi.log`

Neither log contains a callback calling-convention, function-pointer, or
pointer-width diagnostic for `cmdredir.c` or `thread_start_compat.c`.  The
original K&R C4131 diagnostics remain visible on both architectures.
