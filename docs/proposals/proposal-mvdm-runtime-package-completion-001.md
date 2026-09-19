# MVDM runtime package completion

## Objective

Replace trace-led, function-by-function recovery with complete original
package-unit recovery for the Win32/x86 product. A package is connected only
when its complete selected original manifest has one source/build profile,
every reached external contract has one finite owner, its original lifecycle
and failure rules are retained, duplicate project policy is removed or
explicitly retained, and its local acceptance matrix passes.

One S closes exactly one original package unit.  It may not combine two units
under one heading, treat a guest activation contract as an unnumbered suffix,
or claim a selected file is an entire package. A focused runtime trace may
verify an already-complete package; it may not select a new function, shrink a
package, or authorize a new provider. No S closes merely because it compiles.

## Audit baseline

The canonical tracker contains 23 top-level MVDM roots. Current formal x86
build selection splits the applicable runtime roots into 19 original package
units: CCPU386; SoftPC BIOS, keymouse, system, disks, support, video, C-VID,
comms, DOS, base trace and host roots; DEM; COMMAND; XMS; DPMI32; suballoc;
OEMUNI; and VDMREDIR. `VDMREDIR` is a separate DLL; the other selected units
are worker libraries.

Under the strict package-completion definition, **zero** runtime package units
are presently certified complete. The 19 selected units are only build-linked
or locally verified. Their package-level gaps are recorded in the linked audit
and must be closed by their own S packets. `dbg`, `vdd` and `wow32` have only
limited binding or historical provider evidence and are partial package roots,
not connected owners.

The following roots are not current runtime packages: `bde`, `dirs`, `fax`,
`ieuvddex`, `inc`, `makefil0`, `sim16`, `sim32`, `v86`, `vdmdbg`, `vdmexts`,
and `vdmutils`. Each has a tool, declaration, build-control, CPU30-monitor or
extension disposition; none may be linked merely to improve a count. `dpmi`
and `wow16` are load-only guest packages: their source/binary manifests and
activation contracts are closed by their respective DPMI32 and WOW32 package
S packets, never by host linkage.

## Ordered S package units

The exact S order is dependency-first.  This is a **43-S plan**. S1--S20
recover the original packages; S21--S39 then re-open those same packages one
at a time for their missing real-capability acceptance. S40--S43 retain the
subsequent original WOW/debug/VDD package order. Every S therefore has exactly
one named original package owner. No later S may quietly absorb an earlier
row's residual. Each S freezes its complete original manifest and source
hashes, proves its outgoing interfaces, composes original bodies before
adapters, runs its local matrix plus one integrated regression, measures
mirror/adapter changes, then commits and pushes.

