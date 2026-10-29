# S10 P: independent mouse31 acceptance in progress

The owner directed acceptance of the existing independent mouse31 change before
resuming host-source reconciliation. The original Windows and DOS guest media
remain unchanged. The source SHA-256 is
`D05C763D46DEEDF456C7F68B271625385CBE3C30D5AFF14D07042E1E72E505CF`;
the independently rebuilt driver at that initial checkpoint is
`7F8651B163311EC0957FC0301F577345190C8E29EC83A45D131BB69BC622E76A`.
The build script checked Win3.1 NE structure and exports; this is not runtime
acceptance. Build run: `build-win31-mouse-driver.ps1`, 2026-10-28 17:22 PDT,
owned PID 6868, exit 0, `build/t473-s10/mouse31-owner-build3`.
Those hashes describe the initial candidate, not the later reentrancy edit.

The pre-existing separate-worktree evidence `build/w31isolated/driver-build4`
records the same source and driver hashes, and `mock-final` records 55 mock
checks and NTVDM completion. `std-final2` also records cursor motion and a File
menu click. These are useful bounded observations on that package, not proof
that the current installed package, protected callback coexistence or all
Windows modes pass. Retain hashes when copying or archiving those observations.
Both `std-witness/M31WIN.BIN` and `enh-witness/M31WIN.BIN` pass the current
report checker: six direct protected callback calls reached USER with expected
coordinates and both button transitions. The witness explicitly calls the
callback itself; it does not establish registration or hardware delivery.
After both package copies were assigned the same temporary UUID, nine of ten
host images were byte-identical; `ntvdm.exe` remained different. The comparison
operated only on disposable copies (`mouse31-identity-compare.ps1`, PID 26656,
17:31 PDT, exit 0). It does not explain the runtime difference or authorize
publishing the earlier package.

The mock has been strengthened to record the protected callback pointer and
mask separately and deliver events through the registered pointer; it now has
60 checks. A hidden standalone DOSBox diagnostic (PID 21488, 17:26 PDT, exit 0)
and the current ten-image package with a fresh UUID on x86 CCPU40 (PID 9400,
17:37 PDT, exit 0) both pass all 60. The NTVDM case was run via
`verify-win31-mouse-mock.ps1` and recorded in
`build/t473-s10/mouse31-owner-current-mock-exact/mock-verdict.json`. These are
mock logic evidence, not real DPMI, Windows, or physical input acceptance.
The mock still models the real-mode reflector and protected registration and
is not a complete VMD.
The separate pre-existing protected-owner variant `M31PM.COM` ran on the same
current CCPU40 package (runner PID 2648) and exited `0x3d` at check 61:
the saved protected callback was not restored. The new
`verify-win31-mouse-prior-pm.ps1` correctly fails this outcome. This fixture
models the DOSX ownership chain; the independent real standard-mode ownership
test described below subsequently reproduced the same loss.
An isolated Win3.1 standard-mode `M31SWAP.EXE` diagnostic also installed a
known protected handler and called protected `INT 33h/14h`. Expected prior
`0DAF:00CB`, but the returned address was `0000:0843` (mask `001F`); the
diagnostic then reinstalled the current driver callback. This rules out
blindly using that exchange result as the old protected pointer in that run.
The enhanced-mode diagnostic produced no report in repeated 30-second and
60-second observations; its COM1 warning was acknowledged in the captured
session. These attempts are inconclusive, not proof of its interface behavior.
The probe source is `tests/observation/win31_mouse_swap_probe.asm`;
generated binary and reports remain under `build/t473-s10` only.

NTVDM isolated runs using the current ten released images and a unique broker
identity timed out for the existing old mock, updated mock, and independent
MEM control, before a console result. Repeating at the earlier successful short
`O:\tmp\m1` path, with the prior observer executable, and with a reduced
inherited environment did not establish the cause. These are non-passes; they
do not prove a mouse-driver defect or a missing desktop. The first copied
configuration contained existing `O:\winnt` SETVER references; no editor
modified the installed configuration, but runtime execution of those references
was not independently excluded. Later attempts used only package-local
DOS/HIMEM/DOSX inputs. No driver was published.
Installed `SETVER.EXE` still matches the pre-existing `latest-source-release-r3`
package copy (SHA-256 `7D8B0765C302358117186D93283AB70D2D0DC72852757AFB4CFEECA69FD74653`);
this comparison does not prove that no other SETVER state could have changed.
The earlier package was also reconstructed as a separate temporary diagnostic
with a fresh UUID and the same package-local configuration; its MEM, old mock,
and new mock all timed out. This does not identify the cause or establish a
failure of the mouse driver.
Reconstructing the previously successful full DOS startup configuration while
removing only its external SETVER references, and running the exact prior
55-check fixture (`DDCDF9D1…`) in a fresh isolated package, did pass. The
updated 60-check fixture then passed on both the comparison package and the
current released package. The precise difference between the reduced and
full startup configurations responsible for the earlier timeout is not yet
isolated; those earlier timeouts are not driver failures.

