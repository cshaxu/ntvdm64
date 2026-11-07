# T479 S6 VM I/O implementation audit

Reconciled against delivered d3bd17b6b. This is the finite S6 work inventory,
not an acceptance report. Original guest bytes remain immutable.

[Operation inventory](m0-t479-s6-io-inventory.json) enumerates all47 live wire
operations and current provider-reference locations. Fifteen have no explicit
dispatch and reach unsupported default; the previous17 count predates the two
fill operations. The32 remaining entries have dispatch references, not blanket
semantic acceptance. VIDEO_BEGIN still accepts only same-sized text, WINDOW_RECT
only full-grid identity changes, and modes reject processing flags. Source
references and positive payload coverage must remain separate.

## P4 publication093 and review

Cached089, Full09116/16 and actual command/normal-exit092 qualify this bounded
input foundation checkpoint, not S6 closure. Publication093 replaces all11
host images, hook license and common manifest at assets/release and
O:/winnt/system32. All destination hashes match sealed package
daa392bf-02e0-439f-9b1c-e21804398133; recoverable previous packages are retained
under build/t479-s6-integration/publication-backup093. The first preflight
used the wrong cached-build receipt directory and failed before replacement;
the corrected script exits0. No product code changed after Full091.

The tested candidate ntvdm64.386 and updated addon manifest are published only
to assets/release. The existing release had no ntvdm64.vxd; an initial backup
preflight exposed that before replacement. Only the verified Win3.1 artifact
is published. Its original-media/configuration boundaries remain unchanged;
no Win95 acceptance or original guest patch is implied.

Review retains one lock-owned token/VM admission path, rejects new input while
retirement joins callbacks, copies bounded BOP data before use, and leaves
failed input reads unconsumed. The per-VM addon token replaces suspended-stack
ownership. The Break repair adds one distinct key identity without renumbering
Pause or other keys. Three shared-library adaptations are explicitly recorded;
this source is not byte-identical to upstream SoftPC. No MVDM mirror changed.
Negative controls and per-architecture fixtures are recorded below. The passive
input observation probe is test-only; no real guest producer or unread-input
handback is claimed implemented. Owner QUEUE/proposal changes are preserved
outside this delivery.

## Candidate command and normal-exit regression092

The sealed Full091 package daa392bf-02e0-439f-9b1c-e21804398133 runs
from O:/tmp/nb55 with the already built candidate addon078. Exact-owned
Run16 33808, NTCON36060 and NTVDM17164 identify this run. No INPUT.386
observer is loaded. The Console Alt+Enter provider returns0; ordinary
window-key providers deliver CLS/VER/DIR and EXIT. The retained command
frames show DIR output and a prompt, and a separately issued VER shows
the host Windows version and a prompt. Initial garbled text is the
owner-known, nonblocking issue; this run does not repair it.

EXIT retires DOS VM8000 (NIDDB0B) and returns to Program Manager. F10,
File/Exit opens the observed Exit Windows dialog; Enter completes NIDDB17
and root wait0/exit0. Supervisor and root receipts are0. Exact-path process
inspection confirms all nb55 processes exited naturally, without a service
kill. Only then was the authored addon backup restored; its previous hash
and unchanged SYSTEM.INI hash match input-driver092/receipt.json.

Evidence: build/t479-s6-livecmd/control092-{aftercommands,ver,retired,exitdialog}.bmp,
control-root092.txt, control-root092.exit, control092-supervisor.exit and
niddb092.txt. The diagnostic observer had already ended before these commands;
its remaining1 line is not used as process-exit evidence. This run validates
successive native command handoffs and normal exit with the new host package;
it does not exercise new mouse movement, Ctrl+Break, input to a running child,
or unread-input handback, and is not S6 closure or publication.

## Input gap traced after mouse repair

The delivered mouse/VM repair proves the actual Windows pointer, DOS command
entry and noninteractive CLS/VER/DIR sequence. It does not prove input to a
running native interactive child. There is no production caller of
mvdm_vm_executor_key; its implementation calls mvdm_vm_io_key. Correction to
the initial audit: the sweep omitted the singular test/ root. The existing
test/ntvdm-exe/softpc/mvdm_vm_notification_test.c exercises executor input,
cancellation and VM reuse. Those unit callers prove queue mechanics, not a
guest input producer. The current
addon command_return waits on a counted VMM semaphore with virtual interrupts
serviced, but supplies no keyboard records to the host queue. Notification BOP
operations0..15 have no input-delivery operation. Therefore READ_INPUT's queue
mechanics cannot substitute for the absent guest producer.

The bounded input repair must connect guest-selected VM input to the exact
live continuation, retain key-up/down, repeat and modifier meaning, and prevent
both guest BIOS and host child from consuming the same event. Retirement and
channel cancellation must invalidate delivery. Do not read the outer host
Console or route by foreground native HWND: neither identifies the guest VM.
Existing command-entry and mouse tests remain valid within their own scope.
VMM/VKD service choice and actual integration must be proven before runtime
changes; no polling or synthetic successful input is authorized by this audit.

## Finite implementation groups

1. Input ownership: connect actual guest keyboard/mouse/focus to the active
   VM continuation, preserve exclusive consumption, returned receipts, mode
   semantics and cancellation. Keep guest/VMM state access on its owning CPU.
2. Text operations: WRITE, fill and scroll need actual Console semantics;
   reuse existing text primitives where possible. Do not turn unsupported
   operations into success or re-enable incompatible stream output.
3. Geometry and fonts: actual guest mode/grid/font authority, resize and
   viewport application, font queries, codepage and snapshot provenance.
   Existing text-configuration cache is an acknowledged projection, not a
   second authoritative guest state.
4. Graphics/display: implement actual graphics transport and guest rendering,
   mode changes and VIDEO_TEXT; preserve original device/VMM ownership.
5. Window metadata: title versus handoff caption, layout, pointer/clip and
   keyboard layout. Do not answer using the unrelated outer host window.
