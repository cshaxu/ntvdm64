# T481 S7 investigation — initial evidence

The owner reports EDIT.COM mouse menu clicks do not work in the first Win3.1
DOS window, and opening a second DOS window makes the first black and not
recoverable. These are observations, not yet independently reproduced.

On 2026-11-08, read-only inspection of `O:/w31\SYSTEM.INI` found
`mouse=*vmd` and the two project device entries, but no `device=*vmd` entry.
Binary comparisons (`fc /b`) found both installed .386 drivers identical to
their `assets/release` counterparts. The source README for the addon records
that `device=*vmd` was necessary in its isolated mouse acceptance profile;
`mouse=*vmd` alone was insufficient there. This establishes a testable
configuration difference, not the cause of the owner's failure. An earlier
isolated test with the additional entry failed startup while its control
passed; that difference is not yet attributed. Do not add it to the owner's
installation without a verified isolated result.

The existing `verify-win31-editors.ps1` constructs two prompt VMs and can
select `MouseMenu`, `ExplicitVmd`, and `DosOnly`; its result checks menus and
VM identities. Existing positive observations do not establish restoration of
the first visible window after opening the second. The owner-visible black
screen and its cause have not yet been reproduced or identified.

The owner subsequently authorized repair and deployment. The historical
`editor087` mouse pass used `DosOnly` and `ExplicitVmd` with a diagnostic host;
it is not evidence of the reported DOSONLY=0 production path. Subsequent
inspection found `editors535`: `editor535` and `editor536` passed two editors,
mouse menus, saves and both exit orders in DOSONLY=0 with `ExplicitVmd`, using
driver binaries byte-identical to the current releases and diagnostic host
package493. Both prompt windows were opened before editors were launched;
these tests did not reproduce the owner's sequence (open the second prompt
after the first has an active editor), or prove its return display. The PIF tool
confirms the current launch uses O:/w31\WIN.COM /3 and the private CONFIG.NT
and AUTOEXEC.NT. Installed driver SHA-256: INSIGNIA.386
BB964D70FC5FFBE65466E597D0180C614B75FA7A604523CF5F8BC60CB3C3613B;
NTVDM64.386 0E4DA9B0B88FE89B20C30CBD7751EF1C4299316D6317BA40C98116B137A5712B.
Neither failure has yet been reproduced or fixed and the actual installation
has not been changed.

The existing editor integration probe now has an opt-in
`FirstDosBeforeSecond` sequence. It starts only prompt B, waits for its DOS
editor to become live, then starts prompt A through the existing WinExec path
and returns to B during the editor checks. Existing default test order remains
unchanged. Visible-frame restoration still requires explicit
verification against the real window rather than an old snapshot.
The sequence additionally attempts the first editor's menu before the second
VM is created. The diagnostic host now emits a sequence receipt for the first
DOS editor's candidate menu-open, menu-close and post-switch menu-open frames,
and the guest test waits for each receipt. These remain candidate-frame
assertions: a visible-frame witness is still needed. On 2026-11-08, NASM
preprocessed the default and first-editor sequences successfully and rejected
the DOSONLY=1 combination. A scratch controller composed from the earlier
editor535 template and the current include files also assembled successfully;
its placeholder paths make it a syntax check only. The modified integration
runner was subsequently run as described below. The modified diagnostic host C source compiled
with the historical x86 MSVC flags and the complete diagnostic host linked
using the historical command with an S7-owned output directory; this does
not establish runtime behavior. Invalid DOSONLY=1 combinations are rejected
before preparation.

Source inspection: `test/integration/ntvdm64-386/host-continuation.c`
`trace_application` writes `MVDM_SCREEN_TRACE` after the host transfer and
before the guest applies the candidate. Consequently its text and editor-ready
counter demonstrate a candidate frame, not actual guest application or visible
window restoration. The separate operation-14 trace reports application
status, but does not by itself prove visible-window pixels. The opt-in assembly
condition defaults to zero for the shared Win95 caller.

The retained `editor535` SYSTEM.INI also loads the test-only `INTS.386`
observer between VMD and INSIGNIA; the failed `win31b` SYSTEM.INI does not.
The two runs also used different runtime packages and different rewritten
configuration bytes. They therefore are not an isolated VMD on/off pair and
cannot establish whether VMD caused or repaired either reported failure.
The recorded `editor535` runtime NTVDM hash begins `8D630607`, whereas the
current O:\winnt\system32 NTVDM hash is `6DA99B43` and matches S5 publication.
Thus the historical mouse pass did not exercise the currently deployed host
image; the record must not be cited as production-image acceptance.

