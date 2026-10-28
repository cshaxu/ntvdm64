# WOW callback memory helpers: published image sample

This complements `wow-callback-memory-helpers-review.md`. `wow32/wcall32.c` matches both original trees by SHA-256; current x86 WOW32 Ninja compiles `wcall32.obj` and explicitly links it into wow32.dll. Captured source registration in `wow32.c` exposes the original local allocation, dialog and edit-wordbreak callbacks. The published `assets/release/WOW32.DLL` is byte-compared to the cached DLL for all executable sections; full-file SHA-256 differs, so full image identity is not asserted.

Using current map and object symbol boundaries, the relocation-masked function body bytes bind to published image for W32LocalAlloc (52 bytes, 2 relocation words), W32LocalReAlloc (92, 3), W32EditNextWord (169, 7), and W32InitDlg (64, 3). This is four actual function bodies, not a conclusion about all helpers, relocation targets, or registration table bytes. Source semantics remain as recorded: local allocator selects host or guest according to instance; guest realloc can update a parameter mapping; wordbreak allocates and releases temporary guest memory around CallBack16; dialog init writes guest-projected state.

Suggested disposition: partial, not complete. Runtime callback registration, direct/guest execution, nested callback, failure rollback, mapping retirement and task/window destruction remain unverified in this capture. Earlier isolated W32InitDlg fixture had controlled seams and cannot prove these other paths. No guest or product execution or modification occurred.

Replay with `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/wcall32-release-observer.py` and compare `wcall32-release-observations.json`. Map and hashes are snapshots; recheck if the build or release changes.