6. Publication/lifecycle: exact serial acknowledgement, final frame before
   handoff, abort/disconnect/retirement/reuse and cancellation of blocked I/O.
7. Nested acceptance: independent text task chains and nested multitasking
   payloads, no ancestor takeover, real input/output and resource retirement.

For each group, first trace existing producers and original guest services,
then implement as one bounded batch with focused and actual guest evidence.
New semantics concessions require explicit disposition. S6 cannot close merely
because MEM or a native shell runs. Win95 remains the subsequent S4 continuation.


## Input-route calibration067 (not an input implementation)

The isolated authored tests/observation/win31_vm_input_route.asm installs two
observation-only callbacks: VKD PM API chains unchanged; V86 INT09 returns CF
set so original handling continues. It never reads port60, consumes an event,
changes a client frame or delivers a host input record. Original MS-DOS6 VMM
service-table ordinals41/92 and Microsoft DDK API/interrupt callback contracts
are read-only ABI inputs, not copied implementation. No production source or
release asset changes accompany this calibration.

Both installations report success. The ordinary windowed DOS single A key
appears on screen and INT09 observations identify child VM80798000. The PM API
records only initialization function0 in system VM805E1000, not that key. This
is positive IRQ-route calibration and lack of observed PM-key traffic in this
run, not proof that all input must use one route or that host forwarding works.
The program exits abnormally: nativeFF, rootC000013A; no normal system-exit
record. No exception entry establishes a crash cause. Normal retirement is
not passed, and neither probe nor existing runtime is blamed without a control.
The test SYSTEM.INI is restored byte-exactly after all owned processes exit.

Retain build/t479-s6-livecmd/control067-* and niddb067.txt. The initial probe
source hash was CE24161568D06FDFFCDB6813198BADD1A83C66DE0455899277A79E4CA98CD2B9;
image hash DC1EA65CD2EED731CE2706E5D65D20881A4F54DE3BFA9EC78CF70017018F3F51.
Subsequent source adds control-message observations to identify lifecycle
stages; calibration068 below executes those additions. No release qualification.

## Input-route calibration068: normal lifecycle control

The same isolated installation loads the observation-only INPUT.386 beside
INSIGNIA.386 and NTVDM64.386. The owned control-root068.ps1 runner uses the
previously successful logon-environment/physical-runtime route. Retained inputs
and logs are under build/t479-s6-livecmd, not a production installation.
The source SHA-256 is
099CE2ECB50CB7E377C08564D046F1EE252E1A71F93166B54E47B1CA822AB35B;
input-route068/input.386 is
C41B913C9AE8D9AB06FC5A0A210C05539D8737F633A746EE2E32CF4F561045ED.

The BOP log reports both hook installations (flags3), system VM805E1000 and
child VM80798000. Sixteen INT09 observations belong to that child. The only
recorded VKD PM API invocation is function0 during initialization; this does
not establish a usable PM keyboard-event producer. After actual DOS EXIT,
the child receives termination controls0A/0B/0C, and Program Manager is visible
again. Closing Windows produces system controls04/05/06 and root Run1631424
records wait=0 exit=0. The native debugger observer expired before process
retirement; it supplies no independent native exit-code assertion.

The owned Run16, NTCON and NTVDM processes are absent after completion. The
test SYSTEM.INI is restored from input-route068/SYSTEM.INI.before only after
verifying that removing the exact INPUT.386 line reproduces that saved text.
Its restored SHA-256 is
45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874.
No original guest executable, published package or O:/w31 file changed.

This establishes one normal lifecycle with the passive probe present. It
does not explain calibration067's abnormal exit, prove keyboard delivery to
a running host child, or qualify an input implementation. The next bounded
comparison must run a real blocked native input consumer and distinguish
guest IRQ observation, child receipt, and post-return BIOS input. Existing
READ_INPUT buffering has no production caller of mvdm_vm_executor_key; that
missing producer remains the implementation gap, rather than an assumed
failure of an already connected queue.

Reproduce the observation image with NASM `-f obj` on
tests/observation/win31_vm_input_route.asm, then the existing Open Watcom linker
with `format windows vxd static`, resident shared _LTEXT/_LDATA segments and
`export input_route_ddb.1`. The exact response file is retained as
build/t479-s6-livecmd/input-route068/input.lnk. Load it only in an isolated copy
through `device=<absolute PATCH path>\INPUT.386`; enable the existing
MVDM_S42_VXD_BOP_REPORT_PATH diagnostic before starting the owned worker.
The callback's set carry flag deliberately chains every INT09. A future test
that consumes keys must use a separate implementation and evidence, not quietly
change the meaning of this passive calibration.

## Native input receipt069--071: missing producer reproduced

The authored tests/observation/win31_vm_input_child.c opens its report, records
its PID, switches its own Console input to raw mode and blocks in the real
ReadConsoleInputW. It logs every returned key record and exits after Enter
release. An independent30-second diagnostic deadline terminates a missing-input
case with ERROR_TIMEOUT; this is not a product timeout or a passing result.
Successful later acceptance must inspect A/Enter make/break records and
post-return BIOS input, not merely the child's exit code.

Control069's supervisor failed after launch because inspect069 did not exist.
The already-live owned guest was inspected and continued without relaunch;
the known COM1/2/4 warnings were explicitly ignored. Child8732 entered its
read and timed out with no key records. The test key was sent immediately
after the launch command, so ordering was insufficient to establish active-child
ownership. Control070 used the corrected observer directory and child10400,
but fopen_s denied live report reads. The observing PowerShell command also
lacked terminating-error behavior and incorrectly continued after that read
error. Neither attempt proves readiness before the test key.

