# T436 S1 isolated Windows 3.1 launch profile

## Owner-accepted limited conclusion

On2026-10-07 the owner states: “接受不稳定运行；准入S2鼠标驱动，和之后的S3安装程序补丁。”
S1 is accepted as limited research, not stable enhanced-mode compatibility.
The selected recoverable copy uses the checked ten-byte SFT discovery
substitution (C67E667E…); actual /3 desktop and Notepad were observed, while
independent timeouts and unknown remaining startup cause are retained below.
No normal-exit, mouse or general-app capability is inferred. Original NT DOS,
original standard installation and default profiles are unchanged. S2 now
owns mouse implementation; S3 owns the self-contained installer patches.

Owner follow-up authorizes an isolated O:/win31/PATCH launch profile before
continuing feasibility research. Existing START31.CMD/WIN31.PIF/PROFILE inputs
already specify standard mode /S, HIMEM, DOSONLY and no NT/WOW DOSX preload.
Existing PROFILE README records a prior experimentally modified KRNL386 SFT
probe, 640x480 Program Manager observation and possible invalid-handle exit.
This is not a pristine-original Windows compatibility baseline.

Current KRNL386 SHA256:
88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181.
No guest byte, original launcher/PIF/PROFILE or default product NT config is
changed. The new src/addon/win31-launch generator takes explicit InstallRoot
and OutputDirectory, preserving reference PIF extensions and setting /S in
both argument fields. Installed PIF references installed PATCH config, not
build staging; win.cmd resolves run16 via PATH and uses its own directory.
AUTOEXEC does not preload product DOSX; Windows retains its own SYSTEM/DOSX.
No drive mapping, helper or host/wire change is introduced.

r001-profile stages four files and records six reference hashes before work;
the owner-authorized PATCH directory receives exact staged copies.
tests/component-integration/win31_launch_profile_test.ps1 with InstallRoot,
ProfileDirectory and BeforeHashes verifies target/CWD, both /S arguments,
NT profile paths, checksum, environment, no DOSX preload, PATH launcher and
unchanged guest/default-profile hashes. Published-profile API checks pass.
No Windows runtime launch is performed; actual startup/exit remains owner
trial, and historical mode/mouse/exit limits are not assumed fixed.

## Owner-directed enhanced-mode trial

Generator supports explicit Enhanced mode (/3 in both argument fields), with
Standard /S remaining the default. r002 publishes a separate WIN386.PIF only;
the installed standard PIF/profiles remain unchanged. First direct hidden trial
hits an environment-setup dialog, not enhanced-mode success. A private-desktop
probe refuses missing context; it is not evidence for the actual display.
After exact pinned trial cleanup, r003 retries through ordinary CMD Console.
It initially has a live worker; later all trial processes are gone, and old
BOOTLOG has no new timestamp. Owner reports COM1/COM2 prompts, Ignore and
eventual failed exit. No enhanced desktop or actual final exit code is captured.
Cause remains unresolved; serial mouse attribution is not established.
Enhanced PIF/config checks pass with the r001 before-hash manifest; guest core
and default profiles remain unchanged. No driver/guest/runtime fix is made.
inspect-session-windows.ps1 records window text for explicit PIDs only, with
its generated assembly under build. It is observation, not launch acceptance.

## Enhanced DOSMGR failure confirmed and normal configuration attempts

r006 private-desktop observation records COM1/2/4 opens failing; selecting
Ignore only on these exact owned warnings reaches natural launcher exit255.
Console prints ERROR: Unsupported MS-DOS version. r005's COM4 wait is a timeout,
not equivalent successful termination. The observer pins descendants and cleans
only those process objects; no name-based kill or drive mapping is performed.

inspect-win386-dosmgr.py extracts the read-only supplied W3 bundle's DOSMGR LE
objects, page mappings and internal fixups. WIN386 SHA256 is
6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5.
r007 extraction passes; r008 rejects a previously unhandled chained fixup;
r009 handles that source-list record and maps error references. DOSMGR object2
offset185E opens five CON handles, scans, retries another five, measures equal
spacing, and sets the file-entry-size variable at3A9C. Offset190E goes to19C2
after unsuccessful repeated scanning. At19C7, ten successful opens selects the
unsupported-version diagnostic at8410 rather than insufficient-handles at8435.
Other code-signature/version failures can also use that diagnostic; it is not
solely an AH30 version-number test. NT DOS sf.inc has a33-byte sf_entry ending
in sf_NTHandle and no sf_name: the retail filename-spacing probe is incompatible.

