# T478 S31: host DLL entry lifecycle disposition

Parent: 0f68e577d. Finding 07's project BOOLEAN-return defect and launcher
failure-return defect are already repaired. This S adds host lifecycle
verification only. No production/mirror/guest source or release image changed.

## Source reconciliation and ownership

The selected generated CRT bridge declares W32DllInitialize as unsigned char,
matching original BOOLEAN, and widens that value to its Win32 BOOL result.
It invokes original entry first, then registry/bitmap shutdown only for
DLL_PROCESS_DETACH with reserved == NULL. The ordinary process-termination
notification does not request those explicit-unload provider cleanups.

W32DllInitialize was compared with both pinned OpenNT versions. After removing
comments, whitespace and #line annotations, the body is identical except for
the already-retired gpGdiHandleInfo = GdiQueryTable() acquisition. Modern GDI
binding remains its existing owner; no private GDI table dependency is restored.
Small-heap failure, general-heap failure, hook/thunk/resource setup, thread
notifications and detach callback/HeapDestroy order retain original code.

The selected host nt_bop loader calls FreeLibrary on each of five missing
required exports (W32Init, W32Dispatch, two COMM entries and the hung-app entry),
then terminates the VDM. It does not unload/reload WOW32 after successful active
initialization. W32Init failure and later activation failure terminate the
process; this S does not invent retry or live module replacement.

Original CreateSmallHeap creates a separate hWOWHeapSmall. Original detach
only destroys hWOWHeap and does not delete VdmLoadCritSec or comprehensively
roll back partial initialization. No hWOWHeapSmall destructor was found in the
selected WOW32 sources. These are original process-lifetime/rollback limits,
not a new product capability to repair in this S. The test below demonstrates
the reached early failure/unload boundary, not leak-free arbitrary reloads.

## Current tests

The existing selected-bridge witness passes false/true attach, false thread
notification and explicit detach with deliberately dirty high EAX bits. A
small added assertion also checks thread detach and non-NULL-reserved process
termination do not increase the two explicit cleanup calls. The real Windows
loader fixture performs eight failed-attach/successful-load/FreeLibrary rounds
with the selected bridge and controlled BOOLEAN/cleanup providers. It is not
substituted for an actual WOW32 provider test.

The existing actual-provider diagnostic builder now has two explicit modes:

- AttachFailure: the selected WOW32 source copy forces its existing
  CreateSmallHeap-failure edge and writes the existing marker. Both default
  and --wait real Run16 cases return ERROR_DLL_INIT_FAILED (1114), and the
  host trace reports load-failed.
- MissingInit: the diagnostic export definition renames W32Init, while the
  original provider body remains present. Test-only markers are written at
  completed attach and after the original detach HeapDestroy call. Both
  real modes report init-export-missing, the exact attach/detach marker order
  and exit 1114. This executes the actual native loader, selected original
  initializer and host FreeLibrary failure edge. The marker proves return
  from that cleanup call, not the HeapDestroy result or all resource retirement.

The diagnostic DLLs are built in fresh build/t478-s31 directories; source,
cache graphs, immutable guest and production packages are not overwritten.
The stage helper creates separately identified diagnostic packages using the
S29 package's other nine images and existing media. The existing private-
desktop runner makes physical isolated runtime clones and owns their cleanup.
Neither diagnostic DLL was published. A preliminary shell edit command failed
to parse before running and changed no files; implementation used structured
patches afterward. Owned background build PID 35620 and verification PID 22648
both completed successfully. Exact commands, per-mode traces/markers and
manifests remain under build/t478-s31.

The existing image observer uniquely matches the entire 51-byte selected
bridge in assets/release/WOW32.DLL with its three call relocations masked.
This is static selection evidence, not an independent execution claim.
The durable [receipt](../../../artifacts/mvdm-audit/shard-2/dll-entry-s31-results.json)
contains original source identities, direct and OS-loader results, actual
provider traces, diagnostic identities and exact unchanged release hashes.

## Closure

S31 resolves finding 07 as already fixed, with the formerly missing early
explicit-unload evidence now obtained. Normal actual WOW startup/task cases
and Full remain those of the exact unchanged S29 package
7434fc0b-10e5-4081-a37f-b33031436b9e. Test-only changes do not require another
product build/Full/republication; diagnostic builds are never release inputs.
No O:/winnt write or push occurred.

This is host DLL entry/lifetime verification. Arbitrary guest DLL initialization,
unload, active-worker WOW reload, all allocation failures and every shutdown
race are not claimed. They do not justify capabilities beyond original semantics.
Retain original limits; reopen only for a concrete applicable host regression.
Broader T478 reconciliation and subsequent findings remain open.
