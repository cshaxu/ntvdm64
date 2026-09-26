# System Architecture

## Product boundary

`ntvdm.exe` is a non-invasive Windows CLI that hosts NT4-era DOS and bounded
WOW16 workloads without replacing Windows system files, rebuilding the kernel,
recreating a private NT subsystem, or requiring installation-time host
mutation. Public Win32 APIs and ordinary host resources remain valid integration
mechanisms.

Each production source file has one final owner. Original mirrors preserve
upstream package identity; the three executable roots own their process-local
mechanics; and the two small product roots own only stateless shared contracts.

T414 uses `src/mvdm/` as the physical canonical selected-OpenNT
`base/mvdm` tree. In this document, **MVDM host slice** (and retained shorthand
`mvdm-host` in logical dependency names) means the manifest-selected executable
portion of that tree, not a second filesystem root. Guest, tool and firmware
paths in the same tree remain subject to their existing no-link rules.

## Mirror-preserving recovery model

Source recovery has a second product objective in addition to executable
closure: every adopted MVDM and non-MVDM OpenNT package must remain
recognisably comparable with its selected upstream source.  A successful build
does not justify absorbing an original package into adapters or replacing its
control flow with newly authored code.

Consequently, an imported translation unit retains its selected upstream path,
name, data layout, function shape, original algorithm and failure order.  A
small changed include, declaration binding or one-line hook stays in the mirror
with a local `DIVERGENCE:` marker.  A material added mechanism belongs in its
named adapter/ABI family. New mirror paths must correspond to actual original
OpenNT files, retain their original relative paths and pass provenance and
package-boundary admission. They may contain byte-exact source, registered
true subsets or same-shaped minimal adaptations; project-invented files are
not admitted as mirrors. The mirror README indexes every crop and modified
expression. This permits recovering previously absent original files, not
moving autonomous implementations into newly invented mirror carriers.

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

## Capability recovery and host boundaries

Recovering a package means recovering the capabilities that its selected
original callers own, not merely compiling its translation units or resolving
their imports. A completed capability follows the original observable chain:
DOS or Win16 guest, original MVDM caller, original provider or a finite
same-shaped host boundary, modern host resource, return/error mapping, and
task/worker cleanup. Normal operation, representative failure, and teardown
are all parts of that one capability contract.

The recovery order is fixed: restore a directly composable original owner;
then bind its historical host interface through the smallest source-shaped
modern boundary; only then consider newly authored behavior. A mirror diff,
overlay, or executable-local binding remains only when its original owner,
ABI/layout, lifetime, failure ordering, and exact unavailability rationale
are recorded. Unclassified, convenience, or duplicate autonomous policy is
not an architectural boundary and must be removed.

An old service or medium that Windows no longer supplies (such as RAP or DLC),
or hardware/peer absent from the current machine (such as a CTS-capable serial
endpoint), does not make an otherwise reachable original caller disappear.
The product preserves its original failure behavior and never installs a fake
provider merely to claim success. It first exhausts non-invasive bindings to
ordinary modern host resources. If the remaining stop is genuinely external,
the debt ledger records the original caller/provider, attempted bindings,
missing prerequisite, retained seam, evidence, and exact condition for a
future real-host retest.

## Components

### Original mirrors

- `mvdm-host`: the canonical original MVDM host-runtime mirror. It owns
  selected DEM, COMMAND, XMS, DPMI32, VDMREDIR, WOW32, VDD/debugger,
  executable `softpc.new` packages—including `base/bios` reset/BIOS services
  and `base/keymouse` controller sources—SIM/monitor providers, and original
  package-internal `inc`, `oemuni` and `suballoc` support paths. Original
  MVDM monitor routines remain MVDM-owned even when they expose a kernel
  boundary. Non-MVDM kernel VDM semantic carriers belong to `opennt-host`,
  not this mirror. It does not own standalone
  tools or immutable firmware media inputs.
