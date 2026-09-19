# T420 S35 XMS capability investigation

## Status and inputs

S35 remains open. These are real guest observations, not an XMS package
closure. The selected machine is MSVC x86 CCPU40; guest HIMEM is unchanged.
Its deployed SHA-256 is
`08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08`,
identical to `src/mvdm/dos/v86/dev/himem/himem.sys`.

The eight XMS manifest members include a header and the unselected RISC
backend `xmsmemr.c`; eight manifest members do not mean eight compiled C
files. Six now match the pinned `OpenNT/base/mvdm/xms.486` files bytewise:
`xms.c`, `xmsa20.c`, `xmsdisp.c`, `xmsmemr.c`, `xmsmisc.c`, `xmsumb.c`.
The last had only newline differences, restored mechanically after normalized
equality was checked. The two existing semantic deltas remain `xms.h`'s
callback parameter naming/order and `xmsblock.c`'s bounded guest-memory seam.

## PIF path lifetime finding and bounded correction

Original `config.c` calls `init_lim_configuration_data` before `sas_init`.
That function reads CONFIG through `GetPIFConfigFiles(TRUE, ...)`.
Later DOS `cmdGetConfigSys` calls `ExpandConfigFiles`, which asks for that
same path again. The original resolver frees and clears the override on the
first lookup, so the second read falls back to the default CONFIG. AUTOEXEC
has a separate pointer: an AUTOEXEC marker alone cannot prove CONFIG loaded.

The original owner and interface remain `nt_pif.c::GetPIFConfigFiles`.
Direct unchanged composition was tested and lost the override. A separate
adapter cannot retain the private path without duplicating its ownership.
The selected correction therefore retains the original strings across reads,
frees old strings before a new PIF selection, and leaves final reclamation to
the one-worker process lifetime. There is no new ABI, parser, allocator,
guest image, kernel provider or adapter policy. The change extends the
existing `MVDM-HOST-DIV-157` register: four changed executable lines, net zero
executable-line growth; explanatory comments are separate.

Pre-correction `s35-umb-r5.txt.console.txt` showed the AUTOEXEC marker but
not the expected INT15 reservation. Post-correction
`s35-umb-fixed-r1.txt.console.txt` showed the original HIMEM `/NUMHANDLES=17`
message, proving that CONFIG parameters now reached the driver. That reserved
INT15 configuration then timed out; it is not a passing result.

## Real guest coverage so far

All referenced logs are below `O:\winnt\logs`.

| Route | Result and exact evidence |
| --- | --- |
| Public INT 2F XMS entry; allocate, conventional/XMS moves, grow, forward overlap, lock/unlock, A20, free/reuse | `s35-xms-r2.txt.console.txt` emitted `S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK`, with process exit 0. |
| Odd move length, repeated unlock, repeated free, excessive allocation | The same guest asserts the original A7/AA/A2/A0 errors before the success marker. |
| UMB during CONFIG before DOS ownership | `s35-umb-driver-r1.txt.console.txt` emitted `S35_XMS_BOOT_UMB_ALLOC_RELEASE_DOUBLE_FREE_OK`; the test-only driver calls public HIMEM functions, then discards itself. |
| UMB after CONFIG | The same run emitted `S35_XMS_UMB_NO_FREE_BLOCKS`. Original `doskrnl/bios/sysinit1.asm::ConfigDone` unconditionally calls `AllocUMB`; post-boot absence is not proof that XMS allocation failed. |
| Default INT15 AH=88 | The same run returned AX=0 and emitted `S35_XMS_INT15_DEFAULT_ZERO_OK`, followed by the core success marker and exit 0. |
| `/INT15=128` without `/NUMHANDLES` | `s35-int15-reserve-r2.txt.console.txt` reached the boot UMB success marker but not AUTOEXEC; its report records timeout. Root cause is still under investigation. |
| Established product regression | `Verify-CommandExitStatus.ps1`, prefix `s35-config-regression-r1`, passed all 17 transcript-gated COMMAND/MEM/EDIT/native/guest-exit cases. |
| Reproducible XMS gate | `Verify-T420S35XmsGuest.ps1`, `guest-r7`, prefix `s35-xms-gate-r3`, passed direct, PIF profile, nested COMMAND and twice-in-one-batch cases. All four check actual guest markers and exit status. |

