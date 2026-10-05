# T430 S4 redirector copy/completion recovery

## Question and inputs

After S3 250ab563b, audit K02–K04 against original OpenNT and current
production VDMREDIR/worker adaptation. S2 is the unchanged published package.
Original CX0 underflow remains TODO without a verified matching SoftPC fix;
project Unicode/OEM and async staging behavior require separate verification.

## Initial source observations

Original async ReadFile writes into guest data before its later completion
routine publishes error/count words. Current worker staging copies error,
count, then read payload in mvdm_redirector_async_complete. This reverses
the original visible order and requires a project-boundary correction, not
an invented atomic completion contract. Exact destination failures and
callback suppression must be tested before implementation/delivery claims.

VrGetUserName now counts Unicode characters before an OEM guest copy. Review
the added conversion's byte capacity/terminator contract separately from the
unchanged original CX-1 expression. CDNames' original ABI assumes valid
writable buffers and has no error return; preserve that ABI while checking
the project lease/copy and NetAPI cleanup. Current tests are minimal successes
and dead-memory negatives, not complete evidence for these obligations.

## Project corrections and controlled proof

The staging adapter now commits a nonempty read payload before the original
error-then-count words, and refuses an oversized count before publication.
This is not an atomic multi-destination transaction: an error/count destination
can fail after the payload commit. Original VrpCompleteAsyncRequest still
suppresses callback/interrupt queuing when this adapter returns failure.

verify-redirector-async-publication.ps1 compiles actual production async,
location, session and lease sources with x86 /MT/CCPU40 definitions; only the
memory provider is controlled. r001 fails compilation for a missing include
root; r002 reproduces five failures among23 assertions before repair; r003
passes the same23 after it. The final24-case fixture adds cancelled-owner
entry rejection. Cases retain separate destination failures, partial/error
read, oversized/zero read, write, binding/return, stale epoch, dead memory and
idempotent release. Real pipe/guest callback proof remains separate.

The OEM adapter now receives capacity in encoded bytes including NUL and
returns existing NERR_BufTooSmall before any oversized write. Its one
production caller retains the original character gate and BX=0 unchecked /
CX=0 underflow behavior. The updated minimal mirror hook is DIV171, not an
invented correction of that original limitation.

The CD helper had saved all three far fields before writing. Pinned OpenNT
reads each after the preceding result, including when a valid destination
aliases a later pointer field. The helper now uses sequential prefix reads,
preserving the contiguous structure without narrowing offset+8. It does not
invent a guest error ABI. NULL/clear-only and overlapping strings retain
the original order.

verify-redirector-oem-capacity.ps1 binds the real Windows CP932 converter only
for tests (production remains CP_OEMCP), with real copy/lease sources and a
controlled real-mode address seam, not the CCPU protected-mode resolver.
r004 passes OEM bounds but reproduces the CD alias failure; r005 passes15
assertions after correction. OriginalProviders mode compiles the actual
VrGetUserName/CDNames unit with production flags, controlled public NetAPI
inputs and the existing host fixture closure. r009 has missing original link
dependencies; r010 identifies an unbuilt selected archive. Neither is a pass;
r011's expanded link and r013's attempted LTCG isolation also fail, not pass.
The final runner instead compiles the actual vrnmpipe dependency against the
in-process production async adapter, uses the existing selected host fixture
closure and the same call_ica_hw_interrupt alias as the production export.
No unresolved-symbol forcing or invented RAP success provider is used.
Existing fixture-only /force:multiple retains explicit host/register seams;
it is not a production-link change. r016 and final r021 pass10 assertions;
r021 checks its map for both original entries in its fresh vrnetapi.obj.
r012's final async fixture passes24, and final r026 OEM/CD fixture passes15.

## Real DOS boundary witnesses

Build-T420S20VdmRedirGuestTests.ps1 assembles existing test-only COM sources
with NASM into r014-guest-probes; immutable package media is untouched.
The existing console_terminal_observer is rebuilt x86 /MT/C11 into r024 with
src and src/ntcon-exe include roots. Only its launcher/package paths and three
selected Redirector modes change: system32/run16, system32/REDIR and MEM,
tests/VDMPASY, VDMPASW and VDMNETAP. No assertion is removed or weakened.
verify-redirector-guest-boundary.ps1 runs those modes serially, with the local
NEKOGP4 pipe prerequisite, explicit startup guest path, a fresh120s observer
budget, identity-checked package cleanup, restored environment and Z: removed.

r020/r022 are startup-harness failures (old relative guest path / inherited
empty direct-CMD mode); r023 observes ASYNC-OK but fails its old MEM path.
These are retained failures, not accepted runs. Corrected r025 passes all3:
read15281ms, write15129ms, NetAPI13489ms. Actual original INT2F/INT5C callback
witnesses require payload/result verification inside the COM before ASYNC-OK
or ASYNC-WRITE-OK; each also requires subsequent MEM output and exit1.
This proves real successful callback wiring, not exhaustive concurrent
teardown/callback interleavings. Separate native negatives cover destination,
cancellation and stale/dead-owner rejection; source review verifies original
VrpCompleteAsyncRequest suppresses queue/interrupt on adapter failure and
retains its release path. No original scheduler/completion policy is moved.

