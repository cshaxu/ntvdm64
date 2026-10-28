# Selected DEM FCB metadata object: controlled API-failure observations

This isolated non-guest fixture invokes the actual current selected x86 `demfcb.obj::demGetMiscInfo` with deterministic test-owned Win32 boundary functions. The original selected object is not modified: the runner uses `objcopy` on a temporary copy to redirect five import symbols (`GetFileTime`, `FileTimeToLocalFileTime`, `FileTimeToDosDateTime`, `GetFileSize`, `CloseHandle`) to explicit test callbacks. `demClientError` is a counting seam that records the current last-error value; it is **not** the original guest-error conversion. Other object functions are discarded by section GC. The run returns 0 to indicate observation completion, not behavioral acceptance. The linker warns `corrupt .drectve at end of def file` but succeeded; do not infer deployed image identity from this fixture.

[Source](dem-fcb-metadata-failure-fixture.c), [runner](dem-fcb-metadata-failure-run.py), and [machine-readable output/hashes](dem-fcb-metadata-failure-observation.json) are retained. Original selected object SHA-256 `40550EA098C987667A0114709625B57D814AD25EB14E4286B5D644188ADC872D`. Each scenario starts with fake handle 0x1234, zero LastError, metadata buffers initialized by the fake boundary; no real guest or file is accessed.

| Scenario | Observed result | Scope of finding |
| --- | --- | --- |
| Success control | TRUE, all four metadata calls, no error or close | Confirms normal fixture chain only. |
| `GetFileTime` returns FALSE | TRUE, subsequent conversion/size calls continue, no error | Reproduces the BOOL compared to -1 bug. The fake intentionally still fills its output, making downstream behavior deterministic; real failure output is not guaranteed. |
| `FileTimeToLocalFileTime` returns FALSE | TRUE, subsequent conversion/size calls continue, no error | Reproduces ignored return; fake provides output only to isolate the branch. |
| DOS date/time conversion returns FALSE | FALSE, one error callback, no helper close | Tests helper ownership and branch; the fake does not assert the real API's LastError semantics. |
| `GetFileSize` returns FFFFFFFF, error 5 | FALSE, one error callback sees 5, no helper close | Actual error branch under controlled boundary. |
| `GetFileSize` low part 123, high part 1 | FALSE, one error callback sees 0, no helper close | Real sparse-file helper witness is independently documented in [selected-object review](../nine-way/part-8/dem-fcb-metadata-selected-object-review.md); deterministic zero shows original error conversion would need contract review. |
| `GetFileSize` low part FFFFFFFF, high part 0, LastError 0 | FALSE, one error callback sees 0, no helper close | Reproduces indiscriminate sentinel rejection at helper level, not a guest-reachable 4 GiB-1 file outcome. |

The fixture proves branch behavior of the selected object under supplied API returns, not that those errors are common, reachable from a real guest, or mapped correctly by `demClientError`. It does not call `demFCBCommon` or `demGetFileInfo`, so no newly claimed end-to-end handle leak or guest register result follows. The original two-caller ownership and proposed S12 acceptance boundaries remain in [FCB ownership review](dem-fcb-open-ownership-review.md). Guest integration, native error generation, read/write/close, hard-error callbacks and cleanup still require separately bound evidence.

Replay from the repository root with the selected x86 object and bundled i686 MinGW tools installed:

```powershell
python artifacts/mvdm-audit/host-ch-scope/dem-fcb-metadata-failure-run.py
```

The runner creates only temporary test files under `build/t473-s10/fcb-failure-probe` and rewrites the small observation JSON; it does not rebuild a guest or product. Cases record their actual results rather than asserting that known failures are correct behavior; a repaired object requires updated positive expectations.
