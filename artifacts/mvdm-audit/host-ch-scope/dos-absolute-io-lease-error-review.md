# DOS absolute disk I/O: guest PDB lease failure and last-error boundary

This source-level audit identifies a host adaptation failure boundary; it does not establish that a real guest encountered the condition and uses an isolated controlled object fixture; it performs no live guest, disk or product operation.

## Actual chain and ownership

`dos/dem/demdasd.c::demAbsReadWrite` calls original `demDasdRead` or `demDasdWrite`, compares returned sector count with requested count, and on a short return reads Win32 `GetLastError()`, translates it with `demWinErrorToDosError`, and returns carry plus DOS error. For an unchanged INT13 vector the removable branch calls `nt_floppy_read/write`; the non-removable branch calls `nt_fdisk_read/write`. `demGetBPB` can call the floppy reader first when media verification indicates a change, so a test must control this separate path and not assume it preserves a later status.

The current project adaptation reads `current_pdb_location` through `nt_rflop_current_pdb` / `nt_fdisk_current_pdb` → `mvdm_guest_location_read_u16`. All four read/write entrypoints return zero on a failed current-PDB read. In `mvdm_guest_location_acquire`, when `session_thread_current()` returns NULL, the early return does not call `SetLastError`; the current `session_thread_current()` simply returns the thread-local instance. Therefore that *specific no-bound-session failure branch* does not publish an error of its own before returning zero. If the caller reaches DEM's short-count error branch without another operation setting last error, it can translate a prior unrelated thread error. This is a conditional source-level failure propagation gap, not a demonstrated guest failure and not proof what numeric value Windows will return on other lease failures.

This differs from the separate original `nt_fdisk_read` non-void fallthrough after `get_fdisk_handle` failure, described in `../nine-way/part-8/fdisk-open-close-failure-review.md`. It also connects to the existing FDC `CreateThread`/session binding and close-counter finding in `../shard-3/floppy-lease-failure-role-review.md`, but a DEM call is not automatically on that FDC thread. Do not treat the two paths as equivalent or infer that every unbound thread reaches an INT25/26 caller.

## S12/S13 executable boundary

- First establish the exact guest/session/worker ownership expected by each actual route; do not invent an unconditional session or raw pointer bypass. Use a selected-object fixture and controlled lease/session seams, with no real disk.
- On the same calling thread set two distinct known LastError sentinels, force the exact null-session/current-PDB path for each of floppy/fixed read/write, and assert returned count plus the resulting LastError. Include zero/nonzero request length, media/BPB branch controls, PDB success, underlying host I/O success/failure and cleanup. Verify exact current binary/object identity and whether any intervening operation changes the thread's error.
- Separately call the actual DEM original path with a controlled drive/BDS, selected backend and bounded guest buffer; verify AX/CF and error mapping against an identified original/approved failure contract. This is a different check from an isolated native return and must not write user media.
- If an error must be synthesized at the adapter boundary, document the chosen error's original/observable basis, preserve real host errors and register any mirror divergence. Synchronize with the existing FDC lifetime and once-only cleanup repair; do not fabricate success or blindly override host-origin errors.

The required error mapping and real-guest incidence are not yet proven. Record them as targeted verification and repair work, not as passing integration or an observed crash.

## Controlled actual-object sentinel witness

A noninteractive x86 fixture `dos-absolute-io-lease-error-probe.c` (SHA-256 `40b1bef0528c6b2d03e563fe907478dcb69d3a675b9227766dfbb4d5b71174c9`) was linked against the **actual cached** `nt_rflop.obj` (`737c7d9fe945fc090afcb64de55b9729bfe481f5e619b89903b65bf61bba2a30`), `nt_fdisk.obj` (`53b95ec607c817b32c4bde5c78bb7aabdbb50a9dd52bc63d16caf939e6360d9`) and project `mvdm_guest_location.obj` (`44b012725b5ba009dfc779c8569c1300adbb652aebdb8f04a88554b158719c8f`). The test seam supplies a null `session_thread_current`, a valid numeric location, and a valid one-entry fixed-disk table; it does not create an actual session or use media. Guest lease acquire/release and unrelated disk/UI/counter seams terminate the fixture if unexpectedly reached. The test replaces the MSVC security-check cookie hook for isolated MinGW linking; it tests this null-session return path only, not security-cookie behavior.

On the same thread, the actual functions preserved four distinct preloaded LastError sentinels while returning zero: floppy read `0x4a13`, floppy write `0x4a27`, fixed-disk read `0x4a31`, fixed-disk write `0x4a41`. The fixture exited 0. These four controlled failures demonstrate stale error preservation for the exact null-session seam; they do not prove a live session becomes unbound, a real guest sees an error, any AX/CF value, or a valid alternate error mapping. They also do not exercise open failure, the fdisk non-void fallthrough, actual disk/media I/O, races or cleanup.

Replay from the repo root with i686 MinGW GCC and the same object hashes:

```powershell
New-Item -ItemType Directory -Force build/t473-s10/pdb-error-probe | Out-Null
i686-w64-mingw32-gcc -ffunction-sections -fdata-sections artifacts/mvdm-audit/host-ch-scope/dos-absolute-io-lease-error-probe.c build/.ninja/msvc-x86-release/worker/obj/host/nt_rflop.obj build/.ninja/msvc-x86-release/worker/obj/host/nt_fdisk.obj build/.ninja/msvc-x86-release/worker/obj/adapter-softpc/mvdm_guest_location.obj '-Wl,--gc-sections,-Map,build/t473-s10/pdb-error-probe/probe.map' -o build/t473-s10/pdb-error-probe/probe.exe
& build/t473-s10/pdb-error-probe/probe.exe
$LASTEXITCODE
```

The GNU linker printed three `corrupt .drectve at end of def file` warnings for MSVC object directives; the x86 probe linked and executed, but this is not MSVC product-link or release qualification. Its generated map attributes all four native disk entrypoints and `mvdm_guest_location_read_u16` to the retained object inputs and the intentionally fake `session_thread_current` to the fixture.
