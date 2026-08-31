# M0 T324 S2 — Local Redirector source closure

## Result

The admitted local Redirector cohort now selects its original source bodies
through a single x86/x64 Ninja graph.  This is source/link-form closure only:
it does not claim that a `57:xx` guest service, a VDD hook, RAP, NetBIOS, DLC,
or a native guest interrupt has run.

## Selected original cohort

- `mvdm-host/vdmredir/{vrdisp,vrmisc,vrputil,vrmslot,vrnmpipe}.c`;
- the original COMMAND caller `mvdm-host/dos/command/cmdredir.c`; and
- the byte-identical `opennt-host/netapi/netlib/ntstatus.c` helper.

The graph selects the existing original declaration mirrors for DLC headers
only so `vrdisp.c` can retain its complete historical table.  It provides no
DLC implementation.  Remote/RAP/NetBIOS/DLC entries remain their explicit
later-owner unavailable transfers from S1.

## Bounded modern bindings

- The existing Redirector handle and mailslot adapters remain the sole
  session host-resource mapping owners.
- `MVDM-HOST-DIV-167` snapshots write input or stages read output across the
  original named-pipe worker boundary, retaining packed real-mode values and
  reacquiring a guest lease only for each bounded read/write.
- `MVDM-HOST-DIV-169` restores the source-declared
  `VrTerminateNamedPipes` lifecycle: signal the original worker, join it,
  cancel/release original queue records, then allow the session to dispose.
  The overlay owns only its atomic stop bit; it does not own the queue, ICA,
  or guest completion.
- `MVDM-HOST-DIV-170` removes `DWORD`/native `HANDLE` width coincidence from
  local mailslot name accounting and failure comparison.  The original WORD
  mailslot ABI remains mapped through the existing session mapping manager.

## Checks

`New-T324S2LocalRedirectorNinja.ps1` compiled the entire selected cohort and
linked/executed `redirector-async-contract-fixture.exe` on both x86 and x64.
The one bounded fixture verifies:

1. write input is copied before host completion;
2. read staging is copied back only at completion;
3. original 16-bit byte/error result locations receive the expected values;
4. an ended guest-memory session rejects a stale completion; and
5. the worker stop state changes only through the source-owned termination
   seam.

Warnings remain visible.  No warning suppression or `/WX` bypass is used;
the remaining output is historical C style, CRT-security, SDK macro overlap,
and non-width diagnostics outside this bounded binding.

## Non-results and next boundary

The real SoftPC ICA call remains in original `vrinit.c`; this S neither
replaces it nor asserts a runtime interrupt.  Integrating the source-selected
cohort into a whole product link and exercising a guest BOP belongs to the
later product integration task, after the named remote/device owners are
admitted.  The current evidence proves that the local source cohort no longer
depends on a durable raw guest pointer, not that all Redirector functionality
is runnable.
