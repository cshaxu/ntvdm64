# System Architecture

Owner-approved T423 S12 text extension: the common text ABI accepts original
glyph/attribute pairs or optional glyph/attribute/style triples. Both workers
share the decoder; DOS continues to publish pairs. This supersedes the exact
byte-pair-only restriction below, preserving native underline without a second
NTCON renderer or modifying guest/shared-library code.

## Product boundary

### Worker copied-state publication — T429 S5/S6

Original guest/video execution remains the sole extractor of mutable software
VGA state. An NTVDM-owned publisher holds immutable complete snapshots, one
latest pending state and the last successfully sent state. It uses a dirty
event and a one-shot maximum50Hz/20ms publication cap; idle has no recurring
timer. This does not change the guest VGA/CCPU clock or guarantee20ms capture
of arbitrary memory writes. Publication is outside guest IRQ/painter locks.
Block/release quiesces admission and drains before the existing final-frame
acknowledgement; teardown cancels transport and joins before endpoint release.
T429 S6 extracts the copied-state publication mechanism into worker-base for
both workers. Complete opaque native snapshots include Unicode cells,
geometry/cursor and text metadata in one acknowledged transaction. Native
acquisition remains separate (T429 S7 changes its wait from30ms to20ms);
original VGA extraction remains NTVDM's. Capture/RPC cost adds to this wait;
it is not synchronized with the shared publication deadline.
Only the shared publisher compares complete state against its last successful
commit. NTCON remains format-driven without frontend dedup or lifecycle change.
The [S5 source audit](../etc/evidence/m0-t429-s5-software-video-publication.md)
and [S6 sharing evidence](../etc/evidence/m0-t429-s6-shared-video-publication.md)
record the explicit owner exception, shared boundary and retained limits.

### Own-image product root — T427 S2

`common/system_root` supplies checked Windows-root and relative-path
mechanics. S5 installs all six EXEs and both host DLLs in system32; Windows
root is the parent of that loaded EXE directory. Each caller uses its own
loaded image; CWD, PATH, argv and inherited
environment text are not root authorities. Internal executables, media and
the exact WOW32/VDMREDIR provider names use their declared package-relative
locations. Missing local providers do not fall back to user search. This is
not a new RPC authentication or package-identity policy; service acceptance
and protocol versions remain unchanged.

Real Windows directory calls retain host meaning. Common's bounded ANSI
projection serves run16's Win16 task record and NTVDM's initial WOW guest
PDB; the Unicode host worker environment, saved native environment and
native process SYSTEMROOT remain real-host values. Original
KRNL386 module-directory and WIN16DIR rules remain at their owners.
The [S2 evidence](../etc/evidence/m0-t427-s2-own-image-root-bindings.md) records
production bindings, focused tests and the exact runtime coverage limits.

S3 separates user search: CWD then each caller PATH directory, with
COM/EXE/BAT/PIF order inside that directory; explicit paths never search
elsewhere. The package participates only through ordinary CWD/PATH or explicit
paths, not an executable-directory priority. Product-generated nested COMMAND
uses an explicit root/system32/COMMAND.COM path, independently of user PATH.
Classification, shell fallback, CLI and original EXEC semantics stay at their
existing owners. The [S3 record](../etc/evidence/m0-t427-s3-application-search-isolation.md)
contains production search, real image selection and internal handoff proof.

Original deployment locations are independent of application search: guest
DOS utilities, startup media and configuration live in root/system32 as
assigned by OpenNT TXTSETUP.SIF; SYSTEM.INI remains at root. A build staging
folder called runtime represents this root and is not an installed layer.
After relocating utilities out of a flat package root, bare names require
ordinary PATH membership or CWD at system32; an explicit
`run16 system32\command.com` from package root also works. The product does
not silently inject system32 into PATH. Guest-only NtvdmGet directory facades
retain explicit names, leaving host Windows directory APIs unchanged.

### Service-owned management tree — T426

NTSRV projects existing authenticated frontend associations, worker watches,
original WOW records and registered detached GUI records in one copied
management snapshot. NTCON frontends are top-level with associated DOS and
Win32-text workers beneath them; independent Win16 workers are top-level with
their actual WOW tasks beneath them. Detached Win32 GUI targets are top-level.
There is no UNBOUND group. A departed known frontend is displayed as MISSING
only while workers retain its exact authenticated association; a replacement
frontend cannot acquire those children merely by PID reuse.

