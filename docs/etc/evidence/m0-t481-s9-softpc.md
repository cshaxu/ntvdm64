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