- `opennt-host`: the canonical original non-MVDM OpenNT host-service mirror.
  It owns every complete, source-audited OpenNT host package accepted for use
  by `mvdm-host`; BaseSrv/client VDM is merely its first accepted service
  slice, not this component's boundary. It is neither a replacement MVDM
  provider nor a generic compatibility layer. Separately admitted load-only
  OpenNT inputs retain their exact upstream paths here. Individually audited original
  kernel VDM slices from outside MVDM also belong here, retaining source
  paths and algorithm shape behind a finite standalone ABI; this does not
  admit a kernel product shell or a second executor. Adapters bind missing
  interfaces rather than reimplementing original provider policy.
- `mvdm-tools`: the canonical original standalone MVDM tool mirror,
  including `vdmutils/forcedos`, `graftabl`, `pifedit` and `win` resources.
  Tools may be independently built but never enter the main `ntvdm.exe` link
  graph merely because their source is available.
- `mvdm-softpc-firmware`: the canonical original MVDM firmware-input mirror:
  selected `softpc.new/bios`, `softpc.new/roms` and `softpc.new/data` paths.
  It preserves immutable ROM and data inputs but is neither a host-runtime
  library nor a second machine executor. Executable `softpc.new/base/*`
  packages remain in `mvdm-host`.
- Reviewed NTVDMx64-derived declarations or missing-provider fallbacks belong
  to the named adapter/ABI family. They never create a mirror component or a
  new file below `mvdm` or `opennt-host`; only an existing original file may
  receive a minimal owner-local divergence.
- `mvdm-platform-abi`: exact original declarations and contracts outside
  MVDM required to compile imported MVDM packages. It contains no replacement
  behavior.
- `mvdm/dos/v86`: complete selected DOS/V86 guest source, resources, build
  descriptions, intermediates and original products. `mvdm/bin86` and
  `mvdm/wow16` are the selected load-only Bin86 and WOW16 carries; the separate
  `opennt-host/base/win32/winnls/fontsup/system` carries exact original
  16-bit fonts. None
  implies an external WOW16 source-universe mirror.

### Historical adapter placement (retired production roots)

The following names describe the source-family recovery rationale only. Their
former roots contain no selected production source after T418; the live
process-local bindings have moved to the owning executable root.

- `adapter-mvdm-host-in`: selector-blind fixed-width machine-event/frame
  transport into the original MVDM host. It does not select, replace or
  execute a machine backend.
- `adapter-mvdm-host-out`: the sole OpenNT-facing historical-interface component.
  Its explicit internal families are `win32`, `softpc`, `monitor`, `redir`,
  `wow`, `vdd` and `debugger`. Each preserves only the corresponding reached
  original interface shape; none is an alternate MVDM provider. The `softpc`
  family binds the original executable SoftPC CCPU40 call graph. It never
  includes a retired Bochs type, object or global, an NT4 kernel-VDM
  `CPU_30_STYLE` monitor, or a fallback/second executor.
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

### Historical project-component placement (retired production roots)

`session`, `broker` and `app` are likewise historical ownership labels. Their
live implementation is worker-local `ntvdm`, `basesrv`, and `run16`
respectively; they are not current production components.

- `session`: dependency-neutral lifecycle, mappings, resource tables,
  completion/events and teardown for one independent VDM instance.
- `broker`: per-user cross-process VDM registration, identity, command queues,
  notifications, leases and cleanup. It restores required observable
  BaseSrv-style coordination contracts through public IPC without recreating
  CSRSS or inspecting arbitrary processes.
- `app`: CLI and final composition. It creates session instances, selects guest
  images, binds adapters and connects to or starts the broker.  It also owns
  every product error-dialog presentation through the one custom public-Win32
  dialog surface; an original mirror or adapter may report a structured
  source-shaped fault and permitted responses, but may not own final product
  dialog UI.

### Executable-owned component transition

T418 retires the generic project-component topology without changing the strict
original-mirror boundary. The completed layout is deliberately executable
owned:

```text
src/run16-exe/   -> run16.exe
src/basesrv-exe/ -> basesrv.exe
src/ntvdm-exe/   -> ntvdm.exe
src/dtmgr-exe/   -> dtmgr.exe
```

