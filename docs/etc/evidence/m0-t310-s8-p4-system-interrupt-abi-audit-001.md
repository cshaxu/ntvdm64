# M0 T310 S8 P4 — System-Interrupt ABI Audit

## Scope

This package audit covers the reached original `nt_timer.c` heartbeat and the
Redirector-facing `SoftPcEoi` wrapper in `nt_eoi.c`.  It is confined to the
S8 whole-tree scope: `mvdm-host` and `mvdm-support`.

## Source findings and disposition

- `host_GetSysTime` passed `IS32 *tv_usec` to the preserved NTDLL
  `RtlExtendedLargeIntegerDivide` ABI, which requires `PULONG`.  The source
  result is the positive remainder of division by 1,000,000, so a local
  `ULONG` remainder followed by an explicit bounded `IS32` assignment keeps
  the original timer value and call order while making the pointer contract
  exact.
- The original VDM Redirector export is `SoftPcEoi(int, int *)`, while the
  original selected PIC entry is `ica_eoi(ULONG, LONG *, int)`.  The native
  32-bit carrier shapes happen to match on NT4 but the call is not a valid
  checked C ABI.  The private overlay copies to/from a `LONG` local and calls
  only the exact typed PIC leaf; it neither routes an interrupt nor changes
  PIC ownership, timing, or session policy.

Neither item is a guest/host identity conversion.  The session mapping manager
is intentionally not used: timer and EOI values here are local scalar/PIC
state, not guest-visible external object identities.

## Explicit non-scope

The original heartbeat's BDA access, PIC/ICA locking, RTC tick delivery,
session cancellation, and real runtime interrupt scheduling retain their
existing owners.  The remaining original non-width warnings in these files are
not suppressed or changed by this S8 package repair.

## Verification

The formal `original-softpc-candidate` target was regenerated and executed for
x64 and x86.  Both builds compiled and linked the original timer/EOI sources
and `mvdm_ica_eoi_bridge` with no package C4057 or error.  This is an ABI build
closure, not a claim of timer, PIC, Redirector, or VDD runtime closure.
