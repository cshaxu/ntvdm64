# System Architecture

## Product boundary

`ntvdm.exe` is a non-invasive Windows CLI that hosts NT4-era DOS and bounded
WOW16 workloads without replacing Windows system files, rebuilding the kernel,
recreating a private NT subsystem, or requiring installation-time host
mutation. Public Win32 APIs and ordinary host resources remain valid integration
mechanisms.

The product has twenty production source components. A source file has one
owner. Original mirrors preserve upstream package identity, adapters preserve
historical interface shape while translating mechanics, and project components
own composition, session lifetime and cross-process coordination.

## Components

### Original mirrors

- `bochs-core`: complete adopted Bochs 2.6 CPU, memory, firmware and PC-device
  mechanics. It has no OpenNT, DOS, WOW, BOP or host-policy knowledge.
- `opennt-mvdm-host`: the canonical original MVDM host-runtime mirror. It owns
  selected DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32, VDD/debugger,
  `softpc.new` and SIM/monitor provider packages. It does not own standalone
  tools or common support libraries.
- `opennt-mvdm-support`: the canonical original MVDM common support mirror:
  shared `inc` declarations/build carriers plus original `oemuni` and
  `suballoc` library packages. It may be independently built, but it enters a
  host runtime only after its exact interface audit admits it.
- `opennt-mvdm-tools`: the canonical original standalone MVDM tool mirror,
  including `vdmutils/forcedos`, `graftabl`, `pifedit` and `win` resources.
  Tools may be independently built but never enter the main `ntvdm.exe` link
  graph merely because their source is available.
- `opennt-mvdm-firmware`: the canonical original MVDM firmware-input mirror:
  selected `softpc.new/base/bios`, `softpc.new/bios`, `softpc.new/roms` and
  `softpc.new/data` paths. It preserves original source, ROM and data inputs
  but is neither a host-runtime library nor a second machine executor.
- `opennt-platform-abi`: exact original declarations and contracts outside
  MVDM required to compile imported MVDM packages. It contains no replacement
  behavior.
- `opennt-guest-dos`: complete selected DOS/V86 guest source, resources, build
  descriptions, intermediates and original products.
- `opennt-guest-wow16`: the selected load-only WOW16/bin86 carry plus an
  immutable in-place WOW16 product inventory. It does not imply an external
  WOW16 source-universe mirror.

### Mechanical adapters

- `adapter-bochs`: the only production caller of `bochs-core`; owns Bochs-only
  machine construction, bounded run/stop/resume and opaque mechanical endpoints.
- `adapter-bop`: selector-blind fixed-width machine-event/frame transport.
- `adapter-softpc`: same-shaped implementations of reached SoftPC/CCPU/SAS
  calls using bounded `adapter-bochs` mechanics.
- `adapter-win32`: same-shaped implementations of unavailable historical
  Win32/NTDLL calls using supported public Win32 APIs.
- `adapter-vdm-monitor`: the complete same-shaped user-mode interface family
  for `NtVdmControl`, `VDM_TIB`, V86 events and interrupt/fault-handler
  installation. It uses a bound session context and an app-installed opaque
  machine endpoint; unsupported kernel/CSRSS behavior fails deterministically.
- `adapter-redir`: same-shaped boundary for the original VDMREDIR/Redirector
  product-interface family. It never becomes a redirector provider; absent
  private transport or control surfaces fail explicitly.
- `adapter-wow`: same-shaped boundary for the original WOW32/WOWEXEC product
  interfaces. It does not contain guest NE/WOW provider logic.
- `adapter-vdd`: same-shaped boundary for original VDD product interfaces and
  host callbacks. It contains no VDD/provider implementation or Bochs object.
- `adapter-debugger`: same-shaped boundary for the BDE/DBG/VDMDBG/VDMEXTS
  product interfaces. It contains no debugger/provider policy.

### Project components

- `session`: dependency-neutral lifecycle, mappings, resource tables,
  completion/events and teardown for one independent VDM instance.
- `broker`: per-user cross-process VDM registration, identity, command queues,
  notifications, leases and cleanup. It restores required observable
  BaseSrv-style coordination contracts through public IPC without recreating
  CSRSS or inspecting arbitrary processes.
- `app`: CLI and final composition. It creates session instances, selects guest
  images, binds adapters and connects to or starts the broker.

## Runtime cardinality

The current runtime binds exactly one active imported MVDM host context to each
`ntvdm.exe` process. Multiple processes may run concurrently. Inside one
session, original DOS `EXEC`, COMMAND child/re-entry behavior and multiple
WOW16 tasks are guest/task lifecycles, not additional VDM sessions.

All project-owned session and adapter APIs are multi-instance-safe: no hidden
process-global current machine, mapping table or resource registry is allowed.
Imported MVDM code may retain its original process globals for the current
one-session-per-process profile. In-process multiple independent MVDM host
contexts require a separate reentrancy audit that classifies each original
global as immutable process state, per-session state, TLS/thread state,
guest-owned state or broker-owned state. It is not achieved by swapping an
unbounded block of globals.

`adapter-vdm-monitor` binds the active session/`VDM_TIB` context to each
participating thread. Worker threads bind before entering imported MVDM code
and unbind on exit. No raw session pointer enters guest state or a fixed-width
component ABI.

## Dependency direction