| Planned S | One original package unit | Whole-package completion obligation |
| --- | --- | --- |
| S1 | `softpc.new/base/ccpu386` | CPU execution, exception, interrupt, event and descriptor lifecycle. |
| S2 | `softpc.new/base/cvidc` | C-VID vector table, timing, publication and all selected consumers. |
| S3 | `softpc.new/base/video` | Original text/graphics update path and finite Console presentation boundary. |
| S4 | `softpc.new/base/system` | Timers, PIC, reset and device lifecycle. |
| S5 | `softpc.new/base/keymouse` | Keyboard/mouse controller, callback and teardown lifecycle. |
| S6 | `softpc.new/base/bios` | Firmware-state initialization and every selected host-facing failure path. |
| S7 | `softpc.new/base/support` | Shared SoftPC support state, resource and failure contracts. |
| S8 | `softpc.new/base/disks` | Disk device contract, backing storage and error/teardown behavior. |
| S9 | `softpc.new/base/comms` | Communications device state and selected host I/O boundary. |
| S10 | `softpc.new/base/dos` | SoftPC DOS-device hooks and their complete BOP/device contract. |
| S11 | `softpc.new/base/debug` | The full original debug package, including both `trace.c` and `btrace.c`; current trace-only selection is not completion. |
| S12 | `softpc.new/host/src` | Original worker startup, Console, thread and host lifecycle; it must consume the settled S11 debug disposition rather than own debug policy. |
| S13 | `dos/dem` | Full DEM DOS-execution, BOP, file/guest-copy and failure contract. |
| S14 | `dos/command` | COMMAND EXEC, native-child, environment, return and error lifecycle. |
| S15 | `xms.486` | XMS allocation, move, mapping lease and teardown contract. |
| S16 | `suballoc` | Original suballocator state, allocation and release lifecycle. |
| S17 | `oemuni` | OEM/Unicode conversion source, buffers and failure behavior. |
| S18 | `dpmi32` | Descriptor, PM stack, selector, interrupt and protected-mode teardown contract. |
| S19 | `dpmi` | Load-only guest DPMI manifest, DOSX activation and return contract; no host-link substitution may stand in for it. |
| S20 | `vdmredir` | The complete redirector DLL, handle/guest-copy, stream and redirection lifecycle. |
| S21 | `softpc.new/base/ccpu386` capability closure | Real guest instruction/FPU, exception/IRQ, event, descriptor and worker-thread lifetime acceptance. |
| S22 | `softpc.new/base/cvidc` capability closure | Real selected-profile vector/publication/timing/consumer acceptance. |
| S23 | `softpc.new/base/video` capability closure | Real text/video update, scrolling and display-state acceptance within the selected Console presentation boundary. |
| S24 | `softpc.new/base/system` capability closure | Real timer/PIC/reset, CMOS/DMA/ROM and device-failure lifecycle acceptance. |
| S25 | `softpc.new/base/keymouse` capability closure | Real keyboard-controller/modifier and mouse INT 33h callback/teardown acceptance. |
| S26 | `softpc.new/base/bios` capability closure | Real BIOS services, RTC/EMS and selected device/error-service acceptance. |
| S27 | `softpc.new/base/support` capability closure | Real startup/environment/time/IOS/termination resource and failure acceptance. |
| S28 | `softpc.new/base/disks` capability closure | Real disk read/write, backing-store, media/error and teardown acceptance. |
| S29 | `softpc.new/base/comms` capability closure | Real COM transmit/receive, printer/Print-Screen and device-error acceptance. |
| S30 | `softpc.new/base/dos` capability closure | Real default DOS, HIMEM/DOSX and explicit EMS profile acceptance; EMS allocation/map/unmap/release, complete DOS-device/BOP acceptance, and owner-expanded real original COM/LPT host-medium recovery. |
| S31 | `softpc.new/base/debug` capability closure | Real original debug initialization, trace/btrace/event dispatch or a complete profile exclusion. |
| S32 | `softpc.new/host/src` capability closure | Real worker thread, Console, host provider and normal/abnormal resource-lifecycle acceptance. |
| S33 | `dos/dem` capability closure | Real DOS create/open/share/read/write/seek/find, error mapping and guest-buffer cleanup acceptance. |
| S34 | `dos/command` capability closure | Direct/nested COMMAND child, standard-stream, redirection, pipe, return/error and cleanup acceptance. |
| S35 | `xms.486` capability closure | Real DOS XMS allocate/move/overlap/free/A20 and failure/teardown acceptance. |
| S36 | `suballoc` capability closure | Real XMS/DPMI-backed allocation, relocation, exhaustion/release and teardown acceptance. |
| S37 | `oemuni` capability closure | Real DOS/Win16 OEM-Unicode non-ASCII path, buffer and failure acceptance. |
| S38 | `dpmi32` capability closure | Real protected-mode selector/interrupt/memory/return/teardown acceptance. |
| S39 | `dpmi` capability closure | Real DOSX BOP 53 activation, protected-to-real transition and repeated-entry release acceptance. |
| S40 | `wow32` | Original provider load, task/callback, font/glyph, icon/cursor, scheduler and hard-error contract. |
| S41 | `wow16` | Load-only guest WOW16 media, loader/task activation and callback-return contract; attribute and recover the first `WRITE.EXE` blocker through its original owner. |
| S42 | `dbg` | Original debugger initialization, state, dispatch and event contract, or a source-proven complete product-profile exclusion that deletes every thin substitute. |
| S43 | `vdd` | Original VDD lifecycle and consumers, or a source-proven complete product-profile exclusion that deletes every thin substitute. |

The 19 currently formal-linked library/DLL units are only a starting build
inventory, not the S structure: their trace-only `base/debug` selection,
limited `dbg`/`vdd` bindings, and load-only `dpmi`/`wow16` media are precisely
why these four rows are independent S packets. The current `dbg` binding, the
former WOW/debugger proposal and any local replacement scheduler do not
constitute completion.

## S21--S39 retrospective capability closure

The completed S1--S19 source-recovery records establish package selection,
source shape and focused contracts. They do **not** by themselves establish
that every original capability family is reachable from a real guest and
returns through its original cleanup path. After S20's own redirector
acceptance closes, S21--S39 execute before S40. Each is a mandatory,
single-package acceptance S, not a way to rename a missing original package
or to reopen source recovery without evidence.

