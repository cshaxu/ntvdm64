# WOW EDIT tab-stop unthunk and callback boundary

Shard-2 `wow32/wmsgem.c`; frozen reconciliation SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. Current hash 251325307C9E698EE579E3493F15F93ABB6DAFE7D289F334AC3714C513566BE9; original hash 88D05E4770453621472704E90EDCEFD6AD18B618528B4166DA3A9BA1EA1A3619. Selected product source remains partial. No existing guest or product image was changed.

`wmsg16.c` selects `ThunkEMMsg16` and `UnThunkEMMsg16` for the EDIT class. `ThunkEMMsg16` handles EM_REPLACESEL using a copied `LocalAlloc` buffer and EM_SETTABSTOPS using stack-or-heap widened integers, with DIV-346 returning failure on failed allocation. The original unthunk releases replacement or tab storage; the tab release tests the same signed positive count as acquisition. `wumsg.c` call-window-proc path places the actual native result into `mpex.lReturn` before calling the selected unthunker. Other send/callback paths must be individually verified before generalization.

**Source-level candidate:** `UnThunkEMMsg16`'s EM_SETTABSTOPS case has no `break` after cleanup and falls into EM_GETWORDBREAKPROC handling. When `lReturn` is nonzero the latter treats it as a tagged guest or native word-break procedure; it can modify the result and/or invoke `CallBack16(RET_SUBCLASSPROC)` depending on flags and cached state. Source control flow establishes this possibility, not whether a current real EDIT path returns nonzero, reaches this unthunker, changes guest-observable result or yields a failure. The missing break is outside the marked DIV-346 edits and must be compared against the pinned original before deciding if this is inherited semantics or a repair candidate.

T447 S4 documents the allocation guards and signed-predicate repair. `tests/observation/wow_reverse_construction_fixture.c` tests exact copied thunk/unthunk with controlled allocation and zero/negative counts; it sets `lReturn=0` in relevant zero/negative cases and its normal constructor context starts zero, so those cases do not establish correct behavior when the native result is nonzero. T457 dialog and broad edit witnesses likewise do not by themselves cover this branch.

Next finite test: use the existing source-shaped reverse fixture with `lReturn=1` and observe callback invocation, result, globals and release; add a zero-result control. Then verify a real selected Win16 EDIT SETTABSTOPS call's host return and observable guest result, including task reentry, before deciding a source-policy-compliant fix or registration. Preserve the original guest and keep the unrelated generic conversion paths unchanged.

## Direct production-object controlled observation

An x86 probe linked the current selected `build/.ninja/msvc-x86-release/wow32/obj/wmsgem.obj` directly, not extracted converter text. It supplies the selected structure layout and a recording `CallBack16` stub; `/FORCE:UNRESOLVED` leaves unrelated object branches unresolved and is not a production link. `UnThunkEMMsg16` was called with `EM_SETTABSTOPS`, zero item count, and separately injected `lReturn=0` and `1`. Zero: 0 callbacks, result 0. One: 1 callback, result 1, `W32WordBreakProc` set to pointer value 1. The probe exits 0 and records source/object/executable/map identities in `tabstop-object-evidence.json`. This demonstrates the actual compiled-object branch under controlled input, not that a real EDIT returned 1 or a guest saw the effect. The callback stub returns FALSE and must not be confused with the original guest callback. The object/source identity is recorded, not a claim of a fresh complete production rebuild.

The pinned `O:/repos.external/OpenNT/base/mvdm/wow32/wmsgem.c` likewise
falls from `EM_SETTABSTOPS` into `EM_GETWORDBREAKPROC` without a break.
Thus this source behavior is inherited from the original, not introduced by
DIV-346. Its compatibility intent and real guest reachability still require
verification before selecting a correction.

## Real Win16 guest SendMessage observation