Management keys contain service instance, category, generation and object
identity, never process-local pointers or trusted PID-only selectors. NTMON
consumes server ordering, state, labels and permissions; it neither enumerates
processes nor owns relationships. Explicit close goes through authenticated
NTSRV validation and pins the existing actual target before leaving the lock.
Existing worker/frontend shutdown and completion semantics remain at their
owners; GUI close targets its registered process only. WOW task rows have no
fabricated host PID/time or individual close permission. This is a projection,
not a second execution registry or scheduler. The
[S2 evidence](../etc/evidence/m0-t426-s2-management-projection.md) distinguishes
implemented production paths. The [S3 UI record](../etc/evidence/m0-t426-s3-monitor-tree-ui.md)
and [S4 integration record](../etc/evidence/m0-t426-s4-monitor-integration-handoff.md)
retain actual renderer/key behavior, real multi-root/GUI/WOW isolation,
published package identity and the remaining owner-acceptance boundary.

### Broker-controlled single I/O connection — T425 S6 delivery

Owner-approved S6 replaces the frontend pending-owner policy with one
NTSRV-controlled connection. NTSRV alone retains logical frontend/worker
associations and authorizes connect, release, disconnect and reacquisition.
NTCON retains zero or one current worker pipe and persistent display/input
state, not a worker list or pending acquisition queue. Workers retain their
current authorized frontend/channel and local execution state only.

Handoff confirms final publication/input return and closure of the old pipe
before NTSRV grants a new connection. Expected I/O disconnection is not task
completion, worker failure or component retirement. Direct worker/frontend
traffic is data and transport acknowledgement, not control arbitration.
Original NTVDM blocking/resume/reentry and native execution remain owner-local.
The [S6 release evidence](../etc/evidence/m0-t425-s6-broker-io-ownership.md)
records production-linked authority/reentry tests, actual bidirectional nested
handoff, final retained gates and coherent eight-file publication. The
containing reviewed S6 P delivers this boundary; T425 awaits owner acceptance.

### Broker-centered creation and control — T424 S4 delivery

Owner-approved migration of project-added orchestration is delivered by S4;
the [S4 evidence](../etc/evidence/m0-t424-s4-broker-centered-launch-control.md)
records production tests, publication and remaining S5 cleanup. NTSRV
creates and authenticates NTCON and NTVDM/NTVWM, binds their frontend/execution
relationships, delivers direct requests and owns orderly retirement. Run16
finds/starts NTSRV, classifies/submits its target and waits on broker direct
completion when the existing launch semantics require it. Console takeover,
return and restoration acknowledgement also pass through authenticated NTSRV
control: no run16/NTCON IPC exception remains. NTSRV supplies the actual caller
process capability; NTCON alone attaches to that caller's Console. NTSRV does
not attach to or own that Console. NTCON/worker transport remains
direct for frames, input, title/geometry, route activation, input return and
final-paint/restoration barriers; it owns no task registry/completion or orderly
death policy. Shell-out CreateProcess/run16 execution remains local to the
worker and is not a new task-control RPC edge. NTMON uses NTSRV only.

Both worker kinds retain their original/native execution boundaries. For text
targets NTVWM holds/waits on its real target, obtains its actual Windows exit code, completes
I/O and reports the result to NTSRV; run16 receives the broker result, not a
direct worker pipe. DOS retains original BaseSrv record/completion semantics.
Win32 GUI and registered Win16 startup-only semantics are unchanged. No
original MVDM/OpenNT-host mirror modification is planned by this migration.
The caller stays alive/attached until takeover is acknowledged. Root task
completion does not authorize returning to outer CMD until NTCON reports
canonical buffer/input restoration through NTSRV; inner launchers cannot
return the root lease. Restoration preserves the current grid/geometry/cursor
and restores modes/active buffer, not an old startup page or cursor position.

NTCON has no autonomous idle deadline: it waits for NTSRV while its real user
Console remains alive. NTSRV owns cancellable ten-second no-worker/startup
decisions, and cannot start its own ten-second empty-service grace with a live
frontend, worker or legitimate admission. Connected run16, NTCON and workers
watch the authenticated broker process handle using event waits, not periodic
RPC probes. NTSRV orderly instructions have priority; broker loss, true Console
closure and unrecoverable component faults remain separate failure boundaries.
This delivered policy supersedes S3's immediate orphan retirement;
the S3 section below describes the retained predecessor, not the current package.