Control071 corrects those test defects: _fsopen with _SH_DENYWR allows a
read-only observer; that observer uses terminating errors, waits for the
flushed waiting-real-input record, checks the recorded process is alive, and
only then sends A and Enter. The retained ready071.txt records child22308
ready before input at 2026-11-07T00:20:47.0978610Z. Its final receipt records
started, waiting-real-input and deadline wait=258 error=0, with no key record.
The guest IRQ observations continue, and after child retirement the DOS
prompt executes A, producing the native shell's unknown-command error.
This supplies the required negative: input arriving during the admitted host
child still reaches guest BIOS/COMMAND instead of that child's input queue.

The input stimulus is the existing exact-PID/path-checked key-default.exe
window-message provider, not physical SendInput acceptance. Foreground-checked
SendInput was attempted in069 and refused before sending; it was not counted.
The final source SHA-256 is
67F30214457FE10594E0B8E4B726A9BD52E21FD7B92B54133654697C8F252BD0;
the MSVC x64 image is
0FA1A0541E7A6537A6DD93531FD6A62C283A8BF1049DCAC195868B090921658C.
Build: the existing VS2022 x64 environment, cl /std:c11 on this source;
retained input-child071-build.exit is0. The executable was copied as IK.EXE
only into O:/tmp/w479m; command `ik ik071.txt` ran inside its DOS VM.

Retain input-route070/ready071.txt, child-receipt071.txt, niddb070.txt and
control071-after.bmp under build/t479-s6-livecmd. This run ends through explicit
owned-service diagnostic teardown, not a normal Windows-exit pass. After all
owned package processes retired, SYSTEM.INI was restored to the same
45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874 hash.
The production package, original guest executables and O:/w31 are unchanged.

Reuse audit: original nt_event.c owns KEY_EVENT_RECORD history; the existing
mvdm_keyboard_history.c carries source metadata, not a replacement keyboard
translator. Original nt_keycd.c BiosKeyToInputRecord converts BIOS buffer
characters, but it does not reconstruct key releases and its conversion uses
the host layout. Neither alone establishes guest-selected VM identity or a
complete input contract. Keep these as evaluated reuse candidates; do not
claim that forwarding their output alone repairs exclusive consumption.

## Original BIOS input boundary072--073

The passive route witness now also chains INT15 without modifying the client
frame or reading a keyboard port. Original keybd_io.c keyboard_int invokes
INT15/AH4F after obtaining the scan byte, before translation. Original
NtPutInBuffer commits the translated BIOS word and tail before INT15/AX9102.
These are reached original owners, not a new keyboard translation algorithm.

Control072 reports all three hook installations (flags7). While native
child4180 is confirmed waiting, the child VM80798000 sees AH4F with scans
1E/9E for A and1C/9C for Enter. The native receipt again times out without
events. Its observer image SHA-256 is
E94DBE5EBFD6C5387FD4C41C6356D3B19BCC0879F28B6F1C0640F60BE9879241.

Control073 adds read-only BDA head/tail observation at AX9102, through the
documented current VM CB_High_Linear. Source SHA-256 is
4781FDD5ED407513859554B2A3246590ECB73A30AAE2B18597628AD2D0572850;
image is F234F9F7A08BE90F5BEBF7D4FC5C247D042710DBBEFA3FF0C535F30D35E0B729.
Child31128 is ready before input at2026-11-07T00:31:45.8099284Z. The A make
is followed by head0038/tail003A; A break follows without a character commit.
Enter make then yields head0038/tail003C, followed by Enter break. The native
receipt still times out. This proves translated keys remain queued in that
VM's BIOS while the host request waits; the missing producer is not explained
by absent keyboard IRQs, absent guest translation or absent native admission.

Both are targeted window-message provider runs, not physical-key acceptance.
NASM/Watcom build exits are0. Neither observer consumes a key, changes a
client register or advances/rewinds a BIOS pointer. Each diagnostic ends by
stopping only its isolated NTSRV after evidence capture; normal Windows exit
is not claimed. Retain input-route072/073 ready/child receipts, build records
and niddb072/073.txt under build/t479-s6-livecmd.

The implementation should preserve the reached original translation and
hardware acknowledgement. AX9102 offers a concrete candidate transfer point
for translated characters, but is not a complete event producer: key releases
and modifiers do not all append BIOS words. AH4F supplies raw transitions;
correlation, extended sequences, layout, per-request ownership, queue failure
and unread-input handback must be resolved together. No production filter,
buffer rewrite or success stub is introduced by these observations.

## Input recipient validation074

Local S6 work now requires both the VMM-supplied VM and the exact continuation
token at mvdm_vm_executor_key. The executor's shared lock pins the job and I/O
provider through enqueue; its existing continuation invalidation rejects a
retired generation. A different live VM cannot submit to that token. No new
identity table, lock, polling, timeout or mirror difference is introduced.

The existing notification fixture now checks VM0 rejection, both directions
of cross-VM token misuse, valid delivery to each VM, unknown tokens and input
after normal completion. Its existing close/retirement/reused-VM cases retain
their assertions. Review confirms removal from the job list precedes provider
destruction and completion publication, so the post-completion rejection does
not depend on a scheduling delay. These are local queue/lifetime checks, not
proof of production keyboard delivery.

Background input074.cmd uses the existing VS2022 x86 environment and cached
msvc-x86-release tree, builds mvdm-vm-notification-test, then runs the exact
CTest case mvdm-vm-notification. Exit0; 1593 assertions, CTest1/1 pass in0.27s.
Retain input074.run/out/err/test.log under build/t479-s6-io. The source unit
SHA-256 is755D341AE874181D2C73FA050B56A1581149AFC0E44F906096B6EDE0E0C1889D.
Input075 repeats the same fixture in the existing x64 cached tree: exit0,
CTest1/1 pass in0.22s, with its own run/out/err/test.log retained alongside074.
This validates the host-owned unit on both architectures, not an x64 NTVDM
product or actual guest input.

This remains uncommitted grouped implementation, not a P delivery. The guest
producer, exclusive BIOS consumption, complete input semantics and full
production gates remain pending; no new package is published by this check.

## Bounded input ingress076