Current-package Windows 3.1 standard and enhanced isolated installation copies
both loaded the hash-matched driver and passed their `M31WIN.BIN` USER state
checks (`mouse31-window-STD` and `mouse31-window-ENH`). They explicitly invoke
the callback directly and therefore do not cover real device delivery or the
pre-existing protected-owner case. These do not negate the failing mock gate.
Subsequent isolated input-boundary probes (`mouse31-input-STD` and
`mouse31-input-ENH`, runner PIDs 29640 and 31736) sent relative pointer and
button events through each owned NTCON's production input sink. Both reported
accepted button input and showed Program Manager's File menu open in
`mouse-down-3.bmp` (SHA-256
`ACACE7A9BDF7ABAE31C8E439BA5C203A76C2C875D49303E8FA7CD1BA70483C76`).
This covers downstream production event delivery in both modes, not physical
Raw Input, an already-registered protected owner, child-VM switching or clean
Windows exit. The isolated Windows copy was not the user's installation.

The test-only `win31_mouse_prior_owner_probe.asm` establishes an actual
protected callback in a current-package standard-mode Windows copy, then sends
an input-sink event while that callback is installed. Its no-driver-cycle
control observed one callback; the otherwise matching test that called the
driver's Enable and Disable observed zero. Both input sinks acknowledged the
same initial move, the test program reported phases 1 and 2 and no setup
error, and the driver was re-enabled before the test unloaded. The retained
records are `mouse31-owner-std-control` and `mouse31-owner-std-input`. This
establishes the lost prior callback under the test's controlled ownership
sequence; it does not by itself establish a naturally occurring DOS TSR,
physical input behavior, or an enhanced-mode result.
`tests/observation/check-win31-mouse-owner.py` checks both checkpoints and
the acknowledged input for each run before comparing the two results. On
these retained records it exits 1 (`control=1, after driver cycle=0`).
Synthetic independent fixtures verify pass and fail results, and supplying
the same directory twice is rejected as invalid. Those fixtures test the
checker, not the driver.

The previously published driver was independently rebuilt from `HEAD` with
SHA-256 `186AD08C4E75BA528001D29C102797B05901BCD74C720334D5859EB53CF390C7`
and exercised with the same standard-mode input and ownership cycle in
`mouse31-owner-std-original2`. The probe was rebuilt with the original
driver's enabled-state offset (46 rather than the candidate's 42); its
SHA-256 is `FE93BF652537A704706719453E9C3A6BE73F7956ED0C1DEE45413283C0A79885`.
Both phases completed without reported errors, the sink accepted input, and
the prior owner received one callback. The paired checker reports
`control=1, after driver cycle=1`. The first attempted original-driver probe
used the candidate's offset and failed its setup check; it is invalid, not a
driver failure. This establishes a standard-mode regression relative to the
published 0303 path, not that the published path is safe for enhanced-mode
child VM transitions.
The current protected callback also queries function `0026h` using DPMI
`0300h`. The input-sink results above do not establish that the protected
event and that real-mode query address the same mouse instance during a DOS
child VM switch. Cross-VM event and state identity remains a separate
acceptance obligation; do not infer it from the desktop click result.

The original host has a per-VM device-state mechanism, but it is conditional:
`virtual.c` requires its Windows VxD lifecycle notifications before
`virtual_swap_instance()` can select a VM-specific instance. Mouse state is
registered through `NIDDB_Allocate_Instance_Data` in `mouse_io.c`; the
enhanced mouse BOP in `bios.c` invokes that swap first. Merely allocating this
state does not establish that those notifications occur in the current
installation. With the lifecycle active, `delete_NIDDB()` switches the master
pointers to the instance being deleted and frees its storage; the shown code
does not itself restore the master pointers before returning. A later valid
swap or system-exit restoration may repair them, so this is a conditional
lifetime concern, not proof that an ordinary Windows shutdown dereferences
freed memory. Any attempt to revive that mechanism must prove VM creation,
switch, retirement and callback ownership end-to-end; it cannot be treated as
a driver-only fix or justify changing original guest binaries.

`tests/mvdm-host/niddb_object_contract_fixture.c` links the actual cached
`worker/obj/bios/virtual.obj` with controlled register and virtual-byte
boundaries. It initializes a system VM and child VM, switches to the system
VM, and sends the original `VM_Not_Executeable` notification for the child.
The cached object returns with the master pointer equal to the retired child
allocation instead of the still-active system allocation (fixture exit 1).
The fixture compares pointer values only and does not dereference freed
storage. It establishes this host lifecycle defect under the modeled VxD
sequence, but does not prove the current guest emits the sequence, or that the
defect is the sole cause of the mouse regression.

Owner direction: do not use HMA for Mouse31. The following HMA observation is
historical evidence only, not an admitted implementation option.

