# T481 S9: SoftPC import evidence (in progress)

S8 was closed by the owner; its remaining observed-test boundaries are recorded
in [the S8 closure](m0-t481-s8-closure.md). S9 remains active.

Five mirror changes are registered as MVDM-HOST-DIV-894 through 898 and
included in the published S9 candidate. They are selected by the x86 product
build; source selection does not establish that a guest workload hit each
path. The initial three changes are:

- BIOS word and DWORD virtualization fallback: a fixture extracts both
  actual function bodies and uses stubbed address translation and nested
  execution. The prior word cast fails; corrected protected-mode and software
  V86 fallback and successful translation cases pass. This is not proof of a
  real guest fallback path.
- CHAIN2 Copy fill: the actual-provider C-VID fixture failed its full-plane
  comparison before the SoftPC-derived fix. It passes after the fix across
  odd/even addresses, four values and counts including zero.
- Unchained DWORD read: the actual-provider comparison against four consecutive
  byte reads failed before the offset fix and passes afterward.

Cached x86/x64 CMake builds and the separately linked x86 NTVDM worker passed.
An 11-image candidate was staged with a single package identity. Full selected
product gates passed against that package; DOS4GW/Nesticle was skipped because
the external fixture was not supplied. The first Full invocation stopped at
the control tests because the assembled cache lacked four test executables;
the complete second invocation used current built fixtures. The x86 and x64
non-desktop CTest selections each passed 46/46. An isolated Win3.1 matrix
passed standard/enhanced mode with DOSONLY 0/1, including normal exits.

An isolated Win95 test using the candidate host and released NTVDM64.386 passed
two concurrent VM identities, 320 callback checks and four native commands;
the source installation's file hashes were unchanged. The observer reported
guest exit code 1, so normal Win95 shutdown is not established.

The Win95 editor fixture was updated to recognize the activation wait already
present in the shared routine instead of inserting the obsolete version. With
the current CCPU and C-VID libraries linked into a separately built diagnostic
host, `verify-win95-editors.ps1 -MouseMenu -SaveWitness` passed two distinct
prompt VMs, native and DOS editors, mouse menu interaction and both saved
contents; the source installation hashes remained unchanged. The diagnostic
host is not the shipped executable. Its observer reported Win95 exit code 1,
so it does not establish normal shutdown. The exact production-host editor
path, owner's repeated Win3.1 switch sequence and final S10 acceptance remain
unverified.

The CMD multitask installer's synthetic Win3.1/Win95 configuration and exact
restore tests passed. The identity integration fixture now also offers
`-InstallViaTool`: on a Win95 copy it runs the actual installer, checks both
released driver hashes, then runs the ordinary production-host native-command
witness. Two of three runs passed both VM identities, 320 callbacks and four
native commands; the middle run exited `0xC0000420` without the first VM's
result file. This intermittent failure remains unclassified. The observer
exit code alone does not prove which fail-fast site ran. Each run verified the
source Win95 installation bytes unchanged. The failed run is not a pass.

`test/integration/multitask/verify-win31-installer.ps1` additionally exercised
the actual CMD installer on an isolated copy of the owner's Win3.1 installation.
The fixture removed only its own copied driver files and entries to construct
a pre-install state, then applied the tool and checked both released driver
hashes. With the then-published production package, standard/enhanced and
DOSONLY 0/1 all reached the desktop and exited normally. Source installation
file hashes were unchanged. This matrix starts through CMD with a short PATH;
it does not cover the owner's direct inherited-environment failure or full
editor interaction. The installer package is not yet claimed as delivered.

Two subsequent debugger-observed repeats also passed the native-command
witness and recorded no `0xC0000420`; debugger scheduling differs, so neither
is evidence that the prior failure was fixed. The status equals the explicit
invalid virtual-instance fail-fast code, but no exception record or stack was
retained for the failed run, and that equality does not establish its source.
An additional ordinary production-host run with `-Reuse -InstallViaTool` passed
actual VMM handle reuse, rejection of direct and queued old-generation wakes,
new-generation counted completion and 320 callback checks. Its source Win95
installation was verified unchanged. None of these runs proves normal Win95
shutdown; the recorded guest process exit was 1.

SoftPC `1ec0d551` virtual text aperture changes are already registered under
DIV-545; `8c27a3b6` CHAIN2 reads are already DIV-889. CPL access, slave EOI
and virtual text height are already covered by DIV-891, DIV-883 and DIV-890.
SoftPC `42533c7d` matches the S8 mouse fix; its later `421b2fcb` removes the
driver in SoftPC's different runtime, while this project still selects it.
SoftPC `ecf263a8` concerns snapshot interfaces not present by name here; no
claim of equivalent snapshot behavior follows from that observation.