The local implementation adds operation16 to the existing signed, ring0-only
BOP B1 dispatch. It passes the VMM caller identity to the executor and copies
exactly eight DWORDs through the existing guest-memory lease provider: token
low/high, key down, repeat, virtual key, scan, Unicode character and control
state. Boolean/WORD fields are range-checked before narrowing. No new transport,
guest pointer retention, producer timeout or input receipt is introduced.
The queue's existing failure return remains authoritative: no error confirms
event consumption. This is a translated-event ingress, not a raw scan decoder.

The existing notification fixture substitutes only the bounded guest-memory
copy and worker getter for this boundary. It invokes the real BOP dispatcher,
executor and queue. It rejects no executor, non-ring0, other-VM token, wrong
extent, unmapped span, oversized fields and zero repeats. Both queues remain
empty after failures. Successful make/repeat and break records are read back
through the real input provider after the source memory is overwritten; all
fields remain intact and the other VM remains empty. This proves copy and
recipient behavior, not execution of BOP16 by the real guest addon.

Background input076 uses the same cached x86 build/CTest command as074;
exit0, 1631 assertions, CTest1/1 in0.26s. The exact script/run/out/err/test.log
are retained under build/t479-s6-io. Guest producer and end-to-end native-child
acceptance remain open; operation16 is not yet exercised by the released addon.
Input077 builds the same changed boundary in the x64 cached tree: exit0,
CTest1/1 in0.23s. Its separate run/out/err/test.log is retained alongside076.
Both runs are unit evidence; neither is a complete package or guest test.

Further source review confirms the original nt_keycd.c BiosKeyToInputRecord
uses RtlOemToUnicodeN, VkKeyScanW and MapVirtualKey and can rewrite scan/control
fields. Reusing it without reviewing the active guest codepage and the native
recipient's key semantics would not prove preservation of observed guest
transitions. The read-only Microsoft VKD service table
OpenNT-4.5/nt/private/os2/client/thunk/include/vkd.inc, SHA-256
11BECFF50B3FED4F080A795925788F670C2BDE6F645926C8BF6198B61211B947, exposes
message-key and paste/hotkey services but no general translated native-input
record service. These source facts constrain producer implementation; they do
not authorize a host-layout guess or an all-key hotkey consumer.

## Per-VM token ownership and actual command control078

The authored addon now stores the live request token only in the calling VM's
control block. Admission publishes it, work/completion read it, and terminal
return clears it. Creation zeros the fields; executable/generation guards still
exclude retired VMs. The stack copy and its eight-byte adjustment are removed,
so input callbacks need not borrow a suspended command's stack. No mirror or
original guest binary changes.

Background build078 uses the checked-in addon build.ps1 and pinned NASM/Watcom.
Source SHA-256 B9B9246BDEFD19E7B55C0E783FA69EA7730BB37E2F9B451DCC24A31C6E2D8B6A
produces identical 5075-byte .386/.vxd candidates, SHA-256
3EC467536324961A2356E1C17A6437A8B46658491B8689A9BE1C92FE887DA4B3; exit0.
Manifest, listing and map: build/t479-s6-livecmd/input-driver078.

Control078 reuses control-root068.ps1 with fresh evidence names and a precreated
inspector directory. Only the authored NTVDM64.386 in O:/tmp/w479m/PATCH is
temporarily replaced, with an exact backup. The unchanged complete ne30 package
runs Win3.1 with Mouse31 and INSIGNIA.386; the new host BOP16 code is not deployed
for this test. Root22760, NTVDM14140 and NTCON23592 identify the run. INPUT.386 is
not loaded; SYSTEM.INI is unchanged.

The existing owned Console Alt+Enter probe and exact-PID/path window-message
provider exercise CLS, VER, DIR and EXIT. Retained frames show clean prompt,
native Windows version and prompt, directory listing and prompt, then Program
Manager after DOS VM retirement. These successive native commands verify the
new token storage. They are provider-input tests, not physical input or input
to an already running native child. Initial DOS garbling remains nonblocking.

The first Alt+F4 provider attempt does not open an exit dialog and is not a pass.
F10/File/Exit subsequently opens Exit Windows; Enter confirms normal shutdown.
Original NIDDB0B retires DOS VM8000; NIDDB17 records Windows shutdown. The root
receipt reports wait0/exit0. The diagnostic observer's remaining1 line is not
proof of native-process exit; subsequent exact-path CIM inspection finds all
ne30 processes naturally gone, without service kill. Only then is the addon
backup restored, SHA-256
D5F6B13A0DCAA99230F4F873F17A42604B65BD116262B541DF8FC90364D2B5A2.
SYSTEM.INI remains45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874.
All control078 scripts, receipts, logs and frames remain in the livecmd root.
This is local candidate regression, not publication or S6 closure.

Source follow-up rejects an unsynchronized history lookup: kbd_inb/EOI may
retire or replace the hardware output slot; keybd_io.c INT15 releases keyboard
ownership before recursive guest execution and reacquires it afterwards;
nt_event.c updates its original history before entering the dispatch hardware
lock. Reuse needs a stable copied source identity and synchronization proof.
No such unsafe lookup was added. Modifiers, extended sequences, failure
ownership and post-task input remain producer obligations.

## Normal completion admission boundary079/080

The executor previously accepted keys while a completed command was joining
its frontend retirement: the job and provider were alive, but the command no
longer consumed input. A job-owned finishing flag is now set under the existing
executor lock before retirement. Input rejects that state with
ERROR_OPERATION_ABORTED; another VM's token remains ERROR_NOT_FOUND. This
does not claim to return keys already accepted before command completion.

The real executor fixture holds normal retirement on its existing test event,
without requesting cancellation. It verifies rejection before completion is
published, then releases retirement and observes normal completion. Cached
background x86 input079 and x64 input080 each pass1640 assertions, CTest1/1,
in0.39s and0.26s respectively. Scripts, run receipts and LastTest copies are in
build/t479-s6-io. The grouped production changes remain unpublished.

