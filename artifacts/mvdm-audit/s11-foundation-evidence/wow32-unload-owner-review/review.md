# WOW32 provider unload and initialized resources

## Selected loader owner

The original `nt_bop.c` acquires the provider using `SafeLoadLibrary("WOW32")`.
The project firmware resolver maps that internal name to the package's actual
`system32/WOW32.DLL`; FPU save/restore stays in the original owner. The five
explicit `FreeLibrary(hWOWDll)` branches in this path occur when one of the
required exports is missing, before `W32Init` is called. A failure of W32Init
or activation requests VDM termination instead; after success the code marks
`WowModeInitialized` and dispatches without a corresponding free in that
loader path. This is a statement about those source paths, not a proof that no
other code or external provider can change the native module reference count.

The original `W32DllInitialize` runs process attach and, on detach, unregisters
base callbacks and destroys its WOW heap. The project-generated CRT bridge
then invokes registry-token and bitmap-lock shutdown only for an explicit
process detach (`reason == 0 && reserved == 0`). The real-worker
[missing-export test](../wow32-empty-cleanup/review.md) observed explicit
unload with empty binding state. The selected-object [registry](../current-registry-shutdown/review.md)
and [bitmap](../current-bitmap-cleanup/review.md) tests separately exercise
populated cleanup, but do not make it a live-worker explicit-unload result.

## Independent references and remaining boundary

The original generic thunk `WK32LoadLibraryEx32` uses native LoadLibraryEx,
and `WK32FreeLibrary32` forwards its 32-bit module argument to FreeLibrary.
The separate ISV module registration/deregistration path likewise owns native
library references. A balanced extra load/free would not by itself consume the
loader's original reference, but source inspection does not prove all guest
inputs are balanced, identify every dynamically loaded provider, or rule out
an externally initiated unload. Do not equate guest Win16 DLL unloading with
unloading the host WOW32 provider. Do not fabricate a supported post-start
unload by forcing a guest or product change.

The real supported process-termination path, resource ownership under exit,
and any supported independent host-provider unload remain distinct lifecycle
questions. Determine actual callers and failure semantics before requiring a
populated explicit-unload witness or changing cleanup. The above is a bounded
source trace, not a claim that all processes clean up correctly or that an
adversarial or externally induced unload is impossible. No runtime, guest or
product file was modified. Source hashes are in `inputs.csv`.
