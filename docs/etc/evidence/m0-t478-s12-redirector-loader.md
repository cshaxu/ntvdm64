# T478 S12 — redirector failed-load module ownership

Baseline: 3de5b9a21. Finding 24 corrects the original loader's existing failure
contract and removes an unselected local loader; it adds no network capability,
retry, substitute DLL, guest change or malformed-module recovery mechanism.

## Original contract and selected owner

Original nt_bop.c::MS_bop_7 explicitly allows VDMREDIR loading or export
resolution to fail: return ERROR_INVALID_FUNCTION, keep other DOS operation,
and do not retry through that BOP state machine. Both pinned OpenNT families'
LoadVdmRedir use CloseHandle on the LoadLibrary module in that failure path.
This is the wrong resource destructor, including an unnecessary CloseHandle
when loading returned NULL. It can leave a loaded DLL and partially assigned
callbacks despite returning FALSE. Correcting the already specified cleanup
is distinct from accepting a new invalid input or adding retries.

Actual current nt_bop.obj selects host_def.h's !i386 SafeLoadLibrary macro,
which calls native LoadLibraryA. It does not call mvdm_softpc_load_library.
The latter survived only as an adapter and an adapter-only test; it is absent
from the current product map. This corrects the older shard review's claim
that its rooted helper was the active loader. The original selected CCPU
loading route already finds the adjacent product DLL; it is preserved.

## Minimal repair and cleanup

DIV-872 changes only the failed-load tail: reset all nine callback slots to
their original unloaded values (VrInitialized uses DefaultVrInitialized),
FreeLibrary a non-NULL module, clear the module slot, return FALSE. Normal
loading order, export resolution, success caching, and BOP failure state/IP
advance remain unchanged. No new last-error translation or retry is added.

The callback consumer sweep finds demfile named-pipe calls behind successful
LoadVdmRedir, demhndl calls behind IsVdmRedirLoaded, and VrDispatch behind the
successful BOP state. The existing VdmRedirLoaded flag remains false until all
exports resolve. Clearing partial callbacks avoids publishing stale unloaded
addresses and restores the stated DefaultVrInitialized contract.

Delete the unselected mvdm_softpc_load_library body/declaration, its
softpc_product_loader_fixture and the corresponding standalone Ninja target.
The remaining firmware/media/boot-environment targets are retained with their
object indices corrected. No active product call is replaced or removed.
Original vrdll process detach still owns VrUninitialize and DLC cleanup.

## Verification

[Loader test](../../../test/integration/redirector_loader_test.c),
[native test DLL](../../../test/integration/redirector_loader_dll.c), and
[runner](../../../test/integration/verify-redirector-loader.ps1).
The runner extracts the exact DefaultVrInitialized-through-LoadVdmRedir owner
(including MS_bop_7) from baseline/current source; selected release conditionals
are retained. Test seams supply register storage and host API failure points.
The DLL is a test provider with the nine real exports and a native detach
counter; it is not the product VDMREDIR DLL or a replacement implementation.

77 checks pass with the repair; the same test reports 45 failed checks against
the baseline. Cases cover missing DLL, each of the nine missing exports,
native module absence/detach after failure, cleared callbacks, successful
load and repeated success without an extra reference, and the original BOP
failure result/IP increment/no-retry behavior. The test does not simulate
arbitrary concurrent loader callers or assert general network correctness.

Current product VDMREDIR exports all nine required entries. Actual rebuilt
nt_bop.obj selects native LoadLibraryA and the repaired FreeLibrary tail.
The obsolete adapter has no remaining source/test/build references or product
map entry. Both x86/x64 remaining resource graphs pass firmware, media and
WOW boot-environment checks after deleting only their obsolete loader target.
Coherent x86 worker/redirector/hook/WOW32 and x64 worker/frontend/monitor/
launcher/service/hook builds pass. All 16 selected Full gates pass in 429937 ms against the identical ten-image
package. DOS4GW/NESticle are owner-excluded and skipped, not counted as passes.
Application 0.0.478/protocol 67; package 0c8a03f9-60e6-46dc-9638-b25d41ae8e3d.
All ten published images and manifest match package-final; raw ntvdm.exe matches
the manifest input hash. [Compact results](../../../artifacts/mvdm-audit/shard-3/redirector-loader-s12-results.json)
retain source/object identities, exact test scope and gate timings. Worktree
publication, documentation governance and diff checks pass.

Local evidence is under build/t478-s12. Only worktree assets/release is an
authorized publication destination; no O:/winnt write and no remote push.