The adjacent SoftPC main was checked through `6acc1193`: its subsequent S42
P3 removes its own `ccpu386/nop.c` and `ccpu386/vglob.c`, P4 removes its
retired DOS mouse snapshot payload, and P6 updates a binary snapshot. These
are not new software-emulation fixes to copy wholesale. The current project's
generated x86 worker `build.ninja` still compiles `ccpu386/nop.c` and the
distinct original `cpu/src/evid/vglob.c`; its generator explicitly uses the
latter as a C-VID accessor source. SoftPC's retirement therefore does not
establish that either selected local translation unit is removable. Its
snapshot changes target a different SoftPC snapshot composition and do not
establish a corresponding local defect. No source or binary was imported or
removed on that basis.

An isolated production-package `verify-win95-identity.ps1 -InstallViaTool`
repeat was started under PID 26008 at 2026-11-09T04:58:13Z using
`package-rotate`, the released addon, and a fresh copy of the historical
Win95 source. The installer completed and the source installation's final
hash check passed. The observer timed out; the guest API witness reported a
zero signature and VM, so the test exited 1 without reaching its two-VM
assertions. This is a failed repeat, not a reproduction of the earlier
`0xC0000420`, not a pass, and not a diagnosis. Output and exact test artifacts
are under `build/t481-s9/win95-recheck2`. An earlier detached attempt was
reaped with its parent and produced no runtime result.

Short-path comparison: `win95-recheck3` stopped before runtime because its
source was itself an older owned test copy containing `SYSTEM.DAT.CANDIDATE`;
the copy helper attempted to reuse that name inside the new destination. The
helper now writes relocation candidates in the fresh test output directory,
leaving both source and copied prior files untouched. `win95-recheck4`
(supervisor PID 37148, start 2026-11-09T05:14:03Z) used a fresh short-path
copy of the old `win95585` no-addon test installation, the same production
package and released addons, and the actual installer. The test exited zero:
two distinct concurrent VMs passed identity, negative API and 320 callback
checks, and all source hashes remained unchanged. The observer recorded
`result=exited`, guest exit 1; normal shutdown remains unproved. Because this
control changed both source copy and destination path relative to recheck2,
it cannot attribute recheck2's failure to the longer path. The earlier
`0xC0000420` also was not reproduced by this run.

`win95-recheck5` (supervisor PID 33640, start 2026-11-09T05:27:50Z)
repeated recheck2's source, package, installer and equal-length destination
shape using a new independent destination. It again timed out with a zero
driver API signature and VM; no two-VM assertions ran. The source inventory
and hashes were verified unchanged. The successful short-path recheck4 uses a
different prepared installation copy: the selected WIN.COM, VMM32.VXD and
SYSTEM.INI hashes match, while its registry hives and WIN.PIF differ. Thus
these two repetitions show a consistent difference between these test inputs,
not whether path length, registry contents, PIF or another setup difference
causes it. Neither failure demonstrates the earlier `0xC0000420`.

Further read-only inspection found a concrete invalid-input difference:
`O:/tmp/vm47995/PATCH/{CONFIG.NT,AUTOEXEC.NT,WIN.CMD}` references the old
`O:/WINDOWS` installation rather than its own root. The copy helper replaces
the stated source root, not arbitrary pre-existing aliases, so recheck2 and
recheck5 retained those references in their isolated destinations. Their PIF
fields did point to the isolated destinations, but the referenced NT config
and environment files then pointed outside them. By contrast, the recheck4
source and destination config files reference their respective own roots.
The failing inputs thus do not meet the intended fully isolated installation
precondition, and these runs must not be used as Win95 product regression or
path-length evidence. This establishes a test setup defect, not the exact
instruction or mechanism that yielded the zero API response. No source or
external Windows installation was modified during this inspection.

The isolated-copy helper now rejects an explicitly absolute `windir` or
`winbootdir` in the source AUTOEXEC.NT when it names another installation.
It does not infer or rewrite a different source root. The negative control
with `vm47995` rejected the stale root before creating either destination or
output directory (supervisor PID 25068). The positive control
`win95-valid-source` used the self-consistent `z97` source, the production
package and the actual installer: two distinct concurrent DOS VMs and 320
callback checks passed, source bytes remained unchanged, and the observer
reported `result=exited` with exit 1 (normal shutdown still unproved).
This guard covers the explicit `windir`/`winbootdir` form, not every possible
foreign path in every configuration file.

