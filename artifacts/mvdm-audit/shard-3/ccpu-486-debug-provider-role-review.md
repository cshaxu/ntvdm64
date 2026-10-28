# S10 shard 3: CCPU 486 leaves and production debug stubs

Read-only follow-up on pending graph-selected `ccpu386/bswap.c`, `wbinvd.c`, `stubs.c`. No build or guest execution. The selected worker command snapshot in `artifacts/mvdm-audit/ccpu-remaining-header-source/commands.stdout.txt` defines `SPC386` and `PROD`, not `SPC486`; `ddeml-inputs-worker.txt` records the three as archive members. Build graph membership alone is not extraction or runtime execution.

* `bswap.c`: byte-identical original; function is compiled only under `SPC486`. `c_main.c:1832-1860` dispatches BSWAP in that profile and calls `Int6` otherwise. Earlier `ccpu-flags-audit.md` independently records the selected object lacks external BSWAP and the old map has no BSWAP. Thus **not selected by the observed 386 decoder**, and 486 instruction support is a separate product capability decision rather than silently counted restored.
* `wbinvd.c`: byte-identical original; guarded by `SPC486`, its original body is empty with comment that a cache flush would be needed if a cache were implemented. `c_main.c:1312-1323` uses `Int6` under non-SPC486; in SPC486 it checks CPL and calls WBINVD. This is **not selected by observed 386 decoder**; do not conflate original no-op cache model with full hardware writeback semantics, or assume 486 is unwanted merely because it is gated.
* `stubs.c`: byte-identical original production debugger shim defines `check_I`, `check_D`, `force_yoda` as empty and `do_condition_checks=FALSE`. Call sites exist in `c_main.c`, `c_page.c`, and exception paths, often under debugger/conditional logic. This is **a deliberately reduced production-debug provider**, not proof of guest instruction semantics. Determine actual extracted provider/map and debugging requirements before closing its behavior row; do not interpret stubs as an ordinary runtime success path.

Image boundary: on this read the cached worker `ntvdm.exe` SHA-256 was `84F72C0B33D0D0BA8E6837A25B296C17D115157FC75A4E9DC394330CC6FAC5E6` and installed `O:/winnt/system32/ntvdm.exe` was `EF546743393DAD7E45BB7BDA605775524ADB26B94DAE4BDEFE32951EB0510294`; they are different. Existing older map/image evidence cannot establish either installed image's currently selected byte-level dispatch. No installed binary behavior or version is inferred from that mismatch. Freeze/identify the intended publication and matching map before claiming released-image opcode behavior.

Replay:

```powershell
Get-FileHash build/.ninja/msvc-x86-release/worker/ntvdm.exe,O:/winnt/system32/ntvdm.exe -Algorithm SHA256
rg -n 'bswap\.c|wbinvd\.c|stubs\.c' artifacts/mvdm-audit/ccpu-remaining-header-source/commands.stdout.txt
rg -n 'BSWAP|WBINVD|SPC486|Int6\(' src/mvdm/softpc.new/base/ccpu386/c_main.c
rg -n 'check_I\(|check_D\(|force_yoda\(|do_condition_checks' src/mvdm/softpc.new/base/ccpu386
```
