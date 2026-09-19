# T420 S35 XMS capability investigation

## Status and inputs

### Owner-approved original guest bug disposition

The owner explicitly approved registering the proven low-DOS COMMAND defect
as an original guest bug in TODO, committing/pushing all current changes,
cleaning the worktree and resuming S35 closure review. The debt now links the
direct copy witness and retains the permanent guest-media prohibition.
This supersedes the historical "awaiting disposition" statements below;
it does not convert the failing profile to a pass or authorize a workaround.
The final requirement review below completes S35 with this explicit debt;
it does not close T420 or claim full DOS/WOW compatibility.

### Final S35 closure review

| Requirement | Verified disposition |
| --- | --- |
| Selected package and original ownership | Eight-member manifest, six compiled original XMS C units; seven manifest files now byte-exact. xmsblock.c retains only its registered bounded descriptor/copy seam. The RISC backend is not a selected executable provider. |
| Dispatch coverage | Twelve-entry original caller ledger retained below. xmsSysPageSize has no selected original guest caller and is not claimed executed; public test results are not represented as independent per-BOP traces. |
| XMS data and failure behavior | `s35-header-xms-r1` passes all four guest routes, including moves, forward overlap, growth/shrink, same-size, failed growth preserving data, locked rejection, lock/unlock, free/reuse and restored total capacity. |
| A20, UMB and INT15 | Existing boot/low-environment witnesses prove actual A20 alias transitions and restoration, boot UMB allocation/release/errors, default zero and reserved 128KB INT15. Ordinary-environment low-DOS startup remains the owner-approved original guest bug in TODO, not a pass. |
| Lifetime | Early CONFIG fault naturally exits after the pre-resume guard; same live worker completes two XMS tasks; broker loss ends the worker. Isolation cleanup is not natural-exit evidence and resident original command waits are not leaks. |
| Formal product | Nine direct header consumers recompiled, 15-edge x86 rebuild and VdmTib ownership passed; five deployed artifacts hash-match this build. All 17 `s35-header-product-r1` transcript-gated regressions pass. |
| Minimal differences | Remove the 58-line native-wait translator in favor of the original NT call; retain the bounded pre-start join guard and same-owner PIF lifetime correction. Retire the header parameter-name-only DIV-198. No new production overlay, guest patch or CPU semantic change. |

S35 is closed with the explicit original guest bug debt. S36 owns the next
suballocator capability review, including its complete relocation/fragmentation
and DPMI-backed workload; that future work is not claimed complete here.
The chronological pending statements below document earlier investigation
states and are superseded by this closure review.

The selected machine is MSVC x86 CCPU40; guest HIMEM is unchanged.
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

### Header-only difference retirement

The final mirror review retires DIV-198: swapping Source/Destination names
in a prototype whose parameters are both ULONG cannot change the C type,
call ABI or runtime semantics. Earlier S15/S35 statements describing an ABI
repair are superseded by this correction. Actual callback definitions and
suballocator calls are unchanged. xms.h now matches pinned OpenNT bytewise,
SHA-256 `0fad52bf1e4c6db87bdc77cf9ec2f35540da2fc4972d2b32e32d57a46776a9fc`.
Seven of the eight XMS manifest files are byte-exact; only xmsblock.c retains
its registered descriptor/range lease boundary. Four explanatory lines and
the parameter-name swap are removed; no runtime implementation is removed
or added. The upstream newline bytes are restored too, so raw Git line counts
include formatting churn and must not be presented as executable-line savings.

All nine selected direct consumers of xms.h were explicitly cleaned within
the formal build root and recompiled; the 15-edge x86 rebuild and VdmTib
ownership gate passed. The five formal host artifacts were deployed to
O:\winnt with exact source/destination hash checks; original guest media were
not changed. Verification prefixes are `s35-header-xms-r1` and
`s35-header-product-r1`; all four XMS routes and all 17 product routes passed.

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

### Post-diagnosis read-only acceptance audit