The former `app`, `session`, `broker` and adapter roots are README-only move
markers, not compatibility locations or destinations. `run16` owns the public CreateProcess-style CLI path; `basesrv`
owns its service endpoint, authentication, liveness and transport assembly
around mirrored `srvvdm.c`; and `ntvdm` owns worker-local setup, guest-memory
leases, thread binding, teardown and process-local presentation. The broker
does not acquire DOS/WOW record policy, and the worker does not acquire broker
policy.

`dtaskmgr` owns only native Console task-management presentation and its
client-side selection state. It queries BaseSrv's copied, authenticated task
snapshot and asks BaseSrv to terminate one selected registered task; it neither
owns a second registry nor enumerates or controls arbitrary Windows processes.

There is no generic shared Win32/compatibility component. A process-local
Win32 binding belongs to the executable that owns the relevant HANDLE,
Console, thread or teardown: `run16`, `basesrv` or `ntvdm`. Original code
stays in its original mirror even when several executables link it. The
BaseSrv service IDL, protocol implementation and client library are
`basesrv`-owned; `run16` and `ntvdm` may link that library but do not own a
parallel protocol. The only shared product data is the small, stateless
`product-abi`/`package` surface for version identity and package layout.

`opennt-abi/host-compat` is the sole exception for a same-shaped historical
host ABI needed by multiple executable owners. Its README must name the
original contract, its finite public-Win32/NTDLL binding and all consumers.
It may not contain broker policy, worker/session state, guest pointers or a
general-purpose helper collection. Shared records are copied, versioned data,
never session, Console or native-resource policy.

## T423 DOS frontend ownership transition

The admitted frontend split supersedes worker-owned DOS presentation above.
The root run16 owns visible Console input and presentation. Nested launchers
join its broker-authenticated capability association while retaining their own
target-completion waits. A copied, versioned direct run16/ntvdm channel carries
Console operations and indexed video frames; BaseSrv only authorizes and binds
the endpoints. Neither frames nor input are relayed through BaseSrv.

ntvdm retains the original guest keyboard/mouse devices, video mode selection,
painters, worker-local bitmap/mutex backing, command re-entry and execution.
Its local adapter translates the original Console API shapes into the direct
channel. Host pointers, painter storage and mutexes never cross that channel.
Root association, execution ancestry and worker membership are distinct.
Launcher lifetime does not own a handed-off target's execution lifetime.
Inner launcher exits do not actively terminate native targets, DOS tasks,
workers or descendants. A live launcher returns only its direct target's
completion/result. The root frontend defines the interactive session lifetime:
its normal/abnormal exit or true Console closure closes its associated DOS
workers using the original VDM close handler and bounded close completion.
This is a product mapping to Console-session close, not original OpenNT
launcher-death policy. A transient I/O failure is not proof of session closure.
Display switching is not closure. Native descendants and unrelated sessions
are not tree-killed. Already completed task results remain intact; unfinished
records fail on worker closure. Windows Console ownership stays in run16,
not ntvdm. Only pre-handoff
startup rollback may terminate an uncommitted created target; no long-lived
kill-on-close Job, execution-tree cascade or new DOS scheduler is permitted.

S2 delivers this boundary incrementally: native CUI programs still use the
existing visible Console, and native GUI/WOW windows retain their own route.
Hidden native Console belongs to S3; run16's display flag and kvm-window
presentation belong to S4. They are required future stages, not capabilities
claimed by copied-frame or Console transport tests. Status and the active
proposal own the finite verification gates and remaining limitations.

For the admitted S3 extension, the root run16 alone owns visible and hidden
Console presentation resources, its internal Console helper processes and
later Window presentation. Inner launchers authenticate their existing root
association; Console attachment is not frontend ownership. An invalid inherited
association fails rather than silently creating a second root. Inner launchers
submit launches and wait for their own target completion, never operate a
competing frontend or reclaim root-owned backends on exit. A root-managed
helper may physically create a native CUI target on its hidden Console; the
requesting launcher's result is nevertheless obtained from that actual target
process, not the helper's exit. Ordinary native descendants share their actual
Console through Windows inheritance. Backend failure, target completion and
root-session closure remain separate events. This is the approved ownership
contract, not a claim that the hidden backend is already production-wired.

## WOW message transport boundary

