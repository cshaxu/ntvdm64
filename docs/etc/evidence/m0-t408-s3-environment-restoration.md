# T408 S3 original RTL environment restoration

## Decision

S3 replaces the 137-line mutable MULTI_SZ implementation in
`adapter-mvdm-host-out/win32/source/opennt_support_rtl.c` with the selected
original OpenNT body at `opennt-host/base/ntos/rtl/environ.c`.  The imported
body owns create, clone, query, set/replace/delete, current-environment and
destroy behavior.  It is compiled with its historical x86 stdcall ABI into
`original-opennt-rtl-x86.lib`.

The remaining adapter is deliberately finite: it supplies a private PEB
environment ingress backed by a page allocation.  Immediately before the
selected Base VDM projection requests `RtlCreateEnvironment(TRUE)`, it refreshes
that ingress from the current Win32 environment.  This preserves the original
clone's input without substituting its algorithm and prevents a stale
startup-time snapshot from changing the Base VDM projection.

The selected external source is
`O:\repos.external\OpenNT\base\ntos\rtl\environ.c`, SHA-256
`6c67b0c09c40e9186ce28a735221634bade1d9402be085d965223092aa7d4f00`.
The imported file SHA-256 is
`4c0e0e870d97f351cd8b046c2901ba841b439c83b75fdb8a6e936b0c89cf2b4b`:
only its three include/declaration seams differ, while the function bodies
are retained from the selected source.

## Accounting

| Material | Result |
| --- | --- |
| D11 mutable environment algorithm | Removed from the adapter: allocation, ordered insert/replace/delete, name comparison and heap release. |
| Original owner | Imported as `base/ntos/rtl/environ.c`, with only source-private `ntrtlp.h`, `zwapi.h` and `environapi.h` declaration/VM seams. |
| Irreducible binding | Private PEB storage plus a current-Win32-environment snapshot before the original clone path. It is host-local, never guest-visible, and does not edit an environment block. |
| Symbol collision | `environapi.h` routes only selected consumers to `OpenNtRtl*`; this prevents modern `ntdll.lib` exports with the historical names from colliding at link time. |
| COMMAND owner sweep | `mvdm-host/dos/command/cmdenv.c` is also a direct original D11 consumer.  It now enters the same private symbol domain, so its create/set/destroy sequence cannot silently mix modern NTDLL allocation ownership with the imported original body. |

## Verification

| Check | Result |
| --- | --- |
| Full x86 worker link | `build/M0-T408/S3/r002-environ-incremental/original-softpc-process.exe`, 3,238,400 bytes, linked at 2026-09-13 11:52. |
| Focused RTL/environment fixture | `rtl-x86-fixture.exe` exited 0 after the final owner sweep. It covers existing x86 arithmetic/fill contracts plus D11 empty create, set, replace, query, delete and destroy. Empty and embedded-`=` names return original `STATUS_INVALID_PARAMETER` without mutating the block. It also installs a page-backed private PEB environment and proves the original `RtlCreateEnvironment(TRUE)` clone is independent, queryable and releasable. |
| Projection and binding fixture | `environment-projection-fixture.exe` exited 0. It first proves `mvdm_base_vdm_environment_prepare(NULL)` returns `ERROR_INVALID_PARAMETER` without an opt-in trace changing `LastError`; it then compares Base's explicit current-Win32-environment projection byte-for-byte with refresh-private-PEB → original TRUE clone → Base projection. |
| Short-root MEM regression | S3 candidate temporarily deployed to `O:\ntvdm64\ntvdm32.exe` ran `MEM.EXE` with exit 0. `MVDM_RTL_ENVIRONMENT_REPORT_PATH` recorded `prepared`, `restore-begin`, `restored`; CPU return recorded the expected three unsimulate/simulate pairs. The final r014 repeat also returned 0 and recorded `restored` with status `00000000`, proving the trace reports a successful actual `SetEnvironmentStringsW` restoration. The prior deployed executable (SHA-256 `7cf5b4364d06b86675805c7c74e267d8c3c28e21f7574e45de855c5c772049e4`) was restored in each operation. |
| Short-root COMMAND regression | The same temporary candidate ran `COMMAND.COM /c exit` with exit 0 and the same complete environment and CPU-return traces. The prior deployed executable was restored in the same operation. |
| Build-local runtime limitation | `r004` through `r011` timed out before CPU/BOP/environment traces. They were all launched below `build/M0-T408/S3/r002-environ-incremental`, whose 64-character executable root cannot fit the product's original COMMAND 64-byte configuration contract. `entry.c` performs that gate before environment preparation and displays a blocking rejection dialog. These observations are therefore invalid as S2/S3 worker comparisons, not evidence of a D11 regression. |
| COMMAND-owner follow-up | The relinked map contains only the selected `OpenNtRtlCreateEnvironment`, `OpenNtRtlDestroyEnvironment` and `OpenNtRtlSetEnvironmentVariable` for D11; its former modern NTDLL create/destroy imports are absent. This proves the owner sweep is required. Its worker behavior needs a short-root package run; the build-local `r010` timeout has no attribution value. |

## Boundary after verification

The original D11 algorithm, its private-PEB clone input and the selected Base
projection are directly proved by focused fixtures. No build-local MEM
observation can test the selected worker because its executable-relative
package root is rejected before D11 preparation. A short-root package run is
therefore still required for the COMMAND regression; do not characterize the
prior timeout as an environment regression or compensate inside `environ.c`.

The uninstrumented public VM boundary has no deterministic allocation-exhaustion
switch: attempting to manufacture one from a released address is invalid,
because modern `ZwQueryVirtualMemory` can describe a free region rather than
returning allocation failure. The fixture instead proves source-owned malformed
input failures and normal allocation/release; the imported original body
preserves its `ZwAllocateVirtualMemory` failure returns without an added test
hook. This is an explicit coverage limitation, not an accepted success claim
for an artificial OOM case.
