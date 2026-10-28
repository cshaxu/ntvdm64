# S10 shard 3: shared spool loader callers and raw-job failure boundaries

Read-only source analysis extending `wow-print-setup-failure-review.md`. Original `wspool.c` SHA-256 `C76BEC31AE9020EA0173C20D42986F36E6888A315A5D27E91E5CBF2579CB2AD8`; `wgdi.c` `4C28E5166B294AC2A17982B3FF6E6559F369B8A8A2DD766AA18E16C8F74379D6`. Cached WOW32 map includes `WG32OpenJob`, `WG32CloseJob`, `WG32GetEnvironment`, shared loader; its PE executable sections match the current release/installed DLL as recorded in `wow-generic-thunk-published-binding.md`. No device, guest or failure-injection execution was performed.

## Original responsibilities

`WG32OpenJob` uses guest `[devices]` mapping, native `OpenPrinterA` and `StartDocPrinterA`, then `GetPrn16` places the native handle in a USER-instance-owned Win16 local allocation. Subsequent page/write/end/close thunks read it using `Prn32`, while native spooler owns the true printer/job. `WG32GetEnvironment` copies the guest port name before native printer calls. `wow32fax.c::InstallWowFaxPrinter` loads the same WINSPOOL table after creating registry entries; `wuclip.c::W32ConvertObjDescriptor` uses the *same loader function* with a different OLE DLL/table. A fix to the loader must preserve both provider families, not assume every caller uses WINSPOOL.

## Specific static failure conditions

* `wspool.c:87-101`: if `OpenPrinterA` succeeds and `StartDocPrinterA` fails, the path returns `GetLastError()` without a visible `ClosePrinter` for the newly opened handle. If document start succeeds but `GetPrn16` cannot allocate/lock its guest carrier, inspect how a zero/invalid carrier is reported and whether the already-created native job and handle can be retired. These are source-level risk paths; reachability and provider state on actual failure require targeted controlled verification.
* `WG32CloseJob` (`wspool.c:163-174`) stores `GetLastError()` when `ClosePrinter` returns FALSE and then calls `FreePrn` when `ul` is nonzero. Consequently **a failed close with a nonzero last error takes the carrier-free branch**. This contradicts `m0-t468-s3-printing-retirement.md`'s statement that the carrier is freed only on successful close. Its `EndDocPrinter` result is also overwritten by the subsequent close result. Whether and how an actual provider signals failure and retains the native handle is not established; do not assume the effect of a failure without a fixture.
* `Prn32` declares `h32` uninitialized and returns it if `LocalLock16` fails or the mapped pointer is NULL. Validate malformed/stale/reused carrier and failure handling before assuming a safe null result. `GetPrn16` does not visibly free a successfully allocated local carrier when its lock/mapping fails. Actual callback and local-memory failure behavior needs verification.
* `wgdi.c::WG32GetEnvironment` copies `psz1` before loader call and returns immediately on loader failure, bypassing its `free_w(psz1)` at line 2425. `wow32fax.c` writes registry entries before loader and subsequent install steps; a loader failure leaves the already-written state unless another owner rolls it back. Distinguish intended partial configuration from a reproduced correctness defect.
* `wuclip.c::W32ConvertObjDescriptor` checks module-load result but not a missing requested export after the shared helper has returned TRUE. Failure requires a controlled missing-export provider; no current OLE host failure has been proven here.

Existing T468 S4 raw-spool witness only proves the no-profile early return; it cannot certify the post-open paths. Keep selected source/function rows pending and trace exceptional ownership as one group before repair; no generalized new lifecycle manager or guest changes are implied.

Replay:

```powershell
rg -n 'WG32(OpenJob|CloseJob|GetEnvironment)|GetPrn16|Prn32|FreePrn|LoadLibraryAndGetProcAddresses|W32ConvertObjDescriptor' src/mvdm/wow32
Get-Content src/mvdm/wow32/wspool.c
Get-Content docs/etc/evidence/m0-t468-s3-printing-retirement.md
rg -n 'WG32OpenJob|WG32CloseJob|WG32GetEnvironment|LoadLibraryAndGetProcAddresses' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
```
