# T481 S14: Windows 95 launch mode selection

`tools/win95-launch/APPLY.CMD` offers DOSONLY 0 or 1 before making changes.
Mode 0 invokes the existing shared multitask installer; mode 1 sets DOSONLY
and does not install the pair. A launch-owned marker ensures UNAPPLY only
reverses the multitask configuration installed by this invocation. Existing
multitask files or entries are refused before preparation.

The tool reads released materials from `assets/release`, including the new
unchanged SETVER.EXE asset. Its SHA-256 is
`7D8B0765C302358117186D93283AB70D2D0DC72852757AFB4CFEECA69FD74653`,
identical to the previously tracked `tools/win101-setup/SETVER.EXE` and the
installed NT runtime copy. Its addition is a copy, not a guest rebuild.
The selected installed Windows directory remains the source of the existing
identity-bound Win95 patch inputs.

`test/integration/multitask/win95-launch-modes.ps1` passed both mode choices,
released-driver identity, original SYSTEM.INI restoration and protection of
an existing independent multitask payload. The existing Win95 command safety
and failed-prepare rollback cases also passed on isolated fixtures.

The actual launch tool prepared an isolated Win95 copy for mode 0, then
`verify-win95-identity.ps1 -InstallViaLaunch -NativeCommand` observed two
concurrent guest DOS VMs and four actual host native commands, including
guest return and register/stack checks. The original installation hashes were
unchanged. `verify-win95-guest-only.ps1` used the actual launch tool in mode 1,
started Win95 and observed a guest DOS program write its result. Its fixture
then retired the session; it is not evidence of normal Win95 shutdown. Both
checks cleaned their isolated O:/tmp/q95 copy. Both were repeated against
S14 candidate `eee4aa05-2940-47ee-91d1-a34d94f4cd28` and passed; the
same candidate also passed the four Win3.1 mode launches. The first combined
test process completed mode 0 but could not find Python for the next probe
because the first probe cleared its process environment. Mode 1 and Win3.1
were rerun in separate processes and passed.

Cached x86/x64 builds and actual nested link targets passed. The Full
selection passed 16 gates with zero failures; DOS4GW/Nesticle was skipped
because the external fixture was not supplied. The eleven tested candidate
images were published to `assets/release` and `O:/winnt/system32` and
hash-verified. The W31 addon pair matched the released files.

## Owner-reported fresh Win3.1 installation failure

After publication of package `092da729-caf9-4a89-8f2e-305e368faacf`, the
owner reported that running `tools/win31-setup` in a new Windows 3.1
installation succeeded and Program Manager groups were normal. The owner
did not specify the installation directory or DOSONLY choice in that report.
This is manual acceptance of that fresh-install result; the separate automated
original Setup attempt described below remained incomplete.

The owner reports that original graphical Setup completed normally and
returned to the script, but Program Manager was empty. On the specified
`O:/w31` installation, read-only inspection found a zero-byte PROGMAN.INI
and four nonempty MAIN, ACCESSOR, GAMES and STARTUP GRP files. The new
`win31_program_groups_test.ps1` fails against that installation and passes
against the retained `O:/tmp/m481g217` four-group comparison. The point and
root cause of the missing INI entries have not been established. Earlier
startup-only checks did not assert Program Manager group content.

The owner confirmed Setup completed normally and returned to the script.
The S13 script diff (`886dbb0e4..922a6e7b8`) leaves the original
`run16 command /c SETUP.EXE` and its pre-launch environment unchanged;
its mode prompt, driver installation, SYSTEM.INI changes and profile changes
occur after that command returns. At inspection, PROGMAN.INI was created and
last written at 23:11:01, the GRP files at approximately 23:11:03, and
SYSTEM.INI and WIN386.CMD last written around 23:11:16. These timestamps
are consistent with a missing group index before post-Setup configuration,
but do not establish an exact writer or cause. The S13 delivery also rebuilt
the host executables; differences in their hashes cannot be dismissed as
proven timestamp-only differences.

The supplied ewin31 media has neither PROGMAN.INI nor PROGMAN.IN_: SETUP.INF
defines `progman.groups`, and the installed original SETUP.EXE contains the
strings `progman.ini`, `GROUPS` and `progman.groups`. This supports Setup's
involvement but is not an instruction-level proof of the writer. A controlled
post-Setup fixture verifies that both S13 mode choices preserve an existing
PROGMAN.INI byte-for-byte. Two private-desktop attempts reached the real
graphical Setup and returned process exit 0, but their isolated output lacked
WIN.COM and all GRP files; therefore they are incomplete installations and
are not a reproducer or evidence of successful Setup. Their temporary trees
were removed. A preceding bad probe PIF and a large inherited environment
separately caused startup failures; neither is attributed to the user report.

The original SETUP.EXE's NE relocation for segment 5 offset 0x0caa resolves
to KERNEL ordinal 129 (`WritePrivateProfileString`); its arguments include
`Settings`, `Window` and `progman.ini`. Another call site adds the `GROUPS`
atom and posts a USER message. These call sites establish real Setup profile
and message involvement but do not identify the failed group write. On an
isolated copy of the owner's installation, adding references to its four
existing GRP files produced a Program Manager screen showing Main,
Accessories, Games and StartUp. After validating the owner file was still
zero-length and each group file parsed, those same references were written to
`O:/w31/PROGMAN.INI` as local configuration recovery. This recovery does
not establish or fix the underlying fresh-Setup cause.

The actual Win16 profile API was then exercised against an isolated copy:
empty PTEST.INI and PROGMAN.INI writes both returned 0 and left empty files in
DOSONLY 0 and 1, while the same API succeeded on a nonempty INI. Direct Win16
`_lread` of zero bytes returned 0xffff, while `_lwrite` of zero returned 0,
normal four-byte write and read both returned 4. The selected `demRead`
adapter rejected the zero-byte guest-memory lease before original I/O; the
original zero-length read transfers no guest data. The first fix changed
the existing MVDM-HOST-DIV-195 seam; the same real Win3.1 probe returned 0
for zero-byte read, 4 for ordinary read, and 1 for both empty-profile writes;
the resulting files contained their expected group entries. The fixed probe
passed in both DOSONLY modes. This proves the empty-file API defect and its
repair; the isolated full-Setup attempts still did not complete, so they do
not independently prove the original installer's complete call sequence.

Further read-only source tracing found that the original installed SETUP.INF
describes `[progman.groups]` as Program Manager DDE control, rather than
asserting that Setup itself writes each group entry. The installed original
PROGMAN.EXE (SHA-256
`81AB580D6FB9A22D727A58C77125DEB4825186C6656F7368375AD429463D67E4`)
has three NE relocation entries importing KERNEL ordinal 129, identified in
`kernel31/kernel.def` as WritePrivateProfileString. This establishes that
both original Setup and Program Manager use that profile API, and that the
INF names a DDE group-creation path. It still does not prove which of those
call sites writes each `[Groups]` entry or that the empty-read defect was the
sole cause of the owner's empty groups; fresh-install acceptance remains open.

