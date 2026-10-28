# S10 shard 3: selected WOW DIB provider and failure ownership

Scope: `wow32/wdib.c`, original hash `D0BD4F9CB7BBA0DC5E977A8C096337DF0830E3804EF6AEB8759CF979176DD910`; both original editions match current. No guest run, build or fault injection. The cached WOW32 link map attributes the original DIB entry points, list heads and cleanup routines to `wdib.obj`, establishing selection in that map, not actual execution. Cached DLL SHA-256 `C97C512C6F6BD6F16A6FB68F340760F551CD2186B5262B1A6AE892FC8A06BC58`; installed O:/winnt/system32/wow32.dll SHA-256 `FF5AAD62B628F0AF28DF23255D97059ABF089CBBA9226CE46091412F4093E431`. They differ; no installed-image map association is established here.

## Original call and ownership chain

`wgdi.c:420` routes CreateDC for DIB.DRV to `W32HandleDibDrv`; it uses native CreateCompatibleDC, temporary file/section, CreateDIBSection, mapped view and original `RET_SETDIBSEL`, then records them in a `DIBINFO` node. `WG32DeleteDC` (`wgdi.c:967`) and `WK32FindAndReleaseDib` (`wdib.c:515` onward) consult that list. `W32FreeDibInfoHandle` restores old selector via `RET_FREEDIBSEL`, then unmaps, deletes/closes and unlinks. Separately `WG32CreateDIBSection` uses `DIBSECTIONINFO`; `wgdi.c:989` routes DeleteObject through its cleanup. Native GDI owns actual bitmap/DC and Windows owns section/file; original WOW owns guest selector mapping and bookkeeping.

Existing `docs/etc/evidence/m0-t455-s3-dib-palette-boundary.md` verifies a native fixture's bidirectional sharing and cleanup, but explicitly does not verify actual guest DIB thunk traffic or all these failure paths. It preserves original failure logic. Current evidence does not justify full file recovery.

## Specific failure paths requiring investigation, not reproduced defects

1. In `W32HandleDibDrv`, `RET_SETDIBSEL` can succeed before `W32AddDibInfo` allocates the ownership node (329-351). If allocation then fails, finally block at 363-376 does not visibly restore the changed selector or unmap the mapped view, and uses `CloseHandle(hbm)` rather than the success path's `DeleteObject`. Determine callback semantics and actual failure reachability with controlled fixture and assert selector, view and GDI lifetimes before proposing a repair; original-source provenance does not itself prove safety.
2. `W32CheckAndFreeDibInfo` decrements `di_lockcount` before `W32RestoreOldDib`; if the restore callback returns zero the node remains while count is already decremented. `WK32FindAndReleaseDib` has a comparable transition. Validate failure and subsequent retry/reentry contract before concluding whether a stale/underflowed count is reachable.
3. `WG32CreateDIBSection` does explicitly free its newly allocated selector if ownership-node allocation fails, but other callback failures and output-pointer semantics require independent checks; don't transfer conclusions between the two DIB paths.

Keep file **pending**. Validate actual 16-bit CreateDC/section routing, nested lock/release, restore failure, allocation-failure rollback, task loss and current released module identity; no aggregate restoration percentage or confirmed product bug follows from static inspection.

Replay: `rg -n 'W32HandleDibDrv|W32CheckAndFreeDibInfo|W32CheckAndFreeDibSectionInfo' src/mvdm/wow32`; `rg -n 'MapViewOfFile|RET_SETDIBSEL|W32AddDibInfo|CloseHandle \(hbm\)|W32RestoreOldDib|di_lockcount' src/mvdm/wow32/wdib.c`; `rg -n 'wdib.obj' build/.ninja/msvc-x86-release/wow32/wow32.dll.map`; hash the two DLLs and source using `Get-FileHash -Algorithm SHA256`.