Static trace: `ntvdm64.asm` stores the exchanged INT 33 callback and command
token in the VMM per-VM control-block area. Its mouse callback submits the
current VM and token; `mvdm_vm_executor_mouse_bop` only accepts a matching
live job. Display updates are applied as command I/O publications through the
owning continuation. The addon has no explicitly named VM-activation/refresh
handler. These facts alone do not establish that the original VDD requires a
new handler, that the callback was installed, or that either path caused the
observed symptoms; actual event and frame evidence remains necessary.
The text publication path `command_screen_apply` enters nested V86 execution
in the target VM, validates its BIOS text geometry and writes cells through
INT 10. Its success acknowledgment establishes that application path returned;
it does not observe later VDD repaint or physical visibility after focus changes.
No product behavior was changed on this static observation.
The original BIOS dispatch at `mvdm-host/softpc.new/base/bios/bios.c` maps
BOP BF to `v_mouse_EM_callback`, which selects the NIDDB instance before
`mouse_EM_callback`. The authored INSIGNIA companion supplies the VMD query
and BOP BF callback address; NTVDM64's mouse event callback forwards an active
VM/token through the existing host job. This describes the implemented chain,
not independent proof of every event's delivery.

The S7 un-staged diagnostic image was checked read-only for the exact 16-byte
S3 and S5 package identifiers. Neither occurs in that image; the corresponding
identifier occurs in each package's NTSRV. This supports using the normal
package-identity staging step for a scoped runtime test; it is not evidence of
a product fault. The existing staging helper uses PowerShell. An earlier test
run was admitted by automatic review; a subsequent PowerShell test launch was
rejected by automatic review as conflicting with the owner's CMD/addon-only
instruction. Do not treat that rejection as a product failure or bypass it.
The historical test creates a fresh identity for the complete staged package,
not just NTVDM, and checks other brokers before launch. Reusing package493's
identity merely because no NTSRV happened to appear in one process snapshot
would not prove isolation from a concurrent launch. No alternative updater was
found among the checked-in CMD/addon tools.
`src/common/base_srv/scope.c` confirms this is a runtime constraint: it reads
the package ID from the calling module's RCDATA and compares it with each of
the other nine named components before binding the RPC endpoint. There is no
environment override in that path. A diagnostic executable without the
resource cannot join even a correctly stamped copied package.

The first isolated run (`editor001`) used a freshly stamped diagnostic package
and a separate Windows copy. Win3.1 and one DOS reader started, but EDIT.COM did
not before timeout. The input BOP was called from system VM 805e1000 while the
DOS VM was 80797000; the caller identity alone does not establish the ultimate
keyboard recipient. The probe was changed to reactivate the DOS prompt after
the reader wait.

In `editor002`, DOSONLY=0 and the same no-`device=*vmd` configuration reached
EDIT.COM and sent a mouse click, but timed out waiting for the menu frame.
MOUSE0.BIN records requested and actual cursor position as (47,29), while the
VMD API pointer is zero; no `Save As` candidate frame was captured. This
reproduces the missing-menu behavior at the diagnostic frame boundary, not yet
the visible pixels. Otherwise identical `editor003` with explicit VMD passed
the staged mouse-menu sequence, two editors, both saved contents, and normal
exit. This establishes the VMD configuration dependency for this diagnostic
profile, not yet production-host or visible-pixel acceptance. `editor002` did
not reach the second-window condition; `editor003` reached it but checked
candidate frames rather than the actual visible window. An `editor004`
keyboard-only, no-VMD, first-editor-before-second run is underway.
That `editor004` run exited with `0xc0000374` before the Windows startup probe,
so it did not reach either reported scenario. A retry was rejected by automatic
review because its runner uses PowerShell; no runtime conclusion follows from
the rejection. The CMD-only installer fixture passed after adding the Win3.1
`device=*vmd` entry. A further CMD fixture confirmed that an existing exact
`device=*vmd` is retained once and that uninstall restores the original bytes;
another fixture passed for a pre-existing entry with spaces or tabs and an
inline comment. An isolated copy of the owner's original mixed-newline SYSTEM.INI,
with the existing bridge entries removed, exposed an incorrect exact-header
comparison. After correction, installation and byte-identical uninstall passed
on that copy, as did the normal CMD fixture. These are configuration fixtures,
not Win3.1 runtime acceptance.