After b3ff23738, deployed ntvdm.exe still matches formal-native-wait-r2
(`c9004da5...2457cc6f`); deployed COMMAND.COM and HIMEM.SYS still match the
full original hashes above. The eight-file pinned OpenNT comparison again
finds exactly six byte-identical files and the two registered xms.h/xmsblock.c
differences. No diagnostic host binary remains deployed.

The current real guest test verifies growth but not shrink, failed growth
preserving the old block/data, locked-block reallocation rejection, or the
same-size no-op. These are S35 public XMS capability obligations, even though
S36 separately owns the underlying allocator; do not defer them merely because
the general product regression passes. Original himem4.asm::ReallocExtMemory
validates the handle and lock count, then returns via REMExit when the requested
size equals the current size, before XMS_REALLOCBLOCK. Consequently the
uninitialized NewAddress expression in the host's equal-size branch is not
shown reachable through this selected original caller, and is not an admitted
semantic repair. A real public same-size test must verify the guest no-op.
These remaining tests can proceed independently of owner disposition for the
immutable low-DOS COMMAND limitation; no S35 closure is claimed.

The subsequent independent test expansion now closes those four public cases.
`guest-realloc-r1`, built from xms_capability.asm with the existing NASM/MSVC
fixture builder, passes all four real product routes under `s35-realloc-r1`.
It requests the same 128KB size, rejects growth to FFFF KB with A0 while
querying the retained 128KB block, shrinks to 32KB and reads back the original
eight bytes, then rejects locked-block growth with AB. The existing unlock,
free/reuse and initial-total-free equality checks also pass. This proves the
public caller path, not an independent assertion that physical relocation
occurred or every allocator fragmentation branch ran. No product source or
original guest media changed; the low-DOS limitation still needs disposition.

### Direct environment-copy witness

Post-diagnosis formal verification at 8080a1e7c: the five explicit x86 targets
build successfully, with ntvdm.exe and VDMREDIR.dll relinked and the VdmTib
single-owner gate passing. The deployed package passes all 17 established
transcript-gated routes under `s35-final-product-r1`, including EDIT return,
nested COMMAND/MEM, repeat execution, native streams and guest exit status.
Run16, BaseSrv and DTMgr are byte-identical to the formal outputs. Worker and
VDMREDIR differ by four bytes each, confined to the COFF timestamp and debug
directory timestamp: worker offsets 280/281 and 2611636/2611637; DLL offsets
296/297 and 170724/170725. Parsing their PE directories independently places
the debug-directory timestamp at 2611636 and 170724 respectively. No code,
data, import or export content differs. The deployed historical binary hashes
remain unchanged; this is not a claim that the relinked hashes are identical.
This verification does not accept the ordinary-environment low-DOS failure.

`tests/observation/command_environment_trace.c` includes the unchanged current
cmdenv translation unit under a renamed function, then surrounds that function
with read-only guest leases. The formal graph never selects this wrapper.
`Build-T420S35EnvironmentTrace.ps1` retains the formal x86 compile flags and
worker link inputs, hashes all reused inputs, and emits a separate diagnostic
object/archive/EXE below `build/M0-T420/S35/environment-trace-r1`. No debugger
is attached. The wrapper neither changes guest bytes nor reads environment
string contents into the log; normal original environment writes still occur.

Diagnostic EXE SHA-256:
`fb14e1a586d2cbdcbc17ec596322b56c8634a9d1f3042a6a6715b012857757df`.
With hash-checked guest-a20-low-r1 disposable inputs and the ordinary inherited
environment, `s35-envtrace-r1.events.txt` records:

| Boundary | DS | ES | BX | EnvSiz | MCB owner / paragraphs | EnvSiz inside environment allocation |
| --- | --- | --- | --- | --- | --- | --- |
| First entry | 0E2F | 0F5D | 0010 | 0010 | 0E2F / 0010 | No |
| First return | 0E2F | 0F5D | 0118 | 0010 | 0E2F / 0010 | No |
| Second entry | 0E2F | 0F5D | 0118 | 0118 | 0E2F / 0118 | Yes |
| Second return | 0E2F | 0F5D | 0118 | 4543 | 0E2F / 0118 | Yes |