## Original interrupt-return observation080

The [Microsoft Windows3.0 DDK, chapter23](https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/)
documents Call_When_VM_Returns as an interrupt-return callback, paired with
Hook_V86_Int_Chain before the client IRET frame is built. EAX0 requests no
timeout; EBX identifies the returning VM and EDX carries invocation-local
reference data. The original MS-DOS6 dev/smartdrv/sdvxd/vmm.inc service table
assigns ordinal0x0d; file SHA-256 is
28BD6EDABA10434DFCAD26BFCDAB8DBD1A3CC28100CD6910C67397D8C05F92E9.
Only ABI facts are used; no reference implementation is imported.

The existing passive input-route probe now schedules that callback from IRQ1.
It records the prior tail in callback reference data and reads post-handler
BIOS tail and flags. It changes no BIOS buffers, ports, flags or input ownership.
The VMM itself supplies and restores the normal return callback. Probe source
before its subsequent comment-only chapter correction was
6B126E221A109436534FE777305C487E759C98DC03904CF5D40C7AEECEBECF1B;
background NASM/Watcom build080 exits0 and produces INPUT.386
05D740031D1B29FC1131F47272589CCA45D0CF2EC5C65B8A88201C26F99FC646.

Control080 uses the unchanged complete ne30 host package and the existing
addon. Only the isolated w479m SYSTEM.INI temporarily loads INPUT.386. Root
Run16=15736, NTCON=29828, NTVDM=37296. An initial Alt+Enter probe mistakenly
targeted CUI carrier38788; its successful Console write is not VM input
evidence. Exact process reconstruction selected root15736, after which the
owned provider observed the window and delivered keys successfully.

DOS VM80798000 shows raw Shift2a/aa followed by post-handler flags0002/0000
without tail movement. A1e produces a BIOS commit and matching return-tail
advance; release9e does not. Enter1c advances the tail; release9c does not.
These observations also occur after IK.EXE pid12040 reports waiting-real-input.
Its diagnostic30-second deadline still expires with no native input, as
expected for this non-consuming probe and the absent production producer.
This is positive callback-order evidence and negative end-to-end input evidence.

DOS EXIT then retires the VM; F10/File/Exit opens the observed Windows shutdown
dialog and Enter confirms. Root receipt is wait0/exit0. Exact-path process
inspection finds the stage quiescent without killing its service. SYSTEM.INI
is restored byte-for-byte to45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874.
Build, probe, child readiness/failure, input commands, trace and cleanup receipts
remain under build/t479-s6-livecmd/input-route080 and control080*.

The result supports reusing actual BIOS post-processing instead of an
independent modifier-state machine. It does not yet prove translated event
delivery, extended/Pause sequences, layout conversion, Ctrl+Break ownership,
unread-input handback or nested IRQ correlation. Original
nt_event.c ReturnBiosBufferKeys uses BiosKeyToInputRecord for residual BIOS
characters and can synthesize release events; that handback algorithm is not
a complete live transition producer. Those distinctions remain implementation
requirements, not new semantic concessions or accepted omissions.

## Control-key boundary081: reject a blanket post-IRQ producer

Source review of keybd_io.c establishes two materially different paths:
PAUSE sets HOLD_STATE, acknowledges IRQ and executes a nested guest wait before
returning; Ctrl+Break clears the BIOS queue, sets bios_break and calls INT1B
before appending a zero word. Thus an IRQ-return observer alone cannot forward
all control keys promptly or prevent their effects on the waiting DOS parent.
The existing ntvwm-exe/console_state.c already translates a received VK_CANCEL
to CTRL_BREAK_EVENT and processed Ctrl+C to CTRL_C_EVENT. Reuse that native
owner; do not create a second native-process signal path in the guest bridge.

The passive probe adds an INT1B observer and records enhanced BIOS flags at
IRQ return. Background build081 exits0; source
F2E0ABB4CEA383A16EC9144B49D8425F255313947D5F7FC4172F2A544CE5E5BA,
INPUT.38626BB49781437B3C2E405154B2BE97C6F6B38EE20686E416F69CB164D920726D7.
The isolated unchanged ne30 package uses root28924, NTCON26880, NTVDM16072.
IK.EXE17324 reports waiting-real-input before the stimulus. The exact-owned
window provider's VK_CANCEL/control4 attempt actually yields raw E1/1D/45,
not a demonstrated Ctrl+Break sequence. Therefore this is not an INT1B test
pass. The trace stops after45 with no matching IRQ-return record. A later A
provider call succeeds at the window boundary but produces no additional
record. The native child independently expires its diagnostic30-second wait.
The earlier ordinary-key run080 remains distinct positive evidence.

Do not attribute the failure to the product or the observer without a matching
no-observer control. It does disprove using this stimulus as Ctrl+Break coverage
and prevents treating post-IRQ observation as complete input implementation.
No production input filter is enabled. Terminating only the owned NTSRV14248
ends the failed diagnostic; this is explicit teardown, not normal-exit proof.
All stage processes then disappear and SYSTEM.INI is restored to its prior
45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874 hash.
The trace-before-teardown, readiness/child failure, build, command and cleanup
receipts are retained in build/t479-s6-livecmd/input-route081 and control081*.

Input implementation must distinguish translated characters, raw modifier/
release transitions and control-key effects. Use original BIOS translation
where it completes, preserve a request/VM-bound input owner, and hand native
control events to the existing NTVWM consumer. No native consumer may receive
an event while that same event remains pending for the DOS parent. Native
PREPEND_KEYS and the VM queue must also survive normal command retirement;
currently executor teardown destroys remaining queued events. That is an
existing S6 ownership gap to implement together with the producer, not a
separate feature or an authorized loss of input.

## Input handback service selection

### Post-P4 closure of the receipt/replay design question