An independent Win16 DDE client has now exercised the real installed Win3.1
Program Manager in an isolated copy with an initially empty PROGMAN.INI. The
same `[CreateGroup(...)]` transaction succeeded and produced a parseable GRP
both before and after the zero-read fix. On the pre-fix host, however, the
actual PROGMAN.INI remained zero bytes; on the fixed host, it contained the
group reference. The fixed result also passed DOSONLY=1, and the Win16
empty-profile/file-I/O witness was rerun. Source installation hashes were
checked and the owned copies removed. This directly reproduces the observed
empty-group symptom on the original Program Manager's DDE path and confirms
that the fix repairs this path; it does not substitute for a complete fresh
original Setup run. A PowerShell null/empty assertion in the prior profile
test was also corrected so an empty file cannot satisfy the check.

The follow-up source review traced the regression to the shared
`mvdm_guest_location_acquire_vm` wrapper introduced by `369cbf712`:
the underlying session lease already supported zero bytes, but the wrapper
rejected them. The mirror `demRead` zero-byte bypass was removed; the shared
wrapper and copy operation again accept zero-byte transfers. The original
`demRead` I/O path and lease are shared by zero and nonzero reads. The same
wrapper is used by FCB I/O. A location fixture checks zero-byte leases, a
zero-byte copy and address overflow; the existing session and redirector
fixtures also passed. The Win3.1 empty-profile probe passed in both DOSONLY
modes against the follow-up candidate. Fresh interactive Setup remains
unverified. The wider mirror-adaptation audit is not complete.

The follow-up candidate passed cached x86/x64 builds, actual NTVDM relink,
the session and redirector fixtures, and all 16 selected Full gates. The
external DOS4GW/Nesticle fixture was not supplied and was skipped. All 11 images
of package `4089b369-3dd9-419c-aee6-382052fee85e` were published and
hash-verified in both release locations; the installed Win3.1 driver pair
was also verified. Package identity stamping changes image hashes from the
raw linker outputs; the staged tested and published hashes match.

A preliminary source census covers 1,060 tracked `mvdm-host` and
`non-mvdm` C/H files. Each T480 source pin was revalidated by SHA-256;
471 files match their selected original byte for byte and 589 differ. This
does not assess the behavior of any difference. The per-file source paths,
hashes and pending review status are recorded in
`artifacts/mvdm-audit/t481-s14-mirror-restoration-inventory.json`.
The DEM read and seek diagnostic registrations DIV-185 and DIV-186 were
still described as active despite removal of their call sites and providers
in `a2ec4d531`; their entries are now explicitly retired.

Review of the adjacent original DEM write path found that the prior adapter
introduced a second `CX=0` buffer path even though the shared lease now accepts
zero bytes. The write path now acquires one read lease for all lengths, follows
the original named-pipe and regular-file selection, and releases the lease on
all exits, including the original zero-length `SetEndOfFile` route. Its mirror
diff against the pinned `demhndl.c` decreased from +115/-42 to +100/-42 lines;
the extra line restores original diagnostic line 308 after lease cleanup.
The real DOS DEM fixture repeated seek-to-two, zero write and end-position
verification 20 times, then completed its original file and FCB checks. A
real DOS VDMREDIR fixture also completed 20 synchronous zero-byte pipe writes,
then an asynchronous four-byte transfer; the peer received the expected bytes.
The existing asynchronous read and NetAPI guest controls passed as well. A
native local message-pipe control accepted zero-byte NULL and non-NULL buffers.
Both DOSONLY
Win3.1 empty-profile probes passed on the first candidate. Its NTVDM image and
the Full candidate differ only in the PE and debug timestamps and the 16-byte
package identity. All 16 selected Full gates passed on the latter; the external
DOS4GW/Nesticle fixture was not supplied and was skipped. After restoring `#line`,
the raw NTVDM rebuild differs from the tested link only in its PE and debug
timestamps. The owner permits skipping repeat tests for this exact case; the
tested 11-image package `b385aa0d-20bc-4de9-ad4e-3372a4e14bfc` was published
and hash-verified in both host locations; the installed W31 addon pair matched.

The published package was subsequently checked again against both actual Win95
launch choices. DOSONLY=0 observed two concurrent DOS VMs, 320 callback
identity/stack/register/flags checks, and four native commands returning to
their guest callers; the original installation was verified unchanged.
DOSONLY=1 ran a guest DOS witness with no native CUI worker, after which the
fixture retired the session; normal Win95 shutdown is not established.
On separate isolated Win3.1 copies, original Program Manager DDE created and
indexed four groups with an initially empty profile in standard and enhanced
modes with both DOSONLY settings. All four Windows sessions exited normally.
This still does not substitute for a complete original interactive Setup run.

Mirror-restoration review also compared `demgset.c` with pinned OpenNT. The
`GetMediaId` padding change (DIV-279) must remain: the original writes spaces
to `VolumeID` while checking `FileSystemType`, and the S37 real INT21/6900
before/after witness records both the incorrect NUL padding and corrected
result. This is a correction in the original owner, not a second provider.
The separate guest-address lifetime changes in this file were not evaluated
by that comparison; this is not a complete disposition of the file.

For DEM open/create, `GetVDMAddr` is not an absent interface: the selected
macro calls `Sim32pGetVDMPointer`, which calls `c_GetLinAdd`. Source tracing
shows that `c_GetLinAdd` returns a pointer to physical address zero when a
paged translation fails. The current bounded pathname lease instead reports
failure. Consequently, the existing seam cannot be removed on the mere
assumption that both interfaces return a host pointer. Whether it is needed
for every reachable DOS pathname and whether its length/failure behavior can
be simplified require a separate caller and address-boundary comparison; no
runtime equivalence or complete DEM review is claimed here.

The already-selected open and create pathname adaptations duplicated the same
DS:SI bounded copy and invalid-address handling. They now share one local
`demCopyFileName` in the original DEM translation unit; their distinct original
handlers, error ordering and leases remain. This reduces that file by eight
net lines in the worktree diff, without claiming restoration of the original
alias contract. The cached x86 and x64 builds and the actual x86 worker relink
passed. All 16 selected Full gates passed (the external DOS4GW fixture was not
supplied to that run). A real Win3.1 enhanced-mode empty-profile probe also
passed with normal exit. Package `092da729-caf9-4a89-8f2e-305e368faacf` was
published and all 11 host-image hashes matched in both release locations; the
installed W31 driver hashes matched. This does not close the original Setup or
whole-mirror audit gaps.

