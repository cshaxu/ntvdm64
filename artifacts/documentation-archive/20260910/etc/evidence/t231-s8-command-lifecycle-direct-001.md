# T231 S8 — Direct OpenNT COMMAND host-child lifecycle witness

## Scope

This record admits the three coupled COMMAND services as one local Direct
state machine: `54:08` (`cmdExec`), `54:0A` (`cmdExecComspec32`) and `54:0B`
(`cmdReturnExitCode`).  It does not claim native guest continuity or the final
COMMAND-family closure.

## Original ownership retained

The imported `cmdexec.c` bodies retain their original service entries,
argument interpretation, `/c` COMSPEC construction, `CF`/`AL` result shape,
return-code route and re-entry decision.  These services are not split into
independent adapter recognizers.

## Necessary composition divergence

The internal historical `cmdExec32`/`cmdCreateProcess` pair cannot be linked
unchanged: it starts a CCPU-dependent worker thread, reads live SAS pointers
from that thread, and temporarily rewrites the parent process's standard
handles.  `command_misc_shim.c` is the narrow replacement for exactly this
uncomposable internal executor.  It uses public Win32 `CreateProcessA`, waits
for the child, maps the original fixed guest standard-handle tokens into
explicit `STARTUPINFOA` handles, and leaves the CLI process handles untouched.
The original `54:08`/`0A`/`0B` ordering and result contract remain in
`cmdexec.c`; every source change carries a `DIVERGENCE:` explanation.

## Reproduction

`build/M0-T231-S8/direct-r1/t231-s8.exe` is built with MSVC x64
`/W4 /WX /MT` from the admitted COMMAND mirrors, all current neutral COMMAND
shims and fixed-width CPU ABI sources.  It exits zero and proves:

1. `54:08` launches a host `cmd /c exit 37`, waits, returns the exit status in
   `AL`, clears carry, and preserves the original in-place command-tail CR to
   NUL transform.
2. `54:0A` obtains COMSPEC through the original service body and returns a
   controlled test child status of 41.
3. `54:0B` follows the original no-next-command return path and exposes the
   retained host-child exit status.
4. A guest `STD_HANDLES` record maps a fixed session token to an inheritable
   pipe; the child writes `TOKEN` through that pipe.  No parent standard handle
   is changed.

The new formal Ninja fixture target is declared in
`tools/build/t225-s7-full-module-manifest.json`.  As with S7, the existing
first full-module Ninja runner-idle issue is not presented as a pass; this
direct full-COMMAND source closure is the evidence asserted here.
