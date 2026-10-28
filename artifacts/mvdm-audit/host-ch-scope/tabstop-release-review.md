# EDIT tab-stop original object: release branch matrix

The [fixture](tabstop-release-fixture.c) links the cached selected x86 `wmsgem.obj` without `/FORCE` using [explicit fail-closed seams](tabstop-release-seams.c) and normal system libraries. It calls `UnThunkEMMsg16` directly in a fresh process per case; the `free_w` seam checks pointer identity and actually frees its test-owned heap allocation. `CallBack16` is a recording stub returning FALSE. The copied structure declaration is from the existing [fixture types](../shard-2/tabstop-object-types.h); its layout is a test assumption bound to this object/configuration, not a general ABI proof.

```cmd
artifacts\mvdm-audit\host-ch-scope\tabstop-release-build.cmd build\t473-s10\tabstop-release-seams
python artifacts\mvdm-audit\host-ch-scope\tabstop-release-observe.py build\t473-s10\tabstop-release-seams
```

Twelve process cases covered signed counts 1, 0 and -1; a pointer to `MsgBuffer` versus a test-owned heap pointer; and injected native result 0 versus 1. For count 1, the heap pointer is freed exactly once and the stack buffer is not freed. For counts 0/-1 neither pointer is freed by the unthunker; the fixture manually frees its test heap after observation. Result 0 causes no callback; result 1 still causes one callback and sets both word-break globals to 1. All cases exit 0 because the fixture asserts the **observed unfixed baseline**, not correct post-fix behavior. Case outputs and file/object/executable/map SHA-256 identities are in [receipt](tabstop-release-evidence.json).

The zero/negative plus non-null heap inputs intentionally exercise the predicate and are not claimed reachable from real `ThunkEMMsg16`; this fixture does not run construction, test allocation failure, prove a double-free cannot happen through unwind, check cached word-break state, or validate the real guest. A post-fix test must reverse the result-1 callback/global expectation while keeping the release expectations, then add actual constructor/allocator failure, legitimate word-break retrieval and source-bound real guest tests. Object and source hashes are recorded separately; the cached object is not proven freshly built from the current source. No product or guest was modified.