`win95-production-native` combined the actual `-InstallViaTool` path with
`-NativeCommand` against the self-consistent `z97` source, released addons
and production `package-rotate`; supervisor PID 39604 started at
2026-11-09T05:36:30Z and exited zero. Two distinct concurrent VM identities,
negative API checks and 320 callback checks passed, followed by four real
native commands with guest return, register, flags and stack assertions.
The source installation's inventory and hashes remained unchanged. The
observer reported `result=exited`, exit 1; this does not establish normal
Win95 shutdown, physical editor input, mouse interaction, or resolution of
the separate Win+R startup failure. This combined installer/production-native
run does not use the diagnostic editor host interposition.

The tested 11-image package `53589968-545f-4d1d-8951-db38d0131d13` was
published to both `assets/release` and `O:/winnt/system32`, with manifest
hashes checked. The two released drivers already match `O:/w31/PATCH`, and
`O:/w31/SYSTEM.INI` was verified unchanged. This is a manual-test candidate;
S9 and the Win95 acceptance work remain open.

Further S9 review of SoftPC `a43c6d24` found that the local x86 GDP layout
uses direct offsets, unlike its translated compatibility accessor. The local
actual-provider fixture failed the CHAIN2 BYTE mode-1 comparison against a
nonuniform low-order mask before correcting the colour-compare/don't-care
subfield offsets, and passed afterward for CHAIN2 BYTE and CHAIN4 WORD cases.
DIV-897 records 56 changed offset literals in two original C-VID files; no
parallel GDP state or helper was added. A related generic-write rotation
offset remains unchanged: an attempted real-write fixture lacked sufficient
state setup and returned zero even with the offset corrected, so it does not
prove the expected write behavior. That test was not retained as a passing
test. The candidate passed all 16 selected Full gates (with the external
DOS4GW/Nesticle fixture skipped), the four Win3.1 mode/DOSONLY startup and
normal-exit combinations, and the production Win95 installer/two-VM/four
native-command witness. The Win95 observer still reported exit code 1, not
proof of normal shutdown. Both source Windows installations were verified
unchanged. The candidate was published as the complete 11-image package
`d29c4100-b9a2-4a80-a3b4-e82dc98896a4` to `assets/release` and
`O:/winnt/system32`; both W31 drivers remained hash-identical to the released
drivers and `O:/w31/SYSTEM.INI` was unchanged. At that point the earlier
intermittent Win95 `0xC0000420` and generic-write rotation remained open.

The initial rotation fixture used an incorrect zero latch-XOR mask. Setting it
to the real VGA mode-0 assign value (`0xffffffff`) produced a discriminating
actual-provider test: a rotation count of one wrote `0x81818181` before the
change, rather than `0xc0c0c0c0`. DIV-898 corrects the three direct GDP rotate
subfield offsets in `sevid004.c`; selected generic byte and WORD writes then
passed counts 1 through 7, checking both WORD bytes in all four planes. The
subsequent candidate passed all 16 selected Full gates, the four Win3.1
mode/DOSONLY startup and normal-exit combinations, and the production Win95
installer/two-VM/four-native-command witness. DOS4GW/Nesticle was skipped
for lack of the external fixture; Win95 exit 1 does not establish normal
shutdown. Both source Windows installations remained unchanged. All eleven
host images were published with package identity
`95d80e95-adb7-456e-b82c-e9932054909f`; both W31 drivers matched the release
and SYSTEM.INI remained unchanged. The owner's repeated Win3.1 switch sequence
and final production Win95 editor interaction remain unverified.

After the desktop environment was restored, `win95-post-shell` repeated the
actual installer and production native-command witness using a fresh copy of
`O:/WINDOWS` and the released host package. The two concurrent DOS VMs, 320
callback checks and four native commands passed; source installation bytes
were verified unchanged and the owned copy and package stage were removed.
The observer again recorded an exit code of 1. This repeats the existing
runtime evidence but does not prove normal Win95 shutdown or production-host
editor and mouse interaction.

`win95-shutdown-original` and `win95-shutdown-retry-multitask` compared two
fresh `O:/WINDOWS` copies using the same released host and the same Win16
startup/ExitWindows witness. The first copy had both multitask drivers
removed; the second had both installed. Each recorded the witness boot and
Program Manager readiness files, then the observed launcher exited with code
1. Both source inventories and hashes were verified and the owned copies and
stages removed. Thus exit 1 is not specific to installing these drivers in
this control; it does not establish why the launcher returns 1 or prove that
Win95 completed its normal shutdown. The first combined runner's second case
did not start because its first case had replaced PATH; the separate retry
above is the valid multitask-side observation.

The current adjacent SoftPC main remains `6acc1193`. A further literal scan
of this project's generated `base/cvidc/*.c` found no remaining assignments
of the five affected original subfield offsets 1378, 1379, 1382, 1383 and
1407. The selected accessor and existing actual-provider fixture are retained;
this scan supports completeness for those exact offsets, not correctness of
all C-VID field references or proof that every corrected rule was reached by
a real guest.