The common wire's receipt fields do not make native CUI input reversible.
`ntvwm-exe/presentation.c::ntvwm_presentation_input` requests ordinary input,
decodes it to INPUT_RECORD and writes to the hidden Console. Its
`return_unused_input` reads the actual remaining Console records and calls
the untagged `ntcon_worker_prepend_keys`. The native Console does not retain
the frontend's receipt, and a child can generate its own input. Therefore a
new VM ledger must not infer exact original guest bytes by matching returned
native records to previously supplied keys. Equal keys are not identities.
The current VM provider explicitly rejects tagged returns and nonempty live
receipt payloads; changing that alone cannot solve this downstream loss.
Do not add a duplicate receipt framework or alter common wire ABI for this.

The preserved input must instead be the actual returned records, followed by
guest events not yet delivered to the native consumer. The existing native
return loop prepends batches from the tail, preserving FIFO. After broker
retirement has joined those callbacks, finishing closes new native admission;
the executor must transfer the remaining queue before destroying its provider
and before publishing terminal completion. VM destruction cancels the transfer
through the existing continuation invalidation. Native worker death is not
successful input handback.

Original `keybd_io.c::NtPutInBuffer` writes the translated BIOS pair and tail
before INT15 AX9102 and before K26/interrupt exit. That is an exact translated
character boundary, unlike raw scan history. It still cannot stand for all
make/release and control events. The producer must preserve that distinction;
do not use BIOS-only input as the full implementation.

The Microsoft DDK's VKD_Start_Paste copies its caller's buffer into VKD-owned
storage for the explicit EBX VM. It may subsequently wait for that VM's INT16
requests. Consequently, waiting for paste completion before returning the
blocked command callback can deadlock: the DOS parent cannot issue the INT16
that completes the paste until that callback returns. An eventual implementation
must distinguish acceptance of owned replay data from later parent consumption;
it cannot reuse the screen-frame rule requiring actual rendering as the input
completion rule. This is a source-derived ordering constraint, not a runtime
pass or approval to discard input. Concurrent existing paste, input ordering,
OEM/extended-key conversion and cancellation still require actual evidence.

ABI cross-check: OpenNT-4.5 nt/private/os2/client/thunk/include/vkd.inc lists
Start_Paste at service index0A and separates left/right modifier bits; those
bits are not Win32 dwControlKeyState. The DDK describes four-byte OEM/scan/shift
entries, not INPUT_RECORD. Read-only DDK transcript:
O:/repos.external/pcjs/documents/books/mspl13/win/w3ddkvxd/README.md,
SHA256 6F31B0F9328E4319540C15794838353D90865215D3452E853E1D5110A17514BC.
It is Microsoft documentation comparison evidence, never a runtime dependency.
No production input filter, guessed inverse translation or paste call is added
by this analysis; P4 remains the published baseline.

Chapter36 of the same Microsoft DDK rules out VKD_Force_Keys as a direct
per-VM handback: it delivers to the keyboard-focus VM and accepts no target VM
argument. The retiring command's VM need not own focus. Do not silently change
focus or return its input to whichever VM happens to be active.

VKD_Start_Paste does accept an explicit VM and copies its supplied buffer,
but its records are OEM character/scan/shift tuples, not arbitrary native
make/break records. It can finish, be cancelled or lose its VM; an accepted
paste is not proof of final consumption. Existing paste interaction, conversion
and ordering still require proof before selecting it for handback. Neither
service is currently called by the product. Reuse the existing BOP and
guest-application acknowledgement transport for any selected handback; do not
introduce another channel, native-thread guest writes or a polling loop.

## Real VKD paste ownership counterexample094

Independent probes win31_vm_paste_route.asm and win31_vm_paste_child.asm
exercise the actual guest VKD service, not a stub. Read-only ABI source:
OpenNT-4.5/nt/private/os2/client/thunk/include/vkd.inc, SHA256
11BECFF50B3FED4F080A795925788F670C2BDE6F645926C8BF6198B61211B947.
The background NASM/Watcom build exits0. Runtime uses the unchanged published
P4 nb55 package, old configured addon and temporary PASTE.386 in isolated
O:/tmp/w479m. The only setup mutation is a backed-up SYSTEM.INI device entry
and independently authored probe files. Original guest binaries are untouched.

The first launch through the DOS COMMAND window produces no result file and
is not acceptance evidence: that route does not establish execution in the
enclosing Win3.1 VM. Launching PASTE.COM directly from Program Manager's Run
dialog yields the signed52-byte PST1 result. Single paste accepts q/scan10;
the VxD changes its original source byte to z immediately after service return.
The DOS consumer nevertheless reads1071 and receives completion10 (reference1,
reason0), proving copied-buffer behavior in this run.

The overlap case submits q then r, synchronously inside the same VxD API call,
before returning to the DOS client. Both service calls report success. The DOS
client polls original INT16 for36 BIOS ticks and reads only1372 (r); one callback
reports20 (reference2, reason0). No q or first-paste cancellation callback is
recorded. Thus a second successful Start_Paste does not prove preservation of
the first pending paste. This is a concrete counterexample to using uncoordinated
Start_Paste as lossless command input handback. Do not install that design.
It does not prove every VKD version or paste mode behaves identically.

Files under build/t479-s6-livecmd/paste094 retain build/launch receipts, map,
listing, raw result and configuration backup. SHA256 result.bin:
52489902BA50C25CE82C959AA25CB322F469479C76177454E893F59ECC598EE9;
paste.386 C6B12F4DE43276DCEF51A56738B3277B91921CBAA9A662CD6AB1C7D910ECCD30;
paste.com 2FEB9E5402418A8EA846E13AC8D1EEF025179D5070027A6C90E5ABA9164B2C39.
Only the source provenance comment was corrected after the build.

NIDDB records two DOS VM retirements. Observed Exit Windows confirmation and
root25460 wait0/exit0 prove normal shutdown; the supervisor exits0, all nb55
processes naturally disappear, SYSTEM.INI restores byte-for-byte, and only the
three hash-verified temporary probe files are removed. This is bounded design
evidence, not implementation or S6 closure. P4 remains the published runtime.

