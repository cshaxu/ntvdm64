# T479 S6 P2 — combined main integration delivery

## Scope and source identity

The owner requests fast-forward import of `task/mirror-source-audit`, followed
by a new combined build, applicable focused verification, Full gates and a
coherent eleven-image publication. Main was clean at f095c77bf; the ancestry
check found exactly67 commits through661a1315f. `git merge --ff-only` succeeded.
No reset, history rewrite or force push was used. The
[rebase handoff](m0-t478-main-rebase.md) remains historical, not combined proof.
Application0.0.479, protocol67 and the active T479 S6 scope are retained.

The post-import production sources are unchanged. The only additional code
changes are test infrastructure: the CPU/PIC fixture now rejects unexpected
VM bridge/command entry instead of linking an incomplete production frontend,
and the slave-BIOS test rebuilds its actual Ninja prerequisites before linking.
These test-only seams terminate with ERROR_CALL_NOT_IMPLEMENTED; they do not
return fabricated success or enter product libraries. Original guest images
and mirror code receive no additional change in this integration checkpoint.

## Combined verification

All runs use main-worktree sources and the current cached MSVC x86/x64 trees,
with repository-local ccache. Source-worktree assets are not release inputs.
Evidence is owned by `build/t479-s6-integration`; the
[machine receipt](m0-t479-s6-main-integration.json) binds retained logs/results
and publication identities by SHA256.

| Verification | Result |
| --- | --- |
| Complete selected host builds, including VCDEX | x86 and x64 build001 exit0; shared suites14/14 on each architecture. |
| Existing command, VM frontend, broker, frontend lifetime and handoff fixtures | release001 passes its retained assertions before Full. |
| Full product gate |16/16 selected gates pass in409839ms; verifier reports identical11-file package. External DOS4GW/Nesticle fixture remains explicitly skipped. |
| Imported focused owners |13 fixture runners pass: DLC worker event, redirector async baseline, DIB failure/retirement, EDIT tabstops, OLE root, fax, file cache, font, palette, spool/loader and Winsock rundown. Negative baseline observations are not claims of corrected original defects. |
| GDI alias/class brush; XMS; slave BIOS/PIC | Alias fixture errors0; class-brush collision0; XMS139/139; slave BIOS EOI74/74. |
| S6 text model | Current mvdm-vm-text CTest passes, including candidate fill operations. |
| Actual Win3.1 fill and exit | fill003/004/005 each pass both VM identities,320 callbacks, three continuation forms, guest readback and normal system exit. Original installation bytes unchanged. |
| Actual nested task chains | nested007 passes both guest VMs, actual nested MEM output before EXIT and normal Win3.1 exit. Provider input is not physical-input acceptance. |
| Actual WOW guest behavior | Kerning, palette and embedding reports have errors0; WINMINE flag/counter, reset, opened cell, WM_CLOSE and Run16 completion pass. |
| Actual ROM RTC behavior | RtcEntry and RtcAlarm pass normal launcher exit, wait/count/flag semantics and alarm callback/EOI restoration. |

Initial slave-BIOS linking used stale fixture libraries. Rebuilding exposed
the missing test isolation boundary for the newly connected VM entry points;
the bounded correction above resolves it. The first guest-kerning invocation
used an absolute path where its existing runner expects a repository-relative
output; the corrected invocation passes. The first gameplay invocation was
rejected by its existing live-broker guard while the separately owned RTC
probe was exiting; after that probe finished, serial gameplay passes. Failed
attempts remain in the receipt and are not counted as successful runs.

## Preserved failure and original limitations

The [pre-import fill002 failure](m0-t479-s6-pre-import.md) is **not repaired or
reclassified as passed**. Its retained trace already contains six successful
application acknowledgements, six completion consumptions and both VM
retirement notifications, but lacks final system exit. The same preserved
old package rerun as oldfill006 now exits normally, as do the three combined
package runs. This narrows the observation beyond initial fill publication
but does not establish the historical timeout's root cause. It remains an
unlocalized S6 failure obligation; no product workaround, longer timeout or
semantic concession was added. This P delivers the tested combined baseline,
not S6 closure or a claim that intermittent normal-exit failures are impossible.

