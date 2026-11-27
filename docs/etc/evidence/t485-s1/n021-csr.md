# N021 CSR headers - S15 P23

Starting HEAD476bf2281. Fixed XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
DIR positions6-8: ntcsrdll.h, ntcsrmsg.h and ntcsrsrv.h. Raw objects and
before bytes are in build/t485-s15/n021-csr/inputs.json and sibling files.

## Whole-file result

ntcsrdll.h (blob963582def0cc3c79fa49134af218fca5b4a09f5a) and ntcsrmsg.h
(blob9bd7d892d373020948a3f58c64004fc0ba8ace65) already match XP byte-for-byte.
Preserve them, including the earlier CSR capture restoration. No change.
ntcsrsrv.h (blob4ea8fe3479c4e64a36434249a2bdd8a324f5411d) now also matches XP
byte-for-byte: no subset, shim, exclusion or #line adaptation. Restore original
LF, C++ scope, all declarations, CSR_THREAD and CSR_SERVER_DLL definitions.
This replaces the old NT4 header rather than preserving its obsolete layouts.

Actual consumers: non-mvdm/base/win32/server/basesrv.h and ntsrv-exe/opennt/
include/base_server.h. base_service.c constructs local CSR_THREAD objects;
service_internal.h embeds CSR_PROCESS; srvvdm.c accesses fields by their
compiled names. compat base_request/base_process use opaque pointers and
existing provider operations. None references the removed thread members or
removed server dispatch fields. These local structures are not the wire
payload; its fixed32-bit value shapes remain separately asserted.

## Difference accounting

| File | Before baseline / nonerror points / lines | XP after | Delta |
| --- | --- | --- | --- |
| ntcsrdll.h | XP / 0 / 0 | 0 / 0 | 0 / 0 |
| ntcsrmsg.h | XP / 0 / 0 | 0 / 0 | 0 / 0 |
| ntcsrsrv.h | OpenNT / 1 / 1136 | 0 / 0 | -1 / -1136 |

All three raw bytes equal XP; no #error bodies or #line changes remain.
Global1324files,775byte-exact,786nonerror same/538different,
7741points/153031lines;1252XP/72OpenNT. No mirror additions/removals;
one OpenNT-to-XP baseline transition. Counts retain LF/EOF differences.

## Verification

Cached full build and dual13-image/26hash publication of runtime24 is required after the final ABI correction.
Native CSR capture passed3475 assertions on each architecture. The first
native layout fixture failed on its old NT4 CSR_THREAD size assertion:
120/72 bytes. XP removes ShutDownStatus, ServerId and per-thread DLL data,
yielding96/56 bytes. Update only this version-specific expected size and its
source explanation; retain all other wire, process and identity assertions.
The additional layout diagnostic exposed a pre-existing stale BASE_API_MSG
assertion from before S14 P2: XP's SxS CreateProcess member expands the union
to224/152 bytes, with64/40-byte message prefix, giving288/192 total bytes.
Retain fixed expectations with source explanation and failure size reporting.
The x86 layout build also exposed the B duplicate CsrValidateMessageBuffer
prototype's missing NTAPI. Apply NTAPI to its B declaration and definition,
matching XP's four-argument stdcall ABI; retain the validation algorithm.
The x86 CONSOLERECORD assertion also predates the existing
ReenterCompletionPending BOOL:52 bytes, not48 (x64 remains80 via padding).
Update that expected size without changing the production early-completion
field or semantics. Failed attempts remain in the receipt.
No additional mirror diff is needed. Rebuild product and publish runtime24;
recheck the affected capture/layout paths before service and guest tests.
Final results and input identities are in the accompanying receipt.

N0218/17 delivered; next ntexapi.h. Overall56/159 and S15 8/14 directories.
Five audited files since P21 Full24/24 (including two unchanged CSR headers),
three changed mirror files. No guest modification or rebuild.

The service fixture also contained two pre-XP expectations. A newly connected
non-WOW peer cannot register/clear WOWEXEC: retain the real authenticated
provider's ACCESS_DENIED result rather than expect INVALID_PARAMETER for HWND0.
For the shared-WOW positive case supply GetUserObjectInformationA's current
window station and thread desktop. XP BaseSrvFindSharedWow rejects DesktopLen0;
the test previously supplied no desktop. Keep the actual native resolver and
all original reservation/return/rundown assertions, without mocking admission.
These test changes do not alter product/runtime24. Whole fixture reruns use
changed fixture inputs; initial failing cases and diagnostics are retained.

The WOW Get fixture must also provide a desktop output buffer. Previously it
checked only transport success and discarded the original status/payload;
missing capacity could leave the original record undispatched. Add the buffer,
decode with existing OpenNtBaseApplyGetCommand, and assert STATUS_SUCCESS,
exact command bytes and desktop text before claiming dispatch/startup. This
strengthens the existing acceptance instead of bypassing the failed state.

A targeted diagnostic then proved STATUS_NOT_FOUND (c0000225): the old fixture
omitted XP's task-keyed ASKING_FOR_PIF before asking by Console. Follow the
actual two-step startup contract: WOW|PIF with wowTask, then WOW command Get.
Check both original return statuses; verify the command and desktop payload
on delivery. Do not forge hConsole/fDispatched or bypass the original handler.

XP BaseSrvRemoveWOWRecordByTaskId returns STATUS_NOT_FOUND for a task absent
from its original list. The fixture formerly expected success for two invented
label-only task IDs. Now assert ERROR_NOT_FOUND, both labels remain separately
retirable, and the parent is still waiting. For the genuinely dispatched
wowTask bind a ticket before ExitVDM and verify successful original exit retires
that ticket. Both negative identity and positive lifecycle coverage remain.

Shared-WOW reuse additionally requires a real registered WOWEXEC HWND. The
old sleeping child provided none; XP intentionally discards that stale shared
record. Add one native test-child mode with a hidden message-only STATIC
window, PID-qualified readiness event and15-second bounded message loop.
The parent waits for readiness, checks the real HWND and calls the production
registration API after PIF/command delivery. An attempted earlier registration
failed because PIF had not yet established the Console; preserve that evidence.
No product resolver/window callback is replaced. Existing runner still owns
and cleans fixture child processes.

Final runtime24: cached build, dual publication/26 hashes, both capture3475,
both native layout tests, 42 service cases and real guest NetAPI passed.
[Receipt](n021-csr-result.json); [census](n021-csr-census.json).
