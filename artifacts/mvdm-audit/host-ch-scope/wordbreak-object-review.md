# Original EDIT word-break return conversion: controlled object controls

This isolates `UnThunkEMMsg16` in the existing cached selected x86 `wmsgem.obj` with `EM_GETWORDBREAKPROC`, fresh process per scenario, normal link without `/FORCE`, and the same unrelated fail-closed seams as [the tab-stop baseline](tabstop-link-route-review.md). Reproduce:

```cmd
artifacts\mvdm-audit\host-ch-scope\wordbreak-object-build.cmd build\t473-s10\wordbreak-object
python artifacts\mvdm-audit\host-ch-scope\wordbreak-object-observe.py build\t473-s10\wordbreak-object
```

All six cases returned zero exit and their captured values and hashes are in [receipt](wordbreak-object-evidence.json): zero leaves both preexisting globals unchanged; a WOW-marked value with the LDT bit set strips the WOW bit, while a WOW-marked value without that bit restores it; an untagged native value with empty cache assigns `W32WordBreakProc` and invokes `CallBack16` with reason 27/ordinal 535; a failed callback also caches the input as `WordBreakProc16`; a preexisting guest cache returns that cached value without a callback and leaves the existing native global unchanged. In the callback-success test the test seam deliberately assigns `WordBreakProc16=0x12345678` as a simulated guest side effect; the unthunk itself leaves its return as the original native value. That is an observed controlled contract, **not** proof how the real callback registers state.

All pointer values are synthetic bit patterns, never dereferenced, not verified valid native functions or guest selectors. The copied test structure and callback seam are bounded assumptions; a real guest test, actual callback success/failure, selector validity, reentry, retirement, post-fix behavior and fresh source-bound object remain separate. Existing real guest word-break/SendMessage witness is described in [tab-stop review](../shard-2/wow-edit-tabstop-unthunk-review.md), and must not be generalized to these synthetic inputs. No production or guest files were changed.
