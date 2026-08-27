# M0 T280 S17 — `demsrch.c` remaining x64 boundary evidence

## Question

After S16 fixed only the DOS-resident find-list identity, which remaining
`demsrch.c` width-sensitive operations must be recovered before an original
DEM directory-search provider can be enabled, and which session mapping
instance—if any—applies to each value?

## Inputs

- Selected OpenNT mirror: `src/opennt-mvdm-host/dos/dem/demsrch.c`.
- S16 x64 and x86 syntax observations and DOS-layout fixture.
- Existing `adapter-win32` `ntioapi`/RTL and current `NtVdmControl` bindings.
- Current architecture and coding rules for `adapter-vdm-monitor`,
  `adapter-softpc`, and the three per-session mapping-manager instances.

## Procedure

1. Re-ran the formal MSVC `/MT /W4` Ninja syntax graph for the original DEM
   body on x64 and x86 after the S16 source-local change.
2. Reviewed every x64 C4311/C4312 site and its enclosing original control
   path, then swept the non-warning `GetVDMAddr` entry points and all reached
   directory-query/control calls.
3. Classified each value as guest numeric, guest synchronous pointer lease,
   host-local pointer, host-local handle, opaque persisted host identity or
   nonfunctional source diagnostic.

## Observations

- The full result is the eight-row [boundary ledger](m0-t280-s17-demsrch-x64-boundary-ledger-001.tsv).
- S16 is the only persisted host-identity change: `PFFINDLIST` uses the
  `host_resource` mapper. None of the remaining pointer/HANDLE warnings is a
  candidate for that mapper.
- Directory handles, `IO_STATUS_BLOCK`, `VDMQUERYDIRINFO`, Rtl heap buffers,
  pathname pointers and device-name pointers are all host-local synchronous
  values. They must remain native-width inside their owner boundary.
- The original `GetVDMAddr` calls are different: their segment:offset inputs
  are guest numeric addresses, and their temporary native pointer can exist
  only through the existing `session.guest_memory` lease route owned by
  `adapter-softpc`.
- The current `VdmQueryDir` same-shaped facade is source-useful but in the
  wrong component (`adapter-win32`). Current architecture assigns the whole
  `NtVdmControl` family to `adapter-vdm-monitor`; its relocation is an
  explicit follow-on rather than a new S17 implementation.
- The C4100/C4101/C4459/C4996 diagnostics occur independently of pointer
  width. They remain classified source-hygiene evidence, not excuses to edit
  the original algorithms during an x64-boundary package.

## Interpretation

No additional mapping manager, mapper namespace, raw-pointer token, or Bochs
change is justified. The smallest recovery order is: complete the existing
same-shaped public-ntdll directory binding; repair host-local pointer/sentinel
expressions in the original mirror with registered minimal divergences; then
relocate the source-shaped VdmQueryDir facade to `adapter-vdm-monitor` before
admitting the fast reset path. A bounded `adapter-softpc` guest-memory lease
is separately required before any DEM BOP provider can dereference a guest
DTA/FCB.

## Confidence and follow-up

High confidence for value ownership and mapping exclusion: each result follows
the original storage and call sites, and both architecture syntax graphs pass.
This is not proof of a linked directory provider or guest execution. The
ledger is the admission input for the later DEM directory-search owner package;
that package must preserve original status/fallback behavior and add focused
x86/x64 runtime tests.
