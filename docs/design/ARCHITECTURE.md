# System Architecture

## Product boundary

`ntvdm.exe` is a non-invasive Windows CLI that hosts NT4-era DOS and bounded
WOW16 workloads without replacing Windows system files, rebuilding the kernel,
recreating a private NT subsystem, or requiring installation-time host
mutation. Public Win32 APIs and ordinary host resources remain valid integration
mechanisms.

The product has nineteen production source components. A source file has one
owner. Original mirrors preserve upstream package identity, adapters preserve
historical interface shape while translating mechanics, and project components
own composition, session lifetime and cross-process coordination.

## Mirror-preserving recovery model

Source recovery has a second product objective in addition to executable
closure: every adopted MVDM and non-MVDM OpenNT package must remain
recognisably comparable with its selected upstream source.  A successful build
does not justify absorbing an original package into adapters or replacing its
control flow with newly authored code.

Each original mirror may have one private paired `*-overlay` implementation
root.  That root is not an additional architectural component or a generic
compatibility layer: it is part of its matching mirror's implementation and is
compiled into that mirror's library only.  The mirror is the overlay's sole
caller and linker.  Adapters and project components call the mirror's original
or explicitly registered public boundary; they never call an overlay.

Consequently, an imported translation unit retains its selected upstream path,
name, data layout, function shape, original algorithm and failure order.  A
small changed include, declaration binding or one-line hook stays in the mirror
with a local `DIVERGENCE:` marker.  A material added mechanism is implemented
in the paired overlay and reached through the smallest possible hook.  If a
file would cease to be a byte-exact upstream file, true subset, or same-shaped
minimal modification, it is not admitted as a mirror file.  The mirror README
is the index of every crop, modified expression, hook and overlay binding.

## Package-first recovery boundary

`mvdm-host` is a complete canonical mirror of the selected non-guest MVDM
host package union. Recovery proceeds first across **original package
boundaries**, then through named interfaces between admitted packages; it does
not grow from a trace hit or compiler error one symbol at a time.

The source-function graph has a deliberately narrower zero-degree expansion
than the project-wide MVDM mirror inventory. Its base is every original
definition in `mvdm-host`; it then takes the transitive call closure only while
each resolved physical definition remains selected OpenNT `mvdm` source already
mirrored under a project `mvdm-*` component. The first resolved call that
leaves that original source universe is one-degree. An unrelated or unreachable
support, tool, firmware, or guest definition is not zero-degree. This is source
provenance only, not a runtime-link edge; all established tool, firmware, and
guest restrictions remain in force. Identity is physical—selected source path,
edition/provenance, signature and content identity where applicable—not a
function spelling.

An original OpenNT package outside `base/mvdm` is eligible for `opennt-host`
only when a complete-package audit proves all of the following. The audit may
select only the required original slice; it never implies importing unrelated
translation units from an otherwise accepted package:

- a selected `mvdm-host` package directly reaches its original service;
- its retained state machine, data layout, ordering, or failure semantics are
  substantial enough that an autonomous replacement would lose source value;
- its complete outgoing closure bottoms out in a public modern Win32 API, an
  existing bounded adapter, or a small specifically-owned adapter whose own
  contract is finite; and
- it does not require importing an NT4 platform/product shell such as CSRSS,
  the CSR transport, kernel VDM, Kernel32/BaseClient as a whole, Win32k, or
  USER/GDI server internals.

The stopping boundary is a package-interface boundary, not an arbitrary
source-directory boundary. For example, the Base VDM service protocol
(`VDMINFO`, command records, capacity/re-entry, wait/wake ordering) is an
eligible OpenNT-host service slice; the NT4 CSR/CSRSS transport below it is
not. A bounded `adapter-opennt-host` may preserve the reached CSR-facing call
shape and observable result, but never grows into a CSRSS replacement.

Before an adapter-owned implementation is extended, the package audit records
whether an admitted original OpenNT package supersedes it. That record is the
authority to migrate the implementation back to the original owner and
prevents permanent parallel providers.

