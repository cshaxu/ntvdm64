# OpenNT MVDM package-mirror rebootstrap

## Decision

Rebuild `src/` from a quarantined `src.old/` into thirteen production
components while preserving the non-invasive `ntvdm.exe` direct
COM/EXE/BAT/PIF launch contract. Recovery is package-led, not trace-led:
original source, same-shaped adapter, registered private overlay, then a
last-resort authored exception.

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

The source baselines are:

- `O:\repos.external\OpenNT\base\mvdm`;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`;
- `O:\repos.external\bochs-2.6-compat\bochs-2.6`.

OpenNT MVDM inputs form one canonical package-scope union. Each target-relative
path has one selected file. Identical inputs retain dual provenance;
one-sided inputs are included; conflicts are resolved at complete owning
package scope using source, build, resource and artifact lineage. The rejected
variant remains external evidence, not a second product path.

Owner mapping:

- `opennt-mvdm-host`: every selected non-guest/non-tool MVDM package,
  including MVDM includes, DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32,
  VDD/debugger, `softpc.new`, SIM/monitor, utility and OEM packages;
- `opennt-platform-abi`: exact required declarations outside MVDM;
- `opennt-guest-dos`: DOS/V86 and guest DPMI packages and products;
- `opennt-guest-wow16`: WOW16 packages and products;
- `tools/opennt`: historical build tools; never a runtime component.

The guest roots retain selected source, resources, build descriptions, tools
inputs, intermediates and products. Their objects/libraries never enter the
host graph. App loads manifest-selected immutable products through
`adapter-bochs`.

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

`src.old/` is comparison evidence and never a formal source/build/link/runtime
input. Audited project-owned app, session, broker-independent mechanics and
Bochs foundation may be recovered only by explicit per-file disposition.
OpenNT and Bochs mirrors come from pinned upstream baselines. Existing adapter
and mapping-manager code is evidence that may be reintroduced only after its
owner, width model and original caller contract pass review.

## Proposed subtasks

1. **S1 — Governance, package union and owner manifest.** Record the thirteen
   roots, per-path OpenNT union, Bochs identity, package-to-library plan,
   `src.old` quarantine, x86/x64 targets and acceptance scans.
2. **S2 — Session, 32-bit compatibility space and broker contracts.** Recover
   the single mapping-manager implementation with three typed per-session
   instances; prove x86/x64 identical token behavior, checked guest-memory
   leases, thread-bound monitor context, and define the versioned broker wire
   ABI plus one-session-per-process admission.
3. **S3 — Bochs foundation and app shell.** Restore audited `bochs-core`,
   `adapter-bochs` and minimal app composition without importing old BOP/host
   composition or claiming guest execution.
4. **S4 — Platform ABI and complete guest mirrors.** Import canonical
   `opennt-platform-abi`, DOS and WOW16 trees with provenance and load
   manifests; prove guest objects/libraries are absent from host links.
5. **S5 — Canonical MVDM host mirror.** Import the complete selected
   non-guest/non-tool MVDM tree under `opennt-mvdm-host`, preserving package
   topology and leaving unselected translation units dormant rather than
   rewriting them.
6. **S6 — Same-shaped adapters and broker implementation.** Recover
   `adapter-bop`, `adapter-softpc`, `adapter-win32`, `adapter-vdm-monitor` and
   the minimum cooperative broker registration/identity/queue/cleanup path.
   Every entry cites an original caller and deterministic unavailable result.
7. **S7 — Formal dual-architecture build and CLI composition.** Generate
   Ninja graphs for MSVC x86 `/MT` and x64 `/MT`, select initial original
   packages, verify no `src.old` input and produce a documented target
   admission result without overstating runtime/provider closure.
8. **S8 — Whole-tree and boundary audit.** Hash/compare mirrors, verify every
   divergence/overlay, dependency direction, mapping/broker negative tests,
   guest load-only isolation and publish the next package-led work order.

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

This task does not complete every dormant MVDM provider, reconstruct CSRSS or
NT4 kernel VDM, prove in-process multiple imported MVDM contexts, or claim
full DOS/WOW execution continuity. It establishes the clean package mirrors,
width-safe foundations and complete interface boundaries needed for later
owner-package recovery.