`win95-batch-current3` revisited the existing S4 Win95 direct-batch boundary
on the published production host and released pair of drivers, using a fresh
`O:/WINDOWS` copy. Both BATs wrote their `DONE_A` / `DONE_B` markers while
the native CMD redirection files were zero bytes; the observer saw the outer
launcher exit with code 1. This reproduces the historical output symptom,
not its root cause, and does not contradict the separately proven interactive
COMMAND route. The original installation hashes remained unchanged; the owned
guest copy and package stage were removed. The first two attempts did not run
the workload because of test-script setup errors and are not runtime evidence.
This remains a Win95 acceptance investigation, without a guest patch or a new
claim about which guest interpreter emitted the old version error.

`win95-batch-current5` repeated that failure with controls in each BAT. Guest
`ver` wrote `MS-DOS Version 5.00.500`; the test COMSPEC was the isolated
package's `SYSTEM32/COMMAND.COM`, and `%COMSPEC% /c echo CONTROL_A` wrote the
expected text. Both direct native CMD outputs were still empty, both BATs
finished, and the original installation remained unchanged. A read-only byte
comparison found the exact historical `Incorrect DOS version` text in the
deployed NT COMMAND.COM, but not in the installed Win95 COMMAND.COM; the
original NT COMMAND source checks DOS version in `ConProc` before proceeding.
These observations suggest a possible version-check path but do not prove
which process displayed the historical message or why this particular native
command did not reach admission. No guest or runtime change is inferred.

`win95-batch-shells` explicitly set the PIF programs to the copied Win95
COMMAND for A and a byte-identical copy of the released NT COMMAND for B,
each with `/c` and its own BAT. PIF inspection confirmed both fields. B wrote
the version, COMSPEC, internal-command control and DONE files but its native
CMD output was empty. A produced no BAT witness before the observer timed out;
the reason is not established. This shows the NT COMMAND can execute the BAT
in that test while its explicit native command still fails; it does not prove
why native admission is missed or explain A. Source installation bytes were
unchanged, and the owned guest copy and package stage were removed.

`win95-batch-vector2` sampled DOS INT 21h through a disposable test COM in
both failing BATs. Both returned DOS version AX=0005 and the same INT 21h
entry; its first 32 bytes matched `O:/WINDOWS/IFSHLP.SYS` at file offset 1184.
The captured IFSHLP far-jump predecessor began with a stub that calls another
routine before an indirect jump. `win95-batch-vector3` sampled that indirect
pointer without executing the preceding call; it therefore cannot establish
the actual dispatch target during an EXEC request. These observations locate
the first hook only; they do not yet identify the executing DOS loader or a
safe fix. Both runs reproduced empty native output, preserved the installation
hashes, and cleaned their owned copies and stages.

`win95-batch-z` used the production package and ran two otherwise comparable
Win95 BATs: A invoked host CMD directly; B invoked the original guest
`%COMSPEC% /z` with the same CMD and equivalent redirected output. Both BATs
created DONE markers. A's output was empty, while B contained `NATIVE_B`,
establishing that the original `/z` to `cmdExec` backend can execute and
redirect the native command in this environment. The ordinary EXEC route still
did not yield that output. The Win95 observer timed out after the markers, so
this is not a normal-exit or complete lifecycle pass; the cause of the timeout
was not determined. The original installation hashes matched and the owned
guest and package copies were retired. This comparison narrows the missing
boundary but does not prove exactly which classification or conversion step
failed, or authorize a guest mutation.

`win95-batch-svc32` used a separately linked diagnostic host that records
every original COMMAND SVC, with only the PE32 test in each BAT. Both guest
EXEC calls returned status 4, both BATs completed and neither native marker
appeared. The trace contained zero SVC 7, zero SVC 8 and no VM native-command
admissions. In the production-host `win95-batch-classify-short` comparison,
both 64-bit witnesses did write their host markers and the existing classifier
reported two 64-bit results; both 32-bit witnesses returned 4 without markers
or 32-bit classification records. Its observer timed out after both BAT
markers. The diagnostic host result is not exact production-host tracing;
classification records are emitted only after successful host classification,
so their absence alone does not prove that the function was never entered.

A separate Win95 `WinExec` control used a test PE32 with only KERNEL32 imports
and a `GetVersion` file witness. WinExec returned a value above 32, but the
entry-point file was not written before the test controller exited; this does
not prove whether the program ran in guest or host. A Windows process-start
trace subscription was denied access and supplied no process evidence. These
tests do not settle PE32 ownership or justify redirecting all Win95 PE32
programs. Source installation hashes matched and owned copies were removed.