The separate OEM font-create retry fixture was not part of those Full gates.
Its source extractor needed the new shared DEM pathname helper as well as the
original create handler; that test-only integration has now been corrected.
The actual extracted helper and handler pass all five prior font-retry cases
and two invalid-address cases, each rejecting before a host create call. No
production image changed in this follow-up.

The same published package was subsequently exercised through both Win95
launch selections. DOSONLY=0 reached two concurrently active DOS VMs, 320
identity/stack/register/flags checks and four native commands; the shutdown
witness again reached `ExitWindows` but the launcher returned 1 and its
return was not observed. DOSONLY=1 ran an actual guest DOS program without a
native CUI worker; its test fixture retired the session, so it is not evidence
of normal guest shutdown. The source Win95 installation remained unchanged.

The released package `092da729-caf9-4a89-8f2e-305e368faacf` subsequently
passed `verify-win95-editors.ps1` with two concurrent prompt VMs and the
guest USER input, save and read/write witnesses: both editors opened their
menus, saved their distinct modified file contents and closed. The source
Win95 installation hashes remained unchanged. This test synthesizes input
through guest USER; it does not establish the physical RDP mouse path or
normal Win95 shutdown.

After the DEM pathname consolidation, package
`092da729-caf9-4a89-8f2e-305e368faacf` was also tested with the actual
Win3.1 setup-selection script in isolated installation copies. DOSONLY=0
and DOSONLY=1 each produced the expected profiles and driver selection; both
Standard and 386 Enhanced started and exited normally in each mode, and the
four configured program groups parsed. The fixture still substitutes a
successful original Setup invocation, so this is not an automated fresh
Setup result; the owner's separate manual fresh-install report is above.

An additional Win95 DOSONLY=0 shutdown witness reached `ExitWindows` after
both test VMs retired. The observed root Run16 and NTVDM process exit codes
were both 1; NTCON exited 0. Two earlier no-addon controls also returned 1,
so this is not shown to be a new multitask regression. In a further isolated
run, the existing default-off termination observer recorded `TerminateVDM`
return address `005A7E0B` and the pinned NTVDM image base was `00450000`.
The tested image map and disassembly identify RVA `00157E0B` as the return
immediately after `cmdGetNextCmd` copies `VDMInfo.ErrorCode` into
`VdmExitCode` and calls `TerminateVDM`. The original command code populates
that field from the guest COMMAND return record. This establishes the
original host return path in this run; it does not establish why the Win95
guest supplied 1, or that 1 represents normal Windows shutdown. The Win95
identity fixture now explicitly prints the observed root exit code instead
of leaving it implicit; a nonzero exit is not claimed as normal shutdown.

After delivery, the verified external DOS4GW/Nesticle fixture was located
and run separately against that same package. In a reduced inherited
environment, NESticle remained running with a visible graphics window; the
test does not assert gameplay or pixels. With the agent's inherited environment,
the separate test encountered a visible 16-bit subsystem error dialog. A
repeat captured illegal instruction bytes `63 68 61 69 6e`, consistent with
the recorded large-environment failure, but no live environment-block trace
was captured for this run and the cause is not independently proved.
The DDE witness was subsequently expanded to create all four groups named in
the original new-install INF: Main, Accessories, Games and StartUp. The same
four GRP files parsed in all three runs. With the pre-fix host, PROGMAN.INI
remained zero bytes; with the fixed host, all four references were present in
both DOSONLY=0 and DOSONLY=1, and both Windows sessions exited normally.
This is an original Program Manager DDE comparison, not a full Setup rerun.
The witness also sends original Program Manager AddItem transactions for
File Manager, Notepad and Minesweeper. Released GRP.EXE confirmed the
respective executable entries in Main, Accessories and Games in the pre-fix
comparison and both fixed DOSONLY modes; no AddItem was sent for StartUp.
The fixed modes retained all four INI references, while the baseline INI
remained empty. All three test-owned Win3.1 copies were removed afterwards.
The same fixed-host DDE witness also passed standard-mode DOSONLY=0 and
DOSONLY=1, including all four references, three representative program items,
and normal Windows exit. Thus the real Program Manager comparison covers all
four mode combinations; it still does not run original Setup end-to-end.

Review of the retained original-Setup captures narrows why that attempt was
incomplete: copying reached 91%, then Setup displayed “Cannot read from drive
I.” The probe accepted Cancel; Setup continued into its configuration screens
and returned zero, but its installation lacked WIN.COM and group files. Thus
exit 0 is not a completed-install receipt. The rerun must handle the empty
optical drive without canceling a copy operation and assert the installed files
and groups explicitly.

The previous GUI attempts launched a PIF directly rather than the actual
SETUP.CMD. A further isolated test ran the real media PATCH/SETUP.CMD from
host CMD and traced CMD, Run16, NTSRV, NTVDM and NTCON. Original Setup reached
its GUI but remained incomplete after the test controller handled the empty
optical-drive dialog. Run16 reported 0; the real script then rejected the
selected installation directory because WIN.COM was absent, returned 5, and
did not generate the installed PATCH. Its original Setup was not replaced by
a shim. This validates failure handling, not a completed installation. The
isolated media and installation tree were removed afterwards.

The real SETUP.CMD entry was also exercised with the separate test-only
no-I: relink. Setup again returned 0 but left no WIN.COM; SETUP.CMD rejected
the directory and generated no installed PATCH. This relink did not prove
that every guest query saw I: as absent, so the outcome neither isolates the
cause nor establishes production acceptance. The test-owned copy was removed.

The private-desktop controller was also paused on the later configuration
screen. A capture confirms that “let you make the modifications later” was
selected before Enter; subsequent captures nevertheless showed AUTOEXEC and
CONFIG backup dialogs, and the resulting isolated tree remained incomplete.
Thus a selected radio button alone is not proof that the intended Setup path
was accepted; this controller sequence cannot be used as successful Setup
evidence. The diagnostic relink and isolated installation were not published.

A separate test-only NTVDM relink made native opens of `\\??\\I:` return
object-not-found for that process; neither production images nor system drive
assignments were changed. Original Setup still exited without WIN.COM or
groups, although the sampled surfaces did not capture the former error
dialog. This experiment does not establish that the drive was hidden from
every guest query, nor that the `I:` dialog was the sole cause. The test
relink is not suitable for publication or as unmodified-product acceptance.