The resized allocation spans linear `[0F5D0,10750)`; EnvSiz is at `1032C`.
The first call only reports capacity. Before the second call, original guest
EndInit has reallocated and stored 0118 in EnvSiz. The ordinary, in-bounds
environment copy then overwrites that stale INIT variable. On return the
original `cmp bx, EnvSiz` compares 0118 with 4543 and takes the mismatch/error
path into the previously evidenced discarded Alloc_error location. This
directly corroborates the original source lifetime defect; it is not an XMS
allocator failure, an out-of-range host write, or evidence for altering CCPU.
The run times out; this is defect evidence, not product acceptance.

Only the test launcher's confirmed children were terminated. The first restore
attempt raced process image unlocking and failed; the subsequent empty process
query and successful copy restored the formal EXE, SHA-256
`c9004da5219ecd4de8b640e75420e3e29fe027684411f2fd45b2f8be2457cc6f`.
Default guest-a20-r2 test inputs were restored. No original guest media changed.
The restored formal package then passed all four transcript-gated XMS routes
with prefix `s35-post-trace-control-r1` (direct/profile/nested/repeat).
Repairing the stale guest references would violate the owner's permanent
guest-media prohibition. Do not preserve discarded guest memory through a
host allocator special case, truncate the environment, inject /E:, or force
DOS=HIGH and call that original-semantic restoration. S35 remains open pending
an explicit disposition of this demonstrated immutable-guest limitation.

### Current disposition after 144b050e9

Lifecycle measurement is now separate from harness cleanup. The four-route
gate records both immediate post-launcher processes and processes after a
bounded five-second wait on each confirmed live test worker. All four
`s35-natural-lifetime-r2` rows retain a worker and broker before isolation
cleanup. This is not reported as natural worker exit. A separate direct run
(`s35-idle-worker-r1`, launcher 16052, worker 5088) reaches the expected XMS
marker and its exact-map stack is `MS_bop_4 -> CmdDispatch -> cmdGetNextCmd`
at the native wait. It is not the previously fixed event-thread join.
Original cmdGetNextCmd distinguishes nonzero DosSessionId termination from
the existing-Console GetNextVDMCommand route; this source distinction must
remain intact. Runtime reuse/termination acceptance must follow that ownership,
not introduce an idle timeout or call harness termination a successful exit.
The test-owned process group was cleaned after the snapshot. The r1 JSON
used insufficient serialization depth; r2 preserves structured process rows.

The follow-up `tests/observation/xms_worker_reuse.c` was compiled with MSVC
Win32/x86 `/MT /W4` into `build/M0-T420/S35/diagnostics-r1/reuse.exe`.
The existing Console observer ran it with package root `O:\winnt` and report
`s35-reuse-r1.txt`; the probe itself wrote `s35-reuse-r1.reuse.txt`.
It launches `run16 tests\X35.COM` twice in the same Console, resolves the
attached worker by its full image path and retains a process handle across
the calls to exclude PID recycling. Launchers 2660 and 3180 both completed
against live worker 53792. The Console transcript contains exactly two
`S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK` markers followed by
`S35_SAME_LIVE_WORKER_REUSED_OK`; observer exit is zero. This proves actual
task reuse, not two independent worker starts. Each guest invocation also
checks restoration of its initial total free XMS capacity.

After the observer exited, a fresh process query still found only broker
42364 and worker 53792, both parented by first launcher 2660 and with exact
package image paths. After rechecking those identities, the test terminated
only broker 42364. A retained worker handle signaled within the 15-second
observation bound; a fresh package process query was empty. The PowerShell
process object's exit-code property was unavailable, so no exit-code claim
is made. This is broker-loss lifecycle evidence, not graceful Console-close
or internal destructor/lease instrumentation. Original guest media and
product code were unchanged. Ordinary-environment low-DOS startup remains
unaccepted; successful reuse does not close that separate failure.

Earlier paragraphs below retain chronological failed attempts and pending
states; they are not the current deployment or delivery claim. Native wait
restoration and pre-resume cleanup shipped at 76ec920d4 with formal x86,
17 product routes, four XMS routes and a deterministic early-fault witness.
144b050e9 adds verified free-capacity restoration and actual A20 OFF/ON/OFF
alias comparison. The original-guest immutability rule shipped at 466c71f09.
No outbound approval block remains after the owner's renewed authorization.
S35 is still open: ordinary-environment low-DOS startup is not accepted, and
the final source/hash and lifecycle requirement audit must precede closure.