The formal graph is `build/M0-T420/S35/formal-x86-r1`. Its default library
build completed 469 edges; explicit `ntvdm.exe run16.exe basesrv.exe dtmgr.exe
VDMREDIR.dll` completed the remaining 58 edges. Those five artifacts supplied
the post-correction tests. Test fixture sources are `xms_capability.asm`,
`xms_umb_driver.asm` and `xms_profile_builder.c` under `tests/observation`.
`Build-T420S35XmsGuestTest.ps1` builds the default profile; `-ReserveInt15`
builds the retained failing reproducer. Neither option rewrites package media.

The first repeat harness never reached XMS: injected `tests\...` stopped at
the backslash and the observer reported failed input delivery. The successor
uses an original COMMAND batch instead of keyboard injection. Its first
input had LF-only lines, which old COMMAND interpreted as one command tail;
the build now explicitly emits DOS CRLF. Only `s35-xms-gate-r3` is the passing
four-case result. These two harness failures are not XMS failures and were
not counted as passes. The final source-format-only rebuild is recorded in
`formal-x86-r1/format-rebuild.stdout.log`; the final deployed package supplied
this four-case gate. This bounded S35 P1 delivery does not close S35.

## Remaining closure work

### 2026-09-19 low-DOS environment isolation

The reserved-INT15 timeout is not sufficient evidence of an XMS failure.
`s35-low-default-r1.txt` reproduces the failure without `/INT15=128` when
`DOS=HIGH` is absent. Read-only snapshots of that test worker (PID 50200,
launcher 49196) and its exact formal linker map identify the native path as
`illegal_op_int -> host_error -> TerminateVDM -> nt_remove_event_thread ->
mvdm_softpc_event_thread_alert_and_join`. The final wait is secondary to the
guest fault, not evidence that the XMS operation itself is waiting.

`s35-low-default-worker-r4.txt` records the real-mode exception frame
`0E2F:1F8F`, flags `0297`. The segment begins with the COMMAND PSP, not DOS
kernel code. Its fault location contains environment PATH bytes. The original
COMMAND map places `Alloc_error` at `1F8D` and `EnvSiz` at `203C`; the latter
contains `2E3B`, also environment data. The live `EndInit` bytes at offset
`0332` match the deployed COMMAND image, corroborating map applicability.
Source and deployed COMMAND SHA-256 both equal
`908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`.

Original `rdata.asm::EndInit` saves INIT variables before shrinking the
resident allocation, but its permanent-COMMAND environment expansion still
reads/writes `EnvSiz` after that shrink and branches to the INIT-resident
`Alloc_error` on mismatch. The local `rdata.asm` has zero semantic diff from
pinned OpenNT when ignoring CR/LF. `cmdenv.c::cmdGetInitEnvironment` retains
the original ES:0 destination, required-byte count and BX paragraph result;
its bounded lease does not relocate the guest environment. The evidence
supports a stale INIT reference exposed by the low-DOS/environment layout,
not a demonstrated CCPU instruction defect. No guest patch has been applied.

Two isolated short-environment controls preserve all product binaries and use
only a child-process environment, never changing the user's environment:

| Configuration | Transcript and result |
| --- | --- |
| No DOS=HIGH, default INT15 | `s35-low-shortenv-r1.txt.console.txt`: boot UMB, AUTOEXEC, default-zero INT15 and XMS core markers; launcher exit 0. |
| No DOS=HIGH, HIMEM /INT15=128 | `s35-reserved-shortenv-r1.txt.console.txt`: boot UMB, AUTOEXEC, AX=0080, `S35_XMS_INT15_RESERVED_OK` and XMS core markers; launcher exit 0. |

These controls prove the reserved capability can execute; they do not repair
or accept ordinary-environment low-DOS startup. Surviving test workers are
also not counted as natural teardown success. The native alert/join failure
needs its own lifecycle proof. The owner has now permanently prohibited guest
media modification, including rebuilding replacements and runtime patches.
The earlier request to expand scope for guest repair is withdrawn. Further
work must audit original-contract host integration and explicitly preserve
any proven original-guest limitation, not silently suppress the reproducer.
The diagnostic sources are test-only, not product source or runtime inputs.

