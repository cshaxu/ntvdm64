# Win3.1 guest bridge integration witnesses

`verify-win31-identity.ps1` builds independently authored DOS/Win16 probes,
copies the installation, installs the candidate add-on into that copy and
uses the existing private-desktop runtime matrix. The original installation
is inventoried with hidden/system files and checked unchanged afterward.
The probe checks actual VMM identities, callback registers/stack/flags,
counted waits, blocked-state wake, notification serials and normal retirement.
It is not a replacement guest component or a complete command/I/O test.

`host-wake.c` is an integration-only link seam for independent-thread wake.
Build the production `mvdm_vm_notification.c` with its BOP symbol renamed
to `mvdm_vm_bridge_bop_direct`, `RegisterEOIHook` to `test_register_eoi`, and
`RemoveEOIHook` to `test_remove_eoi`. Link that object and `host-wake.c` before the
normal worker archives in a separate test executable. The archive's ordinary
notification object must not be extracted; verify both entry-point owners in
the link map. The wake queue, ICA/PIC and CCPU objects stay unchanged.

Only the witness's one operation-9 publication is moved to a native thread.
All other BOP operations use the real implementation. The producer also invokes
the original ICA cancellation under the lock before CCPU can consume its
publication. The EOI observation wrapper delegates unchanged and requires an
actual `count == -1` callback during that cancellation; a successful wake alone
does not prove the cancellation branch ran. This tests the cancellation path,
not a complete guest PIC ICW1 reinitialization. There is no host sleep,
timer, guest-memory access or CPU-thread join. At guest shutdown the witness
checks that the producer returned successfully; a live/failed producer fails
the process. This zero-time handle check is a test assertion, not production
polling or lifecycle policy. The source is not selected by product builds.

During an observed VM-retire BOP, the seam queues a wake under the ICA lock
before delegating to the real retirement implementation. It then requires
that the queue be empty and a late publication for that generation be rejected.
This witness assumes no other queued request at that point. During successful
bridge close it reacquires/releases the exact IRQ and installs/removes an EOI
hook to check that both resources were returned. These are test assertions,
not a production resource-stealing or retry path.

Stage the test executable as a separately identified coherent package and
run the same guest witness. Preserve the test package and source hashes in
the stage evidence. A successful test proves independent host-thread IRQ
delivery to a real blocked guest VM, not NTSRV command completion, frontend
registration, original BOP continuation restoration or production acceptance.
Those contracts still need their actual owners connected and verified.

`retire-vm.asm` is a separate test-only VxD (isolated device ID 7f81), never
part of the product driver or release assets. Its API rejects the current and
system VM, enumerates actual VMM handles, and calls Nuke_VM only when the target
has the original blocked flag. `dos-retirement.asm` writes A's identity before
waiting; B destroys A and verifies that a late wake for A is rejected. A must
never return from its wait. The normal Win16 controller waits for all ordinary
VMs to disappear before requesting Windows exit. Supply the assembled helper
through `-RetirementDriver` to select this witness; retain both raw results.

The Nuke_VM ABI is described by the original Microsoft Windows DDK
[Virtual Device Adaptation Guide](https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/).
Its service ordinal and VM_Not_Executeable control ordering are checked against
the read-only Microsoft `ms-dos-6/dev/smartdrv/sdvxd/vmm.inc`. These are interface
research sources only; no source body is imported. Tests exercise Win3.1 itself
and do not infer its behavior merely from the older guide.

`-Reuse` selects a subsequent `dos-reuse.asm` VM after the two ordinary
identity witnesses have disappeared from the driver's actual VMM inventory.
The controller launches its PIF through Win16 WinExec. It requires the new
VM handle to equal one of the earlier handles, with a newer generation;
absence of actual reuse is not counted as success. Old-generation direct and
queued wake API calls must fail, while a new-generation counted completion
must succeed. The 36-byte `VRUS` report is cross-checked against the original
two VM reports, and Windows exits only after the new VM has also retired.
This uses the normal candidate package, not the host-thread test seam.

