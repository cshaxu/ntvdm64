# M0 T396 S26 — WOW32 post-initializer abort frontier 001

## Ordinary evidence

After S25's CRT-to-original-DLLENTRY bridge, normal Console-owning WOWEXEC
still loads WOW32 (23 modules) but exits with `0xc0000409`, fast-fail code 7,
at `ntvdm32.exe+0x5cb1b`.  The S20 map places CRT `_abort` at
`ntvdm32.exe+0x5caf2`; the terminal is therefore inside the CRT abort path.

The existing default-off scalar reports add two important negative boundaries:

- no `MVDM_SESSION_TERMINATION_REPORT_PATH` record was emitted, so this does
  not traverse the normal session-owned `host_terminate`/`TerminateVDM`
  conversion;
- the BOP report reaches DPMI protected-mode activity, including `53:01`, but
  contains no `51:*` dispatch before the terminal.

Thus neither a generic process-exit policy nor successful WOW32 dispatch can
be inferred.  The source has several direct `TerminateVDM` routes, but the
existing observations cannot distinguish an unreported host path from an
earlier native fast-fail caller.

## Diagnostic contrast and boundary

The previously existing S19 debugger observer is not usable for this fresh
product: its hardware-watch address is explicitly tied to a stale S18 main
prologue RVA, and its run times out before reaching the fast-fail exception.
It is diagnostic-only and is not ordinary product evidence.

S26 is therefore closed at a finite evidence boundary.  S27 admits a new
build-local, read-only debug observer with **no hardware watchpoints and no
product-memory write**.  It may only capture the first fast-fail context and
walk its x86 stack; its result remains diagnostic contrast and cannot be used
as acceptance evidence.  A separately repeated normal Console-owning run is
still required for any recovery conclusion.