Each S creates a tracked test matrix. Every listed family must end as one
of: real guest end-to-end pass; source-proven no selected caller; or an
owner-approved exclusion. A compile, map entry, host-only fixture, or the
generic COMMAND/MEM/EDIT smoke route is insufficient on its own. Every pass
also records normal operation, representative failure, task/handle cleanup,
and the mandatory direct plus interactive COMMAND regressions.

### Required recovery effort and external-boundary record

For S30--S43, an original OpenNT capability remains in scope when the
standalone NTVDM product is its selected caller, even if the historical NT
service, network protocol, device class, or local hardware is unavailable on
the modern host.  The owner requires the following four records before an S
may close:

1. Remove every proven unnecessary mirror diff and non-mirror autonomous
   implementation; measure removed versus retained lines separately.
2. Recover and connect every capability that can compose through the original
   caller and a source-shaped modern-host boundary, then verify it from a real
   DOS/Win16 guest through normal operation, representative failure and
   cleanup.
3. Audit every retained mirror diff, overlay and adapter seam.  The evidence
   must name its original owner, required ABI/lifetime behavior, why its
   structure is necessary, and why restoring the exact original body is not
   composable; unreviewed or merely convenient code cannot remain.
4. When an original capability cannot be connected only because the modern OS
   no longer supplies the required service/protocol (for example RAP or DLC)
   or the host lacks the necessary hardware/peer (for example a CTS-capable
   serial endpoint), exhaust non-invasive original-path options first.  Then
   preserve the original failure behavior, record the attempted paths and
   observed boundary, and add a concrete follow-up row to
   `docs/states/TODO.md`.  That row must name the original caller/provider,
   missing host prerequisite, retained code/diff, evidence location and exact
   condition that permits a future retest.  An unavailable medium is evidence
   of a platform boundary, never an implicit pass or a reason to add a fake
   provider.
5. If a protocol-accurate mock, fake device endpoint, controlled peer, or
   fault-injection harness can be made without changing product behavior, it
   is mandatory test-only work.  It must exercise the original guest caller,
   selected original provider, request/response or I/O boundary, failure, and
   teardown so that a later machine with the real prerequisite has the same
   executable acceptance fixture.  The evidence labels it **mock/unit
   evidence**, records exactly what host property it does and does not prove,
   and keeps it outside the deployed product graph; a mock success never
   substitutes for the required real-host end-to-end result.

These requirements refine, rather than replace, the per-package matrices
below and the final exit criteria.  A capability can be source-proven absent
only when no selected caller can reach it; a reachable capability cannot be
reclassified as a profile exclusion merely because current hardware or a
historical Windows service is absent.

| Capability-closure S | Revalidated original package | Required real-capability acceptance |
| --- | --- | --- |
| S21--S27 | S1 CCPU386 through S7 support | Each listed S proves its own real guest instruction/FPU/exception/event, vector/publication, text/video, timer/PIC/reset, keyboard/mouse, BIOS, startup/environment/termination capability as specified in the ordered table. Profile-null C-VID slots may remain null only with the existing original-profile proof. |
| S28--S32 | S8 disks through S12 host | Each listed S proves its own disk, COM/printer, EMS/DOS-device, debug-profile or worker-host service through actual device/service workloads and normal plus abnormal cleanup. |
| S33--S37 | S13 DEM through S17 OEMUNI | Each listed S proves its own DOS file/error, COMMAND child/standard-stream, XMS, SubAlloc or OEM/Unicode workload, including representative negative and cleanup cases. Standard-stream parsing/attachment remains COMMAND/BaseSrv-owned, while an attached named-pipe handle must be accepted by VDMREDIR. |
| S38--S39 | S18 DPMI32 and S19 guest DPMI/DOSX | Each listed S proves its own protected-mode/DOSX portion: BOP 53, GDT/LDT/IDT, selectors, interrupts, DPMI/XMS memory, return to DOS, repeated entry and complete release. Descriptor-only host fixtures and a DOSX file hash do not close either S. |

S21--S39 may add test-only host or guest fixtures only under the source policy
and build-output rules. They may not patch immutable guest media, invent an
adapter-owned replacement policy, or change a prior source-recovery result
without a new source-first audit. A failure identifies its original package
owner and the exact entry/caller; the repair is then contained within that
same sequential capability-closure S rather than an unbounded workaround.
Where a real prerequisite is absent, this permission is also the mandatory
test-only mock/unit-harness route specified above; it does not authorize a
product-side fake provider.

## S40--S43 capability closure requirements

