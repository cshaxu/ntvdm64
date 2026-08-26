# OpenNT MVDM package-mirror rebootstrap program

## Decision

Reconstruct `src/` into thirteen production components while preserving the non-invasive `ntvdm.exe` direct
COM/EXE/BAT/PIF launch contract. Recovery is package-led, not trace-led:
original source, same-shaped adapter, registered private overlay, then a
last-resort authored exception.

The quarantined existing project tree is an important audited reference, not
discarded work: existing `app`, `session`, `bochs-core` and `adapter-*` files
may be copied into their new owner roots or used as implementation references
after a per-file provenance, owner, dependency and architecture review.
`src.old/` itself remains outside all formal source/build/link/runtime inputs.

The current profile binds one active imported MVDM host context to each
`ntvdm.exe` process and permits multiple processes. DOS child programs,
COMMAND re-entry and WOW16 tasks are intra-session lifecycles. Project-owned
session/adapter contracts are multi-instance-safe; imported MVDM globals are
not rewritten for in-process multiple sessions until a package-wide
reentrancy audit proves the required state split.

A formal `broker` component recovers required cross-process registration,
identity, command queue, notification, lease and cleanup contracts through
public IPC. It does not recreate CSRSS/BaseSrv internals or transport local
machine/resource identities.

Both x86 and x64 hosts present imported MVDM code with a session-owned 32-bit
compatibility object space. One mapping-manager implementation has separate
`guest_memory`, `host_resource` and `completion_callback` instances per
session. Opaque native x86/x64 resources receive surrogate32 identities on
both host architectures; numeric data retains original semantics.

## Production roots

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
  broker/
  app/
```

## Canonical source union

The host and machine source baselines are:

- `O:\repos.external\OpenNT\base\mvdm`;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`;
- `O:\repos.external\bochs-2.6-compat\bochs-2.6`.

Non-guest OpenNT MVDM inputs form one canonical package-scope union. Each target-relative
path has one selected file. Identical inputs retain dual provenance;
one-sided inputs are included; conflicts are resolved at complete owning
package scope using source, build, resource and artifact lineage. The rejected
variant remains external evidence, not a second product path.

Owner mapping:

- `opennt-mvdm-host`: every selected non-guest/non-tool MVDM package,
  including MVDM includes, DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32,
  VDD/debugger, `softpc.new`, SIM/monitor, utility and OEM packages;
- `opennt-platform-abi`: exact required declarations outside MVDM;
- `opennt-guest-dos`: the already-complete repository-local DOS/V86 and guest
  DPMI mirror, carried forward without a second external-tree import;
- `opennt-guest-wow16`: the already-complete repository-local WOW16 mirror,
  likewise carried forward without a second external-tree import;
- `tools/opennt`: historical build tools; never a runtime component.

The guest roots carry forward the complete existing mirror's source, resources,
build descriptions, tools inputs, intermediates and products. Their
provenance/hash inventory is preserved and audited, but they are not copied
again from OpenNT/OpenNT-4.5 merely to recreate an already-complete mirror.
Their objects/libraries never enter the host graph. The existing
`build/output/dos` and `build/output/wow16` binary trees remain in place as
prior build outputs/evidence; they are not moved into production roots. App
loads manifest-selected immutable products through `adapter-bochs`.

## Adapter and session contracts

- `adapter-bochs` is the sole production caller of `bochs-core`.
- `adapter-bop` is selector-blind copied-frame transport.
- `adapter-softpc` preserves reached SoftPC/CCPU/SAS calls and uses only typed
  `adapter-bochs` mechanics.
- `adapter-win32` preserves unavailable historical Win32/NTDLL calls using
  public Win32 APIs.
- `adapter-vdm-monitor` declares and dispositions the complete same-shaped
  `NtVdmControl`/`VDM_TIB`/V86-event/handler family from the start.
- `session` owns per-instance mappings/resources/events and thread-bound
  monitor context; no hidden singleton is allowed.
- `broker` owns only cross-process coordination through versioned copied IPC.

The mapping manager allocates candidates monotonically from zero, skips
source-proven ABI sentinels, keeps bidirectional lookup and stale tombstones,
and does not reuse IDs during a session. Guest-memory pointers require checked
synchronous leases. HANDLE/pointer-bearing structures are translated to
native x86/x64 layouts inside the owning adapter.

## `src.old/` policy

`src.old/` is comparison/reference evidence and never a formal
source/build/link/runtime input. Audited project-owned app, session,
broker-independent mechanics, Bochs foundation and adapter code may be copied
from it into their new roots only by explicit per-file disposition. OpenNT and
Bochs mirror identity still comes from their pinned upstream baselines.
Existing mapping-manager code is evidence that may be reintroduced only after
its owner, width model and original caller contract pass review.

## Candidate task sequence

This document is the program-level design authority. It is not itself an
admissible T packet. The queue carries the following five bounded candidates
in order:

1. [Repository and neutral runtime foundation](proposal-rebootstrap-neutral-runtime-foundation-001.md)
   establishes the owner/provenance manifest, `session`, the shared
   mapping-manager implementation, broker wire contract and host-width tests.
2. [Bochs machine foundation](proposal-rebootstrap-bochs-machine-foundation-001.md)
   establishes the `bochs-core` / `adapter-bochs` mechanical closure and the
   smallest app-owned machine shell.
3. [Canonical OpenNT source supply](proposal-rebootstrap-opennt-source-supply-001.md)
   imports the platform ABI, guest mirrors and selected MVDM host package
   union without claiming provider execution.
4. [Historical interface adapter recovery](proposal-rebootstrap-historical-interface-adapter-recovery-001.md)
   recovers same-shaped BOP, SoftPC, Win32 and VDM-monitor interfaces plus the
   minimum cooperative broker implementation.
5. [Dual-architecture product composition](proposal-rebootstrap-dual-architecture-product-composition-001.md)
   creates both formal Ninja graphs, links admitted original packages, wires
   the CLI and performs the complete production-boundary audit.

Each candidate receives its numeric T identifier only when admitted. A later
candidate cannot silently pull an earlier candidate's unfinished work into its
scope. The five documents divide this former eight-S outline by independently
verifiable dependency closure; they do not split the thirteen components into
separate component-only tasks.

## Exit criteria

- `src/` contains exactly the thirteen declared roots; no transitional or
  stale component root is a production input.
- Every selected original file has path/hash/provenance and a package-scope
  OpenNT/OpenNT-4.5 disposition.
- Mirrors satisfy exact/subset/same-shaped rules and all material differences
  are registered private overlays.
- Both host architectures compile the admitted graph with MSVC `/MT`; x86 and
  x64 use the same surrogate mapping behavior.
- Mapping tests cover reserved zero/sentinels, bidirectional stability, stale
  IDs, type/instance isolation, overflow, teardown and checked memory leases.
- One app process owns one admitted MVDM session; multiple app processes can
  register independently with the broker; no local pointer/HANDLE/token crosses
  IPC.
- The monitor family has a complete operation/disposition ledger and bounded
  thread binding; unsupported kernel/CSRSS behavior is explicit.
- Guest inventories are complete, host-link exclusion is proved, and every
  loadable image is selected by manifest.
- `src.old/` is absent from every formal manifest/include/build/runtime path.

## Non-goals

The program does not complete every dormant MVDM provider, reconstruct CSRSS
or NT4 kernel VDM, prove in-process multiple imported MVDM contexts, or claim
full DOS/WOW execution continuity. It establishes the clean package mirrors,
width-safe foundations and complete interface boundaries needed for later
owner-package recovery.
