# M0 T281 S1 — original BOP dispatch composition plan

## Purpose

Start Wave 2 through the smallest reached original host-control body:
`softpc.new/host/src/nt_bop.c`. The result is a formal x86 and x64 object
composition of that original dispatcher through declared adapter boundaries.
Its selector table is not enabled in a machine session by this S.

## Source-first disposition

1. **Original source:** the original `nt_bop.c` mirror is the tested body;
   its dispatch order, names, declarations, and failure paths are retained.
   The sole admitted mirror divergence is `MVDM-HOST-DIV-008`: three explicit
   historical function-pointer conversions required by modern x86 MSVC.
2. **Same-shaped bindings:** only pre-recorded `adapter-bop`,
   `adapter-softpc`, `adapter-win32`, `session`, and named successor
   declarations may satisfy an import. The source file does not call Bochs.
3. **External intrusion:** none is admissible; Bochs is unchanged.
4. **New behavior:** none is admissible. A missing required interface is
   recorded in the tracker and blocks the object rather than gaining a stub.

## Delivery sequence

1. Reconcile every direct `nt_bop.c` import with the T277 interface ledger and
   T280/S2 source-range ledger; classify it as direct, an existing same-shaped
   binding, a missing boundary, or a later-owner branch.
2. Build one formal Ninja object graph for x86 and x64 using only the original
   dispatcher and its declaration carriers. Compiler errors are evidence, not
   permission to add selector stubs.
3. If imports are satisfied, link an isolated original-dispatch static library
   and run a selector-disabled surface fixture. If a boundary is missing, stop
   at that smallest recorded interface and revise the active brief before any
   implementation.
4. Record hashes, diagnostics, binding selection, and non-enabled branches.

## Scope

- Original `nt_bop.c` and direct declaration carriers;
- formal x86/x64 Ninja object or static-library island under
  `build/M0-T281/S1/`;
- interface/build tracker state, focused evidence, and a mirror README entry
  only if an admitted binding diff is required.

## Exclusions

- DEM provider bodies, DOS namespace/filesystem behavior, and selector enablement;
- new providers; VDD, debugger, monitor, Redirector, or WOW branches;
- Bochs changes, CPU execution, direct Bochs types in OpenNT code, and guest
  traces;
- generic compatibility, a family-private mapper, or raw guest-pointer retention.

## Exit criteria

- Every direct imported symbol has an original source location, exact declared
  form, named owner, and x86/x64 composition disposition.
- The original body, other than registered `MVDM-HOST-DIV-008`, compiles
  through existing same-shaped bindings, or each
  smallest missing binding is recorded and the S is revised before it is built.
- A selector-disabled proof shows no provider/BOP route became enabled.
- Documentation governance and `git diff --check` pass before P1 acceptance.