Modern Windows USER is the sole native message delivery and reply owner for
Win16-to-Win16 and both directions of Win16/Win32 interaction. Do not introduce
a parallel local SMS delivery queue or infer a private received SMS identity
from an outer native API call. Original MVDM message thunks and composable
OpenNT WOW task algorithms remain source owners; `wow32-dll` binds native
call/return and callback boundaries to WOW execution handoff, bounded guest
memory lifetime and failure cleanup. `ntvdm-exe` supplies worker-local CCPU
and thread bindings, not a second message transport or scheduler. Observable
reply/reentry/lifecycle behavior is the acceptance contract, not equality with
an inaccessible native USER SMS pointer.

## Interactive failure policy

All user-facing error-dialog interaction uses one app-owned custom Win32
presentation contract.  This applies equally to an error reached through an
original `ERRORPANEL`, a direct `MessageBox`, a WOW private hard-error form,
or another source family: those forms remain evidence for source text,
available actions, default action and return representation, but are not
separate product dialog owners.

The lower owner reports copied structured fault data and the finite actions it
permits.  `app` renders the dialog and returns the selected action without
inventing a source result; `session` owns the consequent lifecycle operation
(retry, continue, or terminate the current session).  Where an original ABI
uses a result ordinal or an `RMB_*` value, the boundary maps only the selected
action to that proven representation.  It does not turn an unavailable,
malformed, noninteractive, or unsupported source contract into a fabricated
success.

This is a presentation/lifecycle boundary, not a new generic error provider:
the original/adapter owner still classifies the failure and retains its
source-defined post-response control flow.  Non-dialog failures continue to
use their declared deterministic report/exit contract.  A future implementation
packet must audit every current direct dialog call and private presentation
transport before moving it to this boundary; no blanket `MessageBox` rewrite
is authorized without that per-family result-contract review.

## Runtime cardinality

### Standalone process failure contract

Run16 may start a missing broker during startup. After connection/task admission,
there is no automatic runtime reconnect, task replay or replacement worker.
Protocol 3 supplies an authenticated wait-only broker process capability;
connected workers and running launchers fail with 1722 if that process exits.
An unfinished task whose worker exits fails with 1067 rather than interpreting
the original record-removal zero as success. Original task results and normal
worker lifecycle remain unchanged; this is an owner-approved standalone fault
contract, not a recovered kernel/CSRSS behavior.

Launcher death rolls back an unclaimed startup through original UndoCreation.
A temporary non-inherited Job installed atomically at worker creation protects
the interval before broker registration; its kill-on-close policy is disarmed
after Prepare. A claimed worker retains reservation resources until exit.
Non-root launcher loss does not own that lifetime; root frontend session close
above is the explicit standalone exception. Unrelated workers are unaffected.
No worker idle
timer is introduced; the broker retains its existing empty-only grace.

The current runtime binds exactly one active imported MVDM host context to each
`ntvdm.exe` process. Multiple processes may run concurrently. Inside one
session, original DOS `EXEC`, COMMAND child/re-entry behavior and multiple
WOW16 tasks are guest/task lifecycles, not additional VDM sessions.

Each session activates the original SoftPC CCPU40 machine composition. There
is no fallback or simultaneous executor. `CPU_30_STYLE` is excluded because
its original `v86/monitor` body delegates guest execution to NT4 kernel VDM;
it is not a CCPU profile and cannot enter this non-invasive product. Current
functional machine, guest and MVDM-host acceptance records its selected
Win32/x86 CCPU40 row only. Native x64 compile/link output is not an acceptance
input or a reason to create, preserve, or repair x64-only differences, except
for a demonstrated architecture-neutral mapping-manager correctness defect.
Pure source, static-analysis and documentation work need not create a machine
session.

All project-owned session and adapter APIs are multi-instance-safe: no hidden
process-global current machine, mapping table or resource registry is allowed.
Imported MVDM code may retain its original process globals for the current
one-session-per-process profile. In-process multiple independent MVDM host
contexts require a separate reentrancy audit that classifies each original
global as immutable process state, per-session state, TLS/thread state,
guest-owned state or broker-owned state. It is not achieved by swapping an
unbounded block of globals.