`win95-notepad2-multitask` used the released host and addon pair with a fresh
Win95 copy. An authored Win16 controller first required that the guest USER
tree contain no `Notepad` window, then called WinExec on the copied original
Win95 `NOTEPAD.EXE` (PE32, source SHA-256
`CF2D882D6EC84BB4F9E6475D3B3674E4A480153FF3D34CAAB617F1A799A45AD0`)
and recorded both the successful WinExec return and a subsequently found guest
`Notepad` window. This is a positive guest Win32 ownership control for
WinExec, not proof of the direct DOS EXEC path or interactive behavior. The
observer timed out after the witness; shutdown is not a pass. Original source
installation hashes matched and the owned guest copy and stage were retired.

`win95-dos-notepad` then used an authored DOS EXEC witness to start that
same original PE32 Notepad from VM A, while an authored Win16 controller
looked for the guest `Notepad` window and would close it if found. The DOS
open succeeded and measured 34,816 bytes, matching the original file, but
EXEC returned child status 4 and the controller did not record a Notepad
window. Both BAT completion files were written; the outer observer recorded
exit code 1, which does not prove normal shutdown. This contrasts with the
successful guest WinExec control, but a transient window could have escaped
the controller's observation, and the code returning status 4 remains
unidentified. The runner initially could not perform its own source check or
cleanup because a test-script variable shadowed the source path. A separate
same-format full inventory/hash comparison matched the retained pre-test
snapshot, then the explicitly owned copy was removed. No product or original
guest binary was changed.

`win95-batch-process-watch2` repeated the production-host 32/64 comparison
with a separate process-snapshot observer and delayed native test programs.
Both PE64 targets were observed as Windows processes parented by NTVWM, then
wrote their host markers; neither PE32 target was observed or wrote a marker,
and both DOS witnesses returned status 4. Snapshot polling can miss a short
process, so this is not proof that no PE32 process was ever created. Both
batches wrote their completion markers, the outer launcher returned code 1,
and the original installation was hash-verified and the owned copy retired.
An earlier observer attempt failed before guest startup because the separate
PowerShell runner lacked its file-hash module; it is not runtime evidence.

`win95-batch-exec-controls2` ran an authored COM witness in each failing
batch and recorded real DOS INT 21h open, EXEC and child-status results.
After the witness released excess DOS memory, both VMs opened the native
host CMD successfully, EXEC returned carry clear, and the child's termination
status was `0004`. A missing-file control returned carry set with error 2;
a normal test DOS COM returned carry clear and status `0025`. Both batches
still reproduced empty native CMD output. The initial `exec-controls` run
omitted memory release and its DOS control failed with error 8; it is not a
valid successful-EXEC control. The published host CMD is x64, and its static
MZ stub would exit with code 1, so status 4 alone does not identify the code
actually executed. The source Win95 installation was hash-verified and the
owned test copy and stage were retired. No guest or product binary was changed.

`win95-batch-wowcmd-size`, `win95-batch-pe32` and `win95-batch-z32` narrow
the above conclusion. In both VMs, opening `C:/Windows/System32/cmd.exe`
through the guest returned the 261,120-byte 32-bit CMD, matching SysWOW64;
the 339,968-byte 64-bit CMD copy returned status 0, while hash-identical
copies of the 32-bit CMD returned status 4. Authored PE32 and PE64 controls
have a DOS stub returning 51, native code returning 37 and a per-VM host-file
witness. Ordinary EXEC of PE64 returned 37 and wrote both host witnesses;
ordinary EXEC of PE32 returned 4 and wrote neither. Outside Win95, the
published Run16 executed both controls, returned 37 and wrote their witnesses.
In the separate Win95 `/z` comparison, ordinary EXEC of PE32 A did not write
its host witness, while original COMMAND `/z` for PE32 B did. Both BATs wrote
DONE; the observer timed out, so no normal-exit claim follows. Thus ordinary
EXEC is not generally missing: the reproduced failure is specific to the
32-bit payload route in this VM context. The precise failing component and
any relationship to filesystem redirection remain unproved. All guest copies
were retired and source installation hashes matched.

`win95-batch-termtrace2` captured the DOS termination site for the PE32
failure in both VMs. The exact instruction bytes match the original Win95
`SYSTEM/CONAGENT.EXE` (SHA-256
`086FA12CCD06C56636BF90134F9E2CE98407E6B894CB6E7C3F31622ADCC3348E`),
which exits with `AX=4C04`; the PE64 control returned normally. Static
inspection identifies more than one route to this exit, including agent
initialization and the VCOND 0209 call, so this does not yet identify which
route failed. `Incorrect DOS version` also occurs in CONAGENT; previous
attribution of that message to COMMAND alone was incomplete.