An earlier independent VM-ID-gated real-mode callback prototype used DOS HMA
and reported desktop mouse and enhanced-mode DOS Prompt observations without
the original cross-VM illegal instruction. Its HMA allocation uses the DOS
`4A02h` high-water allocator; `msint13.asm` advances `FreeHMAPtr` and offers
no corresponding per-block free in that path. Repeated driver unload/reload
therefore cannot be declared safe. Another prototype called Windows
`GlobalDOSAlloc`/`GlobalDOSFree` but exited near allocation, without isolating
the cause. The original `3ginterf.asm` contract returns a Windows memory
handle in AX and real-mode segment in DX; it does not by itself prove that
such an allocation is shared or accessible at the same address across the
system and child VMs. Neither prototype is production code or an acceptance
pass. A valid replacement must establish allocation and freeing, address
visibility in both VMs, VM-ID reuse, in-flight callback retirement and the
existing prior-owner contract before replacing the published path.

The separate `win31_mouse_dosalloc_probe.asm` (probe binary SHA-256
`289CCDABFE25C7B0552857A36F8A1BC4D978C90B3D88FD2686910465344354C9`)
called the real Windows KERNEL ordinals 184 and 185 from an ordinary Win16
application in an isolated standard-mode Windows copy. Its three checkpoints
recorded handle `0DF7`, real-mode segment `1FAC`, and free result zero;
`mouse31-alloc-STD/M31ALOC.BIN` retains the data. This shows that ordinary
application allocation/free works in that environment, not that it works in
the mouse driver's initialization context or shares an address between VMs.
The matching enhanced-mode run produced no report in the 30-second
observation; it acknowledged a COM1 warning. It is inconclusive and does not
establish an allocator failure. Both tests used the previously released
ten-image package with a fresh private identity; the owned copy was removed.
The extended diagnostic, SHA-256
`9198330AC673379173E1F0A9AF3547F16D7DD37FF9BB458F446AC4E7AE58AD56`,
also loaded the returned handle as a protected selector, wrote and read
`5AA5h`, then freed it successfully in standard mode. The four checkpoints
are in `mouse31-alloc-write-STD/M31ALOC.BIN`. This establishes writability
for this application and provider, not the driver's init-time call or
cross-VM memory visibility.

The independently authored `win31_mouse_dosalloc_probe.asm` also has a
`MOUSE31_VM_CHILD` diagnostic build. It keeps a DOS allocation live, starts
`win31_mouse_vm_child.asm` with the returned segment, and records whether
the child sees the parent's `5AA5h` marker. Both source files assemble;
this has not yet produced a child execution result and is not evidence that
the conventional-memory address is shared across VMs. The DOS child is a
disposable probe; original guest images remain unchanged.
The child was subsequently run standalone through a fresh private NTVDM
package with a PIF argument `B800`. It exited zero and wrote `M31V`, segment
`B800h`, value `0000h` (`mouse31-child-standalone`); this validates the command
tail parser and report shape, not parent/child memory sharing. An independent
DOSBox attempt did not produce a report and its owned process was retired
after the recorded no-progress budget; it is not counted as a pass.
The parent/child build was then tried in an isolated enhanced-mode copy
(`mouse31-vm-visibility-ENH`). It passed the observed COM1/COM2/COM4 warning
sequence and briefly produced a graphics window, but neither parent nor child
report appeared in the 60-second observation. This is inconclusive about
shared memory: the parent did not establish that it reached its allocation
or WinExec calls. The owned installation and runtime copies were removed.
A separate standard-mode run (`mouse31-vm-visibility-STD`) reached parent
checkpoints 1-4: GlobalDOSAlloc returned handle `0DF7h`, segment `1FB4h`,
the parent read back `5AA5h`, and WinExec returned `0E36h`. No child report
or later parent checkpoint was produced; the owned NTVDM process was no
longer present at a subsequent process snapshot, without a captured exit
reason. This is not proof of cross-VM visibility or its cause. The original
scratch runner returned zero merely because the partial parent report existed;
that exit status was not a pass. The runner now requires both reports and
`check-win31-mouse-vm-visibility.py` validates all six checkpoints, allocation
identity, marker, child launch/completion and release. Synthetic pass and bad
child-marker cases check the verifier itself, not Windows behavior.
After adding pinned-process exit recording to the observer, a repeated
allocation run captured NTVDM exit `000000FFh` after the fourth checkpoint
(`mouse31-vm-process-exit-STD`). A matching control build that skipped
GlobalDOSAlloc and launched the same child with segment `B800h` likewise
reached WinExec and captured NTVDM exit `000000FFh`, with no child report
(`mouse31-vm-launch-control-STD`). This shows the observed failure does not
require the parent DOS allocation; neither run captured an exception reason,
and neither identifies whether the WinExec request, test program or another
product path is responsible. The control is not a shared-memory pass.
The previously successful `M31WIN.EXE` direct-callback witness was retried in
the same isolated enhanced configuration with 100 ms process discovery and
250 ms requested surface captures (`mouse31-fast-witness-ENH`). The COM1
warning received an asynchronous Ignore message but no witness report or
surface capture was recorded before the observation ended. Thus even the
earlier witness cannot presently be reproduced with this setup; shorter
capture intervals alone do not establish the cause.
The earlier successful run's initial console viewport was 75x24, while the
recent run used 120x30. An isolated retry requested the same 75x24 viewport
and retained the existing 9001-line buffer (`mouse31-geometry-witness-ENH`);
it still did not reach the witness report after the COM1 Ignore message.
The console font/maximum dimensions were not identical, so this rules out
neither all geometry effects nor all environmental differences. It does show
that viewport width and height alone did not restore this run.