The controlled alternate-host RTC fixture reproduces the previously recorded
67 checks/13 failures. It is **not a passing gate**. The
[T478 S35 disposition](m0-t478-s35-rtc-investigation.md#final-disposition-and-reproducible-verification)
identifies the normal path as the original ROM, whose actual guest tests pass
again here. No revival or repair of that unselected alternate route is added.
Physical optical/audio devices, every original allocation failure and complete
S6 physical input/fonts/graphics are not certified by this delivery.

## Publication and review

Fresh package795498ef-6a71-4fd8-a82c-0f0a7d02e76c contains the ten existing
host images plus newly built VCDEX.DLL. All eleven images, the hook license
and one common manifest are copied to both `assets/release` and
`O:/winnt/system32` and hash-verified. Prior files are backed up with explicit
absence records for the new DLL/manifest. Only exact installed project
process paths are eligible for publication shutdown. The independent addon
remains an S6 candidate and is not copied into System32.

Implementer/reviewer inspection verifies the owner request, preserved commit
history, selected sources, no original-guest mutation, test-only boundary
changes and exact package identity. Documentation governance and whitespace
checks accompany this P commit/push. S6's47-operation I/O completion and the
retained fill002 observation remain active; no T/S closure is claimed.

## Mouse-triggered exit: direct callback evidence

The control022 diagnostic run uses the actual `O:/w31/PATCH/WIN386.CMD`
installation with an isolated package. Frame80 shows normal Program Manager.
The path- and foreground-checked native probe then moves the mouse; NTCON
exits within the following observation and root Run16 reports1067. This is
a reproduced failure, not a passing physical-input test.

`build/t479-s6-livecmd/mouse022.txt` SHA256
`351FC0A30838D2AC83CD2C7CEEE743D16BB98D9B229C2D6996985E2FEE9D0857`
captures these CPU-thread transitions:

- INT33 function0Ch reaches the real mouse provider with ES=C000, DX=0223,
  CX=001F. The original provider stores that segment/offset unchanged.
- The mouse event changes CS:IP from F000:EE03 to C000:0223, SP0236 to0232.
- At mouse_int2 entry, the existing CCPU translation cache resolves linear
  C0223 to physical C0223; all16 captured bytes are00. This is an observed
  mapping, not an assumption that every C000 address always denotes ROM.
- The return has SP0238 instead of0236 and changed flags/registers. Execution
  did not enter the intended protected callback.

The installed `SYSTEM/MOUSE.DRV` and released Mouse31 are identical SHA256
`F4A4C4F742161AAF7E83441C5440ACEA7C20B73BA6B4EEC0EFF1D5A8F214A36E`.
The NASM listing locates its callback at code-relative0223, beginning669C6660
(PUSHFD/PUSHAD), not zero bytes. Its authored Enable path registers a protected
ES:DX through INT33/0Ch and assumes a reflector will translate it. The observed
real-mode dispatch violates that assumption: the immediate failure is an
invalid real-mode mouse callback target. The precise reflection failure and
the bridge-enabled versus bridge-disabled difference still require attribution;
this evidence does not justify changing CCPU/PIC semantics or declaring all
Mouse31 registration paths broken.

Temporary diagnostic source edits were restored; no production fix or new
publication is claimed. The cached NTVDM image is diagnostic and must be rebuilt
from clean product source before any release. Original guest bytes are unchanged.

Control023 uses the same diagnostic host and a recoverable installed copy with
only the bridge device line disabled. After serial1/2/4 Ignore and a captured
normal Program Manager desktop, the path-checked mouse probe reproduces the
same C000:0223 dispatch, zero mapped bytes and two-byte return-stack mismatch.
`mouse023.txt` SHA256 is
`17C1799C029661461CCE9324385EF43DB9DBCD5349ED92324D08DE0861F050D1`.
The earlier disabled-bridge survival observation is therefore not evidence of
correct callbacks. The new bridge is not necessary for this demonstrated
registration defect. Control023's initial observer stopped because its output
directory was absent; the existing live guest was retained, the directory was
created and serial dialogs were handled using the same observer, without
restarting or declaring the guest failed on a timeout.

The historical DPMI0303 plus conventional-memory VM-ID gate candidate was
rebuilt in `build/t479-s6-livecmd/mouse31-vm-candidate.asm`, with its stale
guard-segment field cleared on successful free. Its uninstrumented control024
exits1 during startup, before desktop input: it is not a repair candidate
eligible for publication. It was installed only in `O:/tmp/w479m`.
Subsequent diagnostic-only control027 preserves the entire50-byte simulated
interrupt image around stage markers. DPMI0303 allocation returns, and both
INT2F mode/VM-ID queries return; the last marker is immediately before
GlobalDOSAlloc, with no post-call marker. Controls028/029 capture the imported
call target as0117:72D4. The existing NE import is therefore populated, not a
null/unresolved pointer; why that call exits remains open. Control026 markers
did not preserve the register image and are not used as semantic proof.
Controls027/028/029 trace hashes respectively:

- EFD08861A941279E17CE05196A22830E91A390187AEB2A0FB1592FAA17064365
- 7E3B47520B503B9E2A3E274EDFBB6AF8D1A341339F29BF78A681516501C9CA6A
- 3C278B49147FE4CB453FB171D6AF45AD1D1A6E74E92FA81F00659863D6423E8B

No original guest or production source was changed for these controls. Actual
cross-VM delivery, normal driver cleanup and the new candidate's startup are
still unpassed obligations; earlier mock or standard-mode passes do not cover
them.

Control031 adds exception-address observation through the existing host hook.
It records vector06 at FEE5:0CC7 / FE4E:1637 (the same linearFFB17), but the
subsequent working-desktop control032 also records these exceptions. They are
not independently sufficient evidence that GlobalDOSAlloc itself is defective.
The call-stage boundary remains the narrower established observation.

Control032 rebuilds the old authored DPMI0303 driver from147e75626 solely as a
diagnostic comparison (SHA256
186AD08C4E75BA528001D29C102797B05901BCD74C720334D5859EB53CF390C7).
It reaches Program Manager; owned physical movement leaves NTCON alive and
the guest arrow visibly changes position. Captured post-input surface SHA256
3AD69E3ED0044FA5D33709A99B14C037AD594E3F95062FAB43DB103BDE95FF40.
This supports the callback-registration correction direction, not publication
of the old driver: its cross-VM and nested-register-image defects remain.
The owned diagnostic cohort was then stopped with exact executable-path
identities recorded in `control032-stopped.json`. Production source is restored;
the cached NTVDM and isolated package still contain diagnostic builds.

The candidate import failure is now attributed: the installed KRNL386 export
table places ordinal120 (UNDEFDYNLINK) at offset72D4 and ordinal184
(GlobalDOSAlloc) at139A. The captured0117:72D4 was the failure entry, not a
resolved allocation function. The authored NE image put the KERNEL module name
at import-name offset0. Original `kernel31/ld.asm` LMImports skips a zero
module-name offset. Adding the reserved empty import-name entry fixes this
dependency; no original KERNEL change is involved.

Control033, with that correction and bridge disabled, reaches Program Manager
and accepts an owned physical mouse move: NTCON remains alive and the arrow
visibly moves. The cleaned product-source candidate builds byte-identically to
that tested candidate, SHA256
9D024CEAB6944A34D0275B6ED070C51DE112EF326DE7E4271069223D4F3E2FA7.
It replaces protected0Ch registration with DPMI0303 plus the VM-ID gate and
preserves the interrupted register image. Existing direct-callback fixtures
must be updated to the actual DPMI entry ABI; bridge-enabled execution, DOS
child VM switching, repeat enable/disable and complete release gates are not
yet passed. This is an implemented candidate, not a published fix.

## S6 mouse / VM lifecycle continuation, 2026-11-06

Owner approves a minimal independently authored `src/addon/insignia-386`
companion because original INSIGNIA.386 is absent. It must reuse the original
host NIDDB state owner, not duplicate mouse/device state in Mouse31 or the
NTVDM64 task/I/O bridge. Original guest binaries remain immutable.

Control038, clean published host package and candidate Mouse31, completed
Program Manager Run -> COMMAND -> actual DOS prompt -> EXIT -> Program Manager.
Physical mouse motion also moved the guest arrow. The subsequent DOS transition
after mouse activity did not yield its expected prompt and remains failed.
Some earlier keyboard observations were confounded by the SendInput probe losing
foreground mid-string; the captured `comma` text proves that partial delivery,
not an independently demonstrated guest keyboard failure. Temporary graphical
window disappearance alone is not failure: the first transition had a real DOS
Console prompt and returned successfully.

Control039 runs the existing `win31_mouse_dosalloc_probe.asm` with
MOUSE31_VM_CHILD and `win31_mouse_vm_child.asm` in the disposable installation.
The parent recorded GlobalDOSAlloc selector1507 / real paragraph1708, wrote5AA5,
launched the DOS child, observed its result and freed the allocation successfully.
The child read0000 at1708:0000. Therefore the current Mouse31 conventional-memory
VM-ID gate is not a proven cross-VM executable entry and must not be published
as such. Records are M31ALOC.BIN (six18-byte phases) and M31VM.BIN (8 bytes).
This is direct mapping evidence, not yet proof that every observed exit has
that cause.

Source ownership: `mouse_io.c` allocates MM_INSTANCE_DATA through original
NIDDB; callback addresses/masks and saved IRQ registers are instanced there.
`virtual.c` owns snapshot/create/select/retire/restore. Device BOP wrappers call
virtual_swap_instance. Current test SYSTEM.INI has no INSIGNIA companion, and
NTVDM64.386 only implements its separate command/I/O protocol. The new addon
must supply the missing VM lifecycle and BIOS_VIRTUALISING_BYTE contract.
The old Device_Init selector-writing path and the unconditional unsupported
ClearInstanceDataMarking System_Exit adapter require explicit boundary audit;
they must not be enabled blindly or replaced with success stubs.
### Minimal companion controls 041--050

The independent companion now registers low V86 address04AC with VMM
_AddInstanceItem, not CB_High_Linear+04AC. The latter diagnostic was wrong:
control043 failed with native C0000420 / instance1 after child retirement.
Control045 records system identity0, child1 and restored system0 directly;
M31ALOC.BIN is120 bytes and M31VM.BIN is9 bytes. The same run completed
normal Windows shutdown: BOPs3,9,0B,17; actual root Run16 exit0. These are
observations of real VMM instancing, not proof from an isolated host mock.
Control044's earlier FF exit remains a failed record, not silently reclassified.

Mouse31 no longer uses the disproved GlobalDOSAlloc real-memory VM gate or
KERNEL imports. It uses DPMI0303/0304 and preserves the interrupted I/O image.
The guest mock, DPMI test, full-register context test and Windows packet probe
are being updated to that entry ABI; fixture assembly/NE audit passes, runtime
acceptance remains in progress.

Controls048 and050 report the real provider's INT71 through MouseGetIntVect.
Both reached the actual COMMAND prompt, switched it windowed using explicit
Alt-down/Enter-down/Enter-up/Alt-up, accepted physical mouse input and window
keyboard EXIT, returned to Program Manager, and completed Windows shutdown.
Actual root exit0 and BOPs3,9,0B,17 are recorded; control050 also observed native
NTVDM exit0. The project provider installs INT71 in mouse_io.c, and the original
MouseGetIntVect ABI requests the mouse vector, not ownership of its PIC handler.
The candidate now returns71 when its provider exists and FFFF when absent.
The WOW thunk's no-mouse return is not enhanced-mode hardware ABI evidence.

These controls do not establish that every earlier failure has the same cause:
control046 included an extra synthesized r in its Run command; control047 used
Console input while windowed; control049 used the same window input as048 but
remained gray after child retirement/system mouse motion. Preserve these failed
receipts. Windowed DOS text was visibly garbled even in successful lifecycle
controls. NTVDM64.386 remains disabled for this isolation; bridge-enabled cls,
dir,ver, Win95, full gates and coherent publication remain outstanding.
No current result authorizes marking S6 complete or replacing the default package.

Owner clarification, 2026-11-06: garbled windowed DOS text is a known issue and
not a blocker for this mouse/VM lifecycle repair; do not expand this packet
to investigate it. Mouse, VM switching, actual commands and normal exit remain
required.

### Final Win3.1 integration and callback fixtures

Control053, with both NTVDM64.386 and the NIDDB companion enabled, runs the
real COMMAND DOS child, switches it windowed, accepts physical mouse motion,
and executes CLS, VER and DIR /B with output and a subsequent prompt. EXIT
returns to Program Manager; mouse motion still works; normal Windows exit
returns root0 with NIDDB3,9,0B,17. Control055 repeats this sequence on the
latest companion build (explicit Create_VM initialization of the per-VM
created flag); root0 and the same lifecycle notifications are recorded.
Control054 uses that latest companion and the real Win16 USER packet probe:
all seven36-byte M31WIN records have zero errors, coordinates and button
transitions are checked by the probe; actual mouse and normal exit also pass.

The current companion is4652 bytes, SHA256
BE8D709A604BF82D79801ADF7A5531E228450F28BEA49CD51BA49B1BDEFE4E45.
It initializes the child control-block flag at Create_VM so a failed child
creation cannot inherit the system VM's registration flag. Allocation and
snapshot ownership remain in original host NIDDB. Failure injection of VMM
allocation is not yet demonstrated by these positive lifecycle controls.

Final callback fixture batch056: actual CCPU mock62 checks and prior-owner65
checks pass; nested callbacks, all32-bit-GPR preservation and actual DOSX
DPMI client fixtures also exit0 with their required markers. The initial050
mock timed out with the agent environment;051/052 and056 use the already
successful logged-on-user environment and pass. The failed050 record remains.
No change to product environment policy was made to obtain this test result.

Win95 follow-up prepares only a disposable copied installation, reusing the
S4 length-preserving REG_SZ relocation witness. Original guest executable
hashes must match the input copy. Full host cached build and delivery gates
are still running/pending; none of these focused results means publication.

Control057 used a previously altered Win95 test tree and stopped at a registry
error before mouse acceptance. It was closed without invoking recovery.
Control058 instead copies the historical successful O:/WINDOWS source with
hidden/system files and length-preserving hive relocation to O:/tmp/k95.
Original VMM32/KRNL386 executable hashes match; only copied configuration and
independent mouse/companion addons change. Win95 reaches Explorer, actual
mouse moves and the Welcome window closes by keyboard. Shutdown invokes
NIDDB17 and exits NTVDM/root with1, matching the historical S4 result but not
a zero-exit pass. Same-copy companion-disabled control059 is in progress.

Cached x86/x64 host builds and both14-test shared-library CTest selections
pass (cached055.exit=0). Full product delivery verification r058 is running.
No default assets or live system32 package has been replaced.

### Callback return-image preservation and final control063

The DPMI callback register image is shared by nested invocations. The latest
Mouse31 saves the adjusted outer return image on its stack alongside the
existing I/O image and restores both before IRET. Reentry fixture061 verifies
outer IP/CS/SP after a nested invocation with a different return address;
M31REENT, M31CTX and M31REAL each exit0. The negative062 binary removes only
return-image preservation and exits0x44 at check68 (outer return IP). This is
a behavior-changing correction, not merely a shorter implementation.
Latest MOUSE31.DRV SHA256:
275D10A3E22946F83416BED9D263B22D2579C38D42E4630616D5B97ECF8A738B.

Control063 uses this exact driver with both NTVDM64.386 and companion053.
Actual windowed DOS mouse motion, CLS, VER and DIR /B complete; VER and DIR
return their output and subsequent prompt. Child EXIT returns to Program
Manager, system-VM mouse motion remains live, and normal Windows exit returns
root0. Original NIDDB notifications3,9,0B,17 are recorded. Initial input
attempts during concurrent Full desktop tests were rejected/partial and are
not passes; the command line was cleared and the sequence completed after
Full finished. Garbled initial window text remains owner-known/nonblocking.

Win95 same-copy baseline060, companion disabled and keyboard-only normal
shutdown, also returns1. Thus control058 exit1 is not evidence of a new
companion-only regression, nor a zero-exit acceptance. Control059's abnormal
exit is retained without unique attribution. The newest callback-image fix
has not yet been repeated on Win95; full Win95 child-VM lifecycle is unproved.

Cached055 x86/x64 builds pass. r058 Full finishes with exit0 and16/16 selected
gates passed (433628ms). The first wrapper stopped after staging; the resumed
runner explicitly selects the existing Node/Python tools and completes the
remaining gates. This is not evidence that the first wrapper passed. Package
0e6d5811-5069-48ee-b54f-7c89e319698c remains staged, not yet published. Full host
gates do not substitute for addon-specific proof or complete S6 I/O coverage.

Control064 repeats Win95 with the exact latest Mouse31 binary061 and companion053.
It reaches Explorer, relative mouse input succeeds, Welcome closes, and normal
shutdown delivers NIDDB17. Native NTVDM and root both return1, matching control060.
The initial combined capture probe lost foreground after capture-click; that
attempt is not a complete pass. A subsequent owned relative-motion probe succeeds.
No claim of full Win95 VM lifecycle or zero-exit success follows.


### P3 coherent publication and review

Publication065 copies all11 staged r058 host images, hook license and one manifest
to assets/release and O:/winnt/system32; every destination hash matches package
0e6d5811-5069-48ee-b54f-7c89e319698c. Prior coherent files are backed up in the
owned integration publication-backup065. No live default project process was
present at the final preflight. Addon-publication065 independently backs up and
publishes Mouse31, insignia.386 and their bounded-candidate manifest only to
assets/release. No addon is installed into host System32.

O:/w31 originally held the old authored Mouse31 F4A4...A36E. With no active
project runtime, deployment backs up that driver and SYSTEM.INI, replaces only
the authored driver, adds the authored companion to PATCH, and registers it in
386Enh. The two addon hashes match assets. Original Microsoft guest executables
remain unchanged. Recovery inputs are build/t479-s6-livecmd/w31-backup065.
This deployment is not an additional original-installation runtime acceptance;
real controls063/064 used isolated installations and package ne30, while Full058
verifies the newly cached host package. Source changes in this repair are addon
and fixture changes only, with no production host/mirror modification.

Self-review checks DPMI stack/image restoration, dependency-ordered callback
release, provider-vector reporting, per-VM created initialization and retirement,
low04AC instance registration and original NIDDB ownership. No mirror diff is
added or removed. VMM allocation failure injection, full Win95 VM lifecycle,
and S6's remaining complete I/O/nesting acceptance remain explicit boundaries.
The bounded mouse repair does not close S6 or claim the full INSIGNIA ABI.