### Architecture/code cleanup — T424 S5 delivery

The [S5 ledger](../etc/evidence/m0-t424-s5-shared-control-cleanup.md) records
consolidated control transport with completed-I/O-first semantics, clarified
owned-client link boundaries, removal of launcher target-handle decisions
from native-text completion/return, dead frontend state and replaced bootstrap
tests. Broker result/handoff state and final restoration barriers remain.
Do not merge the strict frontend transport blindly,
add a generic shared component, expand worker-base to nonworker consumers or
relocate original execution. Detailed checklist and gates belong to the
[T424 plan](../etc/operations/t424-worker-frontend-renaming-plan.md#s5-architecturecode-cleanup-checklist).

### Common library and service separation — T424 S7/S8 delivery

Owner's subsequent closure revision separates these goals: S7 owns common and
the two protocol families; the complete NTSRV block-provenance review and
service-private source split are delivered by S8. Earlier combined wording
below is retained context, not an S7 service-split completion claim. Native GUI,
frontend naming and final audit originally followed as S9, S10 and S11.
The owner's later insertion makes S10 a bounded NTSRV frontend lost-wakeup
repair, S11 frontend naming, owner-added S12 unified logical text surface and
S13 final audit. The S10
[checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s10-rapid-relaunch-and-frontend-notification-checklist)
keeps the shared notification a projection of every actionable pending join/
channel request under the existing service lock. Separate completion/return
acknowledgements retain their contracts; no new control edge or mirror change.

### Delivered T424 S12 text storage and handoff

The [S12 ledger](../etc/evidence/m0-t424-s12-logical-surface.md) records
implementation, tests and release. After S11 the frontend
is NTCON (the renamed frontend); the native worker is NTVWM. The frontend owns one
logical_surface path for both workers' text: operations/frames -> committed
logical state -> visible Console or Window. Replaced native direct-to-visible
Console paths are removed. NTVWM retains its hidden execution Console;
original OpenNT/MVDM execution and graphics paths remain at their owners.
This surface is project-added presentation adaptation, not original OpenNT.

Logical storage retains buffer extent/cells/attributes, viewport origin/extent,
buffer-relative cursor and shape/visibility, and existing text style/font
metadata. Normalize viewport-relative presentation without discarding buffer
rows. Publication/capture/render use consistent committed state and explicit
locking; tiled updates cannot expose half-applied geometry or cursor.

Logical size changes may request grow-only physical resizing, but projection uses the
actual resulting canvas. Console projection is always upper-left anchored:
paint the intersection, clear unused right/bottom characters and attributes,
hide cursors outside it and map mouse events only to logical cells. No reflow,
bottom alignment, cursor-following viewport, row compensation or host-size
truncation of logical storage. Window uses the same state as text, not a bitmap
fallback. Host scrollback history is outside this guarantee.

Both handoff directions drain/commit/acknowledge the old owner's final state,
apply/acknowledge incoming-worker capability conversion, then release input
and execution/resumed-parent output. Inherit current logical state, never the
initial outer CMD snapshot. Retain original DOS VGA height selection and
cursor-aware cell-grid resize; ordinary release does not restore old CMD
geometry/cursor. Existing stream-I/O special restoration remains special.
NTSRV retains control/lifecycle authority, run16 remains thin, and worker I/O
remains direct. No new helper/component, mirror/guest/shared-lib changes or
new scheduling policy. The
[S12 plan](../etc/operations/t424-worker-frontend-renaming-plan.md#s12-unified-logical-surface-and-dosnative-handoff)
owns detailed capability, failure, isolation and release acceptance.

Final owner transport clarification: common carries both protocol families
and their suitable shared client/transport mechanisms. NTSRV control uses
RPC; direct NTCON-worker I/O retains named pipes. Worker-base may depend on
common; common has no reverse dependency on worker-base or EXE-private code.
Neither protocol depends on or relays through the other. Endpoint-specific
authentication policy, execution, rendering and retirement remain owner-local.
RPC migration of worker I/O is not admitted.

After S6 naming delivery, the owner admits converting src/interface into src/common:
a declaration/IDL-only protocol submodule plus narrowly scoped, separately
selected shared implementation modules. Cross-component project-added transport,
codec, authenticated client and Console-snapshot mechanics may be consolidated
after provenance and full-contract review. This delivered library is static,
not another executable/helper or a general compatibility framework.
Worker-only mechanisms remain worker-base; common has no dependency on EXEs
or worker-base. Process resources, renderer, task/lifecycle authority and
authorization policy retain their executable owner. Common instances and
resource/locking/failure contracts must be explicit.

NTSRV's large base_service.c is split by provenance and service responsibility,
not merely file size. Original OpenNT/MVDM code remains in its upstream-relative
mirror, preserving algorithms, ordering and minimal diff. Project-added service
state, admission/retirement, native receipts and projections remain NTSRV-private,
not common policy. The path/banner cannot substitute for an original-source
ledger. The [S7 checklist](../etc/operations/t424-worker-frontend-renaming-plan.md#s7-common-library-and-service-source-separation)
requires source/hash/block classification, mirror comparisons, duplicate removal,
production link coverage and the full regression/publication gate.
The pre-migration S6 runtime retained interface; current S7/S8 delivery selects
common/protocol for copied declarations/IDL,
common/codec for bounded packet validation and common/transport and
common/console for neutral transfer and on-demand snapshot mechanics.
Worker-only clients may depend on these common providers; common never depends
on worker-base or executable-private implementation. Local client instance
declarations are not wire contracts and stay with their implementation owner.
The native control pipe, its transitional interface header and the internal
service pipe test seams are removed. The
[S7 ledger](../etc/evidence/m0-t424-s7-common-service-separation.md) and
[S8 ledger](../etc/evidence/m0-t424-s8-ntsrv-service-separation.md) retain
provenance, dependency and actual production verification, not just source cleanup.
This bounded owner approval supersedes the blanket common-root prohibition
for the stated purpose, not other ownership or mirror restrictions.

### Native GUI registration — T424 S9

The subsequent owner direction removes local run16 GUI creation from the target
topology. Owner places S6 NTW32 -> NTVWM naming before this S9 implementation;
the rename alone does not deliver GUI routing. Run16 retains broad
DOS/Win16/native family discovery and unchanged arguments; every native target
is submitted to NTSRV and delivered to NTVWM. As corrected by the owner during
T424 S9, run16 decides the native GUI/CUI subsystem before service admission
and acquiring a text frontend, matching the DOS/WOW startup classification
order. NTVWM executes the admitted native kind,
creates the actual target and reports authenticated startup/binding success.
NTSRV holds the registered GUI process handle independently of worker occupancy
and retains native GUI registration until actual process exit. The owner's
final clarification assigns monitor/UNBOUND display to the queue-head NTMON T
candidate, not this routing stage. Its future view consumes service projection
only. S9 implementation and exact verification/publication state are recorded
in CURRENT and its ledger; admission or a candidate build is not publication.

Default GUI run16 returns on startup success, not window closure. Explicit
--wait keeps its actual completion/exit-code behavior via NTSRV. Releasing the
GUI request cannot kill its target or a reused text worker. Following the
owner's shared-WOW clarification, release the request but retain the resident
carrier, BUSY if other text work remains and READY otherwise. Later broker
retirement is independent, not an implicit GUI startup cleanup rule.
GUI-only segments acquire no character frontend and do not propagate its
capability. Existing Win16 startup semantics remain unchanged; a WOW task must
not be represented as an invented per-task Windows process handle. No new
process, helper, Job observation or scheduler is introduced.

The ordered T424 plan retains the earlier S4/S5 replanning history; these
sections describe the delivered topology, not an active S4 admission.
Monitor/UNBOUND display remains outside this T.

### Broker-owned retirement — T424 S3 owner approval

NTSRV owns orderly frontend/worker retirement. With no associated worker and
no legitimate in-progress startup, it immediately instructs the frontend to
exit, regardless of presentation lease/idle state. Only NTSRV retains a
ten-second empty-service grace, after all frontends/workers/admissions are gone.
Frontend loss is decided by NTSRV and delivered to associated workers through
an authenticated shutdown event. All recipients prioritize that instruction
over ordinary lease, pending and I/O work. User closure of the visible Console,
broker loss and unrecoverable component faults remain exit paths. Task completion
only returns I/O; it does not retire components. Original DOS close handling and
native Console close acknowledgement remain local operations. Startup admission
has a finite ten-second deadline, not a permanent orphan-root exemption.
This supersedes older frontend-grace/self-retirement descriptions below.

`ntvdm.exe` is a non-invasive Windows CLI that hosts NT4-era DOS and bounded
WOW16 workloads without replacing Windows system files, rebuilding the kernel,
recreating a private NT subsystem, or requiring installation-time host
mutation. Public Win32 APIs and ordinary host resources remain valid integration
mechanisms.

Each production source file has one final owner. Original mirrors preserve
upstream package identity; executable-owned roots own their process-local
mechanics; common/protocol holds shared identity, copied declarations and
service IDL. Worker-base holds project-added worker-only mechanisms shared by
NTVDM/NTVWM when their normal, failure, nested and teardown contracts match.
The neutral presentation client, transport, codec and on-demand Console
snapshot mechanisms now have their separately selected common providers.
Worker-base adapts those mechanisms at the worker boundary; NTCON retains
server/rendering/ownership. No consumer depends on frontend-private client code.
Original OpenNT/MVDM execution, scheduling, task completion, block/resume and
cleanup stay in their mirrors; never extract original logic and reverse-call it.
Backend-specific guest and native Console operations remain explicit locally.

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
live implementation is worker-local `ntvdm`, `ntsrv`, and `run16`
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
src/ntsrv-exe/ -> ntsrv.exe
src/ntvdm-exe/   -> ntvdm.exe
src/ntmon-exe/ -> ntmon.exe
src/ntcon-exe/ -> ntcon.exe
src/ntvwm-exe/ -> ntvwm.exe
```

The former `app`, `session`, `broker` and adapter roots are README-only move
markers, not compatibility locations or destinations. `run16` owns the public CreateProcess-style CLI path; `ntsrv`
owns its service endpoint, authentication, liveness and transport assembly
around mirrored `srvvdm.c`; and `ntvdm` owns worker-local setup, guest-memory
leases, thread binding, teardown and guest-side I/O bindings. frontend owns
character presentation; NTVWM owns native text backend resources. The broker
does not acquire DOS/WOW record policy, and the worker does not acquire broker
policy.

`ntmon` owns only native Console task-management presentation and its
client-side selection state. It queries BaseSrv's copied, authenticated task
snapshot and asks BaseSrv to terminate one selected registered task; it neither
owns a second registry nor enumerates or controls arbitrary Windows processes.

There is no generic shared Win32/compatibility component. A process-local
Win32 binding belongs to the executable that owns the relevant HANDLE,
Console, thread or teardown in the explicitly named executable owners. Original code
stays in its original mirror even when several executables link it. The
BaseSrv protocol implementation and client library are
NTSRV-owned; clients may link that library but do not own a
parallel protocol. Service IDL and shared product data belong to the stateless
`common/protocol` surface for version identity and copied I/O/management contracts. Package
layout updates worker session state and belongs to ntvdm-exe, not a shared root.

`opennt-abi/host-compat` is the sole exception for a same-shaped historical
host ABI needed by multiple executable owners. Its README must name the
original contract, its finite public-Win32/NTDLL binding and all consumers.
It may not contain broker policy, worker/session state, guest pointers or a
general-purpose helper collection. Shared records are copied, versioned data,
never session, Console or native-resource policy.

## T423 DOS frontend ownership transition

### T424 service-private source organization

NTSRV's project adaptation uses one explicit service instance and its existing
recursive lock. Within `src/ntsrv-exe/opennt/source`, `service_core.c` owns
service/connection authentication and rundown; `worker_registry.c` owns
reservation, creation, admission and reuse; `frontend_registry.c` owns root
identity, routes and Console takeover/return; `native_commands.c` owns direct
native command/result transport; `lifecycle.c` owns retirement and shutdown;
`management.c` reads the bounded management projection. `base_service.c` retains
the original-shaped DOS/WOW interface/resource bindings. All seven units link
as one existing service provider. `service_internal.h` is private implementation
state, not copied protocol data or a public client dependency.

Original DOS/WOW execution, task completion, blocking/resume and cleanup stay
in their OpenNT/MVDM mirrors. This organization neither relocates them nor
creates another registry, scheduler or lock authority. Cross-module helpers
preserve existing caller-held lock/resource contracts and original lock order.

### Current lifecycle and direct-completion contract

Owner-approved T428 clarification: admitted shared GUI-only NTVWM carriers
follow shared WOW residency, not an additional ten-second unbound-worker
idle timer. GUI target exit and carrier residency remain separate. Exclusive
native text/GUI semantics must follow original exclusive DOS/separate WOW at
the native worker boundary; original NTVDM policy is not relocated or changed.
The frontend workerless grace, broker empty grace and finite startup admission
remain distinct from admitted worker residency.

NTSRV is the single authority for registered NTVDM and NTVWM workers, their
frontend-root associations, direct-command admission and completion, and
cooperative retirement. NTCON owns visible Console/Window presentation, not
worker lifetime. Run16 classifies and submits one direct target, then waits
only when that target's established launch semantics require it. Execution
ancestry, frontend I/O association and worker residency are separate relations;
none implies recursive process-tree termination.

- A DOS direct request uses the original BaseSrv DOS record and its completion
  event/exit-code path. Original DOS/WOW execution, scheduling and completion
  remain in their OpenNT owners. Internal guest execution without a new run16
  direct request is not fabricated as a broker completion.
- An NTVWM Win32-text direct request binds the actual suspended target's
  process identity to its authenticated NTSRV Win32Record before resuming it.
  NTVWM retains and waits on the real process handle, obtains its Windows exit
  code, completes its Console/I/O cleanup, and reports that result to NTSRV.
  NTSRV stores the result and issues the direct completion receipt; run16
  waits for that receipt and queries NTSRV rather than deciding completion
  from its own process handle. NTVWM's
  ordinary Win32 descendants retain Windows parent/child and exit semantics;
  observation never creates a synthetic direct receipt.
- Run16 uses one outer direct-wait/fault/receipt flow for synchronous DOS and
  Win32-text targets. NTSRV may share the project-owned publication and result
  transport, but the DOS result source remains the original record and the
  Win32 result source is NTVWM's authenticated report of its real target exit.
  Win16 retains its
  original registered startup-only default return, and Win32 GUI retains its
  native startup-only default return. Explicit --wait retains its established
  broker task/process completion contract; GUI startup alone does not acquire
  text I/O or wait for window closure.
- A completed task does not retire its worker. NTVDM and NTVWM remain READY
  and reacquire commands while NTSRV and their associated frontend root are
  viable. A direct Win32 receipt may complete while native descendants still
  use NTVWM's Console; that Console must not be torn down or reused until its
  actual resource state permits it. Monitor-only observations do not decide
  the direct exit code or authorize killing descendants.
- A borrowed NTCON root returns the visible Console to the outer caller on
  direct completion, but parks its presentation and preserves the already
  delivered worker channels. The resident worker can therefore publish the
  next direct request through the same authenticated frontend root; the
  caller's input mode is restored between leases. A new lease resumes that
  root rather than creating a second channel or equating task completion with
  worker disconnection.
- NTSRV arbitrates component death. NTCON exits when its host Console is gone
  or NTSRV is lost; after its last associated worker and in-flight admission
  disappear, NTSRV starts a cancellable ten-second workerless grace and then
  requests orderly NTCON retirement. Registered workers do not die merely
  because a launcher or one task exits. When a frontend root actually dies,
  NTSRV directs only its associated workers to close; unrelated roots/workers
  continue. On NTSRV death, connected NTCON/NTVDM/NTVWM instances fail closed
  through their broker-liveness contract. NTMON remains available as a
  disconnected monitor until its user exits.
- NTSRV itself retains a cancellable ten-second empty-service grace when no
  frontends, workers or pending admissions remain. Its timers never replace a direct
  task completion, force-kill a handed-off native target, or create a second
  scheduler. Explicit NTMON worker termination goes through NTSRV. Worker or
  broker failure completes affected unfinished direct requests with a clear
  failure, not a fabricated target success. No Job kill-on-close, recursive
  process-tree kill, or launcher-death-to-worker-kill follows from this model.

### Delivered native worker boundary

Latest owner approval replaces ConPTY with NTVWM's own ordinary hidden Console.
NTVWM is its attached resident worker and uses public Console APIs directly;
there is no private helper/bootstrap or NTSRV-owned pseudoconsole. NTCON remains
presentation only. The ConPTY descriptions below are superseded design history,
not selectable alternative production backends. Independent-worker lifecycle,
native direct results, same text-frame ABI and screen/input continuity remain.

Latest owner clarification: NTVWM is a Win32-text worker peer of NTVDM,
not an NTCON-owned backend with a frontend-bounded lifetime. Both expose the
same external worker discovery/start/registration, request/completion,
handoff/re-entry, exit/fault and monitor management model. I/O association is
separate from worker identity. NTCON connects as presentation client only.
The selected original OpenNT NTVDM/MVDM interface and lifecycle are the
canonical shape: NTVWM adapts at its own boundary to a same-shaped native
worker contract. No original MVDM mirror may include, call, schedule, or
otherwise adapt to NTVWM. Shared project-added mechanics may be extracted only
when they leave the original owner and its control order intact. Audit and
reuse the actual selected NTVDM lifecycle instead of introducing a second
scheduler or encoding native tasks as guest DOS/WOW records. The older
per-frontend candidate description below is superseded wherever it conflicts.

The owner admits src/ntvwm-exe producing ntvwm.exe as the native text backend.
This supersedes the historical S9/S11 no-helper and per-native-branch rules
below, not their publication history. One character frontend session reuses
one NTVWM/real Console across DOS intervals. NTCON owns visible presentation,
display and direct I/O routing only. NTVWM owns its ordinary hidden Console,
native input/state/member operations, text-frame production and Console closure.
Screen/cursor transfer must reach the real Console, not just a parser cache.
Both backends publish text in the exact existing console_video.h ABI: copied
console_video_description, console_text_style and glyph/attribute byte pairs.
NTVWM publishes text only, with the same bitmap glyph mapping as NTVDM; no
native-specific terminal parser or font mapper remains in NTCON. Default fonts
and active guest font-bank handoff require explicit equivalence tests.
Run16 keeps classification, submission and existing GUI startup-only/--wait
and DOS/native-text direct-completion behavior; it never owns an I/O pump.
NTSRV owns authenticated backend instances and frontend associations outside
original DOS/WOW records. NTMON manages those registered instances, not arbitrary
process trees. Native backend shutdown must address its Console session and
report failure if that session is not closed; killing NTVWM alone is not success.
Actual attached users, pending admission and DOS/native requests determine
retirement; the NTVWM carrier itself is not a business user. Cross-session GUI
boundaries, original DOS execution policy and guest/shared-library immutability
remain unchanged. Status and the S12 ledger own implementation evidence.

### Historical T423 S4 frontend split (superseded backend ownership)

The independent `src/ntcon-exe/` product replaces root-run16 UI ownership.
It owns visible Console, Window, the native text backend and display for one
character session. run16 is exclusively a classification/start/submission and
direct-target wait client; no launcher owns an input pump or presentation.
ntvdm retains guest execution and uses the existing direct I/O contract with
frontend. BaseSrv authenticates the frontend process and separate execution
Console identity, never transports frames or becomes a new scheduler.

Ordinary GGG-CCC-GGG-CCC launch chains have two separate frontend sessions:
GUI segments do not join or propagate character-frontend authority; each
contiguous DOS/native-character segment shares its own frontend. DOS graphics
mode remains part of its character session. Explicit native Console creation
and attachment remain distinct contracts, not deductions from PID ancestry.
Launcher death is no longer frontend death. Actual frontend/session closure
retains the original VDM-close binding; backend/pipe failure is not closure or
target completion. No native process-tree termination is introduced.

The Console ownership split is now implemented and verified by S4's linked
source, graph and real runtime evidence. Its complete runtime set is seven
files, including ntcon.exe. S4 used a helper role of that same executable;
the published S9 replacement below removed that native I/O helper role.
Window/display and Window mouse have subsequent S6/S7 evidence; they were not
proved by the earlier Console migration alone. Status owns publication and delivery state; the
proposal assigns remaining validation and final owner acceptance.

### Historical T423 S9 backend replacement (superseded)

S9 replaces the private hidden-Console/helper backend with ntcon-owned ConPTY.
Only ntcon owns the pseudoconsole, streams, terminal state and display/input
routing. No persistent/transient helper role or launcher/worker Console-I/O
substitute is admitted. The S9 ledger records the published implementation and
its explicitly accepted limitations. The S11 replacement below uses released
per-branch ConPTY lifetime, not a Console-membership query subprocess.

Successful input delivery to ConPTY transfers ownership to that backend.
Frontend handoff orders unsent input but does not recover, shadow or replay
already delivered records. Native-to-DOS return may leave unread input for a
later native consumer; the owner accepts this difference from the original
shared Console. Preserve original DOS unused-key/BIOS-buffer return and do not
close a live native session merely to discard its input. Display changes do
not change the execution session or create another pseudoconsole.

One ntcon retains one pseudoconsole across every native-text launch in that
frontend, including DOS intervals and intervals with no known native target.
Direct-target completion never releases admission or recreates that backend.
No observer process or Job is introduced to infer last-client retirement;
the owner permits conservative retention until explicit frontend close.
The retention predicate is not a count of attached processes. Launcher results
remain independent of this resource lifetime. GUI-only segments do not acquire
character-frontend ownership merely by execution ancestry.

Window native text uses the approved SoftPC-range PC character-to-bitmap
mapping and explicit missing-glyph replacement, while the terminal model
retains Unicode and cell width. DOS keeps its actual guest font banks. This
does not require a general Unicode font subsystem or a second native renderer.

### Superseded S11 native branch lifetime prototype

This earlier owner-approved prototype is superseded by the NTVWM plan above;
it is retained as research context, not the current implementation directive.
S9 above remains the published baseline until verified migration. ntcon still
owns the sole visible Console/Window and display state. Each independently
launched native text branch receives a ConPTY; ordinary native descendants
inherit their real Console without interception. Win32 -> DOS -> Win32 may
therefore retain multiple backends, but selects only one interactive backend.
Suspended outer targets and their PTYs survive the nested DOS/native interval.
DOS itself retains its existing worker input/output route, not a PTY.

The owner requires one continuous visible screen across DOS/native branch
handoffs. Returning to an outer branch must not restore its old screen or
overwrite the intervening output. Independent backend lifetime does not imply
independent user-visible pages. Copying cells into a local terminal parser is
not synchronization of the remote Windows Console cursor; both subsequent
positioned output and scrolling must pass the same continuity tests.

After successful initial attachment, relinquish that backend's keepalive with
ReleasePseudoConsole. It is not reused for independent launches. Windows owns
attached-client lifetime and output EOF; ntcon drains output before disposal.
Direct-target completion still supplies only that requester's result and never
proves all Console clients have exited. No membership query helper, observer,
Job, process-tree kill or new execution scheduler is added. Missing API support
is an explicit unsupported result. Frontend admission and final teardown retain
the broker barrier, separately from each backend's EOF and task completion.

### Superseded root-run16 implementation record

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

Unclaimed startup failure rolls back through original UndoCreation. NTSRV's
worker_spawn owns the retained temporary non-inherited startup Job installed
atomically at worker creation; kill-on-close is disarmed after admission and
before Resume. It is not a target-observation Job or lifetime pairing. A claimed
worker retains reservation resources until exit.
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

The finite `frontend-client.lib` implementation is launcher-owned under
`run16-exe`: bootstrap and native request/receipt clients using service RPC.
Their local declarations stay with the implementation. Wire declarations
and IDL live in common/protocol; the copied launch codec and neutral ordered
pipe mechanics have separate common providers. NTCON links common protocol/
codec/I/O, not launcher target creation. NTVWM reuses the codec and the separately selected launcher-owned
native resource/CreateProcess primitive; worker Console state, execution and
completion remain NTVWM-owned. This is the same owned-client-library pattern
as the NTSRV client, not a generic shared component, process, or runtime
dependency on run16.exe. Worker-base remains worker-only.

```text
run16 -> ntsrv protocol client
ntmon -> ntsrv management protocol client
run16 -> opennt-host Base client + opennt-abi/host-compat
ntsrv -> service RPC transport + opennt-host BaseSrv owner
ntvdm -> worker-local session, command, monitor, SoftPC, Redirector, VDD, WOW and debugger bindings
ntvdm -> original mvdm + opennt-host + opennt-abi/host-compat

opennt-abi/host-compat -> public modern Win32/NTDLL only
common/protocol -> versioned fixed-width protocol declarations and service IDL
common/{codec,transport,console} -> neutral selected shared mechanisms
ntcon <-> workers -> common direct I/O pipe protocol (not service lifecycle)
ntvdm/ntvwm -> worker-base -> shared project-owned worker mechanisms
ntvdm-exe/package_layout -> worker-local media/session configuration
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