## Components

### Original mirrors

- `mvdm-host`: the canonical original MVDM host-runtime mirror. It owns
  selected DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32, VDD/debugger,
  executable `softpc.new` packages—including `base/bios` reset/BIOS services
  and `base/keymouse` controller sources—SIM/monitor providers, and original
  package-internal `inc`, `oemuni` and `suballoc` support paths. It does not
  own standalone tools or immutable firmware media inputs.
- `opennt-host`: the canonical original non-MVDM OpenNT host-service mirror.
  It owns every complete, source-audited OpenNT host package accepted for use
  by `mvdm-host`; BaseSrv/client VDM is merely its first accepted service
  slice, not this component's boundary. It is neither a replacement MVDM
  provider nor a generic compatibility layer.
- `mvdm-tools`: the canonical original standalone MVDM tool mirror,
  including `vdmutils/forcedos`, `graftabl`, `pifedit` and `win` resources.
  Tools may be independently built but never enter the main `ntvdm.exe` link
  graph merely because their source is available.
- `mvdm-softpc-firmware`: the canonical original MVDM firmware-input mirror:
  selected `softpc.new/bios`, `softpc.new/roms` and `softpc.new/data` paths.
  It preserves immutable ROM and data inputs but is neither a host-runtime
  library nor a second machine executor. Executable `softpc.new/base/*`
  packages remain in `mvdm-host`.
- `mvdm-softpc-patch`: a narrow component for reviewed NTVDMx64-derived SoftPC
  patch bodies. It is neither a generic shim nor an alternate machine; original
  `mvdm-host/softpc.new` control flow remains in the mirror caller.
- `mvdm-platform-abi`: exact original declarations and contracts outside
  MVDM required to compile imported MVDM packages. It contains no replacement
  behavior.
- `mvdm-guest/dos/v86`: complete selected DOS/V86 guest source, resources,
  build descriptions, intermediates and original products.
- `mvdm-guest/bin86`, `mvdm-guest/wow16` and `mvdm-guest/font16`: the selected
  load-only Bin86, WOW16 and original 16-bit font carries. They do not imply
  an external WOW16 source-universe mirror.

### Mechanical adapters

- `adapter-mvdm-host-in`: selector-blind fixed-width machine-event/frame
  transport into the original MVDM host. It does not select, replace or
  execute a machine backend.
- `adapter-mvdm-host-out`: the sole OpenNT-facing historical-interface component.
  Its explicit internal families are `win32`, `softpc`, `monitor`, `redir`,
  `wow`, `vdd` and `debugger`. Each preserves only the corresponding reached
  original interface shape; none is an alternate MVDM provider. The `softpc`
  family binds the original executable SoftPC/CCPU40 call graph. It never
  includes a retired Bochs type, object or global, and does not provide a
  fallback or simultaneous executor.
  The `monitor` family owns same-shaped `NtVdmControl`, `VDM_TIB`, V86-event
  and interrupt/fault-handler facades, and unsupported kernel/CSRSS behavior
  fails deterministically. The remaining families preserve their named
  Redirector, WOW, VDD and debugger external boundaries without importing
  provider policy or retired-machine objects.
- `adapter-opennt-host`: the package-private OpenNT host-interface adapter.
  It owns only same-shaped substitutions for reached private-host calls from
  an accepted `opennt-host` package; it has no MVDM, guest, BOP or Bochs
  meaning. Its subfamilies remain named by the accepted original owner package
  rather than being merged into a generic compatibility layer.

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

Each session activates the original SoftPC/CCPU40 machine composition. There
is no alternate-machine selection, fallback or simultaneous executor. Current
functional machine, guest and MVDM-host acceptance records the Win32/x86
SoftPC CCPU40 row. Pure source, static-analysis and documentation work need
not create a machine session.

