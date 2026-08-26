# Proposal: MVDM package dependency and interface-closure tracker

## Purpose

Before any non-guest MVDM provider package enters the enabled host graph,
produce a complete package-to-symbol dependency closure. This replaces
compile-error-led adapter growth with an import-first, tracker-led recovery
sequence.

## Scope

The audit covers the 23 selected `opennt-mvdm-host` package roots:

`dirs`, `makefil0`, `inc`, `oemuni`, `suballoc`, `vdmutils`, `softpc.new`, `dos`, `xms.486`,
`sim16`, `sim32`, `v86`, `ieuvddex`, `dpmi`, `dpmi32`, `vdmredir`, `wow32`,
`fax`, `vdd`, `bde`, `dbg`, `vdmdbg`, and `vdmexts`.

It separates header/utility roots from source-owner packages, and separately
records non-enabled SoftPC CPU bodies whose execution ownership is Bochs.
Guest mirrors are load-only and outside this host compile closure.

All 1,689 selected host paths remain original mirrors regardless of current
build disposition. A path that is not enabled in the current profile is not an
import failure and is never deleted merely because a historical dependency is
unavailable.

## Exact composition classifications

The tracker must produce exact file and reached-symbol counts for these four
mutually exclusive classifications. Planning percentages are prohibited.

- **`exact-direct`**: byte-exact mirror source enters the declared formal
  x86/x64 build without an adapter-owned historical dependency.
- **`exact-adapter-bound`**: byte-exact mirror source enters the declared
  formal build through same-named, same-shaped adapter declarations and links.
- **`minimal-binding-diff`**: the source requires only registered include,
  import, compile-condition or link-binding changes. Every changed expression
  carries `DIVERGENCE:` and a mirror README row; no original algorithm moves.
- **`mirror-only/profile-excluded`**: the source remains byte-exact in the
  canonical mirror but is not enabled in this product profile. This includes
  SoftPC CPU execution bodies replaced by Bochs and paths requiring an
  explicitly unavailable NT4 kernel/CSRSS product boundary.

For each class, the tracker separately reports total selected paths, source
files, build-control/resource files, reached symbols, and unresolved symbols.
No classification may hide a file outside the canonical mirror.

## Required tracker row

Every reached source symbol and dependency records:

1. selected MVDM package, source-relative file and caller symbol;
2. dependency symbol/global/type/callback/resource/build input and original
   declaration/calling convention/layout evidence;
3. dependency class: package-internal, another selected MVDM package,
   `adapter-bop`, `adapter-softpc`, `adapter-win32`, `adapter-vdm-monitor`,
   `adapter-redir`, `adapter-wow`, `adapter-vdd`, `adapter-debugger`,
   `session`, `broker`, or Bochs-replacement exclusion;
4. exactly one owner and a direct composability decision;
5. original-source, same-shaped adapter, registered intrusion, or last-resort
   authored rung disposition;
6. permitted mirror binding diff and `DIVERGENCE:`/README requirements;
7. prerequisite rows, topological wave, x86/x64 build target and focused
   positive/negative test; and
8. final state: unreviewed, direct, adapter-pending, explicit-unavailable,
   excluded, compiled, linked, or locally verified.

## Independent audit ledgers

The package tracker is a coordinated set of independent, machine-readable
ledgers. A status in one ledger cannot be inferred from another.

1. **File recovery ledger** — one row for each of the 1,689 selected paths:
   upstream selection/hash, package, file kind, mirror identity, final
   composition class, build/profile state, divergence/exception ID and final
   non-recovery reason where applicable. Its final class is exactly one of
   `exact-direct`, `exact-adapter-bound`, `minimal-binding-diff` or
   `mirror-only/profile-excluded`.
2. **Interface recovery ledger** — one family row plus one child row for every
   reached external function, global, callback, structure or resource
   interface. Each child records caller and declaration, ABI/layout, direct
   availability, unique owner, adapter requirement, semantic-exception status,
   failure rule and focused evidence. An unexpanded family row is not a
   completed interface audit.
3. **Package dependency ledger** — package-to-package and package-to-interface
   edges, including build-control inputs, source cycles, topological wave and
   blocked successor. It establishes ordering without using a trace hit.
4. **Build/profile ledger** — each declared x86/x64 target's original files,
   headers, libraries, compile conditions, enabled/profile-excluded state and
   compile/link/local-test evidence. It makes “mirrored but not linked” an
   explicit result.
5. **Divergence and exclusion ledger** — every `DIVERGENCE:` site, overlay,
   same-shaped binding change, Bochs-replacement exclusion and permanently
   unavailable NT4 product path. It records the original purpose, adapter or
   exception ID, reason, retained failure semantics and future disposition.

The initial versions of these ledgers are generated from the canonical T274
union and T275 debt records. The tracker task expands them; it never discards
an original file to make a count look better.

## Exit criteria

- Every selected package root has a complete import/disposition record.
- The four exact composition classifications report auditable counts for all
  1,689 selected host paths and for every reached symbol; no percentage estimate
  is used as an acceptance result.
- Every package proposed for the next recovery wave has no unresolved external
  dependency without a named adapter and successor task.
- The tracker gives a dependency-topological order and identifies package
  cycles explicitly; no future task uses trace order to choose an interface.
- Existing 12 family debt rows are expanded into actual reached declarations
  for each selected package.
- The five independent ledgers are regenerated from their canonical input and
  contain no duplicate file identity, interface identity or dependency edge.
- No provider body, adapter ABI or mirror change is enabled by this task.