The owner subsequently reported that the deployed VMD configuration hangs
before the COM1 dialog. Earlier production-host `win31b/386-dosonly0` retained
a live dialog reporting illegal instruction CS:1e61 IP:0001, bytes
`8d cd ca 3f fc`; that earlier observation is consistent with a startup fault,
but it has not been established as the cause or exact same instance as the
owner's report. The user-directed cleanup removed the two pre-existing
O:/w31 SYSTEM.INI backup files; the active configuration remains in place.
The installer itself remains an unpublished candidate.

At the owner's request on 2026-11-08, the already-published eleven host images
were compared against O:\winnt\system32 and both released .386 images against
O:/w31\PATCH; all were byte-identical. No partial binary replacement was
performed. After checking no NTVDM process was running and confirming the
original configuration hash, `device=*vmd` was added to O:/w31\SYSTEM.INI.
The original configuration SHA-256 was
`B8CC85F02C6000C1E48DF9038D2C75FC0E112820DCBCFC3A8A6909BC7D28E95B`;
its backup was subsequently removed at the owner's request.
the published configuration SHA-256 is
`010A1F725886A7DC1746874935B52D6DF32DDF9FA9AC5B445D53F87E844BE61C`.
CMD line processing also normalized one pre-existing lone LF to CRLF; the
remaining text compares equal when the inserted device line is removed. This
is a provisional owner-test configuration, not proof of production mouse or
visible-window behavior. No new binaries, integration pass, commit or push is
claimed.

An additional diagnostic-host startup observation used the isolated
`O:/tmp\m481s6b` copy and a private desktop, without an automatically launched
guest controller. It reached the host startup trace but did not produce the
observer's system-VM input records within 30 seconds. Adding the test-only
`INTS.386` to that isolated copy did not change that observation. Since this
probe did not verify a guest desktop milestone or automatically exit Windows,
its timeout does not establish a hang or explain the owner's production failure.
The first attempted control accidentally emptied the test copy's SYSTEM.INI due
to a CMD quoting error; that run is invalid. The copy was reconstructed from
the existing editor fixture, its relevant configuration compared, and the
control repeated. The owner's O:/w31 configuration was not modified.

The successful editor003 run also used a controller inserted into WIN.INI and
a private-desktop helper that ignores the explicitly identified missing COM
port dialogs. A later CMD-only reproduction reused the built controller and
added a scoped experimental COM dialog handler. The observer reported timeout
after 90 seconds, no guest boot/ready marker was produced, and no serial dialog
handling was witnessed. This does not identify the startup failure: the
reproduction does not establish that its token, desktop and full runtime setup
matched editor003. The experimental handler was removed and the observer
rebuilt from the original source; the test-copy startup entry was cleared.

A subsequent process inventory found that manually run timeout probes had
left 22 processes from the exact `O:/tmp\nb7s\system32` test package alive.
They were identity-checked using open process handles, terminated and waited;
no O:\winnt process was selected. Earlier manual observations using that
shared package may have interfered with each other and are not isolated
comparisons. A clean retry used a test-only owned job and a private-desktop
serial handler restricted to the exact test NTVDM executable and known COM
missing-port messages. It recorded Ignore for COM1, COM2 and COM4; W31BOOT
and W31READY were created.

The first clean retry reused nonempty A.TXT and B.TXT and is not a valid save
test. With those test files and old markers cleared, a second retry again
reached Win3.1; its first DOS editor produced an open and then closed File menu
(MENUSTG=2), and both DOS prompt readers reached readiness (READERS=2). The
controller did not reach its third menu stage after switching to the first
editor, and the observer timed out. These are diagnostic-host candidate-frame
and controller observations, not proof of visible black pixels or a specific
product root cause. The test process job cleaned up on observer exit.
The two mouse receipts both report requested and actual guest pointer position
as (47,29). The first click produced callback records for VM 80798000; after
the second prompt was started, the input trace observed VM 80836000 but did
not record a second callback to VM 80798000. The controller uses guest USER
window activation before that click, but no independent foreground or visible
frame witness has yet established whether the requested first prompt became
active. A subsequent test-only controller recorded guest USER's active window,
focus and requested target before each click. The second click's active and
focus HWND were both 5520 while its target was 5388: the test had not actually
reactivated the first prompt. This invalidates the inference that the missing
callback demonstrated a product input-routing fault.

