# Pending host candidates: file ownership reconciliation

Snapshot: `host-ch-scope/inventory.csv`, filtered to `host-candidate`, `pending`/`待核验`, `.c`/`.h` under the four named directories. Each row below covers one of the 33 resulting paths. Classification is a bounded recommendation, not an edited ledger, runtime pass, deletion authorization or fresh build binding. Stage numbers cite the [provisional worklist](../recovery-worklist.md); an original tool's product scope is not silently assumed.

| File | Original role; observed selection | Proposed classification / functional handoff |
| --- | --- | --- |
| fax/faxcomm.h | Shared driver/UI ABI; neither separate fax image in checked graph | Historical declaration, ABI/ownership unverified; S16 fax. |
| fax/wowfax/wowfax.c | Separate original GDI fax driver; graph not selected | Historical provider, actual necessity pending; S16. |
| fax/wowfax/wowfaxdd.h | Driver declaration; graph not selected | Historical declaration; S16. |
| fax/wowfaxui/wfhelper.c | Separate UI helper in original UI recipe, not checked graph | Historical provider; S16 UI. |
| fax/wowfaxui/wfsheets.c | Original UI property-sheet implementation, not checked graph | Historical provider; S16 UI. |
| fax/wowfaxui/wfsheets.h | UI sheet declaration, not checked graph | Historical declaration; S16 UI. |
| fax/wowfaxui/wfupgrad.c | Original UI upgrade implementation, not checked graph | Historical provider; S16 UI. |
| fax/wowfaxui/wowfaxui.c | Original separate UI DLL entry/callbacks, not checked graph | Historical provider; S16 UI. |
| fax/wowfaxui/wowfaxui.h | Shared UI declarations, not checked graph | Historical declaration; S16 UI. |
| vdmdbg/vdmdbg.c | Original external debugger client, not captured product graph | Separate tool role; scope/ABI pending S17 debugger. |
| vdmredir/vrdebug.c | Original redirector debug body, selected object empty in release | Profile-limited disabled body; S12/S17 preserve redirector/debug obligations. |
| vdmredir/vrdebug.h | Original redirector diagnostic interface, included by multiple sources | Selected declaration; debug/profile and S12/S17 redirector behavior not established. |
| vdmredir/vrdlc.h | Original DLC interface used by redirector sources | Selected declaration; S12/S17 DLC lifetimes pending. |
| vdmredir/vrdlcdbg.c | Original DLC diagnostic body, selected object empty in release | Profile-limited disabled body; S12/S17 DLC. |
| vdmredir/vrdlcdbg.h | Original DLC diagnostic interface included by DLC sources | Selected declaration; debug/profile remains unverified, S12/S17. |
| wow32/debug.c | External WOW debugger extension, not captured product graph | Separate tool role; scope/ABI pending S17 debugger. |
| wow32/wctlm32.h | Original reverse control-message thunk declarations; fragment unselected | Historical declaration; S15 real reverse conversions. |
| wow32/wcurcash.c | Original separate resource-cache implementation, not selected | Historical provider; S14/S15 selected cursor alias/lifecycle. |
| wow32/wcurcash.h | Its original cache declarations, not selected | Historical declaration; S14/S15 cursor capability retained. |
| wow32/wddeml32.c | Original DDEML host thunk, no observed recipe edge | Historical provider; S14 guest DDEML/callback route. |
| wow32/wddeml32.h | Original host DDEML thunk declarations | Historical declaration; S14. |
| wow32/wddetbl2.h | Original host DDEML ordinal table, master does not include | Historical table; S14. |
| wow32/winspldl.h | Historical spooler individual pointer declarations, no observed consumer | Historical declaration; selected spoolerapis and S16 print remain. |
| wow32/witbl.h | Historical internal table declarations, no observed consumer | Historical declaration; actual dispatch remains. |
| wow32/wkglobal.h | Declarations of unselected historical host Global thunks | Historical declaration; guest global heap remains. |
| wow32/wklocal.h | Historical Local thunk declarations included via precomp; provider unselected | Selected historical declaration, S14/S15 guest heap/callback lifetime retained. |
| wow32/wmmalias.c | Historical multimedia alias functions in object, map lacks their callable entries | Compiled but no observed active route; S16 handle identity/lifetime. |
| wow32/wmmalias.h | Its alias types/macros, selected as provider dependency | Selected original declarations, active consumer not proven; S16. |
| wow32/wmmedia2.c | Historical wave/MIDI converters, outer `#if 0` | Disabled in observed profile; S16 actual guest multimedia route. |
| wow32/wmmstru2.c | Historical MCI converter, outer `#if 0` | Disabled in observed profile; S16 actual MCI route. |
| wow32/wmmstruc.h | Original conversion macros/types included via precomp | Selected declaration; observed macro uses disabled, S16 conversion/lifecycle. |
| wow32/wowdde.h | Original DDE data declarations, selected duplicate in wdde.h | Historical duplicate in observed profile; S14 classic DDE/DDEML. |
| wow32/wowtable.h | Original combined table externs, selected duplicate in wowtbl.h | Historical duplicate in observed profile; dispatch/ordering retained. |

Evidence: [fax snapshot](../nine-way/part-2/fax-boundary-review.md), [debug client](../nine-way/part-5/debugger-semantic-reconciliation.md), [redirector object test](../cross-369/redir-debug-provider-disposition.md), [redirector headers](../nine-way/part-6/tools-debug-redirector-role-review.md), [cursor](../cross-369/cursor-cache-disposition.md), [DDEML](wow-ddeml-three-file-roles-review.md), [tables and conversion](wow-three-table-structure-recheck.md), [other WOW handoff](wow-remaining-owner-handoff.md), [aliases](wow-old-multimedia-files-review.md), [local memory](../cross-369/local-memory-header-disposition.md), [DDE data](../cross-369/dde-header-disposition.md), [MCI](../cross-369/mci-converter-disposition.md), [old spool/internal](wow-witbl-winspldl-roles-review.md), [old cursor/global](wow-wcurcash-wkglobal-roles-review.md), [control messages](../shard-1/wow-callback-control-headers-review.md). Referenced snapshots include hashes and reproduce scripts where available. Selected declarations and entry points are not evidence their whole functionality ran; per-feature tests, other profiles, generated/external callers and current shipped-image identity remain separate.
