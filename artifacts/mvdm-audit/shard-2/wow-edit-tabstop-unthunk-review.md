# WOW EDIT tab-stop unthunk and callback boundary

Shard-2 `wow32/wmsgem.c`; frozen reconciliation SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. Current hash 251325307C9E698EE579E3493F15F93ABB6DAFE7D289F334AC3714C513566BE9; original hash 88D05E4770453621472704E90EDCEFD6AD18B618528B4166DA3A9BA1EA1A3619. Selected product source remains partial. No guest/product changes or new runtime test.

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