An instrumented, build-owned copy of the earlier GlobalDOS driver prototype
was then run under standard Windows with the same isolated package. It logged
entry to `probe_guard_init`, returned from the DPMI 0300 call with carry clear,
and did not reach the marker after the `BX != 0` check (`AGH`, retained at
`mouse31-globaldos-diagnostic3-STD/M31DBG.TXT`). Under this test the guard
therefore rejected a zero VM ID before calling GlobalDOSAlloc; the earlier
standard-mode result cannot be attributed to an allocation failure. The
instrumentation writes diagnostic markers through DOS and is not a production
driver. This finding does not establish enhanced-mode behavior or solve VM
identity, allocation lifetime, or cross-VM pointer visibility.

A non-HMA mode-aware build-owned prototype (`globaldos-mode-prototype2.drv`,
SHA-256 `854D856E4F4B0922F5954293A69EAA458C5FE651EA577FA85A347012E7DB75FD`)
uses real-mode INT 2Fh/1600h to detect enhanced Windows, preserves the old
0303 callback path in standard mode, and attempts the ordinary GlobalDOS
allocation gate only in enhanced mode. In an isolated standard-mode run the
prior-owner probe completed both phases and observed one callback after the
driver cycle; the paired checker passed against the positive control. Its
layout-specific probe offset was verified against the assembly listing.
The enhanced run did not reach the probe report during its 30-second
observation; COM1 was ignored. This is not an enhanced-mode pass or a proven
driver failure. A preceding prototype using an imported GetWinFlags call did
not return past that call in the instrumented run; its loaded far pointer was
nonzero (`03AF:72D4`), which does not establish target correctness or the
reason for non-return. None of these research binaries was published.

An enhanced-mode positive comparison using the previously released driver
(`186AD08C...`), not the candidate, likewise produced no diagnostic report
within 30 seconds (`mouse31-alloc-old-enh-control-ENH`). The observer's
additional state records the COM1 Ignore button as visible and enabled and
the dialog as hidden, but still existing, immediately after its click message
was delivered. Message delivery and hiding alone do not prove that the
dialog thread exited or the guest resumed. The comparison prevents assigning
the common startup non-result solely to the candidate; it does not identify
the remaining wait or prove a mouse failure.

For the retained COM1-wait observation, the released and cached NTVDM `.text`
sections are byte-identical (SHA-256
`4F296D30647E923AE1FE9A9AE7E88208067B416958D72DDB1F3FFF1F0C91D02F`),
allowing the cached map to identify the guest execution thread's return site
as `ErrorDialogBox` immediately after `WaitForSingleObject`, and the dialog
thread's return site as `ErrorDialogBoxThread` immediately after
`DialogBoxParamA`. Disassembly and the import table confirm those calls.
This is evidence for the wait at that observation, not proof of a particular
deadlock or of all later attempts.
Source tracing identifies the error path as `host_com_open`: after
`CreateFile` fails it calls `RcErrorBoxPrintf` before setting the per-port
`DisplayError` false and `ReOpenCounter`. Consequently guest execution
cannot proceed past that call until the dialog path returns. The trace does
not yet identify why that dialog fails to return in the affected private
desktop run.

An isolated observer variant posting the Ignore `WM_COMMAND` asynchronously
instead of synchronously sending `BM_CLICK` reached COM1, COM2 and COM4 in
the released-driver comparison (`mouse31-alloc-post-control2-ENH`), whereas
the synchronous comparison had not advanced past COM1. The allocation
program still produced no result. A subsequent mode-aware candidate run using
the same asynchronous method (`mouse31-owner-enh-mode-post2`) likewise
stopped after COM1. Thus asynchronous posting is not established as a
complete or deterministic fix; the shared observer retains the previous
button operation and only adds diagnostic state. Both tests are bounded
private-desktop observations, not mouse acceptance.
An additional scratch observer posted `WM_CLOSE` to the same isolated COM1
dialog, which the original dialog handler maps through `IDCANCEL` to the
Ignore result. Delivery was reported but the released-driver control still
did not produce its allocation probe report within 30 seconds
(`mouse31-alloc-close-control-ENH`). This is not a reliable replacement for
real interactive acknowledgement or evidence of a working mouse driver.

As a diagnostic only, a disposable copy of `SYSTEM.INI` omitted `*VCD` to
remove the serial-warning path. The mode-aware candidate exited before a
probe report (`mouse31-owner-enh-mode-no-vcd`); the released-driver control
also produced no probe report in its bounded observation
(`mouse31-owner-enh-original-no-vcd`). No conclusion about the original
unmodified VCD configuration follows from these runs. The actual user
installation was not changed and the copy was removed.