`ntvdm/monitor` binds the active worker-local session/`VDM_TIB` context to each
participating worker thread.  It binds before entering imported MVDM code and
unbinds on exit.  No raw session pointer enters guest state or a fixed-width
component ABI.

## Dependency direction

```text
run16 -> basesrv protocol client
dtaskmgr -> basesrv management protocol client
run16 -> opennt-host Base client + opennt-abi/host-compat
basesrv -> basesrv transport + opennt-host BaseSrv owner
ntvdm -> worker-local session, command, monitor, SoftPC, Redirector, VDD, WOW and debugger bindings
ntvdm -> original mvdm + opennt-host + opennt-abi/host-compat

opennt-abi/host-compat -> public modern Win32/NTDLL only
product-abi -> versioned fixed-width protocol declarations only
product-package -> package layout/media declarations only
mvdm-tools -> original mvdm/opennt declarations only (independent tool builds)
```

`ntvdm/session` never calls a component-specific provider.  `src/ntvdm-exe` is a
worker-private composition root, not a reusable host/session framework.  The
SoftPC path remains within the original MVDM source-shaped composition.
The broker never receives native or guest pointers. Command records exchange
versioned fixed-width copied fields and stable cross-process identities only.
The approved resource-transfer boundary separately accepts authenticated
OS-managed attachments for required file, pipe, event and process resources.
Recipient-local handles are lifetime-managed capabilities, not wire identities;
sender-local handle numbers are never trusted as authority. Console attachment
and local streams remain separate from kernel file/pipe resource transfer.

Each `ntvdm` specialist directory owns one worker-local historical interface
family; it is not a convenience shim and may not absorb another provider's
semantics.  A missing interface is recovered with original source evidence
where available, rather than edited into an OpenNT mirror.

`opennt-abi/host-compat` is the sole narrow cross-process exception: it carries
historical declaration shapes and a source-shaped RTL binding which is compiled
separately into each consuming EXE.  It has no broker state, guest state,
cross-process identity, shared DLL or generic compatibility policy.

`mvdm-tools` has no inbound production-runtime edge at all.
`mvdm-softpc-firmware` has no host
compile or link edge; `ntvdm` stages an explicitly admitted, manifest-selected
immutable firmware input to the selected backend's source-shaped binding.

## Guest and host width model

Guest width and host pointer width are orthogonal:

```text
guest 16:16 / linear32 / opaque16-or-32
        -> original MVDM logic
        -> original Win32/x86 process-local carrier or bounded lease
        -> native x86 HANDLE/pointer/resource
```

The product has one Win32/x86 MVDM context per process, so directly composable
original 32-bit host carriers retain their native x86 value. Lengths, offsets,
flags, times, error codes and guest addresses retain numeric meaning and
receive explicit range/overflow checks. A directly dereferenced guest pointer
is a bounded epoch-scoped lease, not durable state. When an original narrow
ABI cannot carry a pointer (for example a Redirector WORD handle), its owner
maintains the smallest source-shaped private table. Native pointers and private
tables never cross the broker boundary; broker identities use their own
fixed-width wire contract. Native resources cross only via the authenticated
OS-managed attachment boundary, preserving original ownership and cleanup.

## Source union and mirror rules

The selected OpenNT source is one package-scope union of
`O:\repos.external\OpenNT\base\mvdm` and
`O:\repos.external\OpenNT-4.5\nt\private\mvdm`. Each target-relative path
has one selected file. Identical inputs retain dual provenance; one-sided
inputs are included; differing inputs are selected at complete-package scope
using source, build, resource and artifact lineage. Parallel edition roots and
undocumented per-file hybrids are forbidden.

The two guest mirrors are load-only. Their C, assembly, objects and libraries
never satisfy a host symbol. `ntvdm` selects immutable products through a
guest-image manifest and loads bytes through the selected backend binding; subsequent
communication is only BOP, interrupts, ports and guest-memory contracts.

Every mirror file is exact upstream, a registered true subset, or a registered
same-shaped minimal modification in an original upstream-relative file. A
changed expression carries `DIVERGENCE:` and a README register row. A material
added implementation belongs in a named adapter/ABI family. Either mirror may
gain an audited original OpenNT file at its original relative path, but never
a project-invented file.

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
