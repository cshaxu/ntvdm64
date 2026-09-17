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

The exact S order is dependency-first.  This is a **24-S plan**: every row is
one named original package unit and one eventual S packet.  No row may close
two units, and no later S may quietly absorb an earlier row's residual.  Each
S freezes its complete original manifest and source hashes, proves its
outgoing interfaces, composes original bodies before adapters, runs its local
matrix plus one integrated regression, measures mirror/adapter changes, then
commits and pushes.

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
| S21 | `wow32` | Original provider load, task/callback, font/glyph, icon/cursor, scheduler and hard-error contract. |
| S22 | `wow16` | Load-only guest WOW16 media, loader/task activation and callback-return contract. |
| S23 | `dbg` | Original debugger initialization, state, dispatch and event contract, or a source-proven complete product-profile exclusion that deletes every thin substitute. |
| S24 | `vdd` | Original VDD lifecycle and consumers, or a source-proven complete product-profile exclusion that deletes every thin substitute. |

The 19 currently formal-linked library/DLL units are only a starting build
inventory, not the S structure: their trace-only `base/debug` selection,
limited `dbg`/`vdd` bindings, and load-only `dpmi`/`wow16` media are precisely
why these four rows are independent S packets. The current `dbg` binding, the
former WOW/debugger proposal and any local replacement scheduler do not
constitute completion.

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
the package cannot be a runtime dependency.

## Boundaries

No CPU30/kernel VDM monitor, CSRSS/CSR transport, Win32k/USER/GDI server,
HAXM/injector, native x64 worker, guest-byte patch or blanket NTVDMx64 import.
External patches remain comparative evidence for an individually reproduced,
source-shaped correction only. `WRITE.EXE` is the successor package's
integration workload, not a substitute for these package closures.