`MVDM-HOST-DIV-467` is an unshipped host candidate that restores a live
instance after the termination callback and falls back to the snapshot when no
instance remains. The
actual modified `virtual.c`, independently compiled using the product
`cflags` into `niddb-retirement/candidate.obj`, passes the fixture for an
inactive child, an active child, the last instance and
termination callback ownership. The expanded fixture also gives the snapshot,
system VM and child VM distinct byte values, verifies that a newly created VM
uses the snapshot value, and checks that the surviving system value is not
replaced when the child retires. The existing cached-object baseline fails
the first check. These are controlled host-boundary checks; real VxD/guest
sequences and product regressions are not yet proven. A separate Ninja build
of the product-tree object was still running at the last observation; its
outcome must be checked before relying on that build.
The fixture is now available through `verify-softpc-object-contract.ps1
-Contract niddb`. An earlier candidate run with the explicitly selected diagnostic
object passed 17 checks and three termination callbacks, and its wrong-width
ABI compile failed as expected (`niddb-retirement-candidate-final`). The same
runner against the unmodified cached object reported the expected retirement
failure (`niddb-retirement-baseline-final`). The selected diagnostic object
is not presented as a completed product-cache build.
That earlier candidate silently ignored a stale virtual byte, but the BOP
wrappers continue dispatching the device call after the swap function returns.
It could therefore execute an invalid VM's device operation against a
surviving VM's state. The guard was removed during self-review; the current
candidate retains the original requirement that the VxD provides a valid
virtual byte. The earlier stale-byte checks do not validate the current
candidate. Retirement and later VM switching still require real VxD and guest
evidence before release.
The recompiled current candidate passes 15 applicable checks, three callback
observations and the ABI negative (`niddb-retirement-candidate-r2`); no stale
byte is sent into the original `void` swap API by that positive fixture.
The same explicitly selected object was rechecked in a fresh fixture directory
(`niddb-retirement/verification-new`): 15 checks and three callbacks passed,
and the deliberately incorrect 8-byte pointer ABI failed compilation. This
does not replace a product build or demonstrate guest VM retirement.
The current `O:\w31\SYSTEM.INI` read-only inspection lists no explicit
`INSIGNIA.386` entry, and the `SYSTEM` directory has no matching filename.
That does not prove no other device can send BOP B1. The released NTVDM
contains the default-off `MVDM_S42_VXD_BOP_REPORT_PATH` witness; however,
no retained trace has yet established `VxD_Sys_VM_Init`, `VxD_VM_Init` or
retirement during the actual current Windows enhanced-mode run. The host
unit fixture supplies those notifications itself and must not be presented
as evidence that this installation supplies them.
An isolated 60-second run set that witness variable with the released binary
(`mouse31-vxd-witness-ENH`). The run stopped progressing after its COM1
warning and produced neither a witness report nor a VxD trace file. There is
no positive confirmation that the worker inherited the variable or reached
the relevant Windows lifecycle, so this absence is inconclusive.
The standalone `win31_vxd_trace_probe.asm` was then run in a separate private
released package (`mouse31-vxd-positive`). Its deliberately unrecognised
BOP B1 control (`EAX=0000FFFFh`) exited zero and produced the expected trace,
establishing that the same environment-variable transport and witness work
in that DOS control run. It does not prove the separate interrupted Windows
run reached the relevant lifecycle or that its worker received identical
environment data. Original guest binaries were not modified.
The same harmless probe was then run by the isolated Windows profile's
`AUTOEXEC.NT` before `WIN.COM` (`mouse31-vxd-same-session-ENH`). Its sentinel
record was written to `mouse31-vxd-same-session.trace`; at the bounded
snapshot the process list contained one NTVDM worker. No other BOP B1 record
was written before the COM1-warning stall and no Windows witness report was
produced. This confirms that trace transport was active during the initial
DOS session, but does not establish that Windows reached VM initialization,
nor does it rule out notifications in a completed Windows session. The trace
itself does not contain a PID. The isolated copy was removed.
An additional owned direct `cmd.exe`/Run16/MEM control (PID 18332, 17:33 PDT)
also reached its 10-second diagnostic deadline without output; this was a
separate control, not a product crash or a passing test. The test cleanup
retired only processes from its independently identified package.

Review boundaries: the protected function-0Ch registration replaces the
previous protected callback while the retained function-14h exchange saves the
real-mode callback. Exact preservation of a prior protected consumer is not
established, especially because original OpenNT DPMI states function 14h
translation is unimplemented. In the original DOSX `dxintr.asm` 0Ch stores
the protected pointer in `lpfnUserMouseHandler`; the real-mode 14h exchange
in `mouse_io.c` returns only the real-mode pointer and mask. If that pointer
was the DOSX reflector, the old protected pointer is not recoverable from the
returned values; after unload the reflector could target the new driver's
callback. This is a source-proved conditional lifetime defect, not yet an
observed product crash. A separate DPMI-0303 callback avoids overwriting that
global; however, the earlier enhanced-mode DOS Prompt investigation observed
the system-VM callback being delivered in a DOS child VM, followed by an
invalid instruction. Its VM context and in-flight retirement must be resolved
before treating the old route as a complete replacement. Earlier standard and
enhanced desktop mouse successes alone do not cover that DOS-Prompt path.
The Windows direct-callback witness bypasses
the real event-dispatch path. `WEP` also ignores a cleanup failure, an inherited
issue requiring its actual Windows ABI/lifetime analysis before changing the
return. Verify real registration, event delivery, normal and abnormal cleanup,
and standard/enhanced modes against the selected current package before
claiming full acceptance or publishing a new default driver.

