# M0 T473 S1 — finite S2 source reconciliation plan

## Frozen input

The r26 whole-closure inventory has 4,684 pinned-union rows.  It identifies
444 selected product sources (350 `ntvdm.exe`, 16 `VDMREDIR.DLL`, 78
`WOW32.DLL`), 195 semantic mirror deltas and no unclassified selected source.
The candidate search did **not** find a missing original host source whose
unconditional import is justified.  Therefore S2 is not a bulk-import phase:
it is a finite, source-shaped reconciliation of the already selected original
owners and the project-owned boundary bodies that coexist with them.

The r26 CSV and its SHA-256 are the frozen input in the
[whole-closure baseline](m0-t473-s1-whole-closure-baseline.md).  The
[semantic-delta ledger](m0-t473-s1-semantic-delta-ledger.md),
[unselected-source disposition](m0-t473-s1-unselected-host-source-disposition.md),
[runtime-owner matrix](m0-t473-s1-runtime-owner-matrix.md), and
[boundary matrix](m0-t473-s1-adapter-boundary-matrix.md) are normative S2
worklists, not optional background material.

## S2 work packages

| Order | Bounded owner group | Reconciliation question | Completion proof |
| --- | --- | --- | --- |
| 1 | CPU40 execution and descriptor carriers: CCPU, `fmstubs`, DPMI32 and `softpc-bindings` | Does each project-owned execution/address/descriptor body merely adapt a named selected original call, or duplicate an original algorithm?  Can a complete original provider replace it without reviving CPU30 or private kernel-V86 dependencies? | Caller-to-owner map, field/handle authority table, selected x86 build/link map, normal/failure/teardown fixture.  Remove a duplicate only when the replacement has the same complete closure. |
| 2 | DOS/COMMAND/XMS/DEMs: COMMAND, CMD data, DOS helpers, EMS/XMS and worker command bindings | Do COMSPEC construction, guest memory leases, native-child transfer and return preserve the selected original owners, rather than make the binding layer a shell engine? | Nested COMMAND, native CUI child, redirection/pipe, failure and return-path evidence; each retained lease and host process boundary has a single owner. |
| 3 | SoftPC devices: system, BIOS, keymouse, disks, video/C-VID and COM/LPT | Are device bridges only modern input/output/resource conversions, with PIC/keyboard/video/disk algorithms still selected original owners? | Device-specific source/caller map; reset, EOI/read order, frame/input and cleanup evidence; no bridge maintains a competing controller state. |
| 4 | WOW32 selected owner group and worker/session bindings | Do WOW page, USER/GDI, Winsock, task termination and session bindings transfer identity and lifecycle without replacing original conversion or scheduling behavior? | Task create/normal completion/termination, nested callback, failure and worker teardown evidence; `WOW32.DLL` map and its 78 selected-source disposition. |
| 5 | VDMREDIR, monitor/debugger and unavailable boundaries | Do redirector copy/async, debugger/monitor bridges and explicit unavailable VDD paths preserve their original dispatch or fail explicitly, rather than fabricate a private subsystem? | DLL export/link/runtime reachability, cancellation/teardown and unavailable-result fixtures; exact `VDMREDIR` 16-source selection. |

Each package ends with one of only three source outcomes for every reviewed
body: (a) selected original owner retained; (b) project-owned adapter retained
with exactly one modern resource/ABI responsibility; or (c) redundant body and
its build selection removed.  A fourth outcome—"maybe useful later"—is not
permitted.  A source cannot be removed merely because its normal smoke test
passes.

## Explicit boundaries and non-work

S2 must not import unavailable private kernel V86 monitor code, CPU30 profile
alternatives, guest/firmware media, disabled `!IF 0` WOW code, UMTEST sources,
or historical build artifacts.  Their disposition is already evidence-backed;
reopening one requires a concrete product caller and a finite modern boundary.
Likewise, a carrier that projects a modern handle, Console endpoint, public
window identity or immutable media lease is not made removable solely by an
original symbol with a similar name.

S2 is deliberately finite.  It does not claim runtime-package completion;
S3 admits only the package groups still partial after this source decision,
in the same order, with real behavior evidence.  S4 removes only duplicates
actually displaced by S2/S3.  S5 independently repeats the r26-style
source/build/runtime reconciliation against the post-recovery baseline.

## S1 exit statement

Every upstream row is now categorized by identity, compile selection, product
selection and disposition; every selected semantic delta has an owner; and
every project-owned product link group has one allowed boundary role.  The
remaining work is the five bounded reconciliation groups above, not an open
ended search for original code.