## Post-P4 VKD export and PM API audit095--096

Read-only extraction of the actual isolated WIN386.EXE (SHA256
C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8)
finds W3 VKD at file offset243712. Its ordinal-one DDB is object1 offset1E8C,
device0D, SDK030A, service count15. Consequently no service index0F or above
is exported by this exact guest. Later-version keyboard filtering/byte-insertion
services cannot be assumed available. No probe calls an unknown ordinal.

The existing read-only inspect-win386-dosmgr.py now accepts an explicit VKD
selection, leaving DOSMGR as its default. It validates the 32-bit ordinal-one
export and DDB extent/name before reporting the count; it does not relocate
or call any code. Actual run096 exits0 and reproduces exploratory095's object
bytes (object1 SHA256 68E6D24ABC5789598487842E514B50FEEB934EC1F73CFBA4CFA93914BF806D4F).
The default DOSMGR extraction, disassembly and metadata compare equal against
the pre-change tool, apart from the explicit module-name field; the background
comparison exits0. Outputs remain in build/t479-s6-livecmd/vkd096 and dosmgr096-*.

Static tracing of the VKD PM entry at object offset0A9E accepts client EAX0/1
and rejects other operation numbers. The input operation reads client EBX,
ECX and EDX. Nonzero client EBX is checked by VMM and compared with the same
VKD owner field returned by exported Get_Kbd_Owner (service08 at0954); a
different VM reaches the client-carry failure path0B9B. Zero chooses that
current owner. Force_Keys (service07 at091F) reads the same owner. Thus the
existing PM API also cannot be treated as an arbitrary-background-VM input
route. This is static control-flow evidence for this image, not runtime
acceptance of all PM API behavior. No private offset/pointer is used by product
code and no guest bytes are patched. This closes the two proposed shortcuts
(later-version services and unrestricted PM API), not the S6 handback gap.

The remaining exported Reflect_Hot_Key service is also not a general event
replay interface. Microsoft DDK chapter36 specifies a real hot-key handle,
target VM and shift state, followed by leaving hot-key state. In this same
extracted image its entry08C7 dereferences the handle at08E4, delivers the
make through0F09, sets bit80 at08FB and delivers the break through0F09 again.
It then calls17A4, which clears the global pending hot-key field1FCC and can
invoke notification/shift-restoration paths. This confirms the documented
side effects; it cannot preserve an arbitrary separate make or break record.
Defining hot keys solely to obtain replay handles would introduce actual
keyboard registrations and hot-key state changes. No such registration,
fabricated handle, private-offset call or guest modification is implemented.
This is source/document and fixed-image static evidence, not a runtime
Reflect_Hot_Key test or proof that every possible VKD integration is excluded.

The input handback decision now excludes three concrete shortcuts: replacing
an existing paste (actual094 data loss), injecting into the current keyboard
owner (wrong target for a background VM), and reflecting a hot key (synthesized
make/break and hot-key cancellation). The required remaining design must keep
unread input owned by the exact VM across native command retirement and use
a guest-consumption boundary that preserves that input's semantics. Do not
close S6 or enable the unfinished producer on the strength of these exclusions.

## Failure-atomic input read082--085

Review found that read_keys consumed the queue before resetting its readiness
event. If ResetEvent failed, the operation reported failure but the unread
records were already gone. The correction resets readiness under the existing
queue lock before copying/consuming the final records. Failure leaves the queue
unchanged; no new runtime state, retry or timeout is introduced. Production
mvdm_vm_io.c changes by eight added and six removed lines.

The initial082 negative closed the actual borrowed event and produced Windows
STATUS_INVALID_HANDLE (0xC0000008), not the intended failed-operation assertion.
It is not a valid negative control. The replacement test-only translation unit
includes the real provider and substitutes only ResetEvent for a selected valid
handle. No failure switch enters the production provider or its ABI.

With the unfixed provider, cached x86 run083 fails at line259: input_pending no
longer reports the queued record after the failed read (CTest exit8). With the
fix, x86 run084 passes1646 assertions in0.54s and x64 run085 passes the same1646
in0.24s; both build/test runners exit0. The test checks failure status, zero
delivered bytes, retained pending input and the original record on PEEK.
Receipts and exact build/test output are build/t479-s6-io/input082--085.*.
Fixed provider SHA-256 is
69BC922F5C7309BC2F1E079829A1EB4FF8914818D74171D7BD19133699CC100D;
test seam SHA-256 is
B5E5A83C33B0A747A86AF9A6116E4F86DE5A8F600259D4E04B475FC32E01FF76.

This is local verification, not product publication or complete input delivery.
The real guest producer and normal-retirement handback remain open together;
retaining input on a failed read does not repair queue destruction at retirement.

## Window input identity086--087

The formal console_startup_observer write_window_key_records helper omitted
WM_KEY bit24 even when its INPUT_RECORD specified ENHANCED_KEY. Preserve that
bit for both make and break messages. This is a one-line test-provider repair,
not a guest or product workaround. Cached x64 wow_message_observer build086
exits0. It proves compilation only; no actual Ctrl+Break delivery is claimed.

The temporary key-default helper used in081 also omitted bit24. More
fundamentally, the selected shared base/win32/input.c has no VK_CANCEL mapping.
The checked-in win31_vm_keyboard_decode.c witness links that actual provider,
without mocks, windows or guest changes. Run087 compiles successfully and exits1:
ordinary A, Pause and right Control are accepted; VK_CANCEL with scan46 returns
accepted0/key0. The two existing types_interface.h CRT deprecation warnings
remain warnings, not the failed assertion. base_keyboard_decode returns
BASE_KEYBOARD_UNMAPPED for this transition, before NTCON or guest delivery.
The current SoftPC src/lib counterpart likewise has no CANCEL/BREAK mapping.

