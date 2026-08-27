# M0 T282 S10 — COMMAND register and bounded-lease binding evidence

## Question

Can the original COMMAND environment/current-directory group retain the
reached `softpc.h` register import shapes without a CCPU executor, a generic
CPU-frame abstraction or an unbounded guest pointer?

## Inputs

- Exact `opennt-mvdm-support/inc/softpc.h`, whose non-monitor declarations
  define `getDS`, `getDX`, `getSI`, `getES`, `getBX`, `setAL`, `setBX` and
  `setCF` with the historic 16-bit/byte/carry widths.
- S9 guest-memory seam ledger and the existing
  `adapter-softpc/mvdm_guest_location` bounded lease facade.
- `adapter-bochs/machine_facade`, the sole mechanical caller of Bochs-core.

## Procedure and observations

1. Added only individual typed mechanical register operations to
   `adapter-bochs/machine_facade`; they have no selector, BOP, OpenNT, DOS,
   VDM, WOW or Win32 meaning.
2. Added `adapter-softpc/mvdm_command_registers`, preserving the reached
   original source names and fixed widths.  It owns the compatibility ABI and
   calls the typed mechanical operations; it does not obtain a guest pointer.
3. Generated and executed the formal Ninja fixture on both MSVC x64 `/MT`
   and x86 `/MT`:

   ```text
   PASS: COMMAND source-shaped register boundary
   ```

4. Compiled the real `adapter-bochs/machine_facade.cc` on both architectures
   against the current Bochs headers using the existing Windows configuration
   include selection (`/DWIN32` plus `instrument/stubs`).  Both compiles
   completed with only pre-existing upstream Bochs warnings.

## Interpretation

The reached COMMAND register interface is now adapter-backed and
selector-blind.  S9's bounded `mvdm_guest_location` remains the only legal
guest-memory mechanism; no second mapping manager or raw `GetVDMAddr`
compatibility facade was introduced.  The next group may make only the
ledgered minimal source-body bindings for COMSPEC, current-directory and
initial-environment guest spans.

## Limits

This does not enable a `54:xx` route, host environment policy, a CCPU
executor, generic frame ABI, full Bochs link or a guest trace.
