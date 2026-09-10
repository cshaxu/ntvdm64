# M0 T396 S1 — WOW32 complete-provider admission audit

Date: 2026-09-06

## Question

What exact original provider must be recovered to let a real Win16 NE client
pass the CPU40 DPMI bootstrap, and what is the smallest source-shaped next
cohort that can be admitted without silently substituting an NT4 USER/CSRSS
product shell?

## Inputs and procedure

The audit treats `src/mvdm-host/wow32/sources` as the selected build authority,
not the set of `.c` files which happen to be present in its directory.  It
inspected that manifest, `wow32.def`, `wowtbl.c`, `wow32.c`, `wkman.c`, the
`MS_bop_1` first-WOW route in `softpc.new/host/src/nt_bop.c`, the current formal
graph, the source-policy stopping boundaries, and the retained condition-aware
WOW ledgers.  The following existing controlled ledgers were reverified during
this audit:

| Record | Rechecked result |
| --- | --- |
| `m0-t276-s22-wow32-active-subdomain-ledger.tsv` | 77 selected C bodies across eight original subdomains; none is enabled. |
| `m0-t276-s22-wow32-interface-family-ledger.tsv` | nine source-proven interface families, with an owner and mapping constraint. |
| final provider disposition | all 77 selected provider rows accounted for. |
| final carrier disposition | 124 WOW32 rows accounted for; the selected declaration subset is 105 carriers. |

The 105 declaration carriers are exactly the 104 root headers plus
`wow32.def`.  `wow32.rc` is a resource input and `wow32.prf` is a linker
profile, not a declaration.  The directory also contains six physical C
files that are not selected by `sources` (`debug.c`, `wctlm32.c`,
`wcurcash.c`, `wddeml32.c`, `wkglobal.c`, and `wklocal.c`); they remain
historical evidence and are not silently added to the provider cohort.

The first formal probe reached `wow32.c`'s original `#include <ntcsrdll.h>`
and exposed an omitted prerequisite declaration: `ntcsrdll.h` includes
`ntcsrmsg.h`.  The latter is byte-identical in all three approved OpenNT
baselines (`OpenNT/public/sdk/inc`, `OpenNT-4.5/nt/public/sdk/inc`, and
`opennt-src-2/nt/public/sdk/inc`), SHA-256
`896D25BB240D105C64F96AB06F97BEE8E013AD7C4E679A0292B07C5E594C6FC7`.
S2 therefore admits that exact declaration-only carrier under `opennt-abi`;
it supplies no CSR transport, server, or product behavior.

## Complete source-defined provider cohort

The 77-body ledger is deliberately complete, rather than a trace-selected
thunk subset.  Its eight source-defined groups are:

| Original subdomain | Bodies | Count |
| --- | --- | ---: |
| thunk/frame/structure | `mapembed`, `wow32`, `wowtbl`, `walias`, `wstruc`, `wcall16`, `wcall32`, `wres16`, `wres32`, `walloc16`, `wheap`, `wparam` | 12 |
| message/window procedure | `wcntl32`, `wdde`, `wmsg16`, `wmdisp32`, `wmsgbm`, `wmsgcb`, `wmsgem`, `wmsglb`, `wmsgsbm`, `wmtbl32` | 10 |
| GDI/window object | `wgdi31`, `wgman`, `wgdi`, `wgfont`, `wgmeta`, `wgpal`, `wgprnset`, `wgtext`, `wreldc`, `wcuricon`, `wsubcls`, `wdib`, `waccel` | 13 |
| kernel/task/DOS session | `wkbman`, `wkmem`, `wkgthunk`, `wkman`, `wkfileio`, `wkernel`, `wowhooks`, `wdos`, `wthman` | 9 |
| USER interface | `wuser31`, `wuman`, `wucaret`, `wuclass`, `wuclip`, `wucomm`, `wucursor`, `wudlg`, `wuhook`, `wulang`, `wumenu`, `wumsg`, `wuser`, `wutext`, `wutmr`, `wuwind`, `wusercli` | 17 |
| shell/OLE/printing | `wow32fax`, `wshell`, `wcommdlg`, `wspool`, `wole2` | 5 |
| multimedia | `wsman`, `wmmedia`, `wmmedia1`, `wmmedia2`, `wmmstruc`, `wmmstru1`, `wmmstru2`, `wmmalias` | 8 |
| network | `wsdata`, `wsext`, `wsraw` | 3 |