An independently authored adaptation of the existing controls probe ran in a fresh-identity isolated package-c staging. Its USER16 SendMessage issued old EM_SETTABSTOPS to a real multiline EDIT with 64 entries and with zero-count reset; both returned guest DX:AX = 0:1. Phase 3 reports zero guest errors, after phase 2 had verified three real wordbreak calls and nested USER query. Observer reported `result=exited`. Source, input identities and decoded phases are recorded in `tabstop-guest-evidence.json`; the isolated stage was cleaned after observer exit.

This is a real guest return, but it does not directly observe the native return before unthunk, nor the selected unthunk invocation or `W32WordBreakProc` state. Combined with the selected-object controlled test and source call chain it narrows the risk; it is not proof that the host pointer became 1 in this run or that a later guest operation failed. An explicit non-invasive breakpoint/trace or equivalent identity-preserving witness is still required to establish that observation.

The retained `tabstop-guest-run.ps1` now takes an explicitly supplied probe
and a fresh build-owned output directory; it requires a successful observer
exit and an existing guest report before copying evidence. A second isolated
run in `build/t473-s10/tabstop-runner-verification-r2` exited 0 and produced
the same report SHA-256
`676E32A8E3276DA00BEE887C149737DC676C42A0DE7B0A4D65566E3E8835CA80`
as the decoded receipt above. The runner itself checks report existence and
process success, not individual semantic fields; compare the decoded report
separately. Its stage was cleaned.

## Real guest before/after worker memory observation

A second independent guest probe stops at two file gates, immediately before and after its first EDIT `EM_SETTABSTOPS`. The isolated worker is identified by exact stage image path and its loaded WOW32 module is path-checked. Two non-atomic ReadProcessMemory snapshots of map-derived `W32WordBreakProc` RVA `0xa6adc` in the same PID and module base show **0 before, 1 after** the first guest SendMessage, whose guest return is 1. Guest phase 0x30 and final phase 3 report zero errors; reset returns 1 and observer reports exit 0. The guest remains at each gate while sampled; this is not a direct breakpoint at `UnThunkEMMsg16`, and no later crash was asserted.

The selected Ninja WOW32 DLL and base package-c DLL have identical `.text` and `.data` raw hashes and section layouts, supporting map-RVA correspondence; their full image hashes differ. Fresh package identity creates another staged image and the staged DLL hash was not recorded before cleanup: that limits strict binary identity proof. The direct object probe separately demonstrates the nonzero unthunk branch. All identities, section hashes, PID/base and guest phases are in `tabstop-real-snapshot-evidence.json`. No original guest or product image was modified.

## Second real guest snapshot and staged image identity

`tabstop-snapshot-run.ps1` was adjusted to keep phase logs, snapshots and a
copy of the actual staged DLL in its fresh build-owned output directory. A
second isolated run at `build/t473-s10/tabstop-snapshot-verify-r3` again
observed `W32WordBreakProc=0` before and `1` after the guest's first successful
EM_SETTABSTOPS, in the same owned worker PID and module base; the observer
reported normal exit. The source probe was unchanged (COM SHA-256
`B496F0B899CD5B84B53DB7E4F70B933D7C467FE633337760B42AB122A4D70DDF`).

The actual staged WOW32 DLL SHA-256 was
`F1345987539F53009B86ACF28A3BB25EE3DC085DE2091CE1E87E8CC2FF4D6605`.
Its `.text` and `.data` RVA, virtual/raw sizes and raw content hashes match
both the base package DLL and the Ninja DLL used for the map: `.text`
`E2917CC9D47853D3157D6FBCF911F5C9E75E2073FE35CA505207F6F9FD30DF51`;
`.data` `5255284F964B1914BC8BF7DD4ABC82A6A586788DA48E17C9E39A0EE7CEAF2EFB`.
The real snapshots are still non-atomic and not a breakpoint directly on the
unthunk entry; the two gated observations plus matching section identities,
source call chain and the object-level injection demonstrate observed state
change rather than a post-change crash. Original guest and product images
were not modified.
