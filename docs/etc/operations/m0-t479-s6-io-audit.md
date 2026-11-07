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

## Input gap traced after mouse repair

The delivered mouse/VM repair proves the actual Windows pointer, DOS command
entry and noninteractive CLS/VER/DIR sequence. It does not prove input to a
running native interactive child. Source sweep of src and tests finds only the
definition/declaration of mvdm_vm_executor_key, and its internal call to
mvdm_vm_io_key. No production or test caller enters executor_key. The current
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