One unrelated packaging observation: the installed ten binaries match
`assets/release/release-manifest.json`, while the root
`O:\winnt\product-system32-manifest.json` is stale. The disposable test copy
used the checked release manifest; no installed file was changed by this
manifest reconciliation.

The standard-mode DOS-child launch control was repeated with the process
image base and the existing termination reporter recorded. The parent used
the no-GlobalDOSAlloc control and reached its launch checkpoint, but no child
report was produced. NTVDM exited with `0xFF`; the report recorded
`host-terminate return=006F3D33`, and the pinned NTVDM image base was
`00690000`. Relative address `00063D33` maps to the return after
`host_terminate` in `mvdm_softpc_terminate` in the matching worker map. This
identifies the ordinary SoftPC termination chain, but does not identify who
requested termination or establish a driver fault. In particular, the control
does not use the proposed additional DOS allocation. Both child-VM callback
delivery and the complete standard/enhanced acceptance remain unproven.
The matching binary contains a tail jump from `TerminateVDM` to this cleanup
routine, and its direct call instructions must not be assumed to enumerate all
possible indirect callers. The expected separate `terminate-vdm` trace line
was absent, so this run cannot establish the initiating caller even though the
observed `host-terminate` return site is known.
The original Win3.1 kernel source for `GlobalDOSAlloc` specifies DOS-accessible
memory and returns a handle and paragraph address; both 2.x and 3.x heap
searches reject candidate starting addresses at or above 1 MiB. That check
alone does not establish the allocation's end address. Those statements support
the prototype's conventional-memory use and satisfy the owner's prohibition on
HMA, but do not prove that an enhanced-mode child VM sees the same contents at
that paragraph. The latter remains a separate runtime requirement; the failed
child-launch control cannot be counted as either a positive or negative result.