The controller now calls guest USER BringWindowToTop before SetActiveWindow and
asserts that the resulting active HWND matches the target. A clean rerun had
matching active, focus and target HWND 5388 before the second click; reached
MENUSTG=3; observed both DOS and native editor menus in candidate frames;
reported DONE=2; saved `aaa` and `bbb`; and exited with code zero. The handler
ignored the three identified serial warnings. These results use a diagnostic
host, not the published host, and candidate frames do not prove the restored
first window's visible pixels. They do not resolve the owner's reported
production startup, EDIT mouse or black-window failures.

On 2026-11-08 a clean private-desktop test used the actually published
`O:\winnt\system32` host with a separate Win3.1 copy, explicit VMD, the two
released drivers, and no test interrupt observer. The 90-second run did not
reach its Win3.1 startup witness. A second run captured an NTVDM illegal
instruction dialog before any COM dialog: CS:1BFA IP:0000, bytes
`63 B9 83 00 75`. This is a production-host observation, not merely a
candidate frame. A third 30-second run reached a visible host frontend but
did not reach the witness or show that dialog before timeout; its first MiB of
guest memory was retained under `build/t481-s7/production-memory-observer.txt`.
The failing instruction address varies and the cause is not yet established.
The instrumented test host and production host are not equivalent evidence.

The timeout observer was extended, only when given explicit RVAs from the
matching production map, to read the CCPU's current CS descriptor and EIP in
addition to the first MiB dump. One failure dialog reported 0F00:2670 while
the later sampled CPU was already at F000:FF33; therefore that sample is in
error handling and must not be presented as the fault instruction. A separate
60-second run showed no illegal-instruction dialog or completed startup; its
sample was CS A5E9, base 000A5E90, EIP 0000082C. This is one unsynchronized
sample and does not establish an infinite loop or explain the earlier fault.
The existing optional exception-frame logger produced no record in that latter
run, consistent with no observed illegal-instruction dialog in that run.

With the same published host and isolated copy, removing only `device=*vmd`
allowed the startup witness to reach enhanced Win3.1 and exit normally in
about nine seconds after the three COM warnings were ignored. With VMD present,
removing only INSIGNIA timed out before the witness; removing only NTVDM64 also
timed out. With VMD present and both addon entries removed, the host showed
illegal instruction `0005:0028`, bytes `C7 EF 00 F0 60`. The original handler's
optional exception-frame log recorded SS:ESP `09DF:3543`; the same-run memory
dump contains frame bytes `28 00 05 00 02 02` at linear 0xD333 and the displayed
opcode bytes at linear 0x78. If this is a valid real-mode exception frame,
the reported instruction is in the interrupt vector table. The bytes following
the frame include `mouse.drv`; this observation alone cannot distinguish an
incorrect control transfer from a corrupted exception frame. These tests
isolate a VMD-dependent startup failure but do not attribute its root cause to
either addon or establish the user's production symptoms have all been fixed.

A further pair enabled the already-present host hardware trace on the same
isolated fixture. The no-VMD control reached the COM checks and normal exit;
the VMD-only run timed out. Their last common recorded BIOS milestone was
INT 15h/C0h returning successfully; the no-VMD control subsequently recorded
another INT 15h/C0h and the COM checks, whereas the VMD-only trace did not.
The VMD-only run had two different sampled EIPs, 0379 and 28C3, in the same
real-mode CS 1C3C; those unsynchronized samples alone do not establish the
guest's control-flow history or which instruction is defective.

Another single-entry control retained `device=*vmd` and both addons but
removed `mouse=*vmd`; the production host reached the enhanced-mode witness
and exited normally. Thus merely loading VMD is not sufficient to reproduce
the failure in this fixture; the mouse-binding configuration matters. With
both VMD entries present, replacing only the isolated copy's project-authored
MOUSE.DRV with the previously audited retail Mouse 8.20 binary (SHA-256
B4F0B9DE8043071FA2B82B16373BF5188BBE8162C3EF24C8C3D1B01F86B96B07)
still timed out before the startup witness, with the same last recorded BIOS
milestone. The test copy's original project driver was restored and hash
verified afterward. These results do not prove which VMD/host contract fails.

With `device=*vmd` retained and `mouse=*vmd` removed, the diagnostic-host dual
editor test passed its corrected actual-active-window assertion, three staged
DOS menu transitions, two completed commands, both saved contents, and normal
exit. This is candidate-frame evidence, not a production-host visible-pixel
assertion. The production-host startup-only control also reached the enhanced
desktop witness and exited normally with that configuration.
The saved diagnostic first-click receipt also has a nonzero protected VMD API
pointer, `003B:0372`, with the mouse binding removed. This establishes that
VMD's API existed in this diagnostic profile; it does not prove identical
retail mouse-binding semantics or production input completeness. A separately
retained retail Win3.1 test installation has `mouse=*vmd` without an explicit
`device=*vmd`, so this project configuration is a candidate, not evidence
that the original retail configuration was wrong.