All project-owned session and adapter APIs are multi-instance-safe: no hidden
process-global current machine, mapping table or resource registry is allowed.
Imported MVDM code may retain its original process globals for the current
one-session-per-process profile. In-process multiple independent MVDM host
contexts require a separate reentrancy audit that classifies each original
global as immutable process state, per-session state, TLS/thread state,
guest-owned state or broker-owned state. It is not achieved by swapping an
unbounded block of globals.

The `monitor` family of `adapter-mvdm-host-out` binds the active session/`VDM_TIB`
context to each participating thread. Worker threads bind before entering
imported MVDM code and unbind on exit. No raw session pointer enters guest
state or a fixed-width component ABI.

## Dependency direction

```text
app -> session
app -> broker client -> broker process
app -> adapter-mvdm-host-in -> mvdm-host
app -> mvdm-guest/dos/v86 / mvdm-guest/bin86 / mvdm-guest/wow16 / mvdm-guest/font16
                                                   (data/load only)

mvdm-host -> mvdm-platform-abi
mvdm-host -> opennt-host                           (only an admitted original host-service package)
mvdm-host -> mvdm-softpc-patch                   (only registered SoftPC hooks)
mvdm-host -> adapter-mvdm-host-out
mvdm-host -> session                              (neutral contract only)
adapter-mvdm-host-out/softpc -> original mvdm-host/softpc.new
mvdm-softpc-patch -> adapter-mvdm-host-out/softpc
adapter-mvdm-host-in -> adapter-mvdm-host-out/softpc  (typed mechanics only)
adapter-mvdm-host-out/win32 -> broker client      (only for brokered historical calls)
opennt-host -> mvdm-platform-abi
opennt-host -> adapter-opennt-host                 (only source-audited package-private bindings)
opennt-host -> broker                              (only after package closure admits fixed-width transport)
mvdm-tools -> mvdm-host / mvdm-platform-abi       (independent tool builds only)
app -> mvdm-softpc-firmware                       (manifest-selected immutable input only)
```

`session` never calls a component-specific provider. No
`adapter-mvdm-host-out` family includes a retired-machine type or global. The
SoftPC path remains within the original `mvdm-host` source-shaped composition.
The broker never receives a native pointer, local HANDLE or guest pointer. It
exchanges versioned fixed-width copied messages and stable
cross-process identities only.

Each specialist adapter owns one historical external/product interface family;
it is not a convenience shim and it may not absorb another adapter's caller or
provider semantics. A missing interface is first assigned to this inventory,
then recovered with original source evidence, rather than edited out of an
OpenNT mirror.

`adapter-opennt-host` preserves the original spelling, ABI shape and observable
order for each reached accepted-package private-host interface family. It is
not a second generic Win32 shim and is consumed only by its owning
`opennt-host` package.

`mvdm-tools` has no inbound production-runtime edge at all.
`mvdm-softpc-firmware` has no host
compile or link edge; `app` stages an explicitly admitted, manifest-selected
immutable firmware input to the selected backend's source-shaped binding.

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
guest-image manifest and loads bytes through the selected backend binding; subsequent
communication is only BOP, interrupts, ports and guest-memory contracts.

Every mirror file is exact upstream, a registered true subset, or a registered
same-shaped minimal modification. A changed expression carries `DIVERGENCE:`
and a README register row. A material added implementation belongs in the
matching private `*-overlay`; only that mirror may call its overlay.

Existing project-owned component code is an audited recovery source, not
discarded work. Before authoring a replacement, a packet reviews applicable
current or quarantined `adapter-*`, `app`, `session` and related
owner candidates for provenance, dependency direction, behavior and tests. It
may selectively copy a compliant pure mechanic or adapter into its final owner
root, but never imports a whole tree by default. Retired Bochs material is not
an admissible production source, build or runtime input.
`src.old/` remains outside all formal source/build/link/runtime inputs after
such a per-file recovery.

## Non-goals

The architecture does not claim NT4 kernel VDM or CSRSS/CSR internals,
unbounded multi-session reentrancy, or automatic support for every dormant
MVDM package. Unsupported historical operations remain explicit failures.
