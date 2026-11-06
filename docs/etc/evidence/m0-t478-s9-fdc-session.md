# T478 S9 — original FDC access to COMMAND registration

## Scope and source finding

Baseline: 1fb0a38f5. Finding 13 is a project integration defect: original
OpenNT softpc.new/host/src/nt_rflop.c uses the process-shared pFDAccess
registered by dos/command/cmdmisc.c. The selected project replaced that raw
guest alias with numeric locations, but kept the registration in thread-local
storage. Its FDC helper also used raw CreateThread, unlike other original
workers adapted to the session bridge. An explicitly bound child therefore
still failed the earlier actual-object scalar probe (parent count stayed 1).

Original evidence: nt_rflop.c lines 515, 841, 892, 1043; cmdmisc.c line 600,
in the pinned read-only OpenNT source. The existing [role review](../../../artifacts/mvdm-audit/shard-3/floppy-lease-failure-role-review.md)
and [prior probe](../../../artifacts/mvdm-audit/shard-3/fdc-bound-session-scalar-probe.c)
retain the baseline. No external source is a build/runtime dependency.

## Repair

- COMMAND keeps one session-owned numeric registration, with short locked
  snapshots and existing session teardown ownership. No persistent guest
  pointer, shadow disk counter or process-global registration is introduced.
- DIV-871 changes only the original FDC thread creation boundary to the
  existing void/cdecl parameter bridge. The original thread body, request,
  result, device open/close and reset algorithm remain selected unchanged.
- FD word reads/writes use bounded copies against the captured memory epoch.
  These do not borrow the CPU thread's shared lease slots. Provider retirement
  waits for a current copy to finish; a retired or replacement epoch rejects
  access. Existing borrowed-lease callers retain their lifecycle contract.
- A read/modify/write uses one registration snapshot for both copies. Original
  modular arithmetic remains; no atomicity against guest writes is promised.
  The actual word in guest memory remains authoritative.
- Session's private layout ABI advances 9 to 10; no product RPC/guest ABI change.
  COMMAND registration occurs before original FDC work is launched. Existing
  bound-thread disposal preconditions own registration lifetime.

The same FD adapter also serves fixed-disk access, which receives the same
ownership correction. General guest-pointer consumers and new device behavior
are outside this finding.

## Verification

The focused [actual-object test](../../../test/integration/fdc_command_session_test.c)
and [runner](../../../test/integration/fdc_command_session_test.cmd) link the
current session, COMMAND, numeric-location and real thread bridge objects.
43 checks pass: shared bound-worker access; separate sessions; no binding;
registration and memory epochs; an outstanding CPU lease; modular decrement;
active worker disposal rejection; copied read versus provider retirement;
teardown and binding retirement. The original nt_rflop object references
_opennt_create_void_cdecl_parameter_thread, with no raw CreateThread reference.

Existing guest_memory_lease_test, guest_memory_lease_boundary_test (45 checks),
session_lifecycle_test and thread_start_session_fixture pass against the new
session implementation. The thread fixture initially lacked its existing
ccpu_thread_exit_compat object/include in the ad-hoc runner; after supplying
that actual dependency it passes, including all entry signatures and exit hooks.
No production stub was added to satisfy tests.

x86 worker/hook/redirector/WOW32 and x64 worker/frontend/monitor/launcher/service/
hook builds pass. Logs and owned staging are under build/t478-s9. Final focused
recheck passes before staging package-final2. All 16 selected Full gates pass in 415352 ms against the identical ten-image
package. DOS4GW/NESticle are owner-excluded and reported skipped, not passed.
Application 0.0.478 / protocol 67; package 694495a1-285e-4e8c-940f-3f2425de4445.
All ten published images and manifest match package-final2. Raw ntvdm.exe
matches the manifest input hash. [Compact results](../../../artifacts/mvdm-audit/shard-3/fdc-session-s9-results.json)
retain gate timings, hashes and explicit scope. Worktree publication completed;
no O:/winnt write and no push. Documentation governance and diff checks pass.

## Explicit original limits

This is not physical floppy-media acceptance. No physical device is assumed or
claimed tested. Original reset uses handle closure/its shared termination signal,
not a join, and its existing global request/device model is not redesigned.
No new cancellation, retry, rollback, media emulation or stronger-than-original
fault tolerance is admitted. Historical injected post-close counter failure
shows a possible retirement/failure inconsistency; it is not evidence that
normal valid-session access still fails after this repair. Failed access after
provider retirement remains refusal, not a new device recovery policy.

Only this worktree assets/release is an authorized publication destination;
O:/winnt and remote push are excluded.
