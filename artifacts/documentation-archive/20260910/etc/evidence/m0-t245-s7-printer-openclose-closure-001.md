# M0 T245 S7 — Printer/LPT session open-close closure

## Selected Source Package

S6 established that the reached selector 17 is not generic printer I/O: its
guest caller `mschar.asm:prn_open` supplies `SI=1`, `DX=0`.  The direct
OpenNT path is:

```
prn_open -> BOP 17 -> printer_.c:printer_io
         -> printer_bop_openclose(1)
         -> nt_lpt.c:host_lpt_dos_open(0)
         -> host_lpt[0].dos_opened = TRUE
```

The sibling SI=2 follows the source's close lifecycle and clears that state.
`host_lpt_dos_close` only closes a host endpoint when it is active; this
first profile owns no endpoint, so the original final state clear is the only
selected effect.

## Implementation

- `opennt/softpc/printer_openclose.c` mirrors `printer_io`'s early NTVDM
  subfunction switch and the original modulo-three `printer_bop_openclose`
  selection/order.
- `shim/softpc_printer_openclose_shim.{h,c}` supplies a per-thread,
  three-entry session LPT `dos_opened` state.  It holds no Windows HANDLE,
  buffer, raw guest pointer or Bochs object.
- The C4 C4 17 bridge resumes only SI=1/2 at `RIP+3`; it preserves GPR and
  flags.  SI=0 flush and all ordinary BIOS branches decline.

## Verification

The outside-sandbox formal graph `build/M0-T245-S7/formal-r1` built the
actual x64 `/MT`, CPU5/P-MMX closure (262 Ninja steps).  Its fixture proves:

- direct SI=1 with `DX=4` opens adapter `1` through original modulo-three
  selection;
- global SI=2 closes the same state;
- SI=0 declines without creating a route;
- protected mode and RIP-overflow reject.

One finite source-built run then accepted selector 17 at `0211:02D5` with
the original `SI=1, DX=0` input.  Its next new selector was 5E at
`8E08:08AF`, declined.  No patch was selected from that observation.

## Transfer

S7 closes only the OpenNT session-state open/close pair.  It does not claim
printer output or BIOS INT 17h support.  The reached 5E is a top-level
configuration-complete notification: source `MS_bop_E` calls `UMBNotify(0)`
and `demDasdInit()`.  It transfers as the independent dependency
`BOP-DEPENDENCY-110`.