```text
app -> session
app -> broker client -> broker process
app -> adapter-bochs -> bochs-core
app -> adapter-bop -> opennt-mvdm-host
app -> opennt-guest-dos / opennt-guest-wow16     (data/load only)

opennt-mvdm-host -> opennt-platform-abi
opennt-mvdm-host -> opennt-mvdm-support
opennt-mvdm-host -> adapter-win32
opennt-mvdm-host -> adapter-softpc -> adapter-bochs
opennt-mvdm-host -> adapter-vdm-monitor
opennt-mvdm-host -> adapter-redir
opennt-mvdm-host -> adapter-wow
opennt-mvdm-host -> adapter-vdd
opennt-mvdm-host -> adapter-debugger
opennt-mvdm-host -> session                      (neutral contract only)
adapter-bop -> adapter-softpc                    (typed mechanics only)
adapter-win32 -> broker client                   (only for brokered historical calls)
opennt-mvdm-tools -> opennt-mvdm-support / opennt-platform-abi  (independent tool builds only)
opennt-mvdm-firmware -> adapter-bochs             (manifest-selected machine input only)
```

`session` never calls a component-specific provider. `adapter-bochs` alone
calls `bochs-core`. `adapter-softpc` never includes a Bochs type or global.
The broker never receives a native pointer, local HANDLE, guest pointer or
Bochs object. It exchanges versioned fixed-width copied messages and stable
cross-process identities only.

Each specialist adapter owns one historical external/product interface family;
it is not a convenience shim and it may not absorb another adapter's caller or
provider semantics. A missing interface is first assigned to this inventory,
then recovered with original source evidence, rather than edited out of an
OpenNT mirror.

`opennt-mvdm-support` has no automatic inbound runtime edge: a host package
may use it only after the package/symbol tracker records the original consumer,
exact interface shape and binding disposition. `opennt-mvdm-tools` has no
inbound production-runtime edge at all. `opennt-mvdm-firmware` has no host
compile or link edge; `adapter-bochs` may consume only an explicitly admitted,
manifest-selected immutable firmware input.

## Guest and host width model

Guest width and host pointer width are orthogonal:

```text
guest 16:16 / linear32 / opaque16-or-32
        -> original MVDM logic
        -> session-owned 32-bit compatibility object space
        -> native x86 or x64 HANDLE/pointer/resource
```

Both x86 and x64 builds use the same mapping path. Native x86 values are not
identity-passed merely because they fit in 32 bits. Imported MVDM code observes
historical 32-bit surrogate identities; native resources remain in adapters
and session state.

One mapping-manager implementation is instantiated separately per session and
resource class:

- `guest_memory`: guest 16:16/linear32 ranges to checked synchronous native
  memory leases;
- `host_resource`: MVDM surrogate32 identities to native HANDLE/pointer-sized
  resources;
- `completion_callback`: MVDM surrogate32 identities to asynchronous
  completion, child, event or callback records.

Each instance owns a monotonic allocator, forward and reverse lookup, reserved
ABI sentinels, stale tombstones and teardown. Candidate allocation begins at
zero and skips values reserved by that original ABI. Instances do not share a
numeric namespace, and IDs are not reused during the session lifetime.

Only opaque identity is tokenized. Lengths, offsets, flags, times, error codes
and guest addresses retain numeric meaning and receive explicit range/overflow
checks. A directly dereferenced guest pointer is a bounded epoch-scoped lease,
not a durable token. Structures containing pointers/HANDLEs are materialized
as native x86/x64 structures in the owning adapter and translated back.

Local surrogate IDs never cross the broker boundary. Broker session/request
identities use their own fixed-width wire contract.

## Source union and mirror rules

The selected OpenNT source is one package-scope union of
`O:\repos.external\OpenNT\base\mvdm` and
`O:\repos.external\OpenNT-4.5\nt\private\mvdm`. Each target-relative path
has one selected file. Identical inputs retain dual provenance; one-sided
inputs are included; differing inputs are selected at complete-package scope
using source, build, resource and artifact lineage. Parallel edition roots and
undocumented per-file hybrids are forbidden.

The two guest mirrors are load-only. Their C, assembly, objects and libraries
never satisfy a host symbol. `app` selects immutable products through a
guest-image manifest and loads bytes through `adapter-bochs`; subsequent
communication is only BOP, interrupts, ports and guest-memory contracts.

Every mirror file is exact upstream, a registered true subset, or a registered
same-shaped minimal modification. A changed expression carries `DIVERGENCE:`
and a README register row. A material added implementation belongs in the
matching private `*-overlay`; only that mirror may call its overlay.

Existing project-owned component code is an audited recovery source, not
discarded work. Before authoring a replacement, a packet reviews applicable
current or quarantined `bochs-core`, `adapter-*`, `app`, `session` and related
owner candidates for provenance, dependency direction, behavior and tests. It
may selectively copy a compliant pure mechanic or adapter into its final owner
root, but never imports a whole tree by default. Bochs 2.6 remains the sole
authority for `bochs-core` mirror identity: every retained project difference
is minimized, registered and moved to the private overlay when it is material.
`src.old/` remains outside all formal source/build/link/runtime inputs after
such a per-file recovery.

## Non-goals

The architecture does not claim NT4 kernel VDM, CSRSS/BaseSrv internals,
unbounded multi-session reentrancy, or automatic support for every dormant
MVDM package. Unsupported historical operations remain explicit failures.