`win95-batch-vcond-self` tried to observe the VCOND query but its positive
self-check did not hit the hook, so zero observed queries are inconclusive.
`win95-batch-open-trace` observed CONAGENT's initialization open call in both
VMs: carry clear and handle 5, while the PE32 result remained 4 and the PE64
control remained 0. The hook did not establish whether later initialization
or VCOND failed. Both runs verified the original installation unchanged and
removed their disposable guest copy; the harness reported missing native
output, and neither run is an acceptance pass. No guest or product code was
changed.

`win95-batch-io-trace` and `win95-batch-vcond-query` extended the disposable
DOS witness to record file service return values. In each PE32 case the agent
successfully opened its executable, read its header and resource records, and
read its message resources; the witness still returned 4. This excludes a
simple failure to open or read the agent executable, but does not prove the
rest of initialization or the subsequent VCOND call succeeded. The INT 2F
query counter was corrected to include null service results, yet its parent
self-query still did not reach the hook; the query observation is therefore
not usable as a negative witness. PE64 controls still returned 0. Original
installation hashes were verified and the owned guest copies removed. These
are diagnostic results, not a passed native-output regression.

`win95-batch-dos-version` captured INT 21h/AH=30h at the original
CONAGENT initialization call (return IP 0299): AX=0005 in both VMs. Its
original code requires AX=0007 and returns carry on a mismatch; on both
exits the agent's VCOND pointer and state were still zero and carry was set.
This identifies a concrete pre-VCOND DOS-version failure in this scenario.
The existing launch package's SETVER table supplies 7.0 for WIN.COM and
VMM32.VXD but does not list CONAGENT.EXE. That omission is a possible
configuration remedy, not yet a demonstrated complete fix.

A test-only INT 21h version substitution (`win95-batch-dos7-control`) timed
out and did not produce both requested witnesses; its only observed B record
returned 0 but does not establish the intended agent path. It therefore does
not prove that 7.0 resolves the operation, and no equivalent substitution was
added to product code. The source installation was hash-verified unchanged
and the temporary guest copy retired.

`win95-batch-dos7-scoped` repeated the test-only version substitution with
the caller segment constrained to the observed CONAGENT initialization.
The observer timed out, neither VM produced the requested completion record,
and the harness reported missing native output. This still does not show a
working version-only remedy. The original source matched its recorded hashes
and the disposable copy was removed.

`win95-editors-production` ran the formal two-VM editor fixture with mouse
menu and save witnesses against package-rotate (NTVDM SHA-256
`695801153810E17C0767A42DE1D735CC730307578147E1CD31DAB81018AB3D1A`,
matching deployed O:/winnt) and the released drivers. The test observed its
startup READY marker, but the observer timed out and both saved witness files
remained empty; it did not establish editor interaction or normal shutdown.
The fixture requires a diagnostic host for its test-only BOP markers, so this
production-package invocation was invalid and is not evidence of a product
regression or a substitute for the earlier diagnostic-host pass.
The source installation hashes matched and the owned guest copy was removed.

`win95-editors-production-welcome` repeated the production editor run with
the exact welcome-window dismissal used by the earlier diagnostic pass. It
still timed out with READY but without completed editors or saved text.
This invocation is also invalid because the fixture requires diagnostic BOPs.

`win95-editors-diag-control` used the same current source installation,
driver pair, observer, dismissal and fixture with the retained diagnostic
package. It passed two editor VM identities, both menus, five mouse receipts
and both save contents; its outer Win95 exit was 1 and normal shutdown is not
proved. `win95-editors-ab-ntvdm` then used that diagnostic package with only
its NTVDM replaced by the deployed production NTVDM, restamped together as a
private test package. It timed out at READY with no editor or save witnesses.
The fixture requires the diagnostic-only BOP interposition; replacing NTVDM
removed the very test interface, so the mixed run is invalid and cannot
attribute any product regression to NTVDM. The mixed package was used
only in isolation and was not published. All three runs hash-verified the
original installation and retired their guest copies.

`win95-production-native-recheck` used the released driver pair installed
by the actual multitask installer and the production package, with the
production-native-only witness (no diagnostic BOP markers). Both concurrent
VMs passed identity and generation checks, 320 callback checks, and four
native command return/register/flags/stack checks. The observer saw exit 1;
this is not proof of normal Win95 shutdown, physical input, or editor menus.
The original installation hashes matched and the guest copy was removed.

The Win95 editor fixture now preflights the expected diagnostic-host marker
strings before creating a guest copy. The retained diagnostic image contains
both; the ordinary production image was rejected immediately and no output or
guest-copy directory was created. This is an applicability check, not proof
that a package with those strings implements the test interface correctly.