Separate reentrancy review: the current driver's `mouse_call` uses one shared
`io_regs` register image, and its protected callback writes to that image and
calls `mouse_call` again. If the callback can interrupt an outer operation
after that operation has started using the image, its results may be clobbered.
This is a source-level conditional risk, not yet a proven runtime failure: the
provider's interrupt and reentrancy contract must be established. A scratch
mock that injects a nested event was assembled as `REENTER.COM` (SHA-256
`CDD6B23E69402C571ABC69E5A1C91653DF2FF5520DA61B953B13C117AED40645`). Its
hidden DOSBox process PID 17880 had produced no result when observed; the
process later exited without a report; this is not a test pass or failure.
The independent `M31REENT.COM` variant was subsequently executed through a
fresh isolated NTVDM package (`mouse31-reentry-ntvdm`). It exited `0x40`,
identifying check 64: a simulated callback after the provider writes the
outer version result caused the result to be lost. This establishes failure
under the injected nesting contract, not that a real provider has exhibited
that timing. A scratch candidate saves and restores all 50 bytes of the DPMI
register image on the callback stack. Against the same controlled model it
passed 61 checks (`mouse31-reentry-candidate-ntvdm`, exit 0), and its ordinary
mock passed 60 checks (`mouse31-reentry-candidate-baseline`, exit 0). The
candidate change was then applied to the independent Mouse31 source. Real
protected callback coexistence, enhanced-mode VM safety, stack bounds and the
full product gates remain separately open; the change is not released.
The non-HMA mode-aware research driver was also rebuilt with this buffer
preservation (SHA-256
`047BF54ACF5EE75A2D8CFEF2E2A7E826767F64A9DF0C06B455C60AF919809DE0`).
In a private Win3.1 standard-mode session, its earlier protected owner received
one event after a Disable/Enable/Disable cycle, matching the independent
positive control (`mouse31-owner-std-mode-reentry-phase`). The input sink
acknowledged the same movement and the paired report checker passed. The first
run did not inject input because the desktop-image readiness condition never
matched; it is inconclusive, not a driver failure. The successful diagnostic
used the test program's first completed checkpoint as its input-ready signal.
This is a scratch driver, not the currently implemented protected-0Ch driver;
enhanced-mode cross-VM safety and normal cleanup still require evidence.
To enforce the owner's HMA prohibition in the research-only enhanced-mode
guard, a later candidate (`mode-reentry-lowmem.drv`, SHA-256
`EAAFF4DCDF6ABD4F265A15B008928E9174A123B9A50D14BD9FC54079803BB790`)
checks the complete 33-byte stub before writing it. The assembled comparison
accepts paragraphs through `FFFDh` and rejects `FFFEh` and `FFFFh`; on the
rejection path the recorded selector is handled by the existing rollback.
Listing and arithmetic checks cover the boundary, not execution of the
enhanced-mode rejection path. The paired real standard-mode ownership test
(`mouse31-owner-std-mode-lowmem-phase`) passed with one delivered callback;
standard mode does not execute the enhanced allocation branch.
The same research driver was tried in enhanced mode with checkpoint-driven
input and the scratch asynchronous serial-dialog observer
(`mouse31-owner-enh-mode-reentry-phase`). COM1, COM2 and COM4 prompts were
observed and Ignore was posted, but no owner checkpoint was written and no
mouse injection occurred. This run is inconclusive and does not establish
enhanced-mode callback behavior or attribute the missing checkpoint to the
driver. The probe's asynchronous acknowledgement is diagnostic only.
The subsequent pinned trace (`mouse31-owner-enh-mode-reentry-trace`) reached
the 30-second observation deadline without an owner checkpoint. Its termination
report was written at 19:32:41 PDT, matching the timeout cleanup, and recorded
`terminate-vdm return=0077F3DE` with worker image base `00730000`. The matching
map and disassembly place that return at `CntrlHandler` immediately after
`TerminateVDM` (RVA `0004F3DE`). Thus this particular termination is
consistent with the observer's console teardown, not evidence that Win3.1
or the driver spontaneously terminated. The visible frontend disappeared
earlier, but disappearance is not a captured NTVDM process exit. The trace
does not identify why the checkpoint was absent.
An additional instrumented run (`mouse31-owner-enh-mode-reentry-snapshot`)
recorded five non-atomic CCPU snapshots with the same instruction pointer
`8028D9E3`, IF clear and a pending timer bit; NTVDM CPU time did not advance
between those samples. Paged instruction bytes were unavailable, so these
samples cannot identify the guest instruction or attribute a guest loop.
A separate run with the existing worker thread snapshot
(`mouse31-owner-enh-mode-reentry-threads`) observed the guest execution thread
waiting inside `ErrorDialogBox`; its wait chain led through two more threads
in the same process and reported no cycle. The dialog thread's captured stack
included `msctfmonitor.dll`, and a later thread included COM/RPC modules.
These snapshots support an outstanding host dialog/input-method wait in that
run, not a demonstrated mouse-driver error or a proven OS deadlock. The
observer posted Ignore, but no owner checkpoint appeared before cleanup.
The fixture was extended to deliver a second event while the first callback's
geometry query is in progress. Under the same isolated NTVDM model the current
source passed 62 checks and exited zero (`mouse31-reentry-depth2-ntvdm`); a
scratch source differing only by removal of the buffer save/restore failed
check 64 with exit `0x40` (`mouse31-reentry-depth2-original-ntvdm`). These
negative and positive controls exercise two nested events but do not establish
an upper bound for stack use or prove actual Windows provider reentrancy.
For the original DOSX reflector, `src/mvdm/dpmi/gendefs.inc` defines a
384-byte reflector stack frame, and `dxintr.asm` dispatches the mouse callback
on that stack. The new save adds 50 bytes per callback on top of the existing
saved registers and downstream USER/provider calls. This comparison does not
establish the complete worst-case stack depth or the enhanced-mode VMD's stack
contract, so stack safety remains a targeted verification item.

Owner-directed current-driver DOS Prompt check: an isolated copy used the
current independently built driver (SHA-256
`F4A4C4F742161AAF7E83441C5440ACEA7C20B73BA6B4EEC0EFF1D5A8F214A36E`),
normal serial configuration, Win386 and the installation's own
`DOSPRMPT.PIF`. In the visible-desktop observation COM1/COM2/COM4 Ignore
messages were posted; the MS-DOS prompt appeared, the input `ver` was
observed, and a fresh DOS prompt appeared afterwards. The next input was
`exit`; subsequent captured frames showed the Win3.1 Program Manager
(`mouse31-owner-enh-current-dosprompt-input`). The observer itself marked the
input sequence failed and timed out because its generic DOS prompt completion
contract did not match the switch back to graphics. Therefore these records
support opening the prompt, accepting `ver`, and a subsequent Windows frame;
they do not independently establish VER's output, an acknowledged exit, or
post-return physical mouse function. No old driver, HMA or guest image change
was used in this test.
The current source's `M31CTX.COM` completed under an isolated NTVDM with
`M31CTX:PRESERVED-ALL-32BIT-GPRS` and exit 0
(`mouse31-reentry-context-ntvdm`). A separate real-DOSX `M31REAL.COM` run
reached `M31PM:ENTER` but timed out without its completion marker
(`mouse31-reentry-real-ntvdm`). The wrapper process returning zero is not a
passing result: the observer reports timeout. Its cause remains unverified.
The current-driver test was repeated with a targeted frontend mouse probe
after the `ver` checkpoint and return to the Program Manager region
(`mouse31-owner-enh-current-dosprompt-returnmouse2`). The DOS console again
recorded `ver` followed by a fresh prompt; after `exit`, all four injected
move/button operations were acknowledged. The final framebuffer shows the
Program Manager File menu open (`mouse-up-5.png`), establishing that Windows
processed the post-return movement and click. This probe injects at the
frontend boundary, not through a physical mouse. The generic observer still
reports timeout because it expects a DOS prompt after returning to graphics;
that status is not a full-test pass, and VER's output was not independently
observed. The user-directed COM warnings were handled by Ignore.

