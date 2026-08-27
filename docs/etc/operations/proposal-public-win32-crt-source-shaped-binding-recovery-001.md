# Proposal — public Win32/CRT source-shaped binding and facade recovery

## Placement

This is the first candidate task after the active M0 T294 first-degree audit.
It must not receive a numeric T identifier or enter `STATUS.md` until T294
has passed its own closure gate and the owner admits this proposal.

## Purpose

T294 identifies public Win32/CRT *leaf call sites*: they terminate the
OpenNT-source graph, but they are not automatically modern-compatible.  This
task recovers those boundaries without changing the original MVDM callers'
interface shapes, ordering, return/error contracts or ownership rules.

The input population is the final T294 public-leaf rows (currently 1,065 call
sites).  A call site is not an implementation unit: all rows with the same
source declaration identity, linkage, calling convention and ABI form map to
one interface identity.  Different A/W, ANSI/OEM conversion, 16/32-bit value,
structure-layout, import-library or calling-convention forms stay distinct.

## Required S sequence

1. **S1 — Freeze public interface identities and call-site mapping.**
   Consume every T294 public-leaf row and record its original declaration,
   header/path/hash, DLL/import form, ABI/calling convention, caller set and
   x86/x64 sensitivity.  Assert that every public row maps to exactly one
   interface identity and no source spelling alone merges incompatible forms.
2. **S2 — Direct-binding audit.**
   For each identity, prove whether a supported modern public Win32 API or
   current MSVC CRT export is source- and ABI-compatible.  Direct binding is
   allowed only with evidence for return/error, Unicode/OEM, ownership and
   x86/x64 behavior.
3. **S3 — Same-shaped facade recovery.**
   Where direct binding is insufficient, implement the smallest named facade
   under `adapter-mvdm-host-out/win32`.  Preserve the original name, argument
   shape, observable order and failure result at the OpenNT-facing boundary;
   modern public Win32/CRT is an implementation detail behind it.  Use the
   shared session mapping manager for every guest address, pointer-like value
   or host-resource identity; do not create a family-private mapper.
4. **S4 — Explicit unavailable and negative behavior.**
   For identities that cannot safely retain the historical contract on modern
   public APIs, record the first failed recovery rung and implement only the
   original source-shaped failure/unavailable outcome.  Private CSR/CSRSS,
   kernel VDM, Win32k, USER/GDI server and analogous product shells remain
   outside this task even if a prior classifier mislabeled a spelling public.
5. **S5 — Architecture matrix and reversion review.**
   Compile and test each admitted identity on MSVC x86 `/MT` and x64 `/MT`.
   Verify direct, facade and unavailable paths; check Win32 error direction,
   copied/opaque ownership, token cleanup and no raw host-pointer leakage.
   Remove a pre-existing autonomous binding only when the ledger names it as
   superseded and the source-shaped replacement has passed its tests.

## Boundaries

- This task is limited to the final T294 public Win32/CRT population.  It does
  not inspect second-degree bodies, import another OpenNT product package, or
  enable a BOP/provider merely because a binding compiles.
- `adapter-mvdm-host-out/win32` is the only default owner for historical MVDM
  public-platform facades.  A distinct adapter requires a demonstrated
  owner-package boundary, not a convenience split.
- A current public API validates a selected original boundary; it does not
  authorize replacement of an original MVDM algorithm or policy.
- No call is assumed compatible merely by identical spelling.  Deprecated,
  unavailable, undocumented, private, architecture-dependent and CRT-version
  dependent cases receive their own disposition.

## Exit criteria

- Every final T294 public-leaf call-site row maps to one audited interface
  identity; the mapping has no gaps or duplicate incompatible merges.
- Each identity has exactly one disposition: `direct-binding`,
  `adapter-backed`, or `source-shaped-unavailable`, with original declaration
  evidence and an owner.
- Every adapter-backed identity has a four-rung source-recovery record,
  focused x86/x64 positive and negative proof, and no raw host pointer/handle
  crossing an MVDM or guest boundary.
- Every direct binding has supported modern API/CRT evidence and ABI/error
  behavior proof on both supported architectures.
- Documentation governance, `git diff --check`, focused build/test matrices,
  diff review, commit and normal remote push pass.

## Explicit non-goals

- Recreating private NT4 product shells or treating them as public Win32.
- Modifying modern Windows, its kernel, registry or system files merely to
  install this product.
- Rewriting DEM, COMMAND, XMS, DPMI, Redirector, WOW, VDD, debugger or SoftPC
  package algorithms in an adapter.
