# OpenNT MVDM package-mirror rebootstrap

## Decision

Rebuild `src/` as a clean component layout after the owner quarantined the
previous implementation at `src.old/`.  The product contract remains a
non-invasive CLI `ntvdm.exe` that directly runs admitted `.com`, `.exe`,
`.bat` and `.pif` targets.  The restart changes source ownership and recovery
method, not the user-facing launch model.

The primary implementation source is the canonical package superset selected
from the complete local OpenNT and OpenNT-4.5 MVDM trees, not individual
BOP-family files copied after a trace hit. The two baselines are inputs to one
merged component tree; they are never retained as parallel edition roots in
`src/`. The recovery order is mandatory:

1. import and compose the original OpenNT package unchanged;
2. preserve the reached historical function signature, parameter ABI, data
   layout, call order and failure contract through a smallest adapter;
3. admit a narrow original-mirror overlay only when that adapter cannot make
   the original translation unit composable;
4. author a new interface or algorithm only under an explicit exception.

`adapter-softpc` supplies same-shaped historical SoftPC/CCPU/SAS calls using
Bochs mechanical operations. `adapter-win32` supplies same-shaped historical
Win32/NTDLL calls using supported public Win32 APIs. `adapter-vdm-monitor`
supplies the complete historical kernel-VDM-facing interface family required
by imported user-mode MVDM code—`NtVdmControl`, `VDM_TIB`, monitor events and
interrupt/fault-handler installation—through a bounded single-session state
model and app-installed machine endpoint. `adapter-bop` remains selector-blind
copied-frame ingress. None becomes a replacement OpenNT BOP provider or a
reimplementation of the NT4 kernel/product shell.

## Source And Component Map

The new production roots are exactly twelve:

```text
src/
  bochs-core/
  opennt-mvdm-host/
  opennt-platform-abi/
  opennt-guest-dos/
  opennt-guest-wow16/
  adapter-bochs/
  adapter-bop/
  adapter-softpc/
  adapter-win32/
  adapter-vdm-monitor/
  session/
  app/
```

OpenNT packages are copied from the pinned local OpenNT baseline with their
upstream relative paths retained below the owning root. The two pinned source
roots are:

- `O:\repos.external\OpenNT\base\mvdm`;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`.

The import is a canonical single-tree union. For each target-relative path,
identical inputs produce one file with both provenances; a path present in only
one baseline is included from that baseline; a conflicting path is selected
once after auditing the complete owning package, its build descriptions,
resources and artifact lineage. A conflict is never resolved by arbitrary
file-level precedence. The unselected version remains in its external source
tree as evidence and does not receive a second product path.

The owner map is:

- `opennt-platform-abi`: admitted exact declarations outside MVDM required by
  imported MVDM packages;
- `opennt-guest-dos`: `base/mvdm/dos/v86` and `base/mvdm/dpmi`;
- `opennt-guest-wow16`: `base/mvdm/wow16`;
- `opennt-mvdm-host`: every selected non-guest/non-tool MVDM package,
  including `base/mvdm/inc`, `dos/dem`, `dos/command`, `xms.486`, `dpmi32`,
  `vdmredir`, `wow32`, VDD/debugger packages, `softpc.new`, `sim16`, `sim32`,
  `v86/monitor`, `suballoc`, `vdmutils`, `oemuni` and other admitted MVDM
  support. It may yield many libraries without splitting source ownership.

`opennt-guest-dos` and `opennt-guest-wow16` are guest-image supply components,
not host libraries. Their canonical merged trees retain all selected original
source, resources, build descriptions, tools inputs, intermediates and product
artifacts, including applicable `.obj`, `.lib`, `.exe`, `.com`, `.sys`,
`.bin`, `.dll`, `.fon` and `.sym` files. Host compilation never includes or
links those objects. A guest-image manifest selects immutable binaries for
`app` to load through `adapter-bochs`; the guest reaches the host only through
historical machine-visible contracts such as BOPs, interrupts, ports and
guest-memory layouts.

`tools16`, `bin86`, `convert`, `dat2obj` and other historical build tools move
only to `tools/opennt`.  Kernel VDM/V86 monitor, NT process-manager and
CSR/BaseSrv source stay outside the runtime build closure; they may provide
source/ABI evidence, but are not silently recreated.

## Migration Policy

`src.old/` is quarantined comparison material and never a formal source,
build, link or runtime input.

- `app`, `session`, `bochs-core` and `adapter-bochs` may be restored from
  `src.old` only after per-file owner/provenance review.  `bochs-core` must
  still pass the Bochs upstream mirror/overlay audit; `app` and `session` must
  retain only their stated composition/neutral responsibilities.
- No existing MVDM-host or guest implementation is copied from `src.old` into
  the new mirror roots. Their
  source comes directly from the canonical OpenNT/OpenNT-4.5 package union.
- Existing `adapter-bop`, `adapter-softpc` and `adapter-win32` code is audit
  evidence only.  Each new implementation is introduced only after the
  original caller and historical interface are identified, and retains that
  interface shape whenever possible.

## Proposed Subtasks

1. **S1 — Rebootstrap governance and source-owner manifest.** Pin exact
   OpenNT and OpenNT-4.5 source roots; create the twelve-root manifest and
   per-path union-decision ledger; record the package map, external NT4
   kernel/CSR exclusions, `src.old` quarantine and acceptance scans. Define
   the complete source/ABI/state/failure ledger for the source-proven
   `NtVdmControl`/`VDM_TIB`/monitor interface family owned by the formal
   `adapter-vdm-monitor` component.
2. **S2 — Restore neutral/project and Bochs mechanical foundations.** Restore
   audited `app`, `session`, `bochs-core` and `adapter-bochs` material from
   `src.old`; re-establish the one-way Bochs boundary without importing old
   BOP or host composition.
3. **S3 — Import platform ABI and guest packages.** Mechanically import
   the canonical merged `opennt-platform-abi`, DOS guest and WOW16 guest trees
   with hashes, upstream-relative paths, per-file source selection and README
   registers. Retain the complete selected guest binary/intermediate inventory
   and prove it is absent from the host compile/link graph.
4. **S4 — Import canonical MVDM host package.** Mechanically import the
   canonical merged complete non-guest/non-tool MVDM package tree under one
   `opennt-mvdm-host` root; retain package-internal boundaries but do not
   create conceptual source components for BOP, host, SoftPC or utilities;
   do not rewrite files merely to compile them.
5. **S5 — Recreate adapter contracts from original interfaces.** Establish
   the same-shaped `adapter-win32`, `adapter-softpc`, `adapter-vdm-monitor`
   and selector-blind `adapter-bop` declarations/implementations, with a
   caller-to-adapter ledger and explicit unavailable cases. The monitor
   adapter is created as a complete interface-family boundary from the start:
   every historical operation has a disposition and deterministic failure
   behavior even when its machine/host capability is not yet enabled.
6. **S6 — Formal build selection and CLI composition recovery.** Generate the
   new Ninja manifest; select the first original translation-unit packages;
   verify no `src.old` input; prove `ntvdm.exe` target admission for COM, EXE,
   BAT and PIF without claiming unavailable provider/device/WOW behavior.
7. **S7 — Whole-tree mirror and boundary audit.** Hash/compare every imported
   package, verify all diffs/overlays, scan dependency direction and publish
   the next source-recovery work order from package dependencies rather than
   trace order.

## Exit Criteria

- `src/` contains only the twelve declared component roots and no
  transitional `src/bx-vdm`, `src/cli`, generic `src/opennt`, or stale guest
  root.
- Every imported OpenNT file has an upstream path/hash and is exact, true
  subset, or registered minimal same-shaped modification; all nontrivial
  semantic divergences are private overlays.
- Every OpenNT target-relative path has exactly one selected file and a
  reproducible OpenNT/OpenNT-4.5 union disposition. Conflicting files are
  selected at package scope with the rejected alternative and rationale
  recorded.
- Guest source, resources, intermediates and original binary products are
  complete in their canonical merged components; no guest `.obj` or `.lib`
  enters the host graph, and every loadable guest image is selected by an
  explicit guest-image manifest.
- `src.old/` is absent from every source/build/link/runtime manifest and
  include path.
- Every new adapter entry has a cited original caller and same-shaped interface
  ledger; no adapter owns BOP selector/service semantics.
- The formal Ninja graph is generated from the new source-owner manifest and
  produces a documented CLI target-admission result; unimplemented source
  packages remain explicitly unselected rather than silently replaced.

## Non-goals

This task does not claim full NT4 kernel VDM, CSRSS/BaseSrv multi-session,
VDD, debugger, Redirector, WOW or device closure.  It does not use guest trace
hits to select leaf implementations, does not rewrite Bochs, and does not
delete `src.old`. It establishes the complete bounded
`adapter-vdm-monitor` interface family but does not claim that unsupported
NT4 kernel, multi-session or CSRSS mechanisms have been recreated.