Names in the table omit only the `.c` suffix.  The condition-aware source
manifest additionally selects `wow32.rc`; it does not select the old disabled
table-source alternatives under `!IF 0`.  `wowtbl.c` instead composes the
active table headers, so their layout and order are part of the same provider
contract.

## Bootstrap and lifecycle ownership

The original first `51h` BOP route in `MS_bop_1` loads `WOW32`, resolves
`W32Init`, `W32Dispatch`, `GetCommHandle`, `GetCommShadowMSR`, and
`W32HungAppNotifyThread`, calls `W32Init`, then enters `W32Dispatch` on the
CCPU route.  This is an image/export contract: placing selected functions in
the EXE would be a different product topology.

`W32Init` fills the inbound WOW handler table, calls
`UserRegisterWowHandlers`, registers its Base callback, locks ShareWOW state,
and creates/records task-process state.  `W32Dispatch` consumes the original
guest frame and dispatch tables.  `wkman.c` owns the next command/task path:
it calls `GetNextVDMCommand`, starts/returns WOW tasks, and uses `ExitVDM` for
task and all-task completion.  Thus the normal client proof remains
`51h → W32Init → W32Dispatch → Win16 task → ExitVDM → DOSX 53:13/53:0D`, not
a synthetic BOP sequence.

The current formal CPU40/x86 graph contains no WOW32 object, DLL, or export
manifest.  It proves DPMI bootstrap only and cannot supply this provider.

## Boundary dispositions and recovery ladder

| Boundary | Source form | S2 disposition |
| --- | --- | --- |
| guest frame/callback and VDM memory | `GETVDMPTR`, `CallBack16`, `Sim32GetVDMPointer` | retain source calls; compose through existing bounded `softpc`/`monitor` session leases, never a durable native pointer. |
| task/command lifecycle | `GetNextVDMCommand`, `ExitVDM`, WOW task state in `wkman.c` | retain the full original cohort and connect only through the existing named session/broker lifecycle seams. |
| USER handler/ShareWOW/private words | `UserRegisterWowHandlers`, `RegisterWowBaseHandlers`, `LOCKSHAREWOW`, private `user32p`/`gdi32p` forms | per-call adapter-WOW assessment; no public-API lookalike is presumed equivalent and no USER/Win32k recreation is admitted. |
| CSRSS/hard-error | `CsrIdentifyAlertableThread`, hard-error route | retain source shape; `wow_private_unavailable.c` is an existing controlled-stop boundary for an unavailable private compound service, not a general fake CSRSS provider. |
| public shell/dialog/spool/serial, multimedia and network | original conversion bodies and public libraries | retain each body; map identity through session mappings only after the original parameter/error contract is audited. |

This applies the source-policy ladder: source composition first; an existing
bounded seam or a named source-shaped facade second; no recursive import of
CSRSS, BaseClient, USER/GDI server, Win32k, or kernel VDM.  It also supersedes
the obsolete historical rule that described the retired Bochs executor as a
WOW boundary owner; CPU40/SoftPC is the only product machine route.

## Interpretation and next cohort

S1 passes its admission condition.  The smallest valid next cohort is not one
of the eight subdomains: it is the whole selected original `wow32.dll`
provider—77 C bodies, 105 declaration carriers, selected table headers,
`wow32.def`, `wow32.rc`, and the original DLL/export topology—plus only the
named, separately audited modern facades needed at its private boundaries.
It explicitly excludes the six unselected C bodies, disabled alternate tables,
guest-media changes, and a re-created private NT product shell.

S2 must first make that complete cohort enter the formal x86 graph as a
separate `wow32.dll` and prove its export/import closure.  It may not claim a
Win16 workload runs until the DLL has a source-shaped `W32Init`/`W32Dispatch`
entry route and each unresolved private import has a documented disposition.
The evidence is high-confidence for source ownership and topology; it is not
runtime-success evidence.