Original DOS source tracing identifies the existing per-program version
mechanism: `Scan_Special_Entries` searches the loaded SETVER table by executable
name; EXEC writes the selected `Special_Version` to the new PSP; `Get_Version`
returns that PSP field. The installed private Win95 launch SETVER table was
read-only inspected and contains WIN.COM=7.0 and VMM32.VXD=7.0, ending there;
it does not contain CONAGENT.EXE. The original runtime SETVER table was also
read-only inspected. This is a source-level explanation consistent with the
observed AX=0005, not a runtime proof that adding an entry alone completes the
Win95 operation. No guest binary or installed configuration was modified.

`win31-production-native` used the ordinary current package, released driver
pair and the production-only DOS witness. Enhanced-mode DOSONLY=0 reached the
desktop and exited normally. Two concurrent DOS VM identities, 320 callback
checks, and both 54:08 native command returns per VM passed; this witness
intentionally excludes 54:0A and physical keyboard/mouse. The original Win3.1
installation was hash-verified and the disposable copy removed.

A read-only inspection found that the existing user installation's
`O:/WINDOWS/PATCH/WIN.CMD` contains two references to the removed test stage
`O:/tmp/nca7/system32`; the referenced Run16 is absent, while the deployed
`O:/winnt/system32/run16.exe` exists. The generated launcher template in
`tools/win95-launch/APPLY.CMD` instead writes the selected Run16 path. The
origin of the pre-existing installed launcher has not been established.
An attempt to correct only those two paths in the existing installation was
rejected by automatic approval review because it would write to the guest
installation; no installation file was changed. This launcher observation
is separate from the isolated tests, which use their own copied WIN.PIF.

`win95-shutdown-witness2` extended the production-native-only test with a
Win16 marker immediately before its `ExitWindows` call. After installation
via the real tool, both VM identity and 320 callback checks passed, all four
native calls returned, and the marker was present after both VM retirements.
The observer saw the outer process exit with code 1, so the marker establishes
that the call site was reached, not that Windows shut down normally. The first
attempt failed before launch because the new PowerShell field was incorrectly
nested; the corrected attempt above is the actual runtime result. Original
installation hashes matched and the disposable copy was removed.

`win95-shutdown-return` extended the call-site witness with a second file
written only after `ExitWindows` returns, including its AX value. Both VM
retirements and the pre-call marker were observed, the outer process exited
with code 1, and the return marker was absent. This does not distinguish a
successful non-returning shutdown from termination during shutdown. Prior
same-host controls with and without the multitask drivers both returned 1, so
this exit code alone is not evidence that the multitask driver caused it.
The original installation hashes matched and the disposable copy was removed.

`win31-production-comspec43` and `win95-production-comspec43` ran the
ordinary production package and released drivers with a test-only native
COMSPEC returning 43. In each system both concurrent DOS VMs issued 54:0A,
observed that exact exit value with preserved registers, flags and stack,
and passed the 320 callback checks. Win3.1 reached desktop and exited
normally; the Win95 observer exited 1 after reaching the ExitWindows call
site, without a return witness. The initial Win3.1 zero-exit COMSPEC control
also passed. This tests actual execution and continuation, not physical or
interactive shell input. Both source installations matched their hashes and
the disposable copies were removed. After adding this mode, production 54:08
regressions passed again for both Win3.1 and Win95.

A fresh read-only deployed-hash check matched all 11 host-image manifest
entries against `O:/winnt/system32`. The two installed `O:/w31/PATCH` drivers
matched the released NTVDM64.386 and INSIGNIA.386. The existing
`O:/WINDOWS/PATCH` contains neither driver, and its SYSTEM.INI has no entry
for either, so that Win95 installation is not presently prepared for the
multitask feature. Isolated Win95 tests explicitly install the drivers in
owned copies; their passes do not show that the user's existing Win95 tree is
installed or ready for manual acceptance. No installation was changed.

The owner subsequently granted permanent authorization to modify the Win95
installation. After confirming no project processes were active, the two
expired `O:/tmp/nca7/system32` paths in `O:/WINDOWS/PATCH/WIN.CMD` were
replaced with `O:/winnt/system32`. The actual `win-multitask/APPLY.CMD`
installed INSIGNIA.386 and NTVDM64.386 in that installation; both hashes
matched assets/release, and SYSTEM.INI contains both device entries. This is
installation and configuration evidence, not a Win95 runtime or manual
acceptance pass. The installed original guest binaries were not modified.

