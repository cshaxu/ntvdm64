# T479 S4: driver build and Win3.1 identity checkpoint

This is partial implementation evidence, not S4 closure or a release. The
published S3 ten-component package is unchanged. The owner names the authored
component `src/addon/ntvdm64-386` and its eventual release artifacts
`assets/release/ntvdm64.386` and `assets/release/ntvdm64.vxd`.

## Provenance and build

The authored assembly uses ABI facts from read-only local references:

- `O:/repos.external/ms-dos-6/dev/smartdrv/sdvxd/vmm.inc`, SHA-256
  `28BD6EDABA10434DFCAD26BFCDAB8DBD1A3CC28100CD6910C67397D8C05F92E9`:
  Microsoft VMM service order, DDB and client-frame layouts and lifecycle
  notifications. The source carries Microsoft's 1988--1990 copyright notice.
- `O:/repos.external/OpenNT-4.5/nt/private/windows/inc16/vmm.h`, SHA-256
  `0619376681BF25E410BD3A9B3932398F6FB5483D62C94D223473BD542AC81C91`:
  corroborating VMM declarations and register contracts. Neither reference is
  imported, modified or needed by the build.

Tool recovery used the official Open Watcom Current-build release published
2026-11-05T08:55:11Z. The downloaded x64 Windows archive is 128147558 bytes;
its SHA-256 matches the publisher's digest
`3a14cd186bb7d2af9215816f77dfec9a5d1aa6d2a4e8ec71e1bc916f3df8dd7a`.
Only the linker, adjacent DLL/config and license were extracted into the task
directory; no installer was run. NASM generates OMF and the linker directly
generates static LE, with shared resident code/data. This uses documented
[linker segment attributes](https://open-watcom.github.io/open-watcom-v2-wikidocs/lguide.html#The_SEGMENT_Directive).
No original guest source, old experimental driver or binary-header patch is
a build input. Both extensions currently contain identical static images;
dynamic initialization/unload is not supported.

## Actual verification

[Machine-readable evidence](m0-t479-s4-driver-load.json) retains exact source,
tool and image hashes, background process/start/arguments/exit, guest output
and the existing integration-runner result. Work lives under
`build/t479-s4-driver`. The first assembly invocation exposed two installed
NASM matches; the script now explicitly chooses one executable. That failed
build never entered guest execution and is not a runtime failure or pass.

The raw-linker identity witness loaded on Win3.1 enhanced mode and returned a
real VMM VM handle. The separate control-block candidate then loaded and ran.
The reusable test is
`test/integration/ntvdm64-386/verify-win31-identity.ps1`; it extends the existing
`win31_runtime_matrix_test.ps1` with an independently authored prebuilt Win16
witness that retains its existing boot/ready contract. The default witness
selection is unchanged.

The checked runner uses the desktop-capable modern pwsh background context,
fresh `O:/tmp/vm479d` installation copy, independently authored DOS probes and
a fresh physical instance of the accepted S3 package. It proves:

1. Program Manager starts in enhanced mode with DOSONLY=1 and exits normally.
2. The project's VxD API returns its signature and actual current VM handle.
3. Two DOS VM probes have different handles and nonzero distinct generations.
   Their BIOS-tick intervals overlap; this is simultaneous execution evidence,
   not two sequential launches.
4. Unsupported API operations return carry set and error 1.
5. The enumerated `O:/w31` file inventory and hashes remain unchanged. The
   initial runner omitted hidden/system files from its enumeration; the runner
   now uses `-Force` for both inventory and final comparison. The earlier
   result is not proof for files omitted by that enumeration.

The driver uses VMM control-block storage rather than a fixed VM slot table.
It assigns generations on creation and clears executable state on
`VM_Not_Executeable`. The successful creation/query path is executable evidence;
the forced-retirement/late-callback cases are not yet proved. API ID 7f80 is
still an isolated-test identity, not an allocated global device identifier or
completed collision protocol.

## V86 callback and Win95 fixture checkpoint

The separately built `candidate-callback` image is 4699 bytes, SHA-256
`27D56C1F30FCA45650FB689140823E3A35FA93FCFC1269359137F764ACD439E9`.
The owned `checked-callback` run exited zero. Two simultaneously running DOS
VMs each executed 160 VMM-allocated V86 callbacks, with zero signature,
VM/generation, stack, EBP, EDI or direction-flag errors. Windows exited
normally. These callbacks receive a genuine guest far-call return frame;
this does not prove restoration of a redirected post-BOP continuation.

Win95 copied-installation runs with and without the candidate both stopped
at the same registry-error dialog before the controller ran. Read-only
private-desktop pixel captures establish that result; observer timeout alone
does not establish a driver failure. An equal-length destination still failed:
the existing path relocation utility enlarged/repacked registry blocks even
when replacement strings had equal lengths. A separate layout-preserving
fixture control then passed: same-length strings were replaced only within
REG_SZ values, leaving entry/block sizes and free-list metadata untouched and
recalculating existing block checksums. The same candidate loaded, two DOS VMs
ran concurrently, each completed 160 callbacks with zero errors, and the
observer/controller exited normally. The full original installation inventory,
including hidden/system files, was unchanged. This identifies fixture hive
relocation as the cause of the preceding startup rejection; it does not yet
identify every defect in the general path migration utility.
No restore-registry action was invoked and the original installation was not
launched or modified by these copied-installation runs.

## Remaining S4 gates

Following the owner's Win3.1-first direction, `candidate-semaphore` adds
VMM-owned counted semaphores and generation-checked signal dispatch. The ABI
register contract comes from the existing read-only `vmm.h` wrappers:
Create takes ECX and returns EAX/carry; wait takes EAX/ECX; signal and destroy
take EAX. The older 3.0 manual marks these services reserved and is not used
as proof of the 3.1 ABI. The `checked-semaphore` Win3.1 run exited zero,
with both VMs consuming two signals sent before their waits and rejecting
stale generations (three checks per VM, zero errors), retaining all 320
callback checks and normal Windows exit. The corrected `-Force` inventory
also verified hidden/system original files unchanged. This does not prove
an actually blocked waiter, IRQ notification, or forced-destruction cleanup.

The subsequent `checked-crosswake` Win3.1 run separately passes the blocked
case: VM A publishes its identity in a test-owned rendezvous file and waits;
VM B continues to execute and signals that identity/generation. The signal
records A's waiting state before invoking VMM, and B requires that state to
be one. A then resumes. Each VM records one successful pair check, zero
pair errors; prior identity, precompletion and callback checks remain green,
with normal Windows exit and unchanged original files. File polling and a
short BIOS-tick scheduling interval exist only in this DOS test witness, not
in the driver or proposed host notification transport. The observed waiting
flag and A's return, rather than elapsed time, establish the blocked wake.

Host notification work will use already selected original
`VDDReserveIrqLine`/`VDDReleaseIrqLine` in `nt_vdd.c`; the current product map
contains both symbols. These own line contention under the ICA lock. An EOI
callback alone is not a reservation. Host EOI call count -1 denotes reset or
cancellation, not completion consumption. Host queue acknowledgement and
guest VPICD ownership still need implementation and executable proof.

The new host-owned `mvdm_vm_notification.c` implements that reservation and
counted-publication doorbell boundary; it is not yet selected by the product
build at that checkpoint. Its matching x86 unit passes 68 assertions
with substituted VDD/ICA boundaries: reservation and hook collision rollback,
coalescing, reset rearm, stale/future acknowledgement, close without rearm,
and monotonically increasing serials across reopen. This is not real PIC or
guest notification acceptance. The queue remains caller-owned; ICA serializes
only the physical notification state. No timers, polling or RPC were added.

`checked-blocked` strengthened the cross-VM signal result with the original
VMM `CB_VM_Status.VMStat_Blocked` bit (offset zero, bit 10), in addition to the
driver's wait-entry flag. Both DOS output records passed that stronger check,
but the outer matrix failed normal exit at its 90-second observation bound.
Therefore the whole run failed. The witness previously called ExitWindows
as soon as result files existed, before DOS task termination was guaranteed.
The next candidate waits for VMM's executable inventory to contain only the
system VM; the prior exit failure's cause remains unproved until controlled
verification. Original guest bytes are unchanged.

The `checked-retirement` candidate subsequently passed the complete selected
Win3.1 matrix, including the original VMM blocked-state check, both DOS VM
retirements, normal Windows exit and original-file hashes. This removes a
known premature-shutdown condition from the witness. One passing successor
does not prove that condition was the sole cause of the prior timeout; that
failed run remains in the evidence rather than being relabelled as passed.

## Host BOP integration checkpoint

The subsequent cached x86 worker build selects the notification module and
links `mvdm_vm_bridge_bop` into `ntvdm.exe` (map address 00573d60). The boundary
is original BOP B1 with independent EAX signature 4e544d56 and ECX operation:
0 reserve, 1 arm, 2 publish, 3 snapshot, 4 acknowledge, 5 release. EDX/ESI
carry the 64-bit serial; result EAX is the status, EBX the reserved IRQ, and
carry reports failure. Non-protected/ring-nonzero callers are rejected. Other
signatures fall through to the original NIDDB switch without modifying state.
These are doorbell operations, not NTSRV control or task lifecycle requests.

`MVDM-HOST-DIV-870` registers the 14 added mirror lines in `virtual.c`; the
original diagnostic line reset before its switch remains. No original guest
file was changed. Owned `bridge-build` exited zero with the cached worker
build and 89 notification/BOP unit assertions. Worker SHA-256 is
`DDC7E6237604EB104773E4F914BF324A8A63BF696ED6F41E76785A5F6C6C048F`.
This candidate has not been published or exercised with the guest IRQ bridge.
The coherent package, original-PIC reset coverage, VPICD registration and
actual interrupt/wake tests remain required; link success is not those proofs.

## Win3.1 actual PIC/VPICD delivery

The owned `verify-pic.ps1` run completed cached x86/x64 component builds,
staged all ten current components as package
`5e1fd3ff-a6ed-4d64-b165-8474b64b880c`, assembled `candidate-pic`, and passed
the complete selected Win3.1 integration case. Candidate driver SHA-256:
`096EF79316AAD4B094997A4C3101A99236701C6AF68DFF79A708DC3C3BF43990`.
The probe's independent physical-stage identity is recorded separately by
the integration runner; it is not the source package UUID.

VDD reservation selected IRQ 15; the guest registered VPICD and armed only
after acquiring that handle. B published serial 1 and A serial 2; both
observed serial 2 from the real physical interrupt callback, with zero
notification errors. Coalescing therefore did not discard the later serial.
The callback snapshots/acknowledges the doorbell before physical EOI. Prior
identity, generation, 320 callback checks, counted waits and original-VMM
blocked-state checks remain passing. Both DOS VMs retired and Windows exited
normally; the original installation inventory/hashes remained unchanged.

This run proves actual host PIC to guest VPICD notification. The publisher
was reached synchronously through the diagnostic guest API/BOP, not an
off-CPU native completion thread. It does not yet prove completion-queue
draining, NTSRV registration, forced retirement, reset under real guest load
or IRQ-triggered semaphore wake. No default publication, S4 closure or
commit/push is claimed.

Post-run source review also added cleanup of the already-created system-VM
semaphore when Device_Init cannot acquire/arm its IRQ. That failure-only
assembly change is newer than `candidate-pic` and has not yet been assembled
or fault-injection tested; the recorded driver hash above identifies only
the candidate actually exercised. It must be included in the next build.

Complete initial inventory and host registration,
post-BOP callback restoration, counted wait/wake through a claimed notification device,
resource/reset/cancellation and forced-retirement/handle-reuse cases remain
open. Full product gates, coherent publication and S4 commit/push follow those
contracts. There is no claim that this identity-only candidate implements
commands, guest I/O, independent host frontend registration or READY.

Original guest changes: zero. Host mirror change: the 14-line registered
MVDM-HOST-DIV-870 BOP seam above. Candidate runtime, build and tests remain
uncommitted pending S4 implementation and review.

Checkpoint documentation governance and `git diff --check` pass. The checked
integration runner completed with exit zero; no background test remains live
from this checkpoint. This does not close the remaining S4 gates.

## Win3.1 queued IRQ wake and native producer-thread evidence

The subsequent `verify-wake.ps1` run exited zero after cached x86/x64 builds,
coherent ten-component staging and the selected `386-dosonly1` integration
case. Package: `ed488089-dca9-4a50-bc8a-66d3d84a2fc8`. Driver: 4935 bytes,
SHA-256 `781641DDBFFB52CD4A587E2384654FFF933641CF7669891017A31B167E5DC04F`.
The structured companion records the full manifest, driver/tool hashes and
both VM reports. This includes the Device_Init semaphore-cleanup change;
failure injection of that branch remains outstanding.

API 8 now publishes the target VM/generation into the host FIFO instead of
calling Signal_Semaphore directly. B verified A's original VMM blocked bit
before publication. The physical IRQ handler takes queued identities, checks
the current VMM inventory and generation, then signals A. Both VMs passed
the pair check, all 320 callback checks and counted waits; IRQ 15 serial 3 was
observed by both. Normal VM retirement and Windows exit passed, and the full
original installation hash inventory remained unchanged. This is actual
PIC-driven semaphore wake, but its publisher still runs through guest BOP.

BOP operations 7/8 register/retire local wake identities, 9 publishes a wake,
and 10 takes one. EBX carries VM identity and EDX/ESI its generation for these
operations; the doorbell operations retain their serial contract. Local wake
registration does not create NTSRV frontend/task management records.

A separate native-thread unit uses a real recursive critical section: the
producer publishes, the consumer retires that VM with its wake unread and
reuses the handle, then the producer resumes. The old generation is rejected
and only the new generation is delivered. The owned build/run exited zero
with 184 assertions. PIC and EOI boundary functions remain unit substitutes;
this is not evidence of off-CPU wake inside a running guest.

Remaining S4 work includes independent runtime producer/guest wake proof,
forced retirement and resource/reset failures. NTSRV frontend integration
must use the real I/O provider, not a dummy provider. Complete command and I/O
remain S5/S6. No new Win95 test, default publication or S4 closure is claimed.

## Independent native-thread Win3.1 wake

The subsequent `checked-thread` integration run passed the selected
`386-dosonly1` case, both VM reports, 320 callback checks, blocked-state wake,
normal retirement/Windows exit and unchanged original installation hashes.
The structured companion contains its separate package manifest and hashes.
The driver is the same 4935-byte `candidate-wake` artifact recorded above.

The integration-only `host-wake.c` link seam moves the one targeted publish
to a native thread. Its map resolves `_mvdm_vm_bridge_bop` to `host-wake.obj`,
the delegated `_mvdm_vm_bridge_bop_direct` to `notification.obj`, and
`_mvdm_vm_wake_publish` to the actual production archive. Production ICA/PIC,
CCPU and guest VMM execute unchanged. The producer has no guest pointer or
VMM call. Its completed thread result is checked at normal guest shutdown;
there is no CPU-thread join. No product target selects this test source.

This supersedes the missing off-CPU-to-guest wake evidence above. It does not
connect the real command-result owner or prove post-BOP state restoration,
NTSRV frontend registration, forced retirement or failure/reset recovery.
The test executable is never a default-release candidate.

Source review also closed a host admission gap: VM wake registration now
checks the open notification session under the same ICA lock, rejecting
registration before open or after close. Cached worker build and 196 unit
assertions pass, including the independent producer/retire/reuse sequence.
That production queue implementation is included in this integration link.
Original guest changes remain zero; the mirror seam remains DIV-870 only.

## Real ICA cancellation during independent-thread publication

The next integration-only seam holds the original ICA lock, publishes a wake
and calls original `ica_hw_interrupt_cancel` before releasing that lock. An
EOI observation wrapper delegates to the unmodified notification callback and
requires a real `count == -1` delivery during the cancellation. At shutdown
the producer exit result must be zero. Thus successful guest wake alone
cannot satisfy this cancellation assertion. No product source changes were
needed for this witness.

The first strengthened run exited before the guest probe, with outer result
`0x719` (`RPC_S_NO_INTERFACES`, 1817); no graphics handshake or guest result
was produced. Its matrix remains failed. The unchanged compiled package was
then replayed on a fresh copied installation and passed both VM results,
320 callback checks, actual cancellation observation, blocked-VM wake, normal
retirement/Windows exit and original-installation hash verification. The
structured companion preserves both runs and the exact package/source hashes.
This establishes cancellation/rearm on that successful run, not a cause or
repair for the first startup failure. The 1817 failure remains unlocalized
and must be dispositioned before default delivery. Full guest ICW1 reset,
forced VM retirement and resource-initialization failure remain distinct
proof obligations; this case does not silently claim them passed.

Source follow-up found a concrete bootstrap mismatch: NTSRV main establishes
the ncalrpc endpoint before registering the service interface, while Run16's
own spawned-server readiness loop treated `RPC_S_NO_INTERFACES` as terminal.
The existing real RPC management unit explicitly exercises that status after
interface removal. A narrow candidate now accepts it only after Run16 itself
has spawned NTSRV and within the unchanged absolute startup budget. It does
not alter the shared classifier, established connections or submitted tasks.
This explains a possible path to the observed 1817, but the retained run did
not trace the exact server phase. Controlled bootstrap reproduction and the
new candidate's build/runtime validation remain required before claiming the
observed failure fixed. No publication accompanies this source change.

The follow-up deterministic fixture includes the actual launcher source and
substitutes only RPC/process/clock boundaries. The unavailable -> own spawn ->
no interfaces -> ready sequence fails with the pre-fix function and passes
with the candidate (246 assertions). It also verifies immediate failure for
NO_INTERFACES before own spawn, revision mismatch, RPC call failure and server
death; one spawn only; and the unchanged absolute five-second deadline. The
first fixture link lacked the launcher's dependency closure; after using the
normal dependencies the build and test pass. A permanent
`run16-broker-bootstrap-test.exe` Ninja target now shares those dependencies;
its formal target verification is still running at this record update.

The coherent normal ten-component `bootstrap-package` (no test-only worker
interposition) subsequently passed the selected Win3.1 enhanced-mode case,
both VM reports, all callback/wait/wake checks, normal exit and immutable
installation hashes. Its manifest is in the structured evidence. This proves
the candidate handles the reproduced startup status and preserves the tested
Win3.1 behavior. It cannot retrospectively prove the untraced server phase
of the original 1817 event. Full product delivery gates and S4's outstanding
resource/forced-retirement contracts remain open; no publication is claimed.

The permanent x86 Ninja bootstrap target subsequently passed all 246
assertions after its explicit common-root dependency was added. The failed
target link is retained as a corrected test-build defect, not a passing run.

## Win3.1 forced retirement of a blocked VM

The `checked-retire-forced` run uses the normal `bootstrap-package` and the
unchanged candidate driver. A separate independently authored test-only VxD
calls original VMM Nuke_VM for another ordinary VM, after checking its live
handle and original blocked flag; current/system VM targets are rejected.
This helper is not linked into or shipped with the product driver.

DOS VM A writes its identity then waits. B confirms A is blocked through the
helper, destroys A and attempts a late guest API wake for A's saved identity.
The late wake is rejected; A's result shows no return from its wait. The
controller then observes only the system VM before requesting Windows exit.
Both the forced-retirement assertions and the normal-exit matrix passed.
The original installation's complete file hashes remained unchanged.
Helper/probe/driver hashes and raw parsed VM results are in the structured
companion. Original Microsoft ABI provenance is linked in the test README.

This closes the real forced guest-retirement witness; it does not itself
prove actual VMM handle reuse or delivery of an already queued native result
after forced retirement. Host generation reuse/queue removal has separate
native-thread unit evidence. Initialization failure/rollback, whole-payload
resource disposal and those combined lifecycle cases remain bounded S4 work.
No new Win95 execution, publication or S4 closure is claimed.

The later `checked-retire-queued` run adds host-side assertions at the actual
retirement BOP: enqueue a wake for that VM while holding ICA, delegate to
production retirement, require an empty queue and reject a late publication
for that generation. At successful bridge close the test seam reacquires
and releases the exact IRQ and installs/removes its EOI hook. It fails the
process if those resource checks fail. The forced blocked-VM test and normal
Windows exit pass with this separate test package; the original installation
hashes remain unchanged. The companion records its source/package hashes.
This adds combined guest/host queued-retirement and close-resource evidence;
actual VM handle reuse and initialization failure remain separate cases.

## S4 P1 normal host baseline delivery

The established Full product suite completed against the normal
`bootstrap-package`, without any test-only worker seam: **16/16**, runner
exit zero, **489298 ms**. All ten input image hashes remained unchanged.
The existing owner-authorized DOS4GW/NESticle external-fixture exclusion is
still skipped, not passed. The companion retains all gate results.

Package **ca3b367a-3d54-4d05-a9a8-147594d7450d** was subsequently published to
`assets/release` and `O:/winnt/system32`. All ten images and the Hook license
match the tested package. The publisher completed with exit zero, preserving
the prior files and hashes for rollback. Neither test-only worker interposition
nor the test retirement VxD is published. The independently authored bridge
driver remains a candidate, with no automatic installation into original guest.

The single implementer performed a separate diff/ownership review: native
notification/queue state remains under the original ICA lock; the BOP admits
only the signature and protected ring-0 context and preserves original NIDDB
dispatch otherwise. The Run16 change is confined to its own service bootstrap,
not established connection recovery or submitted-task retry. The only mirror
change is virtual.c +14/-0, registered as MVDM-HOST-DIV-870. Original guest
sources and installations remain unchanged. Documentation governance and
diff checks pass; delivery records are checked again before commit.

This is a safe baseline P delivery, **not S4 closure**. Remaining bounded S4
proof includes real guest initialization rollback and actual VMM handle reuse;
Win95 wait/wake is deferred by the owner's Win3.1-first instruction. Real
54:08/0A command continuation and complete I/O remain S5/S6, not capabilities
proved by the current queue/IRQ witnesses.

## Subsequent actual Win3.1 VM handle reuse

After P1 publication, the normal package and unchanged candidate driver passed
`checked-reuse` (runner exit zero). The Win16 controller waited for both DOS
VMs to retire, then invoked WinExec for a third DOS VM. Its actual VMM handle
**2155442176** reused B's handle; the generation changed from **2 to 4**.
The old-generation direct and queued wake API requests both failed, while a
new-generation signal followed by wait consumed one valid completion. The
host verifier cross-checked these values with the original pair's reports.
The two-VM callback/wait/wake checks and final normal Windows exit also passed;
the complete original installation inventory and hashes remained unchanged.

This closes actual VMM handle reuse for the tested Win3.1 lifecycle. It does
not substitute for initialization rollback or Win95 acceptance. The test
changes only independent fixtures and their runner; production images and
the published candidate are unchanged, so the existing Full 16/16 evidence
is reused rather than rerunning unrelated product tests.

## P3 Win3.1 initialization rollback and bounded handoff

`checked-init-failure` passed with exit zero. Its separate test-only BOP seam
rejects arm after the real host reservation and guest VPICD virtualization.
The actual guest driver calls close; the seam verifies the original IRQ can
be reserved/released again and its EOI hook installed/removed again, then
writes the required one-shot marker. Win3.1 exposes no driver API afterward
(address/signature/VM all zero), reaches Program Manager and exits normally.
Original installation hashes remain unchanged. This proves that exercised
rollback branch; it is not exhaustive fault injection of every VMM allocator.

The unchanged 4935-byte Win3.1 driver is now staged as a **candidate** at
`assets/release/ntvdm64.386`, SHA-256
`781641DDBFFB52CD4A587E2384654FFF933641CF7669891017A31B167E5DC04F`.
The add-on manifest records its exact source hash, verified bridge scope and
candidate status. It is not installed into O:/winnt or any original guest;
the Win95 .vxd is not published. The ten-component P1 package remains unchanged
and hash-verified at both destinations; cached inputs and Full 16/16 are reused.
The first task-local publication check referred to a nonexistent bootstrap.exit;
the corrected strict check reads bootstrap-runtime.exit and all other actual
runner exits with terminating errors, then rechecks source and published hashes.
That guard mistake is retained in the structured publication record.

The owner's Win3.1-first instruction refines the stage sequence: this bounded
S4 Win3.1 checkpoint hands off to S5 command continuation. **S4's Win95 obligations
remain open**, to run after the Win3.1 S5/S6 path and before S7/T closure. No
Win95 acceptance is inferred from this guest. Full PIC ICW1 reinitialization
during an active guest, every VMM allocation failure and complete command/I/O
shutdown are not established by these focused tests; retain them in S7's
failure matrix rather than claim them passed. Tested real ICA cancellation,
guest retirement/reuse, close and the injected arm failure remain distinct
from those untested combinations.

The private probe API device ID 7f80 has no claimed global allocation. Collision
with an unrelated installed VxD is not tested; the candidate is only exercised
in the owned copied fixtures. S5 must authenticate bridge registration rather
than treating that number alone as identity. General-installation compatibility
remains part of S7; this handoff is not permission to silently enable a conflicting
driver. Final documentation governance and publication hash checks pass.
