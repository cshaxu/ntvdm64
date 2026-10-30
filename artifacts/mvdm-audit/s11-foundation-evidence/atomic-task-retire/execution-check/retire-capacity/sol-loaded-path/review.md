# Actual WOW32 module path during SOL

The existing PID-bound SOL Options probe was extended only to print module
paths returned by `GetModuleFileNameExW` when its diagnostic module census
is enabled. In a fresh isolated physical stage it observed the actual worker
at `O:\tmp\nc17\system32\ntvdm.exe` and a loaded `WOW32.DLL` at
`O:\tmp\nc17\system32\WOW32.DLL`. The staged manifest records that DLL's
SHA-256 as `26E077D75BDA5566B6F02E164C2E25D124FD9A08EFD3ADB5AA168EED45EF1D45`.
The probe completed one Options open, close, parent recovery and launcher
completion; output, observer receipt, manifest and test hashes are retained.

This establishes an actual WOW32 module loaded from the same staged product
root during a real unmodified Win16 application interaction. Together with
the source's `LoadLibraryW` path and the separate selected bridge/loader
fixture, it narrows the real-provider identity question. It does not capture
the mapped module bytes or directly log W32DllInitialize attach/detach calls,
prove guest DLL thread/unload or failed-load lifecycle, identify the guest's
WIN87EM module, or constitute Full product acceptance. The runtime stage was
test-owned and removed by the existing cleanup.

The visible-desktop runner now checks the printed WOW32 path against its own
physical stage when PC sampling is explicitly enabled; a missing or foreign
path is a non-pass. A fresh one-round SOL execution passed this assertion and
completed its launcher (`assert-probe.txt`, `assert-manifest.json`, and
`assert-result.txt`). A local negative control confirmed the exact matching
pattern accepts the expected stage path and rejects an `O:\winnt` path. This
is a test-only check; it does not hash the mapped module in memory or add
attach/detach tracing.
