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
original zero-length read has no data buffer to lease. After changing only
the existing MVDM-HOST-DIV-195 seam, the same real Win3.1 probe returned 0
for zero-byte read, 4 for ordinary read, and 1 for both empty-profile writes;
the resulting files contained their expected group entries. The fixed probe
passed in both DOSONLY modes. This proves the empty-file API defect and its
repair; the isolated full-Setup attempts still did not complete, so they do
not independently prove the original installer's complete call sequence.

Review of the retained original-Setup captures narrows why that attempt was
incomplete: copying reached 91%, then Setup displayed “Cannot read from drive
I.” The probe accepted Cancel; Setup continued into its configuration screens
and returned zero, but its installation lacked WIN.COM and group files. Thus
exit 0 is not a completed-install receipt. The rerun must handle the empty
optical drive without canceling a copy operation and assert the installed files
and groups explicitly.

An isolated-process device-map experiment did not hide `I:`: even after the
process-local map call returned success, a child `dir I:\` still reported the
existing device as not ready. It is not a working isolation method; the system
drive assignment and optical device were not modified.

The owner also rejected MULTITASK.HASH, MULTITASK.INI.BAK and
MULTITASK.INI.NEW in the installed PATCH directory. The configuration and
driver hashes were checked against those records and released drivers, then
the three exact records were removed from `O:/w31/PATCH` as requested. A
setup-script change now removes them after verifying the installed drivers
and configuration; its repeat-install and both mode checks passed.

The final cached x86 and x64 build and link completed. Full passed all 16
selected gates with the external DOS4GW/Nesticle fixture unavailable. The
final targeted package passed the empty-profile probe in both DOSONLY modes,
all four Win3.1 mode/startup cases with four real group references (using a
Setup shim), Win95 two-VM native command checks, and a guest-only DOS witness.
The latter retires its own session and is not a normal-shutdown proof. Package
`5478e4fc-e8ce-4dd8-a3cc-075eac4ed50b` was published and all 11 images
hash-verified in both `assets/release` and `O:/winnt/system32`; the installed
W31 addon pair was separately verified. Fresh original graphical Setup remains
an acceptance gap and must not be represented as a successful complete rerun.