The owner subsequently tested the updated `O:/w31` configuration and
reported that Win3.1 interaction was basically successful and the black-screen
problem was gone. This is owner manual evidence for that installation; the
report does not specify a complete DOS/Win95 regression matrix and does not
establish the underlying original VMD-binding fault has been identified.

An additional diagnostic run enabled the existing observer's guest text-memory
trace. After a second VM was observed, the first VM's saved text memory still
contained its editor's File/Edit line and status line. This is guest memory,
not proof of the visible VDD/NTCON pixels. The run timed out before completing
the second click after its stricter immediate activation assertion; enabling
extra tracing also changes timing and that run cannot be counted as a complete
pass. The test now pumps messages until the requested guest window is actually
active, rather than treating one delayed activation as an immediate failure;
the overall observer still reports a timeout if activation never happens.
After that change, the isolated diagnostic-host run `editor003-activation`
exited with status zero, recorded all three menu stages and two completed
commands, and saved `aaa`/`bbb`. Its display assertions remain candidate-frame
evidence; the owner's later Win3.1 manual report is recorded separately.

At the owner's standing request for a usable manual-test configuration,
`O:/w31\SYSTEM.INI` was updated from the current configuration by removing
`mouse=*vmd` and retaining `device=*vmd`. Text comparison showed only that
entry removed; the deployed file byte-matches the proposed output, SHA-256
A83950B9BE72EAFADCDE89254258D811918C298749798941B02897F086EAFF66.
The existing two installed addon drivers byte-match the released files. No
new host or driver binaries were published and no configuration backup was
created, as requested. This is a candidate for owner testing; production mouse,
window pixels, original VMD-binding root cause and both S7 acceptance items
remain open.

The unpublished `win-multitask` candidate was updated to remove an existing
Win3.1 `[386Enh]` `mouse=*vmd` binding while retaining or adding the explicit
VMD device. It does not remove a same-named entry in another section or alter
the Win95 binding. The CMD fixture `build/t481-s7/package-fixture8` passed
installation, section/comment matching, duplicate/edit refusals and the
candidate's existing exact-uninstall checks. These configuration checks do not
replace production interactive or real Win95 verification.
The same updated commands were also applied to `realini2`, an isolated copy
of the owner's earlier mixed-newline configuration: the generated result
retained the device entry, removed the mouse binding and added both drivers;
unapply restored the fixture byte-for-byte against its baseline.

The cached x86 and x64 builds completed. Their non-desktop CTest runs each
passed 46/46 after correcting three stale guest-memory test expectations:
S3 changed the CPL 3 access flag to user access, matching the existing CCPU
call, but those assertions still expected the earlier inverted flag. This
change updates the test only; it does not establish the desktop, full product,
or Win95 gates.

## Full-screen editor mouse visibility follow-up

The owner reports that, after switching a Win3.1 DOS session containing
either EDIT.COM or EDIT.EXE to full screen, mouse movement and clicks still
work but no mouse pointer is visible. This is a separate, unqualified
display issue; the earlier windowed mouse/menu result does not prove it.
Source tracing identifies two candidate drawing owners: the INT 33 software
cursor in `mouse_io.c` for DOS and `ntvwm_mouse_compose` for native text.
NTCON's captured Window deliberately selects a transparent host cursor;
visibility therefore depends on the active presentation path. For DOS,
`cursor_display` also checks the active mouse route and the enhanced-mode
cursor-disable state. For NTVWM, a visible frame mouse is composed into the
text frame, while ordinary Console mouse input is handled separately. These
facts do not establish which presentation, route, or state produced the
reported failure. The actual full-screen switch and final displayed pixels
still need a reproducible witness for both executables before a change can be
qualified; do not treat the earlier windowed-frame probe as that witness.

S8 added a focused host test exercising NTVWM's real text-mouse composition
through the VM text publication copy. It confirms that a visible pointer's
inverted attribute survives this tested transfer on x86 and x64. It does not
exercise the VM's final rendering, full-screen switch or DOS mouse path, and
therefore does not resolve the owner's observed problem. A candidate isolated
full-screen probe was prepared but did not execute: automatic approval rejected
its startup, citing an apparent PowerShell conflict with the owner's CMD/addon
constraint. No outcome is attributed to that unexecuted probe.