After the owner authorized Win95 installation changes and requested the agent
run tests first, the deployed Run16 launched the installed WIN.PIF in a private
desktop. The Windows 95 frontend appeared without an observed host error
dialog during 15 seconds; this title alone does not establish desktop readiness
or normal shutdown. `win95-installed-native` then used a disposable copy of
the now-installed tree, explicitly staging both released driver images, and
the production package. Two VM identities, 320 callbacks, four native command
returns and the post-retirement ExitWindows call-site marker passed. The outer
process exited 1 without an observed ExitWindows return; normal shutdown is
not proved. `win95-installed-editors` used a separate diagnostic host to drive
two editor VMs; both menus, five mouse receipts and the saved `aaa`/`bbb`
contents passed. This is diagnostic input, not physical-input or production
editor acceptance. Both test runners verified the installed source inventory
and hashes unchanged and removed their owned copies. An optional instance
image parameter was added to the identity runner so this check does not
silently drop the installed instance driver in its copy.

On owner request the deployed Run16 was launched against the installed
Win95 PIF. A private-desktop observer saw a frontend titled Windows 95, but
that title did not prove desktop readiness. A separate visible-desktop attempt
produced a black capture; window activation failed twice and no UI input was
performed. A further direct launch using a cleaned process environment stayed
alive for 30 seconds, after which the owned test process was terminated; all
project processes were absent afterward. These observations do not distinguish
capture/activation failure from a guest display/startup failure and are not a
production interactive pass. The independently run diagnostic editor checks
and production command checks remain scoped as recorded above.

A privileged, read-only process-tree check corrected the earlier cleanup
assumption: ordinary sandboxed process enumeration had missed the agent's
live Win95 Run16/NTSRV and related workers. The explicitly identified owned
processes were stopped under the owner's standing authorization, and an
elevated process-tree check confirmed no project processes remained. The
prior black desktop capture and intervening runs with these residual instances
are not evidence of a product black-screen regression.

`win95-clean-instance-frame` then ran the installed WIN.CMD with the production
host on a private desktop, from a clean process state. The task-only observer
read the actual NTCON-published 640x480 graphics frame before ending its owned
process. It shows the Win95 desktop, taskbar and welcome dialog; the PNG SHA-256
is B6B638072D86E37B2E278486DF3045514D65599C75DB7426EEDDAEF28829BF3B.
This establishes installed-system desktop readiness for this run, not physical
editor input, 54:0A interaction or normal Windows shutdown. The observer ended
its own timed test; an elevated process check found no surviving project
processes. No original guest binary was modified.

A read-only check of the owner's named editor files identifies
`O:/winnt/edit.exe` as AMD64 PE (SHA-256
`5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`)
and `O:/winnt/edit.com` as a DOS COM image (SHA-256
`E0A0B24FEE4037CB050670661C30AD7ECC0EA9483938152FCA3D807C443E8A46`).
The latter contains the `QBASIC.EXE` and `/EDCOM` launcher strings, and
`O:/winnt/QBASIC.EXE` exists. Therefore the separately reproduced Win95 PE32
CONAGENT failure is not itself proof that either of these specific files fails;
it remains an independently recorded compatibility gap. File classification
alone does not prove interactive operation or the complete child chain.

`win95-guest-input-mode-restore` used the production host package and released
drivers in an isolated copy of the installed Win95 tree. Two DOS VMs and their
native 54:0A children were simultaneously live. A Win16 controller focused
each VM in turn and used guest USER `keybd_event` to send its distinct key;
the native children received A=`a` and B=`b`, and both guest continuations
passed. The 320 callback identity, register, stack and flags checks also
passed. The controller reached `ExitWindows`, but its return was not observed;
this is not proof of normal shutdown. The original installation's bytes were
verified unchanged and the temporary copy was removed.

`win95-guest-input-reverse-mode-restore` compiled the same test helper with
reversed expected keys. It failed the native continuation as intended, and
the source installation was verified unchanged. These tests exercise guest
USER-synthesized input through the production host path, not physical input,
mouse interaction or the actual EDIT programs.

`win95-guest-input-reverse-order` repeated the production guest-input witness
with VM B focused and keyed before VM A. Both distinct receipts and all 320
callback checks passed. The ExitWindows return remained unobserved; the
original installation was verified unchanged and the owned copy removed.

The owner reported that Win95 blue-screened on mouse movement immediately
after desktop startup. `win95-mouse-repro3` and `win95-mouse-burst100` ran
the installed production package and addon pair on a private desktop. A
test-only frontend hook delivered one, then 100 alternating relative motion
records to the production input sink; the sink acknowledged both deliveries.
The captured frame after each run still showed the Win95 desktop. These runs
did not reproduce the report and do not exercise physical Raw Input or prove
the owner's failure absent. The observer ended each owned run after capture.
The first hook attempt targeted a window recreated during startup and did not
deliver input; it is not a negative test. No product or guest code was changed.