r022 runs independently authored CCPU40 TSR CON tracing followed by VER:
attempts/success/failure/last=0000/0000/0000/0000; DOS reports5.00.500.
r023 brackets actual WIN.COM /3 with the same TSR: Unsupported MS-DOS version,
WIN_FAILED, attempts/success/failure/last=000A/000A/0000/000F and TRACE_END.
This is runtime confirmation of the ten-CON probe failing, not a shortage of
file handles or a low version. Hook chains preserve original calls/registers/
flags; the observation TSR makes no original guest binary or memory-code patch.
Assembly first fails on a relocatable paragraph expression in r020, corrected
in r021; the failed build is not a passing probe. Test artifacts reside in
O:/winnt/tests only. Both baseline and enhanced batch complete and cleanup.

Normal configuration controls use Microsoft Resource Kit Q83435/Q83436:
[A-L](https://jeffpar.github.io/kbarchive/kb/083/Q83435/),
[M-Z](https://jeffpar.github.io/kbarchive/kb/083/Q83436/).
InDOSPolling changes DOS critical-section policy; PerVMFILES0 removes extra
per-VM file-handle allocation; PagingNo disables paging. These are configuration
controls, not SFT-layout conversion or a fake supported DOS version.
r011 clone launch hits the existing long-path defect (Cannot execute WIN.COM,
launcher0); it is not Windows success. Subsequent short-path trials temporarily
apply backed-up SYSTEM.INI and always restore original hash
BC98BD449D70F20B47EBADB9912CF7103D649C5A010E9DF4DE61BC4F0EAD7AE5.
r013 hits denied direct disk access; r015 exposes an observer switch clobbered
by dot-sourced parameters, corrected before r017. Exact disk-warning Ignore
returns the original denied call as failure, never changes disk permissions;
nt_fdisk.c confirms this path. r017 reaches CPWIN386 permanent-swap setup
failure/key wait, then times out; the later pipe error follows observer
teardown and is not evidence of the original guest root cause. r019 removes
legacy permanent-swap settings, disables paging/32-bit disk access and sets
InDOSPolling/PerVMFILES0; it still exits255 with the same DOS diagnostic.
No normal configuration remedy is established. Original INI/default profiles
and all guest executable hashes remain unchanged after the trials.

The previously supplied KRNL386 experiment only adapts standard-mode kernel
SFT probing; it does not change WIN386's separate DOSMGR. Next repair would
require a separately approved recoverable retail DOSMGR compatibility experiment
and its downstream-field audit, not blind removal of the error or changing
the original NT DOS guest. Current S1 does not authorize that binary change.
Goal remains active; enhanced desktop and full compatibility are not achieved.

## Follow-up downstream SFT boundary audit

Automatic goal continuation is not owner approval for binary modification;
S1's no-guest/core-patch boundary is revalidated before further work.
Read-only DOSMGR object2 audit identifies size consumers: offset1822 traverses
the SFT chain using DWORD link at0, WORD count at4, and entries starting at6;
offset184B multiplies the computed entry size by the handle index. These
header fields match OpenNT sf.inc's SFLink/SFCount/SFTable header. Offset131B
uses the same size for per-VM SFT allocation, so the value is operational, not
only an initialization sanity check. This makes blind suppression of the
diagnostic unsafe. Pointer walk and allocation are reviewed, but arbitrary
other register-relative accesses are not classified as SFT fields merely by
their offset; their source provenance/dataflow remains a subsequent audit.
WIN386 and restored SYSTEM.INI hashes still match the recorded inputs.
There is no approved retail binary modification yet. No new runtime trial,
patch, production change or enhancement success is claimed by this review.

## Explicitly approved retail-copy adaptation and actual enhanced desktop

Owner says “批准啊” after the request to adapt DOSMGR only on recoverable
retail copies, retaining original NT DOS and the /S installation. CURRENT and
source policy record this exception. Experimental copy is O:/win31/PATCH/E386;
no original installation executable is replaced. Staging r025 excludes PATCH
to avoid recursive self-copy. Its group paths are later isolated to the copy.

adapt-retail-dosmgr.ps1 pins reviewed WIN386 and NT DOS hashes. It checks the
DOSMGR LE object/page layout, original instruction bytes and absence of source
fixups overlapping the replacement. The original relocated entry-size store
is reused, retaining original client-state save/restore and cleanup. r024
initial patch at object2:1864 reaches the logo but omits original system-VM/
client-register positioning; it is superseded, not the final candidate.
r032 shifts the ten-byte replacement to186D, preserving those operations:
file offset407661, before6A-00-6A-04-CD-20-A9-00-01-00,
afterBA-21-00-00-00-E9-23-01-00-00. Output SHA256
C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8.
All bytes outside that range, including relocation records, are unchanged.
The original SFT size commit/CLC and Pop_Client_State remain. Construction
first failed on a PowerShell array arithmetic expression; correction is a
tool fix, not runtime evidence. Original WIN386 and NTDOS remain unchanged.

r026/r028 initial image passes the old version diagnostic and reaches the
640x480 logo with CR0 paging/protected bits set. It does not reach desktop.
r029 ninety-second observer budget is rejected with68/not-started (the binary
supports at most60 seconds); r030 uses60 and still stays at logo. No timeout
is counted as startup or normal exit. r033 pairs standard VGA with built-in
VDDVGA and adds /B; original Video7 VDD had been left paired with VGA.DRV/3GR.
Microsoft Resource Kit documents this pairing and Q119066 documents /B.

Corrected paged read-only CCPU probe translates high VMM addresses through
actual CR3/PDE/PTE. Non-atomic mixed snapshots are labelled, never assumed
coherent. Ordinary SAS RAM is not VGA device memory, so later inspection also
reads original EGA planes/display globals without altering them. Native AMD64
surface probe compiles against the current kvm_window_frame ABI; the old x86
surface inspector is not reused against the AMD64 frontend. Guest-dump x86
probe uses the exact matching r060 NTVDM map/image and is read-only.

r036 actual frontend pixels show Program Manager; current boot log completes
USER/Program Manager/final initialization. CR0=8000001B and ring-zero flat
code selector28/high VMM EIP establish the forced /3 execution route, not
WOW or standard-mode fallback. The screenshot is surface-3.bmp. This is
startup success in that case, not reliability or normal shutdown acceptance.
r037 independent replay does not reach desktop within60 seconds. Native
timeout stack maps to keyboard_io/idetect/idle_kybd_poll/host_release_timeslice;
source confirms original finite25ms waiting, not an infinite new wait.

r039 adds documented ROM-breakpoint/virtual-HD-IRQ/high-memory configuration
controls on the copy and reaches desktop. The first posted-Alt test fails to
set physical GetKeyState, opening PIF Editor instead of Notepad: input intent
is not credited as Notepad consumption. r040 uses F10, but startup fails;
r041 independently reaches desktop again. r042 clears original fPollingDetect
bit12 only in the experimental PIF; actual _IdleDisabledFromPIF reads1 in r043.
r043 reaches Program Manager, then F10/File/Run opens actual guest Notepad;
surface-5.bmp shows Notepad-[Untitled]. CPU usage rises substantially. r044
independent repeat with identical candidate still fails to reach desktop;
it has high CPU despite idle disabling. Therefore idle detection is not a
proved sole cause or reliable repair. r045 display inspection again observes
graphics mode12, enabled display and valid640x480 surface in its live case.
All successes/failures are retained; no retry-until-pass verdict is formed.

Current result: the requested /3 path can reach real desktop and an application
after the scoped SFT adaptation, but repeated startup remains unstable. The
goal stays active and enhanced-mode delivery is not closed. Normal exit,
physical mouse, paging, virtual DOS tasks and general compatibility remain
unverified. Timeout-induced later pipe errors are teardown observations, not
the original root cause. No CPU/mirror/host/wire change is shipped, and all
experiments preserve original installation/default configuration identities.

## Continued stability and preserving the complete probe lifecycle

Owner extends the goal to S2 mouse/setup work after S1 concludes; startup
instability is not waived and S2 is not yet admitted. Independent TSR logging
records DOS/BIOS output into its own buffer with original vector chaining.
r046 rejects an invalid16-bit SP operand; r047 assembles but its assumed
reference PIF is absent. Partial fixture publication is not a passing build.
r048 verifies only the retained base PIF/VER route. The builder now accepts an
explicit reference; r049 completes all five files before publication.
r050/r052 find the marker but captured text length0; virtualized BIOS may bypass
the vectors, so no-error is not inferred from the empty log. r050 remains at logo.

r051 supersedes the prologue shortcut with a fallback after two exhausted name
scans, preserving original allocation, real CON opens, successful-open count
test, close-all, size commit/CLC and client-state/free cleanup. Only ten actual
successful opens accepts the NT DOS33-byte contract; insufficient handles still
use original failure. Declared LE object2 size3580 becomes3590 inside existing
four pages; checked zero page padding holds a relocation-free ten-byte stub.
The scan conditional's relative DWORD and count conditional displacement change;
page count/maps/file length/relocations and all outside-region bytes are intact.
Input hashes, before bytes and fixup sources are checked. Output SHA256:
380D76D1EAB64CFB9AE8E0B47606AE7A4515CE992CD9D1FD4D8860D57A299EA8.
r052 still times out: improved lifecycle fidelity is not a stability pass.
The recoverable candidate alone is updated; original NT DOS, original /S
installation/defaults and host production code are unchanged. Remaining
startup cause and S2 admission stay open, not marked complete.

## Bounded follow-up: diagnostic package and timer evidence

r053 builds a test-only NTVDM relink which intercepts MvdmWriteConsoleA and
logs text while preserving the original call/result. It is not published to
the product package or assets/release. The isolated RT package's r054/r056/
r057 and explicit existing-PIF replay r058 return161 before an NTVDM worker
is observed. The log is empty. This does not explain enhanced guest startup.
The earlier path-length hypothesis is rejected: MAX_VDM_CFG_LINE is256,
actual paths fit it, and native GetShortPathName succeeds for the existing
PIF, WIN.COM and diagnostic NTVDM. Published/RT run16, NTSRV and NTCON hashes
match. The diagnostic-package failure remains unlocalized; no production
path-length workaround is made.

r059 uses the unchanged published package, the r051 retail adaptation and
the experimental Enhanced PIF; observation is bounded to30 seconds. It
times out after entering paged VMM code. r060 repeats with a60-second budget
and absolute probe paths; it also times out, without a verified desktop or
fresh BOOTLOG completion. The preexisting BOOTLOG is stale and cannot count
as either trial's success. Missing COM1/2/4 warnings are recorded/ignored only
on the owned private desktop; this is not ordinary-startup acceptance.

The read-only page-aware probe now records reviewed retail timer words and
mapped host timer state. During r060, CurrHeartBeat/TimerEventUSec,
LastTimeCounterZero and simulated PIT state change; ticks_blocked remains0
and timer_int_enabled remains1. Thus a wholly stopped host heartbeat/PIT is
not supported. Some retail timer words remain unchanged, but non-atomic
samples and execution addresses do not establish a timer-delivery defect.
ienabled is the original idle detector's flag, not CPU EFLAGS.IF: it must
not be interpreted as proof of disabled guest interrupts. No CPU, timer,
mouse, mirror or production-adapter repair follows from these observations.

Entrypoint: tests/observation/observe-win31-enhanced.ps1, with PackageRoot
O:/winnt, Pif O:/win31/PATCH/E386/PATCH/WIN31.PIF and build roots
build/M0-T436/S1/r059-current-retail and r060-timer-evidence. Both use the
exact published NTVDM map and read-only guest probe; r060 additionally uses
the expanded read-ccpu-paged.py. Only pinned trial processes are cleaned.
Cleanup is not normal Windows shutdown evidence. Originals and default
profiles stay intact. S1 remains active; neither trial authorizes S2 admission.

## Minimal discovery substitution reselected

r061 backs up the r051 candidate before the bounded r062 control switches
only WIN386 to r032's exact bytes. It restores r051 in finally and verifies
its hash. The same published package/PIF/config reaches actual Program
Manager again: r062/surface-3.bmp SHA256
6808C362EE473BB9D3F095F13A1AE7F7469BBB884F824F4E51E54DAECBE1B51B.
This is a positive startup observation, not proof that r051 alone caused
all failures or that independent replays are reliable.

The generator now reselects that smaller ten-byte substitution at407661
(DOSMGR object2:186D). It preserves Push_Client_State, Get_Sys_VM_Handle and
EBP=[EBX+8]. It bypasses the entire unsupported discovery before allocation
or any CON open; there are no acquired probe resources to leak or close.
The temporary allocation field at object2:28E8 is checked initially zero.
MOV EDX,33 / JMP199A reuses the original relocated size store, CLC and
Pop_Client_State. The sole call at12C5 tests carry at12CA and overwrites EAX
at12D0; no success EAX value is needed. Object/page extents, fixups and all
outside-span bytes remain unchanged. The previously expanded object/padding
fallback is retained as r051 evidence, not the selected source path.

r063 reconstructs exact candidate SHA256
C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8,
and copies it only to the recoverable E386 installation. r065 runs
tests/component-integration/win31_dosmgr_adaptation_test.ps1 with explicit
OriginalWin386/NtDos/BuildRoot: PASS exact output, single changed span,
unchanged extent/outside bytes, resource/context manifest and safe rejection
of an already modified input. This is construction/contract evidence only.

r064's independent60-second application-input attempt still fails before
the expected desktop hash, so no Notepad keys are submitted and no input
consumption is credited. r066's30-second diagnostic also times out. Its
opt-in quiesced snapshots briefly suspend only the exact image-checked test
process and resume before evidence formatting/output, with finally recovery
on read errors. They are not timing/non-perturbation evidence. CPU flag
indexes follow original c_reg.h: IF9, IOPL12, VM17. Unlike earlier mixed
snapshots, they establish actual transitions between ring-zero VMM
(CS28/VM0) and V86 NT DOS (VM1/CPL3), with IF both0 and1. A sampled NT DOS
sequence matches immutable NTDOS.SYS offset6853; merely observing it is not
proof of an A20 or DOS defect. Original guest/host execution is not patched.

S1 still lacks repeatable startup and fresh minimal-interaction acceptance.
Owner is asked whether to continue that gate or explicitly accept a limited
research conclusion; no narrower exit contract or S2 admission is presumed.

## Ordinary-launch prerequisite and bounded source comparisons

The normal win.cmd creates PATCH/TEMP before invoking run16. The direct-PIF
observer bypassed that launcher and the E386 PATCH/TEMP directory was absent.
This was a real fixture prerequisite mismatch, not proof of the timeout cause.
r067 creates that installation-local directory, matching the existing wrapper,
then repeats the same candidate/profile for60 seconds. It still times out
before desktop readiness; no application input is posted. Missing TEMP therefore
does not explain the entire failure. The observer now accepts an explicit
RequiredTempDirectory and fails before launching if the declared prerequisite
is absent; it does not create arbitrary directories or alter host environment.

Serial source audit: nt_com.c host_com_open's real-device initialization may
fail after opening COM3 without displaying the missing-port warning, returning
ADAPTER_NULL through host_com_close. Absence of a COM3 warning cannot establish
a usable serial attachment. A later read attempt for the already ended r067
PID fails with87; it is not serial-state evidence and does not restart it.
The diagnostic reader can now inspect the original host_com_ptr array and
the exact x86 HOST_COM handle/type/rx/dcbValid prefix, without guessing stride.
No serial driver or host device configuration is changed.

Microsoft Resource Kit3.1 SYSINI.WRI, preserved in archived Microsoft article
[Q83435](https://jeffpar.github.io/kbarchive/kb/083/Q83435/), distinguishes
COMAutoAssign contention from COMxIrq input disablement. These settings do not
prove a serial defect or authorize claiming a reduced-device profile as full
compatibility. No undocumented serial setting is applied.

clocksSinceCounterUpdate's timestamp update is compared with the existing
O:/repos.hobby/softpc/src/app-softpc/softpc.new/base/system/timer.c implementation:
the complete2089-character function body is identical after newline normalization.
The changing/occasionally noncanonical timestamp samples merit investigation,
but do not prove the startup root cause. There is no matching correction to
adopt from that comparison. Original timer/CPU/mirror code is left unchanged,
following the owner rule; no newly invented timer fix is shipped.
