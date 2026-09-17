# MVDM runtime package completion

## Objective

Replace trace-led, function-by-function recovery with complete original
package-unit recovery for the Win32/x86 product. A package is connected only
when its complete selected original manifest has one source/build profile,
every reached external contract has one finite owner, its original lifecycle
and failure rules are retained, duplicate project policy is removed or
explicitly retained, and its local acceptance matrix passes.

One S closes exactly one original package unit. A focused runtime trace may
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

The exact S order is dependency-first. Each S admits one named original unit,
freezes its manifest and sources, proves all outgoing interfaces, composes the
original bodies before adapters, runs its complete local matrix and one
integrated regression, measures mirror/adapter changes, then commits/pushes.

1. `softpc.new/base/ccpu386` — CPU execution, exception, interrupt, event and
   descriptor contracts.
2. `softpc.new/base/cvidc` — complete C-VID vector, timing and publication
   contract.
3. `softpc.new/base/video` — original text/graphics update and presentation
   boundary.
4. `softpc.new/base/system` — timers, PIC, reset, device lifecycle.
5. `softpc.new/base/keymouse` — keyboard/mouse controller and callback
   lifecycle.
6. `softpc.new/base/{bios,support,disks,comms,dos}` — each is a separate S,
   in that order, with its full manifest and device/failure matrix.
7. `softpc.new/host/src` — worker startup, Console, thread, host lifecycle and
   trace versus debugger disposition; `base trace` is retired or absorbed here.
8. `dos/dem`, then `dos/command` — complete BOP, EXEC, native-child,
   environment, return and error contracts as two separate S packets.
9. `xms.486`, then `suballoc`, then `oemuni` — complete memory, allocation and
   conversion package closures as three separate S packets.
10. `dpmi32` plus its load-only `dpmi` guest contract — descriptor, PM stack,
    selector, interrupt, teardown and DOSX activation closure.
11. `vdmredir` — complete DLL, handle/guest-copy, stream and redirection
    lifecycle closure.
12. `wow32` plus its load-only `wow16` guest contract — full original provider
    load, task/callback, font/glyph, icon/cursor, scheduler and hard-error
    owner recovery.
13. `dbg` — original debugger initialization, state, dispatch and event
    contract, or a source-proven product-profile exclusion with all thin
    substitutes removed. `vdd` is a separate final package S only if its
    product-profile consumer remains after the preceding closures.

Items containing several names are ordering headings only: every named source
unit after a comma or “then” is an individual S. The current `dbg` binding,
the former WOW/debugger proposal and any local replacement scheduler do not
constitute completion.

## Package S exit criteria

Every S records the exact original manifest and selected source hashes;
classifies every manifest member as direct, same-shaped finite binding or
explicit profile exclusion; proves all reached external functions, globals,
callbacks, structures, resources and failure paths; and removes a duplicate
local implementation whenever the original owner now composes. It must run
the full package-local positive/negative matrix, fresh formal x86 build, and a
declared integration workload. It reports original mirror diff, adapter code,
removed autonomous code and residual binding lines separately.

The package remains partial if any manifest member is silently omitted, an
adapter still owns original policy, a lifecycle/teardown path is unproved, or
a local matrix is missing. A profile exclusion closes only when the source
policy and product profile prove the package cannot be a runtime dependency.

## Boundaries

No CPU30/kernel VDM monitor, CSRSS/CSR transport, Win32k/USER/GDI server,
HAXM/injector, native x64 worker, guest-byte patch or blanket NTVDMx64 import.
External patches remain comparative evidence for an individually reproduced,
source-shaped correction only. `WRITE.EXE` is the successor package's
integration workload, not a substitute for these package closures.