Thus fixing the test's extended flag alone cannot establish Ctrl+Break input.
Nor does this decoder result explain the precise E1 sequence in081: that
stimulus still lacks a demonstrated end-to-end key identity. Keep those claims
separate. The next control-key implementation must retain a distinct Break
identity through the shared decoder and NTCON, then exercise the actual VM
input owner; it must not relabel Pause, bypass the selected library or add a
second native control-event path. The pinned shared-library manifest and
upstream identity must be accounted for with any such change. No shared
library or original keyboard controller has been changed in086--087.

## Break identity repair088 and cached build089

The finite correction adds BASE_KEY_BREAK at the unused0x0003000e value,
without renumbering any existing key, maps the exact SDK VK_CANCEL value in
the Win32 adapter, and preserves it as VK_CANCEL in NTCON. Pause remains
BASE_KEY_PAUSE/VK_PAUSE. This is five added production lines across the shared
declaration/adapter and NTCON, with no control-event generation, guest changes,
original mirror changes or alternate input transport.

This deliberately changes three imported library files locally; it is not an
unchanged SoftPC import. nxvm-import.json now records the additive T479 S6
repair and current file hashes, retaining the prior upstreamSha256 identities.
The shared library's license and all other imported files remain unchanged.
Do not claim cross-project byte identity until the same repair is incorporated
upstream and the import is reconciled.

Run088 compiles and runs the real decoder and existing NTCON keyboard fixture
on x64, exit0. The same input that was rejected in087 now produces neutral
196622; Pause still produces196617. Explicit NTCON assertions check Break
make/release, scan46, Ctrl/extended flags and distinct Pause scan45. Existing
layout/dead-key, surrogate, failed-sink, source-isolation and retirement tests
also pass. These are boundary tests, not actual guest Ctrl+Break acceptance.
The same real decoder and complete frontend keyboard fixture pass x86 run090,
exit0. No assertion was relaxed between the negative and positive cases.
Build089 uses the previous successful cached055 x86/x64 commands unchanged
to build the complete host component set and selected shared tests. The owned
runner exits0; x86 and x64 each pass14 selected shared tests (2.00s and1.50s).
The complete output has no failed compilation/link step. This is cached build
and selected-test evidence, not the Full product gate or VM input acceptance.

Full091 reuses the successful Full058 launcher, logon environment and modern
desktop-capable PowerShell runner, with a fresh r091 evidence/package root.
It first rebuilds the formal console observer from current source in its
existing stable Ninja tree, preserving the extended-bit correction086 in the
actual gate input. Supervisor script SHA-256 is
F33D60D8F797165A709C556C394D7498B0B8E5DFDF2EE03EE591FCEF7AD35F92.
Read full091.exit/supervisor.exit and the individual r091-verification results
before assigning a verdict; starting this run is not a passing result.

The first091 wrapper exits1 after successful fixture execution and staging.
Its child output is not retained by that legacy wrapper, so the precise thrown
exception is unproven. This must not be described as a product test failure or
an unavailable desktop. The8 VM cases all satisfy their Expected exit codes,
including the intentionally nonzero73/74 lifecycle exits. The final frontend
scope fixture exits0 with PASS output. The staged manifest contains11 images,
all matching their hashes; the15-file control cache is populated.

The prior successful058 handoff already documents this same wrapper stopping
after staging and its explicit Node/Python continuation. Reuse that actual
successful runner for091, rather than repeating the completed fixtures or
using a new package. full091-resume.ps1 SHA-256 is
F33247514D264828B1E3F7DE56DBF1156207532352F8C87AEB4BBCC1FAD92FC2;
its logon-environment supervisor is
050A84AE9B77FE22192224B4CAEA0D285024844102C272834A8752A1D183A470.
The continuation records a transcript and caught exception explicitly. Preserve
the original exit1 alongside its eventual result. full091-source.json pins17
uncommitted source/test inputs; no product source changes during this run.

The resumed091 runner passes both real Win16 generic-call cases (normal and
calling-convention mismatch): each records three guest phases, zero errors
and three releases. The actual frontend-instances fixture also passes. Full
then reaches the retained WINMINE/SOL/WRITE startup frontier and command-exit
matrices. These intermediate results are not a final Full verdict, gameplay
acceptance, or completion of the missing S6 VM input producer/handback.

Review of the input changes keeps ownership explicit: the shared library
names Break without generating a control event; NTCON only preserves the
native key record; NTVWM remains the existing Ctrl+C/Break policy owner.
The VM queue's failure-atomic change stays under its existing lock and creates
no new lifetime owner. The finishing flag closes input admission before joining
the existing frontend retirement, but does not yet return unread records to
the DOS VM. Those unchanged outstanding obligations prevent S6 closure even
if the ordinary product regression succeeds.

Full091 continuation now exits0 (inner and outer receipts). Its timing ledger
records16/16 selected gates passed in438140ms, including Console/Window command
exit, native GUI lifetime, protocol rejection, terminal handback, frontend loss
and nested-window handoff. DOS4GW/Nesticle remains the owner-approved explicit
skip, not a passing test. Package daa392bf-02e0-439f-9b1c-e21804398133 retains
11 matching image hashes; all17 pinned source/test hashes remain unchanged.
The first wrapper's exit1 remains preserved. No new default publication or
S6 closure follows from this regression result alone: actual Win3.1 candidate
checks and complete VM input/handback still remain.

Background stage092 uses the existing New-PhysicalRuntimeStage helper to copy
that exact candidate to fresh O:/tmp/nb55. All11 image hashes match the sealed
manifest. No old stage or original installation is overwritten. The isolated
Win3.1 SYSTEM.INI still hashes to
45E3A30585E4B74D57A15E9815A495D7EFE85891FB059B2FD58759546FE5C874.
The authored addon source remains B9B9246BDEFD19E7B55C0E783FA69EA7730BB37E2F9B451DCC24A31C6E2D8B6A,
matching078; its verified build may be reused without recompilation. Stage092
is preparation, not a launched Win3.1 run or input acceptance.