The shutdown source audit additionally finds a concrete coverage gap:
`nt_event.c` creates the event thread with `CREATE_SUSPENDED`, and
`nt_fulsc.c::nt_init_event_thread` resumes it later. Original removal only
calls `NtAlertThread`; our DIV-206 additionally waits indefinitely for exit.
An alert cannot execute a still-suspended thread. The existing
`softpc_event_thread_shutdown_fixture.c` creates its worker running, so it
cannot validate pre-resume startup failure. The captured non-main threads
include the heartbeat and a thread with no product frames; this is consistent
with pre-resume failure but does not yet identify its suspension count.
Do not claim this specific runtime cause proven until the original
`nt_init_called` state/thread identity or a bounded reproducer corroborates it.

The bounded local reproducer now confirms the helper defect independently:
`tests/observation/event_thread_startup_failure.c` links the actual
`mvdm_softpc_event_thread.c` and `nt_thread_alert_compat.c` with MSVC x86
`/MT /W4`. Build root: `build/M0-T420/S35/shutdown-reproducer-r1`.
Its child creates the thread suspended and calls the product join helper;
the parent observes a two-second timeout, terminates only its own disposable
child, and emits `S35_SUSPENDED_THREAD_JOIN_HANG_REPRODUCED`. Compilation and
reproduction completed successfully. This is defect-reproduction success,
not product acceptance; the earlier real-worker suspension state still needs
direct corroboration. No product or guest-media change was made for this test.

The next isolated real run (`s35-low-inputstate-r1.txt`, launcher 44240,
worker 60656) timed out and its exact-map read-only snapshot reports
`nt_init_called=0`, with CCPU still at the illegal-instruction BOP. Original
`nt_init_event_thread` sets that flag before its sole initial ResumeThread;
therefore this worker did not reach the initial input-thread resume path.
`s35-low-inputstate-r1.threads.txt` preserves the host stacks. This
corroborates the pre-resume cleanup defect; it does not repair or independently
prove the precise environment-corruption branch in this fresh run. The test
process group was explicitly terminated and the previously passing default
fixtures restored. Product and original guest media remain unchanged.

The shutdown sweep also reproduces a distinct running-wait mismatch. The same
bounded fixture with `--running` creates an unsuspended `SleepEx(INFINITE,
TRUE)` worker and emits `S35_RUNNING_SLEEP_EX_JOIN_HANG_REPRODUCED` after
the child times out. Pinned OpenNT `base/win32/client/synch.c` explicitly
retries `STATUS_ALERTED` in both `WaitForMultipleObjectsEx` and `SleepEx`
(lines 1421 and 1535). The current thread-alert adapter prefers native
`NtAlertThread`, whereas `opennt_NtWaitForMultipleObjects` wraps Win32
`WaitForMultipleObjectsEx`. This composition loses the native alert-return
contract. The old shutdown fixture's `SleepEx`/WAIT_IO_COMPLETION assumption
therefore cannot validate the current native-alert implementation. Both
pre-resume ownership and running-wait notification must be resolved before
claiming lifecycle closure; arbitrary timeouts or TerminateThread are not
accepted repairs.

A local QueueUserAPC-only candidate was tested and rejected, then fully
reverted without deployment. Without a worker-entry handshake its notification
can execute before the thread procedure, leaving the subsequent SleepEx
waiting. With an explicit test-only ready event the candidate returns zero,
but this proves only the already-entered worker case. The retained fixture
now includes that ready event; the unchanged native-alert product helper
still reproduces the running-wait timeout with the handshake. Therefore the
native/public wait mismatch is not merely the fixture's entry race, and an
APC-only substitution is not an accepted lifecycle repair. Product source
has no retained change from this experiment.

Original-wait recovery now has one positive modern-host boundary witness:
`tests/observation/native_console_wait_probe.c` compiles as x86 and runs in a
hidden, real Console. It calls the exported NTDLL NtWaitForMultipleObjects
with the actual STD_INPUT_HANDLE plus a signaled event. Log
`s35-native-console-wait-r1.txt` records status `00000001` and exit 0.
Thus the current Console handle is accepted by native NT wait; the public
Win32 wait translation is not demonstrated necessary in this configuration.
This is not yet Terminal/ConPTY or full pending-alert acceptance. Test the
native contract before replacing the adapter; do not infer universal handle
compatibility or close the pre-resume lifecycle issue from this one result.