S40--S43 retain their one-original-package ownership and must each satisfy its
own real-workload closure; none may use the earlier generic COMMAND/MEM/EDIT
matrix as its sole acceptance evidence.

| S | Additional mandatory closure evidence |
| --- | --- |
| S40 `wow32` | Load the selected original provider in a real worker; prove task creation, callback enter/return, font/glyph and icon/cursor representations, scheduler/wait behavior, private hard-error delivery and task/worker teardown. A provider export or host-only callback fixture alone is insufficient. |
| S41 `wow16` | Verify immutable guest-media identity, then identify the first reached original owner that blocks `WRITE.EXE`, and recover it only through the source-first dependency rule. Launch `WRITE.EXE` through its original loader; it must create a task, execute, return to the parent DOS/worker state, and leave a subsequent DOS workload usable. No substitute workload may close S41. |
| S42 `dbg` | Exercise original debugger initialization, event/breakpoint or exception dispatch and termination through a controlled guest workload; otherwise prove all selected consumers are absent in the product profile and remove every remaining thin binding. Neither option may leave an untested partial provider. |
| S43 `vdd` | For every selected VDD consumer, prove provider load, request/notification, resource lifetime and unload/worker termination; otherwise prove complete product-profile exclusion and remove every thin substitute and stale consumer edge. |

## Package S exit criteria

Every S records the exact original manifest and selected source hashes;
classifies every manifest member as direct, same-shaped finite binding or
explicit profile exclusion; proves all reached external functions, globals,
callbacks, structures, resources and failure paths; and removes a duplicate
local implementation whenever the original owner now composes. It must run
the full package-local positive/negative matrix, fresh formal x86 build, and
the following mandatory established-product regression matrix before it may
close:

| Entry route | Required workloads |
| --- | --- |
| Direct launcher route | `run16.exe COMMAND.COM`, `run16.exe MEM.EXE`, and `run16.exe EDIT.COM` each start, run and return with their established expected behavior. |
| Interactive COMMAND route | Start `COMMAND.COM` through `run16.exe`; at its prompt run `COMMAND`, `MEM`, and `EDIT`; each child must execute and return to the same usable parent prompt. |

The interactive route is not a cosmetic duplicate: it proves original COMMAND
re-entry, broker delivery, worker lifecycle and Console state survive a child
run. `EDIT` must leave the parent prompt usable, after which `MEM` must still
produce its established normal output. A missing interactive-input observer
may limit automation, but cannot convert an unrun or failed owner-run test
into a pass. The S evidence records the exact commands, package hashes,
observed result and any owner-performed visual acceptance. It reports original
mirror diff, adapter code, removed autonomous code and residual binding lines
separately.

The package remains partial if any manifest member is silently omitted, an
adapter still owns original policy, a lifecycle/teardown path is unproved, or
a local or mandatory established-product regression is missing or fails. A
profile exclusion closes only when the source policy and product profile prove
the package cannot be a runtime dependency.  For S30--S43, the closure record
must additionally contain the four required recovery/external-boundary records
above, including a linked `TODO.md` row for every reached capability stopped by
an unavailable modern service, protocol, device, or peer.

## Boundaries

No CPU30/kernel VDM monitor, CSRSS/CSR transport, Win32k/USER/GDI server,
HAXM/injector, native x64 worker, guest-byte patch or blanket NTVDMx64 import.
External patches remain comparative evidence for an individually reproduced,
source-shaped correction only. `WRITE.EXE` is the successor package's
integration workload, not a substitute for these package closures.

### Kernel-VDM boundary recovery rule

When selected original MVDM code calls an NT4 kernel-VDM service, recover the
original user-mode caller and its exact public function name, parameter
layout, call ordering, status/failure behavior and teardown first.  A
directly composable original non-MVDM OpenNT implementation belongs under its
original relative path in `opennt-host`; it must not be re-authored as an
adapter merely because it sits outside MVDM.  Kernel-only implementation
bodies whose required kernel state cannot compose in the standalone worker
remain a finite worker-local adapter only after the source-first rung ledger
records that exclusion.

The adapter must preserve the original service-shaped facade and contain only
the unavailable mechanism.  It may not take ownership of original MVDM
policy.  Each package must explicitly measure and remove mirror-side probes,
declarations, alternate control paths and autonomous state once the facade
can provide the required original observable semantics.  In particular, a
physical-memory facade that can establish the original contiguous alias
semantics must retire CCPU-side translation/resolution probes rather than
retain them for convenience.  A same-named facade alone is not proof of
equivalence: address-space layout, synchronous-access lifetime, failure and
worker teardown must be verified before claiming that reduction.