`-InitializationFailure` uses a separate `host-init-failure.c` link seam:
rename only the notification BOP entry to `mvdm_vm_bridge_bop_direct`, link
the test entry first, and retain the real notification/IRQ implementation.
It fails arm after host reservation and VPICD ownership. The real guest driver
must close the bridge; the seam then reacquires/releases the exact IRQ and
EOI hook and writes a one-shot marker to the runner-owned report path.
The controller requires the failed driver to expose no successful identity,
and the normal Windows startup/exit checks still apply. No DOS witnesses are
launched in this negative case. Marker, matrix and API result are all required;
absence of a functioning driver alone is not a passing failure test.

`build-interrupt-observer.ps1` builds the test-only `interrupt-observer.asm`.
Pass the image as `-InterruptObserver` with a continuation diagnostic host and
`-TraceContinuation -DeferredContinuation`. It chains reflected IRQ0/IRQ1
without consuming either. Diagnostic trace operations 1032/1033 identify the
vector, actual VM and observed CB status. Operation 11 marks request admission;
the first operation 12 bounds the initial waiting interval, not proof of every
subsequent wait. The test host holds each completion pair for 500ms solely to
make that interval observable; this delay is absent from the product link.
No IRQ1 assertion is valid without an actual keyboard stimulus. Observation
alone proves neither input consumption nor eventual native child delivery.

`-Keyboard` additionally requires this observer and a diagnostic host with
operation 17. Each DOS witness creates its RDY file immediately before BIOS
INT16 read; the Win16 controller waits for that checkpoint, restores and
activates its DOS window, then requests one make/break pair through the probe
API. The host calls the original scan-code conversion/controller under ICA;
it does not write the BIOS buffer, fabricate an IRQ or operate host UI. The
runner requires exact A/B BIOS results and normal shutdown. This is stimulus
calibration only, not proof of native child input or delivery during a blocked
command. Historical USER keybd_event attempts failed and are retained as such.

`-BlockedKeyboard` moves A's readiness checkpoint before command admission.
The observer enumerates live VM handles and waits to observe its blocked bit
before sending. A diagnostic event holds completion until that stimulus;
trace18 proves the blocked observation and19 precedes any host application
wake. Compare target IRQ1 callbacks between17 and19. The runner records that
count without pretending the flags0 negative control passed early delivery.
The outer matrix budget still bounds a failed test; no product wait or timer
is added. B retains the ordinary readiness/BIOS-consumption control.

The observer also chains INT15/AH=4F and records the actual VM, scan byte and
client flags. `-ConsumeKeyboard` requires `-BlockedKeyboard`: only the selected
blocked VM's diagnostic receiver accepts its two scan bytes. On acceptance
the observer clears client CF and reports the interrupt handled; all other
input chains normally. A must report an empty BIOS keyboard buffer and B must
still read its calibrated key. This proves the guest interception/consumption
boundary, not a production native input queue or native command execution.
The host trace serializes append opens/writes across CPU and completion threads
so its evidence collection cannot fail with sharing violations.

`-VideoApplication` checks the candidate's palette and both font banks by
guest BIOS/device readback after each continuation. `-VideoFontControl` adds
ordinary DOS INT10/1100 loads immediately before the same font assertion. This
is a diagnostic control only: even if it passes, it does not prove that the
driver installed those fonts. The default application case retains its strict
readback without this intervention. Neither case proves production native I/O.
The control checks the height it actually requested, not a later BIOS report:
original font loading restores mode tables and can change that report. Optional
diagnostic links may rename video_io/ega_video_io/mouse_video_io in bios.obj,
and c_sas_store in ega_vide.obj, to the test host observers. Use each original
file's exact Ninja flags. These observers call the original functions; font
store sampling reads only the first byte of each bank after the real write.