An isolated-process device-map experiment did not hide `I:`: even after the
process-local map call returned success, a child `dir I:\` still reported the
existing device as not ready. It is not a working isolation method; the system
drive assignment and optical device were not modified.

A separate short-lived test logon was also tried as a DOS-device isolation
boundary. Creating a private shadow for I: failed with Windows error 5; the
original user-session mapping remained `\\Device\\IsoCdRom0`. No real account
credentials or network connection were used. The helper was removed.

A subsequent isolated original-Setup run used the final published candidate
and sent keys intended to deselect printer setup and hard-disk application
scanning in the optional tasks dialog; no post-input screenshot proved both
checkbox states. Setup still displayed the same `Cannot read from drive
I` error during file copying; the test controller selected Cancel and the
result again lacked WIN.COM and program groups despite exit 0. Disabling those
key sequence is therefore insufficient. The owned O:/tmp/qst copy and the
isolated runtime were cleaned after the test.

A further original-Setup run paused after applying those choices: its captured
options screen shows both printer setup and hard-disk application scanning
unchecked. After explicit continuation, copying reached 93% and Setup still
reported `Cannot read from drive I`; the existing controller again selected
Cancel and the incomplete installation had neither WIN.COM nor groups. This
rules out those two optional choices as a sufficient way to avoid this test
failure. The owned temporary copy was removed.

The DOS host drive enumeration does not rely only on GetDriveType: its
`GetPhysicalDriveType` in `dos/dem/demgset.c` opens each drive device and
queries its filesystem device type, and `demGetDrives` records CD-ROM drives
as present. This identifies an actual host-to-guest enumeration path; it does
not by itself establish which Setup operation requested I: or justify a
production change to hide empty CD-ROM drives.

Source tracing additionally found that original `demIoctlChangeable` reports
CD-ROM as fixed for DOS IOCTL 4408, while IOCTL 4409 reports it as remote;
the original guest dispatch forwards both requests to that handler. This
explains a possible distinction an installer could observe, but there is no
evidence yet that Setup calls either function before its SWAPTST5 probe. An
isolated DOS IOCTL witness timed out before producing output; it is not a
passing runtime check and does not justify changing those original semantics.
That attempt had a visible 16-bit subsystem error dialog, so it did not prove
an IOCTL wait. A subsequent independent DOS witness used a short test-stage
path and a minimized process environment: it exited 0 after asserting that
4408 returned AX=1 and 4409 returned DX=0x1000 for I:. The observer captured
its PASS output. This confirms the runtime values but still does not establish
which IOCTLs the original Setup program actually calls.

An independent test-only hook was then positively controlled with a DOS
witness: it observed both IOCTL 4408 and 4409 on I: and their expected
results. During the real SETUP.CMD isolated run, the same hook recorded five
I: 4409 calls returning DX=0x1000, followed by one 4408 call returning AX=1;
the empty-drive error was also captured. The hook was process-wide, so these
records do not identify the precise guest caller, and this run did not log a
SWAPTST5 create through the instrumented NtCreateFile entry. The original
installation remained incomplete and the real script returned 5. This is
evidence that both IOCTLs are reached during installation, not proof that
either caused the error. The diagnostic image was not published.

An additional test-only relink logged only native file opens on I: without
changing their return values. During original Setup it recorded an initial
successful open of `\\??\\I:`, followed by four create attempts on
the `SWAPTST5` probe on the I: drive and repeated opens of the root returning NTSTATUS
`0xc0000013` (no media). The name is consistent with a swap-file probe, but
the exact guest caller has not been identified. This establishes that the
guest did request the empty drive; it does not prove why Setup selected that
drive or that addressing it alone completes installation. The diagnostic
image was not published, and its isolated installation copy was removed.

A follow-up test-only relink attempted to redirect this process's I: native
opens into an owned temporary directory. The original Setup run again lacked
WIN.COM and groups after exit 0. The probe did not independently verify that
all guest drive accesses used the redirect, so this is not evidence that a
valid writable drive was supplied; it is not a product fix or acceptance.

The owner also rejected MULTITASK.HASH, MULTITASK.INI.BAK and
MULTITASK.INI.NEW in the installed PATCH directory. The configuration and
driver hashes were checked against those records and released drivers, then
the three exact records were removed from `O:/w31/PATCH` as requested. A
setup-script change now removes them after verifying the installed drivers
and configuration; its repeat-install and both mode checks passed.

The final cached x86 and x64 build and link completed. Full passed all 16
selected gates with the external DOS4GW/Nesticle fixture not supplied. The
final targeted package passed the empty-profile probe in both DOSONLY modes,
all four Win3.1 mode/startup cases with four real group references (using a
Setup shim), Win95 two-VM native command checks, and a guest-only DOS witness.
The latter retires its own session and is not a normal-shutdown proof. Package
`5478e4fc-e8ce-4dd8-a3cc-075eac4ed50b` was published and all 11 images
hash-verified in both `assets/release` and `O:/winnt/system32`; the installed
W31 addon pair was separately verified. Fresh original graphical Setup remains
an acceptance gap and must not be represented as a successful complete rerun.

The Program Manager group checker now uses released `GRP.EXE show` to validate
each referenced `.GRP`, instead of accepting any existing file. A real empty
StartUp group passed, while a corrupt group and a non-group file were rejected;
all four groups in the owner's recovered O:/w31 also parsed. This improves the
assertion for later runs, but it does not prove an original fresh installation.
The full four-case Win3.1 mode matrix was rerun with this strengthened check:
all four launches and normal exits passed, both copied installations had four
parseable groups, and the owned O:/tmp/q31 copy was cleaned. Original Setup
was still a shim in that particular matrix.

## Win3.1 eight startup cases and no-driver prompt investigation

On isolated copies of the owner's installed Win3.1, both Standard and Enhanced
booted, reported the requested DOSONLY value (0 and 1) and exited normally
with the addon pair configured and with it absent. The runner checked source
file hashes before removing the copies. These eight results cover startup and
shutdown without using a DOS prompt.

A separate Enhanced DOSONLY=1 probe without either addon launched a copy of
DOSPRMPT.PIF via WinExec. With its original COMMAND.COM program field and a
test-only /c echo command it wrote S14_OK to the requested DOS file and
returned normally. In the explicit-path control, using a copy of the package
COMMAND.COM, WinExec also returned 14DE but no output file appeared. Guest
graphics frame 13 showed an invalid-instruction system-integrity error for
MS-DOS Prompt; frame 20 showed ExitWindows refusing to exit while it remained
active. The observer timed out. This establishes an actual guest error under
that control. It does not establish a pathname defect or a mouse callback
cause. The interactive original prompt and post-error mouse remain open.
Task evidence: build/t481-s14/w31-eight-startup and
build/t481-s14/prompt-command-resolution2.

### Win3.1 profile coverage

Eight-prompt-receipts passed the DOS command output, Notepad and zero-exit
checks in all eight cases. Eight-prompt-mouse also required guest-observed
movement, press and release after the DOS command. Both Standard profiles
passed with both DOSONLY values. Enhanced passed both with the addons. Without
them DOSONLY=0 produced command output and reached mouse readiness, but the
frontend probe returned 68 and the launcher exited 0xffffffff. DOSONLY=1 timed
out before the command result and before mouse input. The failure points differ.

In the no-addon Enhanced DOSONLY=1 paired experiment, the stock mouse=*vmd
and the explicit device=*vmd configurations each passed once and timed out
once. The latter's failing frame shows the guest invalid-instruction dialog
before mouse readiness. The final source-inventory check also reported a count
change; original inventory identity was not established by this run. Separate
diagnostic package fault-source/run2 passed once and twice lost the frontend
after command output. Run1 was rejected before guest execution by the manifest
check and supplies no guest evidence.

The common matrix uses a copy of DOSPRMPT.PIF with /c echo. This verifies DOS
command output and desktop recovery but cannot prove which host scheduling
path was used. Existing dual-editor tests exercise guest DOS and PE rejection
with DOSONLY=1, and two host payload chains with DOSONLY=0, under their own
enhanced-mode addon fixtures. S14 still needs per-profile runtime evidence
for synchronous child wait and resumption, independent VM progress with a
host child in flight where supported, and fullscreen presentation handoffs.

Four further production-package trials each started from a freshly generated
no-addon profile copied from the same source. All four enhanced DOSONLY=1
trials produced the DOS receipt, observed mouse movement and press/release
after returning to Notepad, and exited Windows normally. Their source
inventory SHA-256 was identical, as were SYSTEM.INI, WIN.INI and CONFIG.NT
input hashes. See build/t481-s14/fresh-prompt-reproduction. This tests the
PIF /c scenario; it does not establish interactive prompt stability or the
cause of the earlier failures on reused installations.

The same released package was then exercised twice per installation on two
more isolated copies, with pre-boot file inventories. Copy A timed out on
both its first and second boots before the DOS receipt. Its two inventories
were identical. Copy B passed both times, including mouse and exit. Its
DOSAPP.INI and PROGMAN.INI changed on normal shutdown. Thus a second boot
is neither necessary nor sufficient to make this particular probe fail.
The four single-boot plus four paired trials gave six passes and two
timeouts; this is a bounded sample, not a failure-rate estimate. See
build/t481-s14/prompt-second-boot. These runs did not attribute the guest
fault to a component.

Four further independent no-addon enhanced DOSONLY=1 runs retained process
handles through observation. Two timed out with Run16, NTVDM and NTCON all
alive before cleanup. On both passes all three exited with code zero
(build/t481-s14/fault-source/owners).

A separate four-run release-package control placed guest markers immediately
before and after WinExec of DOSCHECK.PIF. Two passed. In fresh4 both markers
existed, but the DOS command receipt was absent and no mouse input had been
sent. The return marker does not record the WinExec return value. In fresh3
the command and Notepad readiness receipts existed. The mouse helper returned
68 with an empty report while NTCON was still alive. In that helper this
meant no matching frontend window was found, with no input delivered. See
build/t481-s14/fault-source/stages. The matrix now distinguishes an empty
window lookup from desktop enumeration and delivery failures. It permits a
new lookup only while no input has been sent, within the existing observer
deadline. A permanently absent frontend still fails the case.

The eight-profile rerun with the updated test helper and the same release
package passed seven rows (build/t481-s14/eight-presentation-ready).
Enhanced DOSONLY=0 without the pair timed out after the Win16 boot marker
and before the DOS command output or the first mouse request. Enhanced
DOSONLY=1 without the pair passed on this run. No product fix follows from
the change to the mouse test helper.

On four more independent release-package boots of enhanced DOSONLY=1
without addons, the PIF WinExec returned 0x14DE every time. With the
inherited observer environment, both runs timed out before command output;
with the Windows-generated logon environment, both passed. The measured
environment entries occupied 3908 and 1931 characters respectively, using
the sum of each name, equals sign, value and terminator. Padding the logon
environment to 3908 using one inert variable passed twice; the interleaved
1931-character controls also passed twice. This rules out a deterministic
threshold at that host environment size. See fault-source/environment-control
and fault-source/environment-padding under the S14 build directory.

An isolated diagnostic used the existing command_environment_trace.c around
cmdGetInitEnvironment. All four diagnostic runs passed (two per environment),
and none of their observed environment allocations overlapped DS:203C.
These samples cannot establish what changed in a failing guest. The original
COMMAND EndInit calls that service on the permanent-shell path; a secondary
shell can take copy_old_env without entering the traced service. The prior
discarded-INIT bug remains unproven for this S14 fault. Diagnostic artifacts
are in fault-source/writer-control and env-writer. Only test copies ran the
diagnostic executable.

The setup matrix also checks the installed [386Enh] entries for each addon,
including its exact installed path, multiplicity, or absence. The ensuing
release-package run passed all eight prompt/Notepad/mouse/exit cases,
including enhanced without drivers for both DOSONLY values
(build/t481-s14/eight-profile-references). It did not run the original
graphical installer again. Dual DOS-window interaction and the fullscreen
presentation transitions remain separate acceptance requirements.

PIF.EXE show on the owner's DOSPRMPT.PIF reports W386.FLAGS=0x21008.
The original pif.h defines 0x8 as fFullScreen and 0x2 as fBackground.
The eight cases therefore exercise a profile requesting fullscreen and
foreground scheduling. They do not establish the actual visible mode of
each frame. In a four-run isolated enhanced DOSONLY=1 no-addon control,
a direct DOS .COM writing the same receipt via INT 21h passed twice; the
original shell running /c echo passed once and timed out once
(fault-source/direct-dos-control). These counts do not isolate a shell bug.

With the original shell and arguments held fixed, changing only fFullScreen
on a test PIF produced two successful windowed-profile runs. Both fullscreen
controls timed out before DOS command output (fault-source/display-control).
Other fullscreen runs have passed. This comparison identifies a useful
trigger, without establishing a deterministic failure or its cause.

Two independent no-addon enhanced DOSONLY=1 trials launched distinct
windowed COMMAND prompts with background execution enabled. Each launched a
DOS .COM which wrote its own marker and could finish only after reading the
other's. The guest controller started B after observing A's marker, and
required A.END and B.END before exercising Notepad mouse input and requesting
Windows exit. Both passed (fault-source/windowed-dos-pair). This proves
overlapping guest execution; keyboard focus and interactive fullscreen
switching are separate properties.

The checked-in test sources were rerun together in eight-plus-guest-pair:
seven of eight single-prompt cases passed. The no-addon enhanced DOSONLY=1
case timed out after the Win16 boot marker and before the command receipt.
Both additional windowed guest-pair cases passed (addons configured and
absent). A.RUN, B.RUN, A.END and B.END were collected independently for each
profile, followed by the Notepad mouse and normal Windows-exit witnesses.
The combined runner correctly returned failure for the single-prompt timeout.

Two isolated host-only diagnostics then observed original DEM file operations.
The open-only instrument passed four times (dem-prompt/results). With both
open and read observation and the original fullscreen PIF flags throughout,
two runs passed and two timed out (dem-prompt-read/results). After the guest
WinExec return marker, all four searched for COMMAND.COM and recorded a
successful open. On both passes a second successful open was followed by
successful reads of 62 bytes at offset 0 and 50384 bytes at offset 62, then
creation of EXEC.TXT. Both failures stopped after the first successful open.
No second open, tracked shell-image read or output file was observed. This
distinguishes the failures from shell search exhaustion and from a lost
mouse event following command completion. The read probe follows the most
recent COMMAND.COM handle opened through DEM; it cannot rule out accesses
through another interface. Neither diagnostic was published.

In a separate four-run diagnostic, fault recording was enabled at the
controller's PIF-launch marker and disabled upon a successful open/create
of EXEC.TXT. Three runs passed. Their recorded invalid-opcode traps in this
interval were on opcode 63h in V86 mode. The timeout in prompt-exception/
results/fresh2 again lacked the second shell open, and recorded an additional
V86 #UD at 01B7:033D, with preceding instructions decoding a long series of
zero bytes. This provides a failing guest execution trace before the command
receipt. The trapping address does not identify who transferred control there.

The prompt-edge diagnostic retained changes of CS/VM execution preceding
each distinct non-ARPL invalid opcode. In two failing runs (second/fresh3
and fresh4) the chain entered the BIOS mouse IRQ at F000:EE00, executed the
BIOS OUTB trampoline at F000:3050 for the PIC acknowledgements, entered
FEFE:0B37 (ARPL), then returned from the guest VMM to 01B7:0228. Execution
there reached the invalid opcode. The source matches
mouse_io.c::jump_to_user_subroutine followed by the two outb calls in
mouse_int1 and the original guest BIOS OUTB trampoline. The eight edge
runs included six passes; no production CPU behavior was changed.

Independent registration/dispatch observation (prompt-mouse-owner/second)
confirmed two failures with the same installed INT 33h callback FEFE:0B37.
The failing dispatch had condition mask 1 (movement), SS:SP=00A7:0792.
The earlier desktop dispatch of the same callback had SS:SP=0E29:0236.
Both reported CR3=1DC000, IVT physical address 0 and callback physical
address FFB17. Those values cannot distinguish the two contexts in these
samples. Stack addresses are diagnostic evidence and do not identify a VM.
MOUSE31.DRV obtains the callback using DPMI 0303 and installs it using
INT 33h function 0Ch. Without NIDDB activation virtual_swap_instance
returns before selecting an instance. This accounts for how one stored
callback can still be selected during a DOS-prompt transition.

In an isolated causal control, the original IRQ, counters and EOI were
retained. A test flag allowed withholding only movement callbacks between
PIF launch and first DOS output. All four boots passed. One flagged boot
actually withheld a dispatch to FEFE:0B37 with SS=00A7; both later guest
mouse phases and normal exit passed. The other flagged boot saw no callback
to withhold. Both unflagged boots passed on this occasion. This control is
in prompt-mouse-control/results. It supports investigating callback ownership;
dropping movement during startup would change input semantics and has not
been installed or proposed as the fix.

Four further no-addon fullscreen runs instrumented the original INT33
dispatcher and enhanced mouse BOP in prompt-mouse-functions/results. All
four completed. Each trace contains exchange 14h, registration 0Ch with
mask 1Fh, desktop button down/up and final 0Ch removal. None reached
15h/16h/17h or mouse_EM_callback. The original MOUSE_CONTEXT includes
the callback address and mask; changing its serializer would not affect
these four boots.

An independent DOS child probe in prompt-child-context/results queried
INT2F/1683h for the executing VM ID and INT33/24h for the mouse version.
The latter is a read-only provider query. The first boot reproduced the
fullscreen failure before this program ran; the dispatch log again shows
a movement callback on SS=00A7. The second boot ran the program and shut
down normally. VMCTX.BIN was 02 00 00 08 02 03; the host recorded VM ID 2
from the marked query and found FEFE:0B37 still selected, matching the
desktop callback registered earlier in that same run. Thus the stale
registration is directly observable even on a successful boot, independent
of the timing of mouse motion. Neither test changed the installed system.

With both addons installed and DOSONLY still asserted by the boot witness,
the same VM 2 query found 0000:0000 twice. Both controls completed. On the
published-code reference, eight-distinct-vms passed all eight configuration
cases plus the two DOSONLY=1 prompt-pair cases. Each pair recorded VM IDs
2 and 3 and completed the mutual wait. The private desktop sent the mouse
input; Setup's post-install stage used its existing shim.

An addon-only experiment left the desktop callback out of INT33 and chained
the protected IRQ9 handler before querying the original provider's position
and buttons. In both prompt-pm-input boots the DOS child reported success
and the host found a null callback for VM 2. One completed all mouse and exit
checks; the other accepted frontend input but did not record the guest's
mouse-down marker. Two additional prompt-pm-samples boots recorded IRQ,
position query, press, release and exit. This proves a usable protected IRQ
path, but not reliable delivery on every transition; the experiment is
confined to build-owned test copies.

Two alias controls in prompt-rm-vector changed only the representation of
the system VM's real IRQ9 address (same linear address). The Win16 client
read back offset EE10; DOS VM 2 still reported F000:EE00 in both runs.
A real-vector-only driver in prompt-rm-input nevertheless missed the desktop
press in both boots. Both reached the DOS command receipt. In both traces
the original IRQ provider processed the press and the experimental handler
never queried position. An isolated IVT entry alone is insufficient.

The prompt-pm-scoped experiment instead exchanges the INT33 callback only
while chaining the system VM's protected IRQ, then restores the interrupted
registration. The original provider computes the event mask and handles EOI.
Two no-addon DOSONLY=1 boots completed the DOS command, Notepad movement,
button-down/up and normal Windows exit. Both DOS children reported VM 2 and
the untouched F000:EE00 IRQ vector; the host saw a null callback during the
child's version query. These are experimental results, not release acceptance.

The experimental IRQ scope subsequently passed eight launch/mouse/exit
configurations and both dual-prompt runs with the formal host package.
The repository candidate uses that scope for enhanced Windows with a
shared DOS mouse provider. It keeps direct registration for standard mode
and an instanced provider, and uses a stack-local DPMI register image.
The NE ABI and guest fixtures assembled. Its two no-addon ownership trials
completed with a null desktop callback in VM 2. In scoped-reviewed/eight2
all ten runtime reports passed, but the copied outer runner expected the
earlier three-byte pair receipt and rejected the new five-byte format.
The four retained END records contain role A/B, VM ID 2/3 respectively,
and callback mask zero. This run did not pass the outer gate.

The updated outer runner in scoped-reviewed/acceptance passed 8/8 basic
configurations and 2/2 enhanced DOSONLY=1 pairs with driver SHA256
5FA167B71FB12B69AEE78E9FF8AB46BB82D4A9FED00AAFDE94FAD87592CE9D07.
The pair fixture exchanges and immediately restores the actual DOS callback
before either child announces readiness. Both driver profiles returned
4102000000 / 4203000000 (A in VM 2 and B in VM 3, both masks zero).
The same no-addon windowed pair on the prior released driver completed and
exited normally but returned 4102001F00 / 4203001F00; the new gate failed
on the inherited mask. This control detects the bug without requiring a
timing-dependent crash or host instrumentation.

The prewin copy ran the freshly assembled M31TEST (62 checks), M31PM (65),
M31REENT (67) and M31CTX (32-bit GPR preservation), with guest output and
failure checks. M31REAL reported DPMI unavailable in that boot profile.
Direct COM launch from the long package root returned 042Bh; a separate
short-root direct launch timed out. Neither direct invocation passed.

Follow-up with a short isolated package root and the integration runner's
user environment: M31TEST and M31PM reported 62 and 65 checks respectively.
The real DOSX run (M31REAL) reached ENABLE, DISABLE and PASS (16 checks),
and M31CTX completed the register-preservation check. Results are in
scoped-reviewed/shortenv-{mock,prior-pm,M31REAL,M31CTX}.

The native-command-receipt run exited 0. All eight configurations reached
normal Windows exit, including enhanced mode with both addons and DOSONLY=0.
The four DOSONLY=0 cases checked the host CMD version via an atomically
published file. Both extra DOSONLY=1 prompt pairs passed. Setup used a shim.
The earlier native-command-short run timed out on normal exit in the enhanced
two-addon DOSONLY=0 case. The new run used the default PIF directory and a
complete command receipt; it does not isolate which difference mattered.

The separate native-order/test run exercised production BOP 54:08 with two
concurrent native programs in different enhanced-mode VMs. Both entered
before either could finish, the second observed the first process exit,
and the guest checked the two exit codes (37 and 38). Both VM receipts show
160 callbacks with zero callback errors. These runs cover command execution
and return; they do not exercise switching the fullscreen presentation.

fullscreen-key-01 passed the eight basic cases, both windowed DOS VM pairs
and two fullscreen DOSONLY=1 runs (both addons installed and both absent).
The actual active Console screen contained S14_DOS_FULLSCREEN_READY.
CAF opened a visible NTCON window owned by the isolated frontend. Input sent
through that window satisfied the DOS INT16 check for lowercase k. On DOS
exit the Win16 controller opened Notepad, observed a mouse press and release,
closed it, and exited Windows normally. This checks entry using the PIF
fullscreen flag; a windowed prompt switched to fullscreen with Alt+Enter
is a distinct entry path.

window-fullscreen-all passed all 14 invocations (8 basic, 2 concurrent guest
prompt pairs, 4 enhanced fullscreen transitions). Each transition started
with a windowed PIF. After the guest reported READY the private-desktop
observer sent Alt down, Enter down/up and Alt up. All four Console snapshots
contained the DOS marker. CAF selected a visible NTCON text window, and DOS
received k from that window. The reports record six frontend key messages
including one Enter keydown. All four Windows sessions returned to graphics,
accepted the mouse press/release and exited. In these cases DOS completed on
k: the two-prompt coexistence and fullscreen transition are separate tests,
so they do not establish that both prompts accept input after switching back.

The cached x86 and x64 default builds had no work after configuration. The
11 host image hashes agree across candidate-dem-unified, assets/release and
O:/winnt/system32. The updated private observer built and linked separately.

full-gate-mouse31/timings.json records 16/16 selected Full gates passing.
The external DOS4GW/Nesticle fixture was not supplied and was skipped.
MOUSE31.DRV was staged to assets/release from scoped-reviewed; its SHA-256
is 5FA167B71FB12B69AEE78E9FF8AB46BB82D4A9FED00AAFDE94FAD87592CE9D07.
The same file replaced the previously released project-authored driver in
O:/w31/SYSTEM/MOUSE.DRV after confirming its prior hash, and the installed
hash matched. setup-release-roundtrip used the current CMD patch tool with
the original media files from the installation kit's backups in an isolated
copy. Apply, repeat apply, restoration, and preservation of an unrecognized
file all passed. No original installed guest executable was replaced.

The stronger pair-key-01 combined sequence did not pass. In the no-addon
run both VMs were alive and B accepted k in fullscreen text and b on return
to graphics. A did not acknowledge input before timeout. Two earlier
single-window failures in this run sent zero key messages. The harness
restored an absent environment variable as an empty but present switch.
The runner now removes absent switches, and the paired guest controller
uses the established OpenIcon/BringWindowToTop sequence for A.

The paired guest controller actually created A.FOC: the DOS filesystem
shortened A.FOCUS to an 8.3 filename while the host observer waited for its
long spelling. The prior guest files include B.KEY and B.END and the captured
first-window caption was "S14 prompt A". Both ends now use A.FOC and the
controller keeps the first window handle before launching the second. On the
released driver the setup-release-final two-addon pair completed both A.END
and B.END, registered 14 front-end key messages with two Enter keydowns,
and exited Windows normally. The extra four messages after the two chords
and fullscreen k are the b and a down/up pairs for the two guest windows.

S14 closure: the final unmodified setup-release-final runner exited 0.
Both driver profiles produced A.FOC=41, A.END=4102000000 and B.END=4203000000
and exited normally. Each pair report counts 14 key messages, two Enter
keydowns and successful input delivery. Both VMs resumed after switching
VM 3 back from fullscreen: VM 3 received b, then VM 2 received a. All
14 matrix-results.json files report passed and normalExit. The physical
desktop and original graphical installer were outside this private setup-shim
run. Its two temporary install roots from diagnosis and final verification
(q31p and q31) were absent after cleanup. Product baseline acced8559 is on
origin/main. S15 owns Win95; S16 owns the outstanding mirror audit.

### Owner acceptance checkpoint

The owner superseded the automatic S14 closure and S15 admission. Keep the
published driver and require manual S14 acceptance. The two Standard profiles
with the addon pair configured now use the basic Win16 startup witness and
the DOSONLY query. Standard profiles without the pair and all four enhanced
profiles retain command and mouse interaction. The fullscreen and paired-VM
checks remain in the enhanced matrix.

Both cached host builds completed without recompilation. A fresh assembly in
owner-accept-driver reproduced the released MOUSE31.DRV byte for byte, including
the copy in O:/w31/SYSTEM/MOUSE.DRV. All 11 host images matched their manifest
in assets/release, O:/winnt/system32 and the tested package. The existing
16-gate Full result applies to these unchanged product images.

six-interactive exited 0. The six interactive configurations produced
command and mouse receipts. The two Standard controls with the drivers
configured each observed the expected DOSONLY value and normal Windows exit
without starting the prompt or mouse fixture. All four enhanced fullscreen
cases and both two-prompt return-and-focus cases passed in the same run.
Standard DOSONLY=0 without drivers exercised a delegated host command and
desktop recovery; blocking is the legacy cmdExec/cmdExec32 contract.

The preliminary Win95 fixture edit is retained separately in local Git stash
95b41389de0f1b0d05299e775fe95c23adf7888a. Its isolated runs ended before
manual acceptance; they do not constitute S15 closure or authorization to
resume S15.

## S14 final scheduling verification and owner-directed closure

On 2026-11-10 the owner closed S14 on the completed six-configuration contract
and explicitly deferred the later EDIT mouse transition failure. S15 is now
setup patch work awaiting detailed instructions. The former Win95 matrix and
mirror review remain pending, not silently completed or cancelled.

The tracked verify-win31-setup-modes.ps1 runner now holds an authored DOS or
native CMD child alive until the host test releases SCHED.GO. An independent
Win16 task appends progress bytes before, during and after the child lifetime.
SCHED.RUN identifies the execution owner. No product timeout or guest binary
was changed. The final run used candidate-dem-unified, O:/w31 as read-only copy
source, and the rebuilt message-probes/console-startup-observer.exe.

Run: build/t481-s14/scheduling-focus/run.ps1, background modern pwsh PID36848,
started 2026-11-10T20:53:31.3759526Z, completed exit0. Runner SHA-256:
19DEE3393A5E87E979F3F676C50A0DAFD1A6B267BCEF525CA2D0DDD618AFD59C.
Results are under build/t481-s14/scheduling-matrix-focus. All 14 case records
report passed and normalExit: six full configurations, two Standard startup
controls with inert enhanced addons, four fullscreen cases and two paired
DOS windows. Setup is still a shim; this is not a new original installer run.

| Mode | DOSONLY | Loaded pair | Expected scheduling | Progress during hold | After child | Result |
| --- | --- | --- | --- | --- | --- | --- |
| Standard | 0 | 0 | Blocking | 7 -> 7 | 9 | Pass |
| Standard | 1 | 0 | Blocking | 7 -> 7 | 9 | Pass |
| Enhanced | 0 | 0 | Blocking | 98 -> 98 | 100 | Pass |
| Enhanced | 1 | 0 | Nonblocking | 104 -> 633 | 635 | Pass |
| Enhanced | 0 | 1 | Nonblocking | 263 -> 1926 | 1933 | Pass |
| Enhanced | 1 | 1 | Nonblocking | 185 -> 769 | 770 | Pass |

The earlier scheduling-matrix run passed these six contracts but timed out
in the final no-addon pair. A graphics frame did not establish keyboard focus;
the fixture now requests and verifies the second DOS window's focus before
sending its post-return key, matching the first-window focus handshake.
The final complete run passed. This is a test synchronization change, not a
product mouse fix. Observer compilation/linking passed using the cached x64
wow_message_observer target (background PID34900); no product source changed.
The preceding Full evidence applies only to the unchanged product identities.

### Deferred EDIT mouse transition investigation

Actual owner installation: O:/w31, PATCH/CONFIG.NT contains DOSONLY,
SYSTEM.INI has mouse=*vmd and no configured INSIGNIA.386 or NTVDM64.386.
MOUSE.DRV matches released MOUSE31.DRV SHA-256
5FA167B71FB12B69AEE78E9FF8AB46BB82D4A9FED00AAFDE94FAD87592CE9D07.
Host NTVDM SHA-256:
0FBEFB8F20420C23F7615C7C01B9B5A3BD1655530B2C32342ACCB27A26A91447.

Reproducer: start PATCH/WIN386.CMD; open MS-DOS Prompt in Console text;
run O:/winnt/edit.com (pointer/clicks work); CAF to Window text (still works);
Alt+Enter back to the guest graphical DOS window. The DOS block remains beside
the Windows arrow, and EDIT menu clicks do not work. Earlier attempts showed
two tall colored glyph strips. The owner supplied screenshots and the live
read-only frontend capture independently showed both cursors. Do not replace
this with a test that finishes EDIT before returning to graphics.

Captured read-only evidence: build/t481-s14/live-mouse-return-5492 and
live-mouse-return-33832; surface.exe captured NTCON PID20200's 640x480 graphics
frame. The NTVDM map was used only after checking .text/.rdata/.data identity.
Snapshots are non-atomic: emulated mode103, BIOS modes27/3 respectively;
jap_mouse0/1, cursor_flag=-1, callback mask31, cursor_EM_disabled0 and
save_area_in_use0. These observations do not identify the writer or establish
a temporal callback sequence. No input was sent to the user's scene.

Source leads, not proved causes: mouse_io.c's jap_mouse routing and
mouse_EM_callback hide/show contract; mouse31.asm's enhanced IRQ-scoped callback
restoration; the original VMD graphics/text handoff. The current no-addon
configuration uses mouse=*vmd, while win-multitask/APPLY.CMD normalizes it to
explicit device=*vmd. Whether this changes actual VMD loading is unproved;
the proposed isolated query did not execute. Do not install the addon pair
as a substitute for fixing the no-addon contract.

Next investigation must reproduce the exact live-EDIT sequence on an isolated
copy, compare explicit VMD configuration only as a control, and trace cursor
hide/restore, active VM and callback ownership across both transitions.
Retain ordinary Standard and enhanced no-addon behavior. No new product fix
or causal attribution is claimed. The owner was told the live scene can be
closed; further testing must not depend on it. This deferred issue does not
count as a passing mouse test, and is explicitly excluded from S14 closure.

Closure verification (background PID3332, exit0): documentation governance
passed; git diff --check passed; all 11 manifest images matched assets/release,
O:/winnt/system32 and candidate-dem-unified/system32. Installed MOUSE31 matched
the release. All 14 final matrix records passed normal exit; owned O:/tmp/q31
was removed. No product source or release image changed in this test/document
delivery, so no redundant product rebuild or redeployment was performed.
The deferred Win95 test stash 95b41389de0f1b0d05299e775fe95c23adf7888a is retained.
