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

The same published package was subsequently exercised through both Win95
launch selections. DOSONLY=0 reached two concurrently active DOS VMs, 320
identity/stack/register/flags checks and four native commands; the shutdown
witness again reached `ExitWindows` but the launcher returned 1 and its
return was not observed. DOSONLY=1 ran an actual guest DOS program without a
native CUI worker; its test fixture retired the session, so it is not evidence
of normal guest shutdown. The source Win95 installation remained unchanged.

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