## Build and delivery status

Formal cache build/M0-T427/S2/r001 incrementally builds product-programs with
MSVC14.43/SDK22621 x86 /MT CCPU40. APP/RPC/I/O identity and guest media remain
unchanged. r006-runtime copies S2 and replaces the six EXEs and VDMREDIR,
retaining WOW32/media. It is not the published O:/winnt package.

r007-product fails before launching WOW because Windows PowerShell5 lacks
ProcessStartInfo.ArgumentList; this is not evidence of a product regression.
r008 uses the existing required PowerShell7 API and passes unchanged serial
Console17/Window17 and independent WOW-frontier assertions:249875ms total,
WOW69723, Console85935, Window88026. The build now reports no work to do.
No new WOW capability or physical RDP observation is claimed.

r027-publication first verifies the published S2 identity and candidate against
r008's tested manifest, retains all eight old files in recovery/system32, and
publishes the coherent set to O:/winnt/system32. All eight hashes match;
NTVDM=26DBA57C4195A75060BFDCC76E27D371BF11A19F58C0FFE8C62AEA8099FC977C,
VDMREDIR=3D7BAAC2B5F4952DDB5C8920C13628BEF8559B7D14712928CA714609F9735876.
APP0.0.427/RPC38/I/O25, WOW32 and guest configuration remain unchanged.
The generated base configuration header's refreshed identity also caused
dependent EXE relinks; the tested and published hashes, not older hashes,
are the acceptance inputs. Original compiler warnings are retained.

r028-published-smoke passes actual published MEM/native-zero in Console and
Window. r029-handoff also preserves all three S2 input/parent-return/exit23
frontiers (native, nested-console, nested-window), with Z: removed. Cleanup
is test-owned explicit cleanup, not a claim that every service retires itself.
Final review confirms only the registered mirror hook and project copy/staging
adapters change production behavior; no guest, original completion scheduler,
worker policy or protocol changes. Documentation governance, relative links
and diff checks pass. S4 exit criteria are met for sequential commit/push;
T430 remains open. Unrelated proposal and TODO changes remain excluded.

## Reproduction commands

| Obligation | Selected entrypoints / proof | Verdict and limit |
| --- | --- | --- |
| K02 | redirector_oem_capacity_test and actual VrGetUserName in redirector_netapi_provider_test; r026/r021; real INT2F1180 in r025 NetAPI. | Added OEM bytes/NUL capacity fixed; original BX0/CX0 retained and explicitly reproduced. CP932 test converter is not a production codepage change. |
| K03 | mvdm_redirector_async_complete/begin/end/release in redirector_async_publication_test; r012; original guest async read/write callback in r025. | Payload before original error/count, individual refusal, cancellation/dead/stale binding and release covered. Source-owned callback suppression reviewed; no atomic transaction or exhaustive concurrent teardown claim. |
| K04 | mvdm_redirector_write_cd_names in OEM fixture and actual VrGetCDNames in provider fixture; r026/r021. | Sequential aliased far fields, NULL/overlap, unwritable destination and NetAPI frees covered with controlled real-mode memory/public APIs. Original no-error guest ABI retained; PM selector validation is not this fixture's proof. |

From the repository in PowerShell7 with the installed x86 MSVC environment:

```powershell
tests/observation/verify-redirector-async-publication.ps1 -OutputRoot build/<fresh-async>
tests/observation/verify-redirector-oem-capacity.ps1 -OutputRoot build/<fresh-oem>
tests/observation/verify-redirector-oem-capacity.ps1 -OutputRoot build/<fresh-provider> -OriginalProviders -BuildCache build/M0-T427/S2/r001
tools/build/Build-T420S20VdmRedirGuestTests.ps1 -RepositoryRoot $PWD.Path -BuildRoot "$PWD/build/<fresh-guest>"
tests/observation/verify-redirector-guest-boundary.ps1 -RuntimeRoot build/M0-T430/S4/r006-runtime -Observer build/M0-T430/S4/r024-guest-observer/terminal-observer.exe -GuestProbes build/M0-T430/S4/r014-guest-probes -OutputRoot build/<fresh-guest-run>
```

The original-provider fixture requires the existing dependency-selected host
archives and controlled seam objects; it fails closed if missing. Guest
observations use the existing recorded machine-local pipe name, not an
external SMB-server availability claim. DBCS proof forces CP932 only in the
controlled fixture; production CP_OEMCP is unchanged. CDNames keeps its
original no-error ABI; failed guest writes cannot be called successful data
publication. Original CX0 remains a retained original limitation.