The additional startup-contract audit compares pinned OpenNT `cmdconf.c`
and COMMAND `init.asm`: init.asm has zero normalized difference; cmdconf.c
changes only the system-root binding, not shell/environment policy.
Original ExpandConfigFiles preserves an explicit SHELL `/E:` argument but
does not automatically size it from the host environment. Original
COMMAND Init chooses its initial allocation from EnvSiz/UsedEnv; the later
permanent-COMMAND GetInitEnvironment expansion remains in EndInit after
resident shrink. Thus no omitted original host pre-sizing step was found
in these owners. Automatically inserting `/E:`, dropping environment entries,
or forcing DOS=HIGH is not admitted as an equivalent repair. The observed
stale INIT reference remains a guest-lifetime limitation under investigation,
not evidence for changing CCPU or weakening immutable-media policy.

The next upstream check includes the actual first-shell launcher, not the
unrelated CONFIG INSTALL launcher: `doskrnl/bios/sysinit1.asm` declares
`comexe exec0 <0,command_line,default_drive,zero>` at line 718 and passes
that block to INT 21h EXEC at lines 1613--1620. Its normalized diff against
pinned OpenNT is empty. Thus a zero explicit environment segment at first
shell startup is original, not evidence that the standalone host omitted a
required pre-sized environment. COMMAND `init.asm` lines 506--526 either
uses the PSP environment or builds its temporary COMSPEC/PATH environment;
lines 1285--1295 size the initial allocation from EnvSiz and UsedEnv.

The matching original map locates INIT at 1660--248F, EnvSiz at 203C and
Alloc_error at 1F8D. `Setup_res_end` retains resident data plus code for a
first low COMMAND, excluding INIT; `EndInit` explicitly saves initial sizing
values before SETBLOCK, but the NTVDM-specific GETINITENVIRONMENT retry
still reads and writes EnvSiz afterward. This is a concrete source-lifetime
hazard. The existing fault snapshot proves those locations contain host
environment bytes at failure, but a complete allocation/write timeline is
still needed to prove the exact transition independently of the final
snapshot. No environment injection, allocator policy change or guest patch
is justified by this static check alone.

A host-only hardware-breakpoint experiment did not supply the missing write
timeline. `s35-envwatch-r1` and `s35-envwatch-r2` launched the product under
Windows DEBUG_PROCESS, without writing guest memory. The second observer
covered new worker threads as well as the initial thread. It records worker
29296 exiting with 120, launcher 15848 with 1067, and broker 19360 with zero,
before any cmdGetInitEnvironment observation. Current
`ntvdm-exe/debugger/source/dbg_init.c` detects ProcessDebugPort, and
`dbg_unavailable.c` terminates a debugged session with
ERROR_CALL_NOT_IMPLEMENTED on reached private VDM notifications. This is
consistent with an unsupported debugger path, not an environment result;
the exact terminating notification was not captured. The exploratory
observer source was removed; binaries/logs remain disposable evidence.
No debugger-policy bypass or original guest-media change was retained.
After restoring hash-checked guest-a20-r2 disposable test inputs, the ordinary
non-debug four-route gate passed with prefix `s35-post-debug-control-r1`:
direct, profile, nested and repeat all retained their required guest markers.
This controls for test contamination, not low-DOS repair. Product source and
deployed host binaries remained unchanged throughout the experiment.

### Original dispatch/caller ledger

The selected `xmsdisp.c::apfnXMSSvc` contains twelve entries. The following
ledger distinguishes public guest witnesses from per-entry trace coverage;
a passing public function is not represented as an independently logged BOP.
Caller paths below are relative to `src/mvdm/dos/v86/dev/himem`.