The current published ten-host-image package was then subjected to a fresh
Full product run (`mouse31-delivery-full/full`), with all runtime hashes
checked against its current manifest. WOW frontiers and Console-17 passed;
Window-17 failed its first `empty` case. The observer saw the correct NTCON
window and reported ten key messages and one Enter, but `input-delivered=0`
and the launcher timed out. A separate fresh-identity rerun of only that case
(`mouse31-window-empty-recheck`) produced the same result. These host gates
do not load the new add-on and do not establish that Mouse31 caused the
failure. They do block the required complete delivery; no new addon or host
package was published from this run. The cause of the missing input
acknowledgement has not yet been established.
The identical released package and `empty` case were then run on the already
authorized visible desktop using the observer's existing owned-Console mode:
it exited 0 and reported input delivered (`mouse31-window-empty-visible`).
The official case verifier subsequently passed the same mode
(`mouse31-window-empty-owned-gate`). This distinguishes the observed
private-desktop failure from a general failure of the tested product route;
it does not explain the private-desktop behavior. ProductVerification now
offers an explicit `WindowOwnedConsole` option for the Window-17 matrix;
it retains all cases and assertions and records the chosen mode. Calls with
neither a private desktop nor the explicit owned-Console mode are still
rejected before launch. A fresh Full run with that option is in progress.
That run passed WOW frontiers, Console-17, Window-17, RPC and native GUI,
then stopped at version negatives because the deployed manifest has a UTF-8
BOM and the Node parser did not remove it. The version test now strips only
that optional prefix. Its separate real-peer rerun passed all five original
negative cases (`mouse31-version-recheck`); no protocol assertion changed.
The owned-Console mode now supplies the isolated launcher's full path to the
existing observer's exact-NTCON lookup, and the command gate rejects absent or
mismatched identities. The valid isolated case passed
(`mouse31-window-empty-exact-owner`); an incorrect launcher was rejected
before launch. The complete suite is being rerun with these corrections.
That Full rerun passed 13 of 14 gates and failed `nested-window-handoff` on
its private-desktop input path (`full-owned-r2/timings.json`). Its standalone
owned-Console variant first exposed the observer's inherited-console geometry
assumption (exit 94); launching that observer with its own Console passed
geometry but the nested DOS session subsequently presented an illegal
instruction dialog (`mouse31-handoff-owned3`), so the original output and
exit assertions did not pass. The dialog reported CS:IP `03F4:2170` and bytes
`63 69 65 70 5C`. This host-package gate does not load Mouse31. Neither the
guest exception's cause nor its relationship to the private-desktop failure
has been established. No candidate was published from this incomplete run.
The standalone owned-Console reproduction had inherited the invoking shell's
large environment, unlike the Full runner. A read-only live snapshot
(`mouse31-handoff-snapshot/ccpu-later.txt`) found the exception return in the
COMMAND PSP segment, with the fault address containing an inherited
PowerShell module-path string. This is consistent with the separately known
COMMAND large-environment limitation, not proof of a Mouse31 issue. Repeating
the complete nested-window case with the Full runner's minimal environment
and the observer-owned Console passed its actual CMD/DOS input, parent return
marker and exit 23 (`mouse31-handoff-owned-cleanenv`). The test did not alter
guest files. The unclean-environment exception and private-desktop failure
remain retained evidence; neither is represented as repaired.
The host package selected for these gates matches all ten current release
manifest hashes. A Git path comparison from the published Video7 host commit
`16c9ca687` through `HEAD` found no committed changes under the host component
source paths checked. The uncommitted `virtual.c` experiment is not included
in that released package and remains an unshipped candidate.
The final repeated Full run passed all 14 gates with the owned-Console option;
the durable [timings](mouse31-delivery/full-timings.json),
[runtime manifest](mouse31-delivery/runtime-manifest.json), and
[post-Prompt mouse frame](mouse31-delivery/win386-after-dos-prompt.png) are
retained independently of the scratch build directory. Current driver source
SHA-256 is `8BA5FF0E77DE03FF735FF34B7960E7C2DF808E5F479DCD35E131641CD7089288`
and the built add-on SHA-256 is
`F4A4C4F742161AAF7E83441C5440ACEA7C20B73BA6B4EEC0EFF1D5A8F214A36E`.
Setup runner safety passed. Profile generation initially failed in a deep
98-character temporary path; rerunning its unchanged assertions at the short
build-owned path `build/t473-s10/p1` passed. The full host suite does not
exercise the add-on, and neither that suite nor the user's Win386 Prompt
scenario proves the separate real-DOSX fixture timeout or all callback
lifecycle cases. The original deep-path failure remains recorded.
