# M0 T310 S8 P2 — complete original SoftPC host package selection

## Decision

Recovery proceeds by the original `softpc.new/host/src/sources` package, not
by fixture-triggered source selection.  The formal candidate therefore selects
all 47 manifest source files plus the two original architecture-conditional
carriers (`nt_cprgs.c`, `nt_aorc.c`): 49 original host translation units.

## Result

- The complete package is in the formal `original-softpc-host-roots.lib`
  candidate on both supported host graphs.
- The complete x86 candidate and its forced-link audit compile/archive/link
  without adding a fixture or a source-local stand-in.
- `nt_event.c` now compiles as its original source after only a narrow
  public-SDK `winuserp.h` include boundary and reached `conapi.h` declaration
  shapes are supplied by `adapter-mvdm-host-out/win32`.
- The original `fprt.c` diagnostic package now compiles unchanged on x86 and
  x64.  The per-source UCRT `_NO_CRT_STDIO_INLINE` setting is necessary because
  the original source deliberately defines historical `printf`/`fprintf`/stdio
  interception bodies, while the current UCRT otherwise predefines inline
  bodies with the same names.  `_CRTAPI1`/`_CRTAPI2` bind to their historical
  `cdecl` ABI spelling.

## Boundary still exposed

`fprt.c`'s original `fwrite` path reconstructs a pointer through a `DWORD`.
It compiles on x64 but emits truncation diagnostics.  This is not hidden by a
fixture or a cast: it is a named future CRT-stream/host-object boundary.  Any
runtime recovery must preserve the original diagnostic-stream contract through
the session mapping manager or a source-shaped stream adapter; it must not
reinterpret a 32-bit value as a host pointer.

## Scope discipline

This is source/package selection and compile closure evidence only.  It does
not claim that the selected host package is a runnable machine, that video
state is x64-safe, or that a new controller implementation exists.  No Bochs,
MONITOR, kernel-VDM, `src.old`, or new fixture input participates.