`-VideoFontDirectControl` is a separate diagnostic: the DOS witness writes the
font planes with guest MOV instructions after virtual VGA register selection,
then runs the full readback. It excludes the BIOS control and does not prove
that the driver's application worked. `-FullscreenVmControl` only requests the
original PIF full-screen bit; it is not evidence that VDD actually granted
foreground/full-screen ownership. Default acceptance enables neither switch.

`-RealCommand` requires `-Continuation`. It uses production BOP entry,
registration, capture, executor and VM frontend / NTSRV / Run16 / NTVWM
execution. Falling back to the diagnostic CALLF interposition is a failure.
The two DOS witnesses request CMD exit codes 37 and 38 through both54:08
forms; the approved54:0A `run16 command` route receives fixture `exit` input
and returns0. Returned registers, flags and stacks are checked. Input is a
translated-key fixture, not a physical guest keyboard. This mode does not
claim full input/font/caption support and cannot be combined with diagnostic
video, keyboard or deferred-completion variants. Earlier operation0x355 traces
belong to the pre-production provider wrapper, which has now been removed.
The fixed-result three-call diagnostic remains separate; AL=25h is not native
execution evidence.

`-RetireCommand -RetirementDriver <retire.386>` uses the production worker:
VM A starts host CMD, which publishes a readiness file before waiting in
PAUSE. VM B must observe that file before the test VxD destroys blocked A.
The test rejects a late wake, checks both reports and normal Windows exit.
With `-Reuse`, the controller starts a new VM after retirement; it must reuse
an actual old VM handle with a new generation, reject the old generation,
and execute another native command returning37. The runtime matrix checks that the marked native CMD is absent before scope
cleanup and records that observation. This does not force the new command to
overlap an old frontend's pending retirement or prove all worker retirement.

`-OrderedCommandImage <ordered-command.exe> -FirstCommand A|B` extends
`-RealCommand` with two real native processes per54:08 round. The first
publishes its PID and waits for the second to open that process. The second
waits on the actual process handle and validates its exit code before exiting
itself. Per-fixture/per-round named objects prevent cross-run pairing. Both
orders retain the guest register/flags/stack checks;54:0A still uses the
translated input fixture. This proves native exit ordering, not a prescribed
ordering of subsequent guest callback scheduling.

## Real native unread-input handback

The continuation witnesses execute test-only BOP54:254 markers. They require a
separately linked host-continuation.c diagnostic executable, not an ordinary
published NTVDM. Rename the notification entry to mvdm_vm_bridge_bop_direct and
interpose CmdDispatch in the test-only nt_bop object. Preserve its link map and
never publish that diagnostic executable. In RealCommand mode, command admission
and input BOP16--18 delegate to production; old keyboard diagnostics cannot
return fabricated success for these requests.

Compile tests/observation/win31_vm_unread_input_child.c as an ordinary native
CUI test executable. Run verify-win31-identity.ps1 with Continuation,
RealCommand, UnreadInputImage and UnreadInputMode (sequence, repeat, function or
extended), plus the usual addon/package/install/observer/output paths. Each
invocation needs fresh output and installation roots. Invoke each in a separate
modern pwsh process: the existing runner narrows its environment for DOS.

Both VM parents run the real child twice through BOP54:08. The child leaves
synthetic unread INPUT_RECORDs in its own Console; the parent checks BIOS
peek/read results after actual command return. Sequence uses20 alternating
characters with different values for A/B. Repeat checks20 occurrences from one
repeat-count record. Function uses standard INT16 F1; extended uses extended
INT16 F11. Every word must survive two peeks, be consumed once, match its scan
and character, and leave no extra word. Native reports and guest results are
retained. BOP54:0A separately uses the existing input stimulus to exit its real
interactive shell. These are input-handback proofs, not physical keyboard or
complete interactive-I/O acceptance.

VM lifetime concurrency is proved by distinct generations plus actual cross-VM
wake of a peer reporting blocked+waiting0x401. The DOS .timing intervals begin
after commands and this wake; they must not be compared as lifetime intervals.
Command execution overlap and opposite exit orders use the separate ordered
native-command witness.