The extended probe `s35-native-console-wait-r2.txt` also queues a native
alert to its own thread before entering a finite native wait, and receives
exactly `00000101` (STATUS_ALERTED), exit 0. The product candidate now removes
the 58-line `nt_wait_compat.c` and its build selection, retaining only the
original-shaped NtWaitForMultipleObjects declaration linked from the existing
ntdll.lib dependency. No MVDM body is changed. Formal x86 build
`formal-native-wait-r2` completed all 525 edges, including the four EXEs,
VDMREDIR.dll and VdmTib ownership gate. Deployment/runtime regression are
still pending; the pre-resume cleanup issue is not solved by native wait.

Subsequent bounded native-wait deployment validation passed: five host artifacts
match formal-native-wait-r2 hashes (`s35-native-wait-artifacts-r1.json`). Prior
host binaries remain in `build/M0-T420/S35/pre-native-wait-deployment`; guest
media were not replaced. `Verify-CommandExitStatus.ps1`, prefix
`s35-native-wait-regression-r1`, passed all 17 default transcript-gated routes.
`Verify-T420S35XmsGuest.ps1`, fixture guest-r7, prefix
`s35-native-wait-xms-r1`, passed direct/profile/nested/repeat. These results do
not close pre-resume shutdown or ordinary-environment low-DOS startup.
Commit/push remains pending the reported outbound approval block.

The pre-resume guard is now an unaccepted local candidate: original
`nt_init_called` is passed to the join helper; a never-resumed thread remains
suspended for process-exit reclamation, while a started thread is alerted and
joined. The updated bounded test uses native NtDelayExecution, verifies the
started thread's exit status and verifies that the suspended thread never
enters its procedure. Both child cases return zero; formal incremental x86
build passes. No new independent lifecycle state or guest change is added.

The subsequent ordinary-environment low-DOS run did NOT validate error
shutdown: `s35-startup-cleanup-r1.txt` times out, but worker 18940's exact-map
stack now shows CCPU executing `printer_io -> inb -> printer_inb ->
notbusy_check`, not host_applClose or the event join. Corrupted guest startup
is not a deterministic shutdown trigger. Do not count this as either a
successful real shutdown test or proof the guard still hangs. The owned test
group (launcher 59828) was terminated; O:\winnt was restored from
`pre-native-wait-deployment` and the default guest-r7 test fixtures restored.
The latest guard remains build-only WIP pending a deterministic real fault
witness and the full regression matrix. Earlier deployed native-wait results
remain historical evidence, not the current package identity.

Deterministic early-fault follow-up: the independently authored test driver
has STARTUP_FAULT mode: write `O:\winnt\tests\S35F.HIT`, then execute UD2
during CONFIG. Original guest media are unchanged. The first run exited but
its empty Console snapshot was insufficient evidence. The second run
(`s35-deterministic-startup-fault-r2.txt`, launcher 35552) produced
`S35_TEST_DRIVER_STARTUP_FAULT` in the initially absent witness file, then
exited naturally with no worker remaining. Launcher exit was zero, recorded
without inventing an error-code contract. The remaining test broker was
cleaned by verified parent PID; default guest-r7 fixtures were restored.
Four XMS routes pass on this guard candidate (`s35-cleanup-xms-r1`); the
17-route regression (`s35-cleanup-regression-r1`) is in progress.

That regression subsequently completed: all 17 routes passed. The existing
thread-shutdown fixture was also updated to the native wait contract and
passed direct x86 compilation/execution. The current deployed host package is
the formal-native-wait-r2 incremental guard build used by these regressions,
not the briefly restored older package. This bounded delivery fixes the
proven wait/shutdown defects but does not close S35 or fix low-DOS startup.

Resolve the reserved-INT15 startup failure without misclassifying it as
default-profile success. Finish the dispatch/caller and negative-case ledger,
repeat/task/worker teardown evidence, and source-manifest/hash review. Test
observer launcher exit is not proof that all worker resources have exited:
observed resident test workers were explicitly identified by parent PID and
terminated between isolated runs. No leak-free claim follows from that cleanup.
S35 must not close or advance to S36 on this record alone.