| Host provider | Original guest caller | Current evidence/disposition |
| --- | --- | --- |
| xmsA20 | himem1.asm, himem2.asm | Public local enable/query/disable passes. Actual alias-address wrap and full initial-state restoration still need an explicit witness. |
| xmsMoveBlock | himem5.asm | Conventional-to-XMS and reverse data comparison, forward overlap and odd-length rejection pass. |
| xmsAllocBlock | himem4.asm | Allocation, excessive-size rejection and post-free reuse pass. |
| xmsFreeBlock | himem4.asm | Release and repeated-free rejection pass. |
| xmsSysPageSize | No caller found in selected MVDM text | Full-tree symbol search finds declaration, definition and table slot only; no original guest XMSSVC invocation found. Do not invent a caller or count this as executed. |
| xmsQueryExtMem | himem2.asm initialization | Source-connected to original HIMEM boot; public presence/free-memory witnesses pass, but no independent entry trace is claimed. |
| xmsInitUMB | himem2.asm initialization | Source-connected to boot UMB setup; boot-driver allocation/release witness passes. |
| xmsRequestUMB | himem4.asm | Boot oversized request, successful allocation, and post-boot no-free-block result pass. |
| xmsReleaseUMB | himem4.asm | Boot release and double-release rejection pass. |
| xmsNotifyHookI15 | himem.asm initialization | Source-connected to UpdateKbdInt15; default AH=88 and reserved 128KB control pass, without claiming an independent BOP trace. |
| xmsQueryFreeExtMem | himem4.asm | Public largest/total query returns nonzero; exact free-total restoration across repeated allocations remains to be asserted. |
| xmsReallocBlock | himem4.asm | Grow 64KB to 128KB, query size and verify retained bytes pass. Shrink/relocation/exhaustion details belong in the remaining lifecycle matrix and S36 allocator acceptance, not an assumed pass. |

The existing lock-count query and repeated-unlock tests cover HIMEM-owned
public policy; lock/unlock are not extra host dispatch slots. The selected
eight-source manifest and the outgoing bounded commit/decommit/move bindings
remain distinct from the twelve-entry guest-service table. Remaining S35
verification must include real A20 state effects and resource restoration,
not only table inclusion or host-only fixture results.

The free-total gap now has an explicit public witness: the guest stores the
initial XMS AH=08 DX total, then asserts equality after allocate/move/grow,
lock/unlock, free, error cases and a second allocate/free. New fixture
`guest-free-total-r1` passed all four direct/profile/nested/repeat routes
under prefix `s35-free-total-r1` on the deployed cleanup candidate. This
proves restoration of the test-owned XMS capacity, not all worker resources
or host-memory lease counters. No product or original guest-media change.

The boot driver now adds a read-only A20 address comparison: with interrupts
saved/disabled, compare 256 bytes at 0000:0000 and FFFF:0010, bracketed by
public HIMEM local-enable/local-disable calls, and verify the initial A20
query state is restored before restoring flags. It never writes IVT/HMA.
`guest-a20-r2`, prefix `s35-a20-r2`, passes all four XMS routes. Its profile
emits `S35_XMS_BOOT_A20_ALIAS_STATE_RESTORED_OK`, not the distinct
`S35_XMS_BOOT_A20_OFF_ON_OFF_ALIAS_OK` marker. Therefore this configuration
starts with A20 already enabled: non-aliasing and nested state preservation
are proven, while actual disabled wrap remains unverified. Do not force away
the original enable-count ownership or claim that this run tested OFF/ON/OFF.

The subsequent low-DOS control closes the physical-wrap witness:
`guest-a20-low-r1` is built with `-LowDos`, and
`s35-a20-low-r1.txt.console.txt` emits
`S35_XMS_BOOT_A20_OFF_ON_OFF_ALIAS_OK`, the UMB boot marker, default INT15
zero and the XMS core marker; launcher 28248 exits 0. This uses only a short
child-process environment to isolate the already known COMMAND startup
failure. It is a real A20 capability witness, not a product environment
workaround or acceptance of ordinary-environment low-DOS startup. The fixture
uses public balanced local-enable/disable calls and restores flags/state;
no internal guest counter, original binary or IVT/HMA contents are patched.
Owned test processes were cleaned and default test fixtures restored.

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