`-ServiceLoss -OrderedCommandImage <ordered-command.exe> -RealCommand
-Continuation` selects a distinct failure-path witness. Both VM native tasks
atomically publish their PIDs and wait. The matrix pins their handles and the
exact isolated host processes, terminates only that NTSRV and requires exit
before containment. It records serviceLoss separately from normalExit; this
mode does not claim BIOS handback or normal Windows shutdown. A surviving
process fails the matrix, even if the finally block later cleans it up.

`UnreadInputMode oem` checks the existing CP437 path without changing runtime
code: native VM A leaves U+00E9 and VM B U+00E4, each with its scan identity.
Both command rounds must return exact BIOS words1282/1e84, survive repeated
peek and leave no extra word. The child rejects a non437 Console prerequisite
and records its actual code page. This is non-ASCII single-byte handback, not
DBCS, arbitrary code-page switching or physical keyboard coverage. The unit
suite separately proves an unrepresentable U+4E00 returns a conversion error
without consuming the original queued record.

`-GuestGridControl -GuestGridSwitch -GuestCustomFont` with real ordered
commands loads glyph65 through original INT10/1110: VM A uses3c bytes and VM B
uses66 bytes, with8/16-pixel grids changing between continuations. The probe
queries actual BIOS geometry after loading, reads virtual VGA font memory to
prove those bytes were installed, and compares both complete font banks and
text palettes after each command. Run both `-FirstCommand A` and `B`. This
proves guest custom-glyph preservation, not native-requested font replacement,
DBCS or preservation across a separately requested native resize. Original
INT10/1100 restores mode registers through load_font; do not assume it retains
the prior row count (retained failing fixture: artifacts/t481-s3/custom328.json).

`-GuestCodepage 437` (or850 on a matching host) with independent real ordered
commands verifies DOS INT21/6601 before/after native commands and BIOS cell
readback of Unicode U+00A2. The native child deliberately sets its own output
page to850; the guest cell must remain9b for437 orbd for850, with attribute2f.
The original cmdconf.c filters CONFIG.NT COUNTRY settings and synthesizes them
from GetOEMCP; the runner must not pretend a COUNTRY override selected850.
A prerequisite mismatch fails explicitly before child launch. This test does
not alter machine locale, original guest binaries or production code, and is
not DBCS or guest-runtime codepage-switch acceptance.

`verify-win95-editors.ps1` runs the same two-prompt editor interactions in a
disposable Win95 installation copy. Supply the authored VxD, INSIGNIA driver,
test interrupt observer, diagnostic real-command package and console observer.
The copy path must be an unused short `O:\tmp` path of the same length as the
source, so `relocate-win95-hive-equal.py` can update copied registry values
without changing their extents. The test leaves the source installation
unchanged, assigns conventional memory explicitly in its PIFs, and checks
that native and DOS editor menus belong to different VMs and both commands
complete. `-MouseMenu -SaveWitness` also verifies guest mouse menu choices,
pointer receipts and saved contents. Win95's `ttyGrab` child, rather than
its toolbar-containing parent, supplies mouse geometry. For a copied guest
whose welcome window covers the test, `-DismissWelcomeTitleHex` optionally
specifies its exact guest-ANSI title bytes; no other window is closed. The
test records the observer exit code without claiming normal Win95 shutdown.

`verify-win95-identity.ps1 -Reuse` uses the same disposable Win95 copy and
checks two concurrent VM identities, 320 callbacks, actual handle reuse, and
old-generation wake rejection. `-InitializationFailure` instead requires the
test-only `host-init-failure.c` link and verifies that a failed driver arm
rolls back the actual host IRQ and EOI hook; no DOS identity witnesses run.
Both record process exit without claiming Win95 normal shutdown. The shared
copy and PIF setup is in `New-Win95ProbeCopy.ps1` and
`Set-Win95ProbePifMemory.ps1`.
