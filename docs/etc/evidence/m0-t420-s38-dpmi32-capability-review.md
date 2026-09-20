# T420 S38 DPMI32 Capability Review

## Current Acceptance Checkpoint

### Current-source native fixture revalidation

Matching-artifact runtime follow-up is complete: all 17 rows in
O:/winnt/logs/s38-relink-product-r1-summary.json have Actual=Expected,
with the verifier's guest-text gates enabled. Tested worker SHA256 is
3a4b7d3b36f322c014d0aed583cbee81502da9b39f5a1dc0ffb25145cef21a03,
which remains in the isolated package. The temporary drive and exact test
processes were cleaned up; the official package was not replaced. This
supersedes the pending-runtime statement in the relink chronology below,
not the remaining selected-package obligations.

Test hygiene: removed ten duplicate result-field assignments from the
DPMI verifier without changing their values or gates; PowerShell parsing
passes. Recompiling the TSR witness after its invalid-profile guards gives
the same 05f55f3c hash used for its successful retention test.

The current formal graph was incrementally rebuilt with targets ntvdm.exe,
dpmi-debug-tests and cpu40-descriptor-domain-fixture.exe. All four native
executables returned zero: ccpu-debug-match-test, debug-register-binding-test,
ccpu-halt-reset-test and cpu40-descriptor-domain-fixture. Observed markers
include debug match/status separation, checked span/register order, HALT
RESET AX=beef, divide-fault cancellation, RF suppression, real descriptor/
paging refusal and TSS T-bit single delivery. VdmTib storage remains 4208
bytes with no overlapping symbol.

The relinked worker SHA256 is
3a4b7d3b36f322c014d0aed583cbee81502da9b39f5a1dc0ffb25145cef21a03.
The isolated package still contains tested 308e493a; no publication occurred.
The 17-route result below belongs to 308e493a, not this new relink. A fresh
matching-artifact product regression is required before publishing the new
binary. The initially silent sandbox Ninja was inspected (no child process),
stopped by its exact PID and retried with normal build access; the retry
completed successfully. No product runtime process was terminated.

### Current production-diff review

Current candidate-versus-HEAD normalized source count (excluding README)
is eight existing MVDM files +96/-14 and one worker debugger binding +65/-0.
No opennt-host source change or new mirror/overlay file is present in this
packet. These are changes against HEAD, not project-versus-upstream totals
and not claimed reductions. Raw Git counts include upstream CRLF recovery
under the existing -text mirror attributes; do not count that as code.

Reviewed DPMI selection, checked debug copy, fatal PM publication, CPU debug
event latch and INT21 pointer/FLAGS/packet/EXEC changes are functional
repairs/composition, not added diagnostic hooks. The new event loggers and
failure injection remain under tests/observation and diagnostic builders;
the reviewed product owners contain no S38 trace/fopen/fprintf additions.
This review does not authorize deleting necessary checked-copy behavior or
CPU event state merely to lower line counts. Final upstream totals and
whole-package residual-owner review remain separate closure obligations.

### Extended-error pointer return (verified host repair)

The strengthened pointer probe and reusable NetworkDplPointer gate pass
all four `s38-error-pointer{16,32}-gated-r2-*` routes on 308e493a. EDI is
poisoned with A5A50000 before 5900; the 32-bit case asserts the returned
EDI has no high bits BEFORE the test's own normalization. Thus the test
does not mask a missing SetDIRegister zero-extension. AX/BX/CH and the
non-aliasing linear address still pass. This named boundary is now fixed
and verified; the following paragraphs retain the diagnostic chronology.

Formal 308e493a now passes all 17 established product routes under
`s38-div290-product-r1-*`, with real guest-text gates for MEM/repeated MEM,
nested COMMAND, EDIT, child status and native streams/EOF. Documentation
governance and git diff --check also pass. This completes the matching-
artifact product regression, not the other S38 package obligations.
Official O:/winnt publication remains deferred.

DIV-290 is now implemented and targeted-tested: route 59h to original
ReturnESBX, capture whether its output offset is DI, and use SetDIRegister
instead of SetBXRegister for that call. Original ES conversion, selector
owner and return path are reused; no new mirror file/adapter mechanism.
Formal x86 worker SHA256
308e493a741ef0c6ff57fd54a525bdf46b2a4cc4c9fbb28a08645658a87f5a59
passes `s38-error-pointer{16,32}-fixed-r1-{direct,nested}`. Tests assert
AX=5/BX=0803/CH=2 and actual returned linear pointer, plus normal I/O.
The probe treats ES as an output rather than incorrectly requiring it
unchanged. Formal warnings remain the historical anonymous union and Length
warning. Full 17-route regression for this new hash remains pending;
31a1f80a's earlier full-suite result must not be attributed to this artifact.

The diagnostic `s38-error-pointer16-trace-r1` localizes the loss:
real-return and protected-entry show AX=0005/ES=06EB/DI=0000; before and
after simulated IRET, ES has become saved selector 019F while DI stays
zero. No segment-to-selector conversion occurs. This confirms the pointer
is lost by host segment restoration, not by original guest 5900. Diagnostic
SHA256 28f18d53ccece07788af8991e63c2d78ca4fcf761a1422cf7ae4891632727d30;
raw log s38-error-pointer-r1.events.txt. Original ReturnESBX already owns
the required ES conversion but zero-extends BX rather than DI. A minimal
repair should reuse that owner while selecting the documented offset output,
and retain AX/BX/CX error fields; no new standalone pointer policy is needed.

NETWORK_DPL_POINTER supplies a test-owned real pointer through 5D0A and
checks 5900's ES:DI by querying the returned selector base. The initial
`s38-network-pointer16-r1` passes with original-PSP:0100, but that aliases
the caller's existing ES and is insufficient. The corrected r2 uses
(original-PSP+10h):0000, the same valid linear address with a different
segment representation, and fails the direct route. Original getset.asm
returns ExtErrPt in user_ES/user_DI; selected host dispatches 59h through
NoTranslation, which restores saved ES. This is an unresolved host pointer
translation boundary, not an immutable-guest exemption. Capture actual
returned pointer and establish a source-shaped repair before acceptance.

### Loaded-entry execution witness

The reusable ExecLoadCall gate and manifest flag pass all four repeats
`s38-exec-load-call{16,32}-gated-r2-*`. Duplicate newly added parameter/
gate declarations were removed before these runs; PowerShell parsing and
the documentation-governance gate pass. Compile-time selection now rejects
loaded-call mode without both load-only and cleanup. These are test-quality
checks, not additional product behavior or whole-S38 closure.

All four `s38-exec-load-call{16,32}-r1-{direct,nested}` routes pass on
formal 31a1f80a. After EXEC/4B01, the parent supplies returned CS:IP and
SS:SP to original INT31/0301. The independent loaded child checks DS=CS,
PSP CD20 and command tail, returns C0DE by RETF, and the protected parent
then frees environment/image through real INT21/49 and completes I/O.
S38_EXEC_LOADED_ENTRY_STACK_TAIL_CALL_OK proves execution, not merely
non-poisoned output fields. The same binary still takes ordinary EXEC's
exit-7 path in the preceding control case. This callable witness is not a
claim that RETF implements DOS process termination or automatically cleans
up a debugger-loaded process. The explicit cleanup and ordinary EXEC exit
remain distinct evidence. ExecLoadCall adds a reusable marker gate.

### INT21 memory failure boundary attribution

Final disposition of this reproduced branch: original guest limitation,
registered P1 in TODO under standing owner authority. Original host failure
returns CF without committing XMEM_BLOCK changes; original guest does not
stop consuming success-only outputs and updates descriptors before testing
Success. Source equality and the post-failure publication trace establish
that this guest behavior is inherited, not an added overlay policy. The
portable host BOOL correction remains necessary and is not reverted.
No allocation-success fabrication, guest patch or descriptor rollback
shadow policy is added. Strict failing reproducers remain failures; normal
growth/shrink/free and the earlier pre-host refusal preservation are separate
passing cases. This closes disposition, not functionality of failed-growth
recovery. Isolated worker restoration to 31a1f80a was rechecked by SHA256.

Post-failure descriptor execution is now captured in
`s38-xmem-failure-r4.events.txt`: after CF=1/BX:CX=00004570, guest submits
selector 01C8 with base 00004570/limit 0010, then selector 2028 with base
00004580/limit 455F. Both original descriptor calls return AX=0. The
retained host block remains address 1342464/length 272. This demonstrates
that requested bytes really become the guest header base, followed by a
descriptor derived from the wrong header; it is no longer only a source
hypothesis. The test-only dispatch recorder activates after the exact
injected growth failure and observes slot 00, without changing descriptors.
Diagnostic SHA256
bb886cb3b51844510f1143b31b4f5188f3e2e01a2328c596364ea5cade92766c.
This failed run is not resource-preservation acceptance. Original guest
and host contract ownership must remain explicit in its final disposition.

Host-side before/after capture is now concrete. Diagnostic
`s38-xmem-return16-r3-direct` again fails the required guest marker;
`s38-xmem-failure-r3.events.txt` records old address 1342464, length 272,
injected request 17776, return CF=1/BX:CX=17776, and retained address
1342464/length 272. Thus the original host XMEM_BLOCK remains intact;
the register pair still contains requested bytes, not its surviving address.
This proves the host half of the suspected mismatch without relying on
launcher exit zero. Actual guest descriptor publication remains to capture.
Diagnostic SHA256
ac32e3a974a2b2b8052539624b6bb293eb83db60ce5b124d23caf6ce80030b19.

Original-provider comparison narrows the reallocation hypothesis. The
selected dxmmgr.asm is text-identical to pinned OpenNT after CRLF/LF
normalization; makefile explicitly compiles this file for ASM486. Its
mxb60 republishes BOTH returned base and requested MemSize-derived limits,
then checks Success only at mxb100. Restoring only old BX:CX at the host
failure return would therefore not preserve the old limit and is not a
complete repair. Original OpenNT xmem.c also returns immediately with CF
on NTSTATUS failure without old-address publication; the selected portable
BOOL correction must not be reverted to hide this path. No product change
is made. Next evidence must capture host before/after address/length and
guest descriptor publication before assigning final ownership/disposition.

The reduced-size follow-up `s38-xmem-preserve16-r2-direct` is NOT passing.
Both S38_ALLOCATE_INJECTED=214384 and S38_GROW_INJECTED=17776 are recorded,
so the desired reallocation boundary was finally reached. Launcher exit
zero accompanies an empty final console; the required lifecycle marker
is absent and the runner correctly rejects it. No subsequent route ran.
Source reveals a specific return-contract risk: after failed ReallocXmem,
dxmmgr.asm sets Success=0 but falls through mxb60, stores BX:CX as MemAddr
and republishes the header selector base. Current host DpmiReallocateXmem
sets CF and returns without replacing input BX:CX (the requested size)
with the surviving block address. This is a host/guest contract hypothesis
requiring original-provider comparison and execution evidence, NOT an
accepted guest limitation. Do not waive it under immutable-guest policy.
Reduced-size diagnostic worker SHA256
c72583731ef25324ae8e09b903bf5e2e541e0ab94a99c9bffa4a1dd5fdab5aff;
formal isolated candidate was restored after the failed run.

Preservation-mode runs `s38-xmem-preserve{16,32}-r1-{direct,nested}`
complete subsequent growth/shrink/free and ordinary I/O. They separately
record allocation AX=3A00/BX=FFFF/CF=1 and growth AX=8/BX=0/CF=1.
However the host event log contains allocation injections only, with no
S38_GROW_INJECTED: the requested multi-selector growth was refused before
the targeted host reallocator. This is valid rejection-preservation
evidence but NOT proof of host-reallocation failure rollback. ModifyXmemBlock
documents failure when additional 64K selectors cannot be allocated; the
next probe must stay below that threshold and require an injection event.
Strict error-code mode remains distinct from preservation mode; the latter
does not claim the original allocation AX bug passed.

The trace repeat `s38-xmem-failure16-trace-r2-direct.txt.console.txt`
captures AX=3A00, BX=FFFF, FLAGS=3247 (CF set). The error-8 assertion
therefore fails on actual AX, not an absent injection. Source axb130 calls
XMS function 08 and carries AX through axb110; pmallc converts the returned
capacity into BX and copies AX into rmrg.xax. This is an original DOSX
error-code propagation limitation under a real host-allocation failure.
Register it under the owner's standing immutable-guest policy, retaining
the failing strict error-code test. Do not count that code as correct or
patch guest/host to normalize it. Growth preservation and successful free
must be checked separately, without claiming the AX contract passes.

First controlled failure run `s38-xmem-failure16-r1-direct` is failed,
not acceptance: S38_ALLOCATE_INJECTED size=214384 proves the host allocation
boundary was reached, then the guest reports S38_FAIL_INT21_IO_STATE before
the growth injection. The probe currently expects CF with AX=8. Original
dxmmgr.asm::axb130 queries pmxmssvc 08h and returns through axb110 without
normalizing AX; dxmain.asm::pmallc ultimately copies AX into rmrg.xax.
Actual AX/BX/CF must be captured before classifying the mismatch. Do not
repair guest or silently relax the expected code to manufacture a pass.
The test-only xmem wrapper rejects sizes 34570h/45680h (requested bytes
plus original DOSX's 16-byte header), otherwise delegates to real providers.
Diagnostic worker SHA256
841bc1bcb0f12e1e03864e0a470e5a1d64026426743f84ba70b614acb1300f5a.
The isolated worker was restored after this failed experiment.

The protected INT21/48/49/4A family must not be described as conventional
DOS MCB allocation. Selected 486/dxmain.asm dispatches to pmallc/pmfree/
pmmodb: pmallc and pmmodb take the WORD pmrg.xbx paragraph count, convert
it to bytes and call AllocateXmemBlock/ModifyXmemBlock. The selected host
int21map.c bodies are conditional NT_ALLOC_DOS_MEM alternatives, not the
active allocator. Existing DOS_MEMORY tests therefore exercise DOSX's
INT21 selector facade over extended memory; TSR's real DOS MCB witness is
a separate contract.

Requesting BX=FFFF is at most 1 MiB minus 16 bytes and is not a reliable
failure injection. Existing INT31 failed-growth tests do not prove this
facade's AX/BX/CF translation. The remaining failure test should control
the original host allocation boundary, then verify facade error return,
unchanged existing selector/data and successful subsequent free. Do not
exhaust host RAM or alter guest memory-management code to force failure.

### IOCTL dispatch reconciliation

The control-data marker is now a reusable IoctlControlMock runner gate,
with manifest selection and invalid-combination rejection. All four routes
pass `s38-ioctl-control{16,32}-gated-r2-*`. The isolated package's restored
worker was independently hash-checked as 31a1f80a before this repeat.

Control-data mapping 02--05 now has controlled full/short/error coverage:
`s38-ioctl-control{16,32}-r1-{direct,nested}` pass 12 cases per route.
The resource mock receives the original owner's mapped 16-byte buffer,
validates input, returns 16/8 bytes or CF/error 5, and changes only read
responses. Guest assertions verify all bytes, untouched short-read tails,
failure preservation and a trailing guard; normal I/O follows. The original
MapDSDXLenCX still performs mapping/copyback/segment restoration; no product
provider is replaced. This is not proof a physical character/block device
implements these operations. Diagnostic worker SHA256
bc47969e238948ecef86504fdb38fcb29aa8758c694350fd20f172105e7f530d;
events are in s38-ioctl-control-r1.events.txt. The explicit control marker
was checked for every transcript; reusable runner integration remains.

The reusable IoctlDeviceMock runner gate now requires the device-packet
marker and records its selection in the result manifest. All four routes
pass again under `s38-ioctl-device{16,32}-gated-r2-*`; no ad-hoc screen
inspection is needed to detect a missing assertion. NASM rejects device
mode without its required metadata mode and inherited-EXEC mode without
an executing child. Four current formal native targets were also rebuilt/
run successfully: debug-match, checked debug binding, real CPU HALT/reset/
RF/fault/T-bit, and descriptor-domain fixture. These checks do not replace
the remaining package acceptance or final matching-artifact product suite.

Controlled device-packet coverage now passes all four 16/CODE32 direct/
nested routes in `s38-ioctl-device{16,32}-r1-*`. The original mapping owner
executes against the test-only BX=00FE resource boundary. CL40 has two
track-layout entries and maps exactly 48 bytes; CL60 maps/returns 38;
CL62 maps five bytes; CL68 maps four and returns CF/error 1 unchanged.
The guest checks every packet byte and its trailing guard, followed by
existing metadata tests and normal I/O. The boundary checks balanced small/
large translation buffers at return. This is mapping/return evidence, not
physical format/verify or a successful SENSEMEDIA implementation.
Diagnostic worker SHA256
5d07fb6d9e387295ce02c8cf2f6ed9d9604d0b8df4f9c5e1d43b070debc75593;
event log `s38-ioctl-device-r1.events.txt`. The isolated worker was restored
and hash-verified to formal candidate 31a1f80a after the experiment.

The selected int21map.c table has exactly 16 minor slots (00--0F):
00/01/06--0B/0E/0F retain NoTranslation; 02--05 use MapDSDXLenCX;
0C uses IOCTLMap2Bytes; 0D uses IOCTLBlockDevs. Real invalid-handle tests
cover 02/03/0C, and ordinary disk-file 4400 is exercised separately.
This does not establish successful control-data transfer through 02--05.

Within 0D the admitted packet set is 40/41/42/46/47/60/61/62/66/67/68.
The current test-only provider covers track 41/61, format 42 and metadata
46/47/66/67. Its 60 response supplies track geometry, but that is not a
standalone complete device-parameter test. Dedicated 40 variable-length,
60 output and 62 fixed-length packet assertions remain necessary. No
physical medium is written by these mock tests.

Source demioctl.c::demIoctlDiskGeneric provides 40/41/42/46/60/61/62/47/67
and an early 66 path. Although demdasd.h defines SENSEMEDIA=68 and DPMI
maps four bytes for it, that selected DEM switch has no 68 case: it takes
the default invalid-function result after its device-presence gate.
Therefore 68 is a mapped request without a successful selected DEM
implementation, not a capability restored by mapping alone. A controlled
negative response and buffer/return validation remain appropriate; do not
invent a device provider or classify this host-side fact as a guest bug.

### EXEC inherited handle acceptance

`s38-exec-vectors{16,32}-r2-{direct,nested}` extend the same four passing
routes with real INT23/24 restoration. The protected parent records both
real-mode vectors using INT31/0200. The real child installs independent
test handlers using INT21/25 and reads them back using INT21/35, asserting
both segment and offset. After child exit the parent requires both vectors
to equal the pre-EXEC values. The explicit child readback prevents a failed
installation from masquerading as restoration. Original msproc.asm's
Abort_Inner restores the six words beginning at SavExit, including these
vectors; no product implementation was added. This does not test delivery
of Ctrl-C/critical errors or override the load-only execution obligation.

The extended positive/negative pair passes all four routes in
`s38-exec-noinherit{16,32}-r1-{direct,nested}`. In addition to inherited
handle 5, the parent opens handle 6 with INT21/3D80 (no-inherit). The child
must receive CF=1/AX=6 when closing handle 6; after child exit, the parent
still reads CHD! through its own handle 6, closes it and deletes the file.
Thus source-owned sf_no_inherit exclusion is now actually exercised, not
just inferred from misc.asm. This does not cover network-FCB inheritance.
Updated probe SHA256 identities for this run:
16-bit 1edaa0edb7866ac6bcd50b06a8ebd041afc6239cb6800060cc707410cb05bed5;
32-bit a32094e5cc56b2ceb820af6e9f600f63b1c505a358dad904db80b9c7b75f8611;
child 83a71b9e3f4cd91fbada14fc3faece5c8178b3c2803bbfd1ada13bdfe870a27c.

`s38-exec-inherit16-r1-{direct,nested}` pass with real child output and
S38_EXEC_INHERITED_HANDLE_PARENT_SURVIVES_OK. The parent creates its test
file, asserts the controlled first handle is 5, and EXECs the child. The
child writes CHD! through inherited handle 5, closes its copy and exits 7.
The parent verifies the exit code, PSP/environment, seeks/reads the exact
four bytes through its still-open handle, closes and deletes the test file.
Original DOS misc.asm's handle duplication is the source owner; this is
not a new adapter policy. It proves this inheritable local-file case, not
no-inherit flags, network handles, vector restoration or load-only execution.
Parent SHA256 fa9b8dbb4c377e346ca31cfe7377a460887d7571ce6ce14cde2025f60198f336;
child SHA256 61062cfe46a8d551449c6612de0364d4504fba0ca00cf9709aff9bc3c420aec6.
Both initial console transcripts were inspected in addition to the existing
runner checks. The reusable ExecInherit gate now requires this marker and
absence of D38INH.TMP, and rejects incompatible load-only selection.
`s38-exec-inherit{16,32}-gated-r2-{direct,nested}` all pass that gate on
formal worker 31a1f80a; the 32-bit case selects CODE32/high-memory/high-offset
composition. Its parent SHA256 is
34003d05cfad92c922d18456da18121100a96e5a22cd4e0d882a1a35a120d345.
Source review also identifies explicit sf_no_inherit and sharing_net_fcb
exclusions in misc.asm::Create_dup_jfn; these exclusions are not proved by
the positive local-file test and must not be described as unconditional
inheritance of every open handle.

### TSR lifecycle probe (open)

The protected-residency witness now passes on formal 308e493a:
s38-tsr-pm-retention-r1.txt.console.txt contains
S38_TSR_PM_SELECTOR_MEMORY_RETAINED_OK and the existing MCB/parent/following-
client markers. The independent TSR allocates 4096 bytes via INT31/0501,
creates its selector/base/limit and writes a test-owned sentinel. It leaves
these allocated at INT21/3100, recording only its own resource identities.
After two ordinary protected-mode clients exit, the separate checker enters
DPMI, queries that selector's base (0006), verifies its 4095 limit with LSL,
and reads the sentinel. It neither patches guest internals nor frees another
client's allocation. TSR SHA256
05f55f3c2c309afe4e4f7ea90a7180b3fefde18130dc7d215d16e4962d033259;
checker eac65ef94be19f0292604a8da5eb6afbdc83c05bd445d1b35864906bc300a958.
Sources are the TSR_PM_RETENTION variants of dpmi_int21_state.asm and
dpmi_tsr_resident.asm, built under tsr-pm-retention-r1. This variant is
explicitly 16-bit; no 32-bit TSR or resident callback claim is made.

Original 486/dxmain.asm PMIntrDos explicitly handles 4Ch using
TerminateProcess but not 31h. TerminateProcess replaces the current PSP's
termination vector with ChildTerminationHandler. Original host Terminate
returns to that DOSX owner, whereas TSR performs NoTranslation. Combined
with the previous trace and this retention witness, missing TSR slot 10 is
not grounds for a forced-free fix: the tested resource is deliberately
resident and remains usable while ordinary clients release theirs. The
observer again times out at the parent prompt; natural worker termination
and full resident interrupt/callback behavior are not proved by this test.
No product repair or original-guest limitation is asserted for this path.

The lifecycle-filter repeat now separates TSR from subsequent ordinary
termination. Diagnostic worker 8719812e0c846fbb196f2bc559485bb524a57caa7f929007b56bd6439d2d8400
records s38-tsr-lifecycle-r2.events.txt. It observes InitApp for PSP selectors
01AF (TSR), 01D7 and 01FF (following clients). Slot 10 / DpmiFreeAppXmem is
then reached for 01D7 and 01FF, but not for the retained TSR 01AF. Slots 13
(FreeAllXmem) and 0D (NoLongerInUse) are absent. All existing batch markers,
including resident MCB owner/size/content, remain present at the parent
prompt. The observer times out there, so this is NOT natural-exit acceptance.

The first diagnostic a0dbc1f1c6d2faa2a1f312df4149e7aa384d9c8b2643a4f56276ba755e961386
used the older lifecycle filter, which omitted slot 10; it cannot prove
absence of per-client cleanup. The test-only filter now includes that slot.
No product or original guest change was made. After both runs, only exact
isolated-package processes were reclaimed, the temporary drive was removed,
and isolated ntvdm.exe was restored and hash-checked as 308e493a.

Source disposition: the original int21map.c TSR owner delegates directly
to NoTranslation. Selected makefile uses 486/dxstrt.asm; its
ChildTerminationHandler decrements cDPMIClients, calls FreeMemByOwner and
TerminateApp, and only the last-client branch frees all XMEM and resets
IDT/GDT. xmem.c DpmiFreeAppXmem frees blocks matching the DX PSP owner.
These source contracts and observed ordinary-client frees must not be
mistaken for proof that a TSR takes ChildTerminationHandler. The new trace
contradicts that assumption in this workload. Next required proof is the
TSR retained protected-resource contract and selected PMIntrDos termination
branch, not a speculative forced-free hook or a blanket guest-bug waiver.

`s38-tsr-resident-r1.txt.console.txt` adds actual residency evidence: after
two subsequent protected-mode I/O clients, the independent real-mode
checker passes S38_TSR_MCB_OWNER_SIZE_CONTENT_OK. It reads the preceding
probe's recorded PSP, checks MCB type M/Z, owner equal to that PSP, exact
requested paragraph count and the retained S38-TSR-RESIDENT sentinel.
Only the independent probe publishes its own identity to tests/D38TSR.DAT;
no original guest code/data is patched. This proves the tested DOS block
remains resident across those clients, not that protected-mode handlers or
every DPMI allocation survives/is reclaimed. The observer still times out
at the usable parent prompt; its timeout is not a passing exit assertion.
The TSR probe was rebuilt with identity publication, superseding its older
hash below for this run; original r1-r4 observations retain their old hash.
Resident-run TSR SHA256:
52724834a15f90473560711dbfc53a358994469651c6186160b6047dc458722e.
Read-only checker SHA256:
3473455596f405d9bce68bec0210cad0a119461fdb78f57140954b9369456201.

The corrected CRLF batch (`s38-tsr-following-r2.txt.console.txt`) reaches
S38_TSR_PARENT_RETURN_OK, two subsequent independent protected-mode I/O
success markers, S38_TSR_FOLLOWING_DPMI_CLIENTS_OK and the parent prompt.
This proves return and subsequent client usability in this one worker,
not retained-memory contents or comprehensive resource reclamation. The
following-client probe SHA256 is
5870ac7a3a2bf24bf00bb815f5af33f2abfa7e26870813d1f5220aef1db0805c.
Remaining isolated processes are explicitly reclaimed after observation;
forced test cleanup is not evidence of natural worker termination.

A following-client batch experiment (`s38-tsr-following-r1.txt`) returned
launcher status zero but its captured screen showed batch source fragments,
not executed success markers. It is rejected as acceptance evidence. The
new test BAT had LF line endings; these were mechanically corrected to
CRLF before any repeat. Launcher exit alone is not a passing assertion.

The opt-in control-event recorder now identifies the received event:
`s38-tsr-exit16-control-r4-direct.txt.control.bin` contains DWORD 2
(CTRL_CLOSE_EVENT). The pre-input snapshot contains the I/O success marker
and parent prompt; after injected exit the observer terminates C000013A.
The test handler records the event and returns FALSE, preserving default
termination. Original `nt_event.c::cmdPushExitInConsoleBuffer` posts
WM_CLOSE to the Console; its comment explicitly describes notifying all
attached processes. This explains the missing observer report without
establishing TSR residency or DPMI resource cleanup. The event recorder
does not identify the sender; attribution to that source path still needs
execution evidence. No product behavior was changed for this observation.

The observer early-exit is now measured, not assumed: the runner reports
missing reports with the observer's actual exit code. Repeating the same
input-driven run under `s38-tsr-exit16-r3` returned -1073741510
(C000013A), not the observer's argument-validation code 68. An input-ready
screen snapshot exists; no final report does. This is consistent with
Console/control-event termination during the exit experiment, but does not
identify the event sender or prove worker cleanup. No isolated product
process remained in the subsequent elevated query. Separate observer,
Console and worker lifetimes before interpreting this as guest failure;
do not suppress product termination behavior just to obtain a passing test.

Original `int21map.c::TSR` delegates to NoTranslation. Selected DOSX
PMIntrDos explicitly treats 4C specially but not 31; original DOS
`msproc.asm::$Keep_process` truncates the current block and takes exit_inner.
The independent TSR_EXIT probe installs no hooks, keeps its own image/stack
paragraphs, and invokes 3100 after successful I/O. Its first isolated run
(`s38-tsr-exit16-r1-direct.txt`) reaches the normal success marker and the
isolated-package DOS prompt, then times out because the generic observer
expects worker exit. This is evidence of return to a parent prompt, not
proof of resident memory correctness, DPMI cleanup or a worker-exit defect.
Probe SHA256:
3e421013341658a21ed811ef3b0a195fba8a8980141f163d1e75356dc81af1c9.

A TsrExit runner mode was added to inject exit after the marker, but the
r2 attempt produced no observer report and therefore establishes no runtime
result. A fresh elevated process query found no isolated product process
left, and V: was removed. Resolve the observer failure and prove actual
parent continuation before accepting this family; no product change or
guest limitation classification has been made.

### Remaining closure reconciliation

This checkpoint supersedes stale pending statements in the chronological
INT21 and per-slot tables below, without deleting their historical findings.
Passing rows retain their explicitly recorded artifact identities: earlier
families use 31a1f80a and the corrected error-pointer/full-product runs use
308e493a. The newer relink is not covered by the latter runtime evidence.
This is not a declaration of whole-package acceptance.

| Work group | Evidence now available | Remaining closure action |
| --- | --- | --- |
| EXEC and PSP | CreatePSP signature/guard, parent identity/environment, ordinary child tail/status, inheritable/no-inherit local handles and real INT23/24 restoration; 16/32 load-only output/failure preservation/explicit image-environment free and actual loaded-entry/stack/tail call; TSR parent return, two following clients, resident DOS MCB and retained 16-bit DPMI selector/base/limit/data pass | The selected lifetime contract is disposed: ordinary clients free by owner while TSR resources remain usable. Do not equate a loaded child's RETF with DOS process termination or infer untested resident callback protocols. |
| Network translation | Real name equality; assignment create/5F02/5F05 query/break and error buffers; all 5D dispatch families classified; 22-byte DPL extended-error roundtrip; original print error comparison; DIV-290 non-aliasing ES:DI return and poisoned high EDI tests pass | No host repair remains: 5D01/03/04 are original no-ops, 5D00/02/05/06/11 are original untranslatable paths, 5D07--09 are the registered original spool defect, and 5D0A is fixed; preserve limits rather than invent services |
| IOCTL | DIV-285--288 real printer/media queries; controlled track/format/metadata/device packets; 02--05 full/short/error transfer and balanced buffers; 68 mapped negative result | Disposition complete: 00/01/06--0B/0E/0F are original register-only forwarding, 02--05 and 0C have real failure/guard plus controlled transfer proof, 40/41/42/46/47/60/61/62/66/67 have their original packet contracts, and 68 is selected DEM's explicit invalid-function path. Physical media success and a new DEM 68 provider are neither claimed nor added. |
| Other INT21 | File/FCB/directory/country/pointer/keyboard/DOS-memory families with named real positive/negative cases | Disposition complete at the selected host boundary: pointer-owning translators have direct/nested witnesses; no-translation and original unsupported-forwarding entries retain source semantics without a new host owner. Untested DOS action variants are not represented as a host gap or a blanket functionality claim. Original allocation/debug/spool defects remain TODO negative evidence. |
| PM publication and BOP ledger | Real IRQ/fault/nesting, publication layout/reuse/failure, descriptor/debug/native fixtures; selected CPU40/486 consumer audit complete; accepted guest debug/0500 limitations | Update the 25-slot ledger from actual evidence only; no speculative synchronization or unsupported WOW claim |
| Delivery | Formal x86 308e493a and all 17 product regressions; current-source four native fixtures and subsequent 3a4b7d3b relink; governance and whitespace checks | Matching-artifact regression after final relink, original mirror comparison/counts, source-shaped review, package publication and commit/push after the package criteria hold |

The DPL and print probes now have explicit runner marker gates, exercised in
both routes under `s38-network-dpl32-gated-r2` and
`s38-network-print16-gated-r3`. These four runs pass; they strengthen
automation, not broaden the capabilities proved by the existing probes.

### Original server DPL translation

#### Original guest spool defect disposition

The optional spool probe establishes the actual original dispatch, rather
than a missing DPMI conversion. The real-mode INT2F installation check returns
AX=11FF (handler installed). The real-mode direct AX=1125 request then returns
AX=0001 with CF, and protected 5D07 returns the same AX=0001/CF result.
Both direct and nested routes reproduce this in
`s38-network-spool-multiplex-r1-*` on worker 3a4b7d3b. The handler has
therefore received the multiplex value but rejected it.

Source shows why: DOS `srvcall.asm::SPOOL_OPER` invokes
`CallInstall NETSpoolOper,multNet,37`, yielding AX=1125h. Original
`resident.asm::MultHandler` dispatches AL=25h to `NetSpoolStuff`, whose
unmodified body subtracts and compares AL against 9, 8 and 7 only; 25h takes
its `ERROR_INVALID_FUNCTION` exit. `REDIR.EXE` independently reports that it
is already loaded in the isolated package, confirming this is not merely an
unloaded handler. A first batch had LF line endings and showed source text;
it was rejected, mechanically converted to CRLF, and then yielded the
already-loaded message. No guest binary, mirror source or host provider was
changed.

This is a reproduced original guest defect and is registered in TODO under
the standing immutable-guest authority. NETWORK_SPOOL remains a separate
negative test mode; it does not broaden the passing NETWORK_DPL no-op/DPL
roundtrip claim. No 5D07--5D09 spool functionality is claimed.

The original `srvcall.asm::SERVER_DISP` selects SC_NO_OP for 5D01/03/04,
not an unconnected host file-close provider. Its 5D0A selects
$SetExtendedError, consuming DPL AX/BX/CX and pointer fields; dpl.asm defines
the original 22-byte packet. NETWORK_DPL executes the three no-op returns,
then sets AX=5, BX=0803, CX=0200 through protected 5D0A and reads back
AX=5, BX=0803, CH=2 through 5900. All four 16/32-bit direct/nested routes
pass the packet guard, checked segment/stack, subsequent I/O and normal exit
on 31a1f80a. All transcripts contain
`S38_NETWORK_DPL_ERROR_ROUNDTRIP_OK` under
`O:/winnt/logs/s38-network-dpl{16,32}-r1-*`.
The no-op results are accepted only as the original no-op contract, not as
proof that remote files were committed or closed. No product repair or
substitute service was added. Nonzero UID/PID and returned extended-error
pointer translation are not established by this zero-identity packet.

### Original printer-setup unavailable boundary

NETWORK_PRINT compares real DOS 5E02/5E03 with protected calls, using
BX=FFFF, a 64-byte buffer plus guard, and both DS:SI/DS:DX input aliases
and ES:DI output addressing. Four 16/32-bit direct/nested routes pass
comparison of AX/CF, full ESI/EDI, segments/stack and final buffer/guard
under `s38-network-print{16,32}-r1` on 31a1f80a. The r2 16-bit witness
prints both AX values and both CF values as `0001 0001 0001 0001` and
passes both routes (`s38-network-print16-r2`). This proves matching
invalid-function errors for the tested invalid index, not usable remote
printing or all possible indices. Original macro.asm explicitly documents
the absent remote-print implementation; preserve that guest boundary.
No product or guest changes were needed. Probe hashes (16 r1,32 r1,16 r2):
633f923fe711c16bd94ed31ae49b03f743c2413335136be45656cd0e1f0b883a;
c8666eaae8757dbfca0b08e73731dd457fb4e174dc1fe48957840d8de59ffb24;
5c6bff141fef3b68ad5fd108eecb68f99a486e7b2fc7cad8918c40eee776fb70.

### Protected assignment create/query/break

The integrated runner's own Z: preflight, guest-removal check and finally
cleanup were executed successfully with the 16-bit r2 probe in both routes
(`s38-network-assign-runner16-r3`, outer exit zero). This supersedes reliance
on the external wrapper for the normal successful cleanup check; injected
failure cleanup is not claimed tested by that run.

The r2 extension checks the same selected entry through both 5F02 and
5F05 (type, BEEF user word and remote name), then breaks it and verifies
both exhausted-enumeration results. All four 16/32-bit direct/nested routes
pass with the explicit NetworkAssignCreate marker gate under
`s38-network-assign-create{16,32}-r2`; outer execution returns zero and
`net use` confirms no entries. r2 probe hashes: D38A16
7f6bcbd87836dd7b56b60573f0dea92d8344079427b2ca833d32d5ae51d8ac2c;
D38A32
1e86912a1d43885da0c495dba594308d4ca3d1d237d88689bc1fd058781674b9.
The runner now incorporates the previously external Z: preflight, checks
guest removal before PASS and removes only the exact test mapping on a
failed run. This does not test the unsupported legacy LSN metadata.

The S20 original guest recipe can execute through DPMI32 without a new
provider. Read-only preflight found local O$ present, Z: unused and no host
network mappings. NETWORK_ASSIGN_CREATE uses 5F03 to map Z: to
\\\\LOCALHOST\\O$, enumerates with 5F02, checks device type 4, user word
BEEF and all remote-name bytes, then removes it with 5F04. The existing
5F02/5F05 end-of-list buffer-preservation checks follow, then ordinary I/O
and exit. Direct/nested 16-bit and CODE32/high-memory routes all pass on
31a1f80a; all four transcripts explicitly contain
`S38_NETWORK_ASSIGN_CREATE_QUERY_BREAK_OK`. Logs:
`O:/winnt/logs/s38-network-assign-create{16,32}-r1-*`.
Host `net use` after each width reports no entries. The outer shell returned
1 after its final absent-mapping query, despite both inner tests passing;
the transcripts and independent empty mapping query are the evidence, not
that outer exit code. No product code was changed. Positive 5F05 metadata,
other network families and cancellation remain separate obligations.
The 16-bit probe hash is
b75f67065c0fe402e94a365b1f2e1d6cb3d48974015935cceda346dcb519a117.

### Original assignment-list negative translation

NETWORK_ASSIGN exercises protected 5F02 and 5F05 with enumeration index
FFFFh, without creating/deleting host mappings. Original Func5Fh translates
the 16-byte DS:SI and 128-byte ES:DI buffers; original resident.asm routes
both requests to VrGetAssignListEntry, whose exhausted list returns
ERROR_NO_MORE_FILES. All four 16/32-bit direct/nested routes return CF/AX=18,
preserve full ESI/EDI, segment/stack state and all 144 buffer bytes plus
the guard, then complete normal file I/O and exit on candidate 31a1f80a.
Logs: `O:/winnt/logs/s38-network-assign{16,32}-r1-*`; all four transcripts
contain `S38_NETWORK_ASSIGN_END_PRESERVED_OK` (separately inspected).
The runner now includes a NetworkAssign marker gate for subsequent runs.
Probe hashes: D38A16
bc4fc5bd23247ebad600de4e6674390c2ade1ca12b1b2a1bdf5ea9d45e7e1ac2;
D38A32
da0ca2212791f32ab94fe0ca8644d1b85a9a991411767dfa4edffa964d089ee9.
No product diff was added. This is negative dual-buffer mapping/return
evidence, not successful entry enumeration, define/break lifecycle, printer
setup, MapDPL, or arbitrary modern SMB coverage.

### Original network machine-name translation

Adjacent source audit: original `dos/v86/doskrnl/dos/macro.asm::$UserOper`
explicitly says NT ignores setting the computer-name string (retaining
MYNUM), and delegates printer-info calls to INT2F even though the original
redirector does not implement remote printing there. `UserPrint` uses
INT2F/111F. Current DPMI32's GetPrinterSetup/SetPrinterSetup names alone
therefore do not prove a working printing facility: the 5E table selects
them for AL=2/3 and maps input/output buffers before invoking that unchanged
guest. This is a source-supported original capability boundary, pending
real baseline/protected return comparison before any limitation is closed.
It is not authority to add a modern print provider or modify guest media.

Current `Func5Eh -> GetMachineName` maps sixteen bytes through the original
DOS 5E00 provider. Independent NETWORK_NAME probes first query real DOS,
then compare every returned byte from protected-mode 5E00 against that
baseline, require CH validity, preserved EDX/segments/stack and an unchanged
trailing guard, and finish normal protected file I/O/exit. Both 16-bit and
CODE32/HIGH_BUFFER/HIGH_OFFSET builds pass direct/nested routes on candidate
31a1f80a (`O:/winnt/logs/s38-network-name{16,32}-r1-*`); all four transcripts
contain `S38_NETWORK_NAME_REAL_PM_EQUAL_OK`. The runner now has an explicit
NetworkName marker gate; r1's markers were separately inspected.
Probe hashes: D38N16
deba0e79b1b3b76d992ef9a6ffe052ed4d9b7d244ce4635789893fc479f5176d;
D38N32
57b2c0a050e6eb520d54b2d33af87ecf49e16dad8d87e65f96f0a5f187faaa8c.
No product changes were needed. This closes only machine-name buffer
translation evidence, not MapDPL, printer setup, assignment-list operations,
or every 5F redirector extension; S20 provider proof is not substituted for
those protected-mode translation obligations.

### EXEC load-only original-owner audit (not runtime acceptance)

All 17 established text-gated product routes now pass on candidate
31a1f80ab2bfe5596f51fcff8e5722d4abdf4b36d884fd55ab4e12689e6a145a;
logs use `O:/winnt/logs/s38-exec-div289-product-r1-*`. This includes direct,
nested and repeated MEM, native streams/EOF/status, guest exit status and
EDIT return. The formal candidate remains isolated and unpublished; S38
package closure still requires the remaining capability/lifecycle ledger.

The explicit ExecLoadOnly-gated matrix now passes all four 16/32-bit
direct/nested routes (`s38-exec-load-complete{16,32}-r1`). Before the positive
load, an absent filename returns CF/AX=2 and preserves both poisoned output
DWORDs. Successful COM loading returns matching CS/SS and IP=0100, exposes
the PSP signature, and allows the probe to free the environment/image via
original DOS services, finish its normal I/O, and exit. The 32-bit variant
uses CODE32/HIGH_BUFFER/HIGH_OFFSET; this proves the selected extended
parameter layout used by LoadExec, not every historical debugger ABI.
Probe hashes: D38L16
41e172292c4f34ba64ae164c1815be3daac7c75d1614dc7706659455d62e0180;
D38L32
70b3f199441f0e075d110d1673a8958435fcb76088971dd43a49ee311ed7180b.
The earlier isolated CODE32 run also passed both routes
(`s38-exec-load-clean32-r1`). These results do not imply loaded-child
execution or a complete inherited-handle/vector lifetime audit.

The 16-bit load-only cleanup witness now passes direct and nested routes on
DIV-289 candidate 31a1f80a under `s38-exec-load-clean16-r1`. Probe hash:
eedf7f0a6be4501e25f6b6cbd852ab79a61f4fac999fd8648a42d87747595e70.
It checks the returned COM IP=0100, equal initial CS/SS and PSP INT20
signature, reads the child's environment segment, restores parent identity,
then uses original real-mode DOS AH49 via DPMI 0300 to free environment and
image. Both transcripts contain `S38_EXEC_LOAD_ONLY_RETURN_FREE_OK`;
ordinary subsequent protected file I/O and normal exit pass. Thus the earlier
COMMAND allocation failure was resolved by completing test-owned cleanup,
without another product change. This does not prove all inherited handle or
vector cleanup, loaded-child execution, failure-output preservation, or the
32-bit caller. The runner now offers ExecLoadOnly to gate this marker in
future runs; r1 was independently checked for the marker after its run.

DIV-289 now mirrors the original dosex4 eight-byte output copy in the existing
host LoadExec owner, only for successful AL=1. It retains the parameter-block
destination before the original command-tail dereference; no loading or PSP
policy is replaced. Direct unchanged host composition was proven to omit
the return. The guest assembly cannot be edited/rebuilt, so its finite copy
contract is expressed at the existing host translation return, not in a new
loader or adapter. Failure publishes no output. The 32-bit caller contract
and lifecycle remain unaccepted pending their tests.

Formal x86 compilation/link and VdmTib ownership pass for candidate
31a1f80ab2bfe5596f51fcff8e5722d4abdf4b36d884fd55ab4e12689e6a145a.
Only D38TRACE received this candidate. The identical r3 negative probe now
captures `FC FF FC 09 00 01 FC 09` (SS:SP=09FC:FFFC, CS:IP=09FC:0100),
instead of eight A5 bytes; see `s38-exec-load-only16-fixed-r1-direct.txt`.
The intentional failure and subsequent COMMAND memory error still occur;
this is output-copy evidence, not load-only lifecycle acceptance. Ordinary
EXEC child direct/nested controls both pass on the candidate under
`s38-exec-div289-control16-r1`. Full product regression remains to run.
The pre-repair 29d27139 worker is retained in
build/M0-T420/S38/exec-load-only-r3/before-ntvdm.exe. The earlier 17-route
pass applies to that pre-repair hash, not to this new candidate.

The r3 test-only variant calls protected INT21/50 with the saved parent PSP
after AL=1, before emitting its captured output. Its SHA256 is
5d9583637ecca867ecdcfad4005bf56c4550a378a4503983239a7e6cf2a58b84.
`s38-exec-load-only16-r3-direct.txt.console.txt` now shows the prior child
tail success, `S38_EXEC_LOAD_ONLY_CF0_OUTPUT_UNCHANGED`, the intentional
probe failure marker, then `Memory allocation error` and
`Cannot load COMMAND, system halted`. The observer timed out after 15s.
This strengthens attribution of the output omission but does not validate
the probe's load-only teardown: restoring PSP identity alone is insufficient.
The child image/environment allocation and parent return state need an
original-owner cleanup audit before a passing load-only test or repair can
be claimed. No product change was made. The r2 checkpoint files were moved
without deletion to logs/s38-exec-load-only16-r2-{pre,post}.bin.

The next controlled run establishes the missing output dynamically. The
unchanged prior EXEC child probe passed direct and nested routes again under
`s38-exec-control16-r2`. Test-only `exec-load-only-r2/D38L16.COM`
(SHA256 26473b19fb10774ebaca6b3251faa653b28a49231364f12dd28c2adc5cb25dd2)
created and closed `tests/L38PRE.OUT` before AL=1, then created, wrote and
closed `tests/L38POST.OUT` only after checked AL=1 returned with CF clear.
The latter contains exactly `A5 A5 A5 A5 A5 A5 A5 A5`: both poisoned
stack/entry DWORDs were unchanged. Paths are below the isolated D38TRACE
test directory and were preflight-absent. This proves the 16-bit output
omission, not valid child cleanup or 32-bit behavior. The launcher still
ended with 1067 and no captured Console text
(`s38-exec-load-only16-r2-direct.txt`); its later termination remains a
separate unresolved observation. No product repair is claimed.

The first isolated negative probe (`exec-load-only-r1/D38L16.COM`, SHA256
e4137713fd904a88c8681e02991af68a39b5ca2103094f143176ea34ec1cbd6e)
ran through the normal observer with logs
`O:/winnt/logs/s38-exec-load-only16-r1-direct.txt` and its Console transcript.
The launcher exited 1067; the transcript contained no guest text or probe
marker. This does NOT establish that AL=1 was reached, nor prove its output
gap dynamically. No nested route ran. The runner's cleanup and subsequent
elevated process query left no ntvdm/basesrv process; V: was removed.
The next run needs a known-good control and a pre-call checkpoint before
attributing this failure. The test-only EXEC_LOAD_ONLY build always fails
acceptance intentionally; it emits a specific marker only if CF=0 and both
output DWORDs remain poisoned. Ordinary EXEC builds retain their old layout.

Inspection of `dpmi/486/dxmain.asm::dosex4` found an existing original
AL=1 return implementation: four `movsw` copy `xsssp` and `xcsip` from
`npXfrBuf1` to the protected caller's parameter block. The original
`execblk` and `dos/v86/inc/exe.inc::EXEC1` put these eight bytes at offset
14. `dos/v86/doskrnl/dos/msproc.asm`, after testing
`EXEC_FUNC_NO_EXECUTE`, writes SP, SS, IP and CS there and returns success.
Thus the original guest has a positive load-only return contract; this is
not source evidence for an unsupported guest operation.

The selected `PMIntrDos` invokes `XlatInt21Call` before its assembly
translation path. Its comment explicitly says a translated call simulates
IRET instead of reaching that fallback. Current `int21map.c::LoadExec`
sets ES:BX to `LargeXlatBuffer`, executes DOS, restores the environment and
registers, and simulates IRET without copying those output fields back.
A function-bounded ordinal comparison against
`O:/repos.external/OpenNT/base/mvdm/dpmi32/int21map.c` returned equality
for all 4,320 characters of LoadExec through its following separator.
This identifies an original host translation gap, not an added project diff
and not an approved immutable-guest limitation.

Next verification must observe actual AL=1 output and child/PSP/environment
lifetime before selecting a repair. The existing assembly return is the
first source-shaped recovery candidate; 32-bit client layout and safe
load-only cleanup still require proof. No product change, new loader,
guest patch, passing runtime claim or TODO exemption follows from this
static finding. Formal and isolated workers were rehashed equal at
29d2713966be0ff0b79d4846cad12c278f79b07a3256ef45f02c6b7224abf8cc.

### Original protected EXEC success and refusal witnesses

Original `int21map.c::LoadExec` owns filename/tail translation, low-memory
DOS parameters and PSP environment-selector restoration. It rejects AL=3
overlays with CF/AX=1. EXEC_FAILURE checks that refusal and a preflight-proven
absent tests/NOEXEC38.EXE returning error 2 via 4B00. Each call checks BX/DX,
segments/stack, PSP identity and environment selector at 2Ch, followed by
ordinary file I/O/cleanup. Four 16/CODE32 direct/nested routes pass under
`O:/winnt/logs/s38-exec-failure{16,32}-r1-*`. Probe hashes under
`build/M0-T420/S38/exec-failure-r1`:

- D38E16.COM: `edfac1a76449435003225c7bdd63b0ffcce3df652353fdeb352696bfbdf7790d`.
- D38E32.COM: `5010e3925326314ae5ecdc34eb1a66e8852625462fa74c837142e430e70efdc7`.

EXEC_CHILD adds an actual 4B00 guest child after these failures. Independent
D38CH.COM checks its three-byte ` OK` tail and CR, emits
S38_EXEC_CHILD_TAIL_OK and exits via INT21/4C07. The protected parent checks
INT21/4D AX=0007, restored PSP/environment selector and later I/O/cleanup.
Four 16/CODE32 direct/nested routes pass on formal 29d27139 under
`O:/winnt/logs/s38-exec-child{16,32}-r1-*`. The child is not a run16-launched
sibling worker. Artifacts under `build/M0-T420/S38/exec-child-r1`:

- D38CH.COM: `f37ced2b938b5e0498e91b1619495352199fc28a1bec24546f48badb5219bc37`.
- D38E16.COM: `a6459ef17ad793940c0d22aa81a4c2d579e143042ca37489f43fd8b3bdfed48f`.
- D38E32.COM: `9dc79c2de9701bac177caa007417bb5ecf4b1bc2521cb6cd4d376e92d65897a7`.

The disposable child is staged under the isolated package's tests directory;
the runner requires child and parent markers and rejects S38_FAIL. Future
run metadata includes its hash; these r1 runs precede that metadata addition
and use the explicit hashes above. An initial NASM conditional/repetition
error was corrected before executing a parent; it was a test build error,
not a product failure. No product code or original guest media changed.
Load-only AL=1, TSR and all inherited environment/PSP fields remain separate
obligations, not implied by these EXEC/return and refusal tests.

### Original CreatePSP real-call witness

Selected host `int21map.c::CreatePSP` converts the target selector to a low
real-mode segment, invokes original INT21/26, restores DX/segments/stack,
and leaves targets above ONE_MB untouched. Original guest
`dos/v86/doskrnl/dos/misc.asm::$CREATE_PROCESS_DATA_BLOCK/SETMEM` owns the
PSP structure and pointer publication. No new PSP implementation is needed.

CREATE_PSP uses INT31/0100 to allocate 17 DOS paragraphs, invokes INT21/26
on that allocation's selector, and checks INT20 at 0, INT21/RETF at 50h,
the local JFN pointer at 34h/36h and a guard at 100h. It then frees through
INT31/0101 and checks that INT21/51 still identifies the original parent.
The existing probe checks segment/stack preservation and subsequent ordinary
file I/O/cleanup. All four 16/CODE32 direct/nested routes pass on formal
29d27139, logs `O:/winnt/logs/s38-create-psp{16,32}-r1-*`.

The strengthened CODE32/high-memory variant first calls CreatePSP with its
above-1-MiB DS selector and verifies the target's first DWORD is unchanged,
then runs the same low-DOS create/free and parent-identity checks. It does
not invent a CF error contract: the original high-memory path does nothing.
Both direct/nested runs pass under `O:/winnt/logs/s38-create-psp32-r2-*`.
This is a prefix mutation sentinel, not a whole-allocation bytewise audit.

Artifacts:

- `build/M0-T420/S38/create-psp-r1/D38P16.COM`:
  `02bc99f978f476a4c9f0cbb70279a58e08d4132cb7747c5e83e1f8eed04c66c3`.
- `build/M0-T420/S38/create-psp-r1/D38P32.COM`:
  `b0d8e89999c4050a7426f43129464596205c4e6cae91a4bc482975a3f4c13756`.
- `build/M0-T420/S38/create-psp-r2/D38P32.COM`:
  `103151973273897e28e7920522f3343379a9a7127e18aa24945a58fd6d4f62c7`.

The probes write only their allocated test memory using the original API;
no original guest media/private state is patched and no production source
changes occur. These results do not establish the semantics of every copied
PSP field, arbitrary invalid selectors, child EXEC, or TSR. Those are not
implied by the generated entry/pointer and parent-identity checks.

### DIV-288 media/access packet mapping recovery

The 0866 failure below is repaired by admitting both original DEM pairs
46/66 and 47/67 into IOCTLBlockDevs and selecting their original packed
MID (25 bytes) and ACCESSCTRL (two bytes) lengths. Original buffer allocation,
copyback, driver dispatch, volume query and policy are reused unchanged.
NoTranslation previously forwarded protected DS:DX without this conversion.
Unchanged reuse reproduces the real query failure; the existing mapping
owner is sufficient once its dispatch covers the original provider's pairs.
No new adapter, provider, guest change or external-code import is needed.

Formal x86 compile/link and VdmTib gate pass. Non-mock worker SHA-256:
`29d2713966be0ff0b79d4846cad12c278f79b07a3256ef45f02c6b7224abf8cc`.
Real read-only media queries now pass all four 16/CODE32 direct/nested
routes, comparing all 25 returned bytes against the same run's real-mode
query, plus guard, state and subsequent file cleanup. Logs:
`O:/winnt/logs/s38-media-query{16,32}-fixed-r1-*`. Probes under
`build/M0-T420/S38/ioctl-media-query-r1`:

- D38Q16.COM: `5b98a52384297ef45018df00810fac1ad84b2650a9e080e275d82242e0836206`.
- D38Q32.COM: `8f3a58efc3b2a54e63604ffc7a6fe302e28194bf86dabd0ceca50dbf50ff8425`.

Separate controlled tests exercise all four 46/66/47/67 packet transfers in
each of four 16/CODE32 direct/nested routes. The mock checks exact length
and every input byte, leaves set data unchanged, supplies query data, and
checks original buffer-pool balance. Guest assertions check every returned
byte, the immediate guard at each actual packet length, state and ordinary
file cleanup. All 16 requests pass with 16 balance records and no S38_FAIL
record. Logs: `O:/winnt/logs/s38-metadata-pairs{16,32}-r1-*` and
`O:/winnt/logs/s38-ioctl-boundary-r10.events.txt`.

Diagnostic worker `build/M0-T420/S38/ioctl-boundary-r10/ntvdm.exe`:
`e74e4cb76dd8104a168a74ad559dd09e98cf698874bc7c83901024420c34a750`.
IOCTL_METADATA_MOCK probes under `build/M0-T420/S38/ioctl-metadata-pairs-r1`:

- D38M16.COM: `2853cd889f7958fe9fc95f80a6b7eb63685f458f8821fb330bc23e795f259f3a`.
- D38M32.COM: `0245abfbd70256580d2d3fb5f95b630faf12d8d30bd9248e0755cbaaa79ca060`.

Both CODE32 probes use CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET. The mock
intercepts only AX=440D/BX=00FE and is never a product provider. No real
volume metadata was set; these tests prove the pointer/packet boundary,
not physical media-setting/access-control capability. The real 0866 query
alone exercises original DEM and the host volume-information service.
All 17 text-gated product regressions pass on the same non-mock candidate,
logs `O:/winnt/logs/s38-media-div288-product-r1-*`. An exhaustive predicate
check over CL=0..255 yields only 40/41/42/46/47/60/61/62/66/67/68: the two
track cases return through their separate owner and every other admitted
value has an assigned length. This is dispatch evidence, not whole-family
runtime coverage. S38 remains open; official O:/winnt is unchanged.

### DIV-287 format packet length recovery

The original IOCTLBlockDevs predicate admits CL=42h but its length switch
omits that case. The controlled pre-copy check reproduced Length=1 for a
basic five-byte packet; it terminated before a device call. Logs:
`O:/winnt/logs/s38-format-before-r1-*` and
`O:/winnt/logs/s38-ioctl-boundary-r8.events.txt`. Diagnostic worker hash:
`3b8dee73b224217c4867be21900ef79867ef1f68543952772945894ca5323bcb`.
This is an original host initialization defect, not a guest limitation.

Original `dos/v86/inc/ioctl.inc::A_FORMATPACKET` defines Functions, Head,
Cylinder and FP_TRACKCOUNT, with its explicit note that track count is
meaningful only when Functions bit 1 is set. Original DEM's packed FMT_BLOCK
consumes the five-byte prefix. DIV-287 adds the missing switch case and
selects five bytes without that bit, seven with it; existing mapping/copyback,
driver selection and error return remain unchanged. This does not add fast
format to DEM or invent a physical format provider. Direct unchanged reuse
fails the length witness; an outer facade cannot initialize this owner's
unassigned local Length, so the two-statement owner-local case is the first
viable source-shaped recovery. No external-code import is needed.

The test boundary verifies head/cylinder and the optional count before
returning Functions=0. Real protected probes check success, field copyback,
guard, segment/stack preservation and subsequent file I/O. Eight basic/
extended, 16/CODE32, direct/nested routes pass under
`O:/winnt/logs/s38-format-{b,e}{16,32}-r1-*`. The r9 boundary log contains
four five-byte packets, four seven-byte packets, eight balanced original
buffer-pool records and zero S38_FAIL records. No disk call or guest-media
mutation was performed. These are mapping mock results, not physical-medium
or full DEM format acceptance.

Diagnostic worker `build/M0-T420/S38/ioctl-boundary-r9/ntvdm.exe`:
`3feed748589a7269ec8115d04c8fe0108924bdf68e33f7213f824c28acf6a614`.
Formal non-mock x86 compile/link and VdmTib gate pass, worker:
`981c1ead4f62ce64c8b0727dac020233dbd37c6890d6456062394dfbd8ee8a95`.
All 17 text-gated product routes pass for this candidate under
`O:/winnt/logs/s38-format-div287-product-r1-*`. The initial attempted prefix
s38-format-product-r1 already named older logs and was refused before a run;
those historical logs were preserved and are not used for this candidate.
The isolated package now holds 981c1ead; official O:/winnt remains unchanged.
The compiler still issues its general
switch-path Length warning; no default policy was added just to suppress it.

Probes in `build/M0-T420/S38/ioctl-format-probe-r1` define IOCTL_FORMAT_MOCK;
E adds FORMAT_EXTENDED; 32 adds CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET:

| Probe | SHA-256 |
| --- | --- |
| D38B16.COM | `ad2efd75e593a252b9996a7bf3cdabcc463dbd007d2bc8be968dcaf0a30ed6f3` |
| D38B32.COM | `1495f29578ede8df66f458c6a11d2fd32859823782b18a449d6c2c23ae7bc5cc` |
| D38E16.COM | `9a548c71b15224d3d2aa97a6ddac18d30db7406ade90825bf7f1ff2bec4bd3e5` |
| D38E32.COM | `4fe4e8571b93be437e2c95911b3bbd02b2deb8b92ee57457ad79de321f75de1a` |

The complete 8-bit predicate sweep admits precisely 40/41/42/60/61/62/68.
The explicit 41/61 branch returns before the length switch; after DIV-287
all other admitted values have a length assignment. No default behavior was
invented merely to silence the compiler's broader path warning.

### Additional media-query mapping gap

The selected original DEM implements 46/66 (media ID) and 47/67 (access
flags), but DPMI's IOCTLBlockDevs predicate rejects them into NoTranslation.
Original demIoctlDiskGeneric handles 66 before its BDS/no-attached-media
check, calling GetMediaId and ultimately GetVolumeInformationOem. This is
a real supported read-only host-volume query, not a physical-track request.
Packed MID is 25 bytes; ACCESSCTRL is two bytes. NoTranslation does not
convert the client protected DS:DX to a real-mode buffer for these calls.

A real CODE32/high-memory probe queries 0866 in real mode first, then
compares the protected-mode result with that 25-byte baseline and a guard.
On formal candidate 981c1ead, the real query succeeds but the protected test
fails: S38_IOCTL_MEDIA_REAL_OK followed by S38_FAIL_INT21_IO_STATE, exit 1.
Logs: `O:/winnt/logs/s38-ioctl-media32-before-r1-direct*`. Probe
`build/M0-T420/S38/ioctl-media-query-r1/D38Q32.COM` has SHA-256
`8f3a58efc3b2a54e63604ffc7a6fe302e28194bf86dabd0ceca50dbf50ff8425`.
This failure is not yet a trace-level attribution of its precise return or
write destination; it corroborates the source mapping gap, not a complete
family disposition. The new IoctlMediaQuery gate requires its explicit
success marker. No volume metadata was changed. Audit/recovery of the
paired media/access mapping family and its controlled write-side tests
remains within S38; do not waive it as an immutable-guest limitation.

### DIV-286 controlled track packet witness

Original IoctlReadWriteTrack copies 13 bytes into a low-memory DOS packet,
then the 32-bit branch reads a WORD at offset 0D from that copy. A 32-bit
client's selector occupies bytes 13--14 in its original parameter block,
outside that copy. The correct minimal recovery does not enlarge the DOS
wire packet: DIV-286 reads this selector from original ParameterBlock instead
of BufferedPBlock. The transfer DWORD at offset 9 is already inside the copy.
DOS's original 13-byte layout, chunk algorithm, allocation/free and driver
interfaces remain unchanged. No additional production helper is introduced.

The test-only `dpmi_ioctl_boundary.c` compiles the actual original int21map
owner and replaces only its host_simulate call for AX=440D/BX=00FE. This
probe-only invalid drive token receives geometry (512 bytes/sector) and one
sector of 5A data. The wrapper supplies the real-mode interrupt-frame return;
it does not invoke DOS/DEM/device I/O for the intercepted request. Therefore
this proves the DPMI mapping boundary, not a real disk or complete DEM route.
All other calls use the original host_simulate function. No physical disk
write/format or original guest-media change occurred.

The r3 diagnostic stops before the short copy with logs
`O:/winnt/logs/s38-ioctl-boundary-r3.events.txt`: geometry reached, requested
parameter bytes=13, client bytes needed=15. This is a boundary detector,
not a reproduced physical disk corruption. Its diagnostic worker SHA-256 is
`5f1125e98d893d84e8afe5dc86bf05200efab9829a2326febd0e5d9980943b3f`.
The final fixture correctly requires the unchanged 13-byte DOS copy, while
the real protected guest verifies all 512 returned bytes and a trailing
guard, register/stack preservation and subsequent file I/O/cleanup.

Four 16-bit/CODE32 direct/twice-nested mock routes pass under
`O:/winnt/logs/s38-ioctl-track{16,32}-fixed-r2-*`; the CODE32 client uses
high-memory backing, forcing the parameter bounce. Boundary logs are
`O:/winnt/logs/s38-ioctl-boundary-r5.events.txt`. Diagnostic worker
`build/M0-T420/S38/ioctl-boundary-r5/ntvdm.exe` has SHA-256
`e04a65d21b36e7bb56f69e0dbc04f2423c1409d6c32c4bba2160203da6469ab5`.
The formal non-mock worker compiles/links and passes the VdmTib gate, hash
`adcd4be38e13256de2bd6fec60f44cf19419551bb0c12728eaca2bad0a862aee`;
all 17 text-gated product routes now pass under
`O:/winnt/logs/s38-track-product-r1-*`. Early mock runs restored db03efc9;
after this regression, the final balance run restores adcd4be3 to D38TRACE.
The official O:/winnt package remains unchanged.

Fixture corrections are not product failures: r1/r2 wrapped host_cpu's
function-pointer name, but this DPMI unit calls softpc.h's host_simulate
function directly, so no boundary log appeared. r4 incorrectly checked the
following 512-byte data mapping as a parameter mapping; r5 consumes the
geometry marker exactly once. The retained failed logs distinguish these
fixture defects from the source-proven short-copy defect. The later matrix
below extends transfer evidence; full lifetime accounting and the CL=42
length defect remain open. These passes do not close the IOCTL family or S38.

The r6 controlled boundary verifies a nine-sector transfer split into 4096
and 512 bytes, including advancing the first-sector field from 0 to 8. Reads
fill every byte with 5A; writes verify every byte before reporting success.
The injected failure refuses the second read chunk with CF/AX=5 without
touching its data. The guest asserts the first 4096 bytes changed and the
remaining 512 bytes stayed zero, plus its trailing guard, segment/stack state
and subsequent ordinary file I/O/cleanup. Six probes times direct/nested
routes pass (12 runs); this is bounded transfer/error evidence, not proof
that every buffer accounting counter or every future device is correct.

Logs: `O:/winnt/logs/s38-track-{r,w,f}{16,32}-r1-*` and
`O:/winnt/logs/s38-ioctl-boundary-r6.events.txt`. The test worker in
`build/M0-T420/S38/ioctl-boundary-r6` has SHA-256
`7609a12d9d11cb7e3f9debb8af46bed12593db311b3da07377c866ac066c616a`.
NASM probes in `build/M0-T420/S38/ioctl-track-family-r1` define
IOCTL_TRACK_MOCK/TRACK_SECTORS=9; W adds TRACK_WRITE, F adds TRACK_FAIL,
and 32 adds CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET:

| Probe | SHA-256 |
| --- | --- |
| D38R16.COM | `0fc18ca2e8301e72d48d4cabdd49a2cdcf20c4f6212d200b4cca1dd4c9133947` |
| D38R32.COM | `e7edca816a2063df860badce03d81ecda8d45c0709bb3303ebf847121c2b70be` |
| D38W16.COM | `96eb9b72f70786c9107012c4cf46f5445ce59bbcfa815a2c0f94fde6467b4c3b` |
| D38W32.COM | `4fc07bcc66bb6e1fa445555c70931c41fb99c9f0178e2aa6dec00144c0885ce5` |
| D38F16.COM | `dffd0b770ca9f59ae9e222749304292715eece1fe0c36b27d20ff7bf12ecb98c` |
| D38F32.COM | `d5841baa0c6855126cdd5392836c4cc8ccf7e5a1d7907e8487d90e3cc126761b` |

The runner requires S38_INT21_TRACK_MOCK_TRANSFER_OK for this expanded
matrix; the earlier one-sector probe used the historical READ_OK marker.

The r7 wrapper additionally observes SmallBufferInUse and
LargeBufferInUseCount before entering the original track owner and requires
both to equal their entry values immediately before its IRET. It changes
neither counter. All 12 transfer/error routes pass again under
`O:/winnt/logs/s38-track-{r,w,f}{16,32}-balance-r1-*`; boundary log
`O:/winnt/logs/s38-ioctl-boundary-r7.events.txt` contains exactly 12
S38_IOCTL_BUFFER_BALANCED records and zero S38_FAIL records. Diagnostic
worker SHA-256 is
`bc7ca3c861174b294b5c4923806c80ec141b41107bf8c7b4bdf13097968ac1b8`.
This proves synchronous buffer-pool balance for these success and second-
chunk-failure cases; it does not prove asynchronous cancellation, unsupported
geometry, or physical medium behavior. No new production change was needed.

### DIV-285 real printer IOCTL address recovery

The real read-only PRN witness replaces the initially proposed mock for
440C. It opens PRN read-only, reads CX=0565 retry count in real mode, closes
the handle, complements the target WORD, enters protected mode and repeats
the query into the nonzero-offset target. It checks the returned value
against the real-mode baseline, a trailing guard, segments/stack and later
file I/O/cleanup. No printer output or retry-count mutation is requested.

Before correction, acf9b0a9... returned success from both original-driver
queries but failed the protected target comparison. The r2 transcript has
S38_PRINTER_REAL_READ_OK, S38_PRINTER_PM_RETURN_OK and
S38_FAIL_INT21_IO_STATE; logs are
`O:/winnt/logs/s38-printer-ioctl16-r2-direct*`. This is an original host
mapping defect, not an immutable guest restriction.
Both pinned OpenNT and OpenNT-4.5 int21map.c have SHA-256
`b0a8632ddb1bb0ed609115bcb113c7a43a0b042930cd281d333540089589c7e1`
and contain the same omission.

DIV-285 adds only `Data += (*GetDXRegister)();` in original IOCTLMap2Bytes,
using the adjacent original MapDSDXLenCX addressing shape. Original driver,
bounce/copy/free and IRET owners remain; no adapter, overlay or new provider.
Unchanged direct reuse reproduces the failure; a separate facade cannot
restore a client offset that this owner omitted. The one-expression owner
correction is the first viable source-shaped recovery, with no reason to
introduce external-code or autonomous replacements.

Formal x86 compilation/archive/link and VdmTib ownership gate pass. Candidate
and probes are retained in `build/M0-T420/S38/int21-printer-ioctl-r2`:

- ntvdm.exe: `db03efc9b32e2c00c5d48e55d1a59db993a56835e0f57b22f1aff47d87080296`.
- D38P16.COM: `fe93b004287fed729069fb62335624709f4bbf6c6de4c5b4c316e6d53c3af1e5`.
- D38P32.COM: `5ff098c41f40931ed38c813cf92df1cfd480d6a9cfb7c97591c0434e9c5bd040`.

Both define IOCTL_PRINTER; the latter adds CLIENT32/CODE32/HIGH_BUFFER/
HIGH_OFFSET. Four real direct/twice-nested routes pass with
S38_INT21_PRINTER_IOCTL_READ_OK under
`O:/winnt/logs/s38-printer-ioctl{16,32}-fixed-r1-*`. The PRN target has a
nonzero low offset in both cases and high-memory backing in CODE32; the
existing subsequent file test, not the PRN query, supplies the above-64K
offset assertion. The runner now offers IoctlPrinter to require that marker.
The explicit IoctlPrinter gate passes all four routes again under
`O:/winnt/logs/s38-printer-ioctl{16,32}-gated-r1-*`. All 17 established
text-gated product routes pass on the same db03efc9... candidate under
`O:/winnt/logs/s38-printer-product-r1-*`, including EDIT return.
S38 remains open;
the other IOCTL mapping hazards below remain unaccepted. Only the isolated
test package was updated; official O:/winnt was not replaced.

### IOCTL boundary audit and safe failure evidence

Source follow-up identifies a real, non-printing success witness before
introducing a mock: `dos/v86/doskrnl/bios/mschar.asm::prn_genioctl`
accepts category IOC_PC (5), GET_RETRY_COUNT (65h), and writes the WORD
retry count through the supplied generic-IOCTL packet. A protected client
can open the original printer device and issue AX=440C/CX=0565 with a
nonzero buffer offset and guards. This reads driver state, not printer
output; no printer data write or retry-count change is needed. Whether the
deployed immutable driver accepts that open/call still requires a real run.
The next witness should attempt this original provider before a substitute.

The original `dos/v86/doskrnl/dos/ioctl.asm::GENERICIOCTL` dispatches disk
category 8 through SVC_DEMIOCTL; its introductory unsupported comment is
not the selected implementation. `dos/dem/demioctl.c::demIoctlDiskGeneric`
implements device parameters, track read/write/format/verify, media ID and
access flags. Its GETDPM path can query host geometry even without a BDS;
therefore absence of attached floppy media alone does not exclude this call.

For the missing 42h length case, original `dos/dem/demdasd.h` declares a
packed five-byte FMT_BLOCK (BYTE Functions, WORD Head, WORD Cylinder), which
both DEM format and verify consume. However, original `dos/v86/inc/ioctl.inc`
declares a seven-byte A_FORMATPACKET with optional FP_TRACKCOUNT for fast
format. This is an explicit consumer-contract distinction: do not infer a
universal five-byte format packet from the existing 62h mapping. Determine
the selected provider's complete supported contract in the isolated test.
No real track write/format was executed and none of these source findings
constitutes a runtime pass or an immutable-guest limitation.

Selected IOCTLXlatTable sends 4402/4403/4404/4405 through MapDSDXLenCX,
440C through IOCTLMap2Bytes and 440D through IOCTLBlockDevs. These are live
host mappings, not excluded by the nearby historical block-driver comment.
Review against pinned OpenNT identifies three pre-existing source hazards
that still require controlled success-path witnesses before repair:

- IOCTLMap2Bytes resolves the DS base but never adds DX, unlike the adjacent
  MapDSDXLenCX owner. Its two-byte copy can therefore target the wrong bytes.
- IOCTLBlockDevs admits CL=42 through its predicate, but the length switch
  has no 42 case before using Length. The formal compiler warning is
  consistent with this uninitialized path; the correct format contract must
  be established from the original device owner, not guessed from 62.
- IoctlReadWriteTrack copies 13 bytes, while the DPMI_32BIT branch reads a
  WORD selector at offset 0D (requiring 15 bytes). A high-memory bounce case
  must verify this; a direct low-memory view could hide the short copy.

No physical track write/format will be used to investigate these. The next
success-path evidence must isolate the real-resource boundary with a
test-only controlled provider while retaining the original mapping owner.
These findings are not immutable-guest limitations and are not accepted
as completed capabilities.

IOCTL_FAILURE now supplies real negative evidence for 4402, 4403 and 440C:
BX=FFFF prevents reaching a valid device; each call must return CF/error 6,
preserve DX/segments/stack and leave the two-byte test buffer and trailing
guard unchanged. Subsequent ordinary I/O and cleanup must pass. All four
16-bit/CODE32 direct/twice-nested routes pass on acf9b0a9... with
S38_INT21_IOCTL_INVALID_HANDLE_BUFFER_OK. Logs:
`O:/winnt/logs/s38-ioctl-failure{16,32}-r1-*`.
NASM artifacts in `build/M0-T420/S38/int21-ioctl-failure-r1`:

- D38J16.COM: `10c08b99108add7e4bcf9cec9eecd79658b553b87d790dcd1246327987509d6a`.
- D38J32.COM: `4ed739b3aec2e092e0032c08c87ec8b02f0b3ef276ed90cd54787fd3fde9f625`.

Both define IOCTL_FAILURE; CODE32 adds CLIENT32/CODE32/HIGH_BUFFER/
HIGH_OFFSET. No device I/O, product changes or guest-media mutation were
performed. Negative passes do not prove the three success-path mappings.

The sibling flags sweep found the explicit user_F bit-40 writes in original
DOS cpmio.asm: the non-DBCS raw-input pair covered by DIV-284, and a separate
DBCS-only InterApRet pair. This supports the narrow correction and does not
authorize globally returning all FLAGS or enabling the excluded DBCS profile.

### DIV-284 nonblocking input ZF recovery

The KEYBOARD_STATUS probe exposed an original host return-contract defect:
after proving the queue empty, AH=06/DL=FF entered with ZF=0 and failed to
return ZF=1. Original guest cpmio.asm $RAW_CON_IO explicitly sets user_F bit
40h on empty input and clears it when consuming a character. A test-only
original-int21map wrapper records AX=0600 with FLAGS=3246 before
DpmiSimulateIretCF and FLAGS=3202 afterward. That helper merges only CF from
the returned flags, restoring the caller's original ZF. This is not a guest
limitation; no guest repair or workaround is appropriate.

Failure: `O:/winnt/logs/s38-keyboard-status16-r1-direct*`, probe
`build/M0-T420/S38/int21-keyboard-status-r1/D38K16.COM`, SHA-256
`fefa9a8b49252357c52a12d61d9abfacc43fbfd4fbb6a27a3fc3d5e1d5cda201`.
Trace: `O:/winnt/logs/s38-keyboard-flags-trace-r1.events.txt`, diagnostic
worker SHA-256 `769044507641288c0614c564909bfc3559cfd17f03d6feaff629cb3bf10620f5`.

DIV-284 retains original NoTranslation and its common IRET helper, adding
only the source-defined ZF output for input forms AH=06 or AX=0C06 with
DL=FF. It captures the original DOS result and republishes that bit after
the CF-only helper; no input algorithm, queue, character policy, general
flags merger or new adapter is introduced. Other calls keep their original
flags behavior. The direct original return helper cannot retain this bit
unchanged, so this smallest owner-local correction is the first viable
recovery rung; external adopted code and new input semantics are unnecessary.

Formal x86 compile/archive/link and VdmTib gate pass. Candidate
`build/M0-T420/S38/int21-keyboard-status-fixed-r1/ntvdm.exe` has SHA-256
`acf9b0a9e2e2608b4d28d8c9626b77bd16a38d767eadae8e63d8385116cfd914`.
The strengthened probe tests both flag directions: empty input with incoming
ZF=0, then BIOS-queued Z with incoming ZF=1 (must consume Z and clear ZF),
then empty 0C06 with incoming ZF=0. All four 16-bit/CODE32 direct/twice-nested
routes pass, alongside earlier content/guard/queue-discard assertions.
Logs `O:/winnt/logs/s38-keyboard-status-fixed{16,32}-r1-*`; probe artifacts
under the same build directory:

- D38K16.COM: `c7e14a2ef9bdb0ab33f41f2a7194140f13d5bc6a1abe559f16efabcf1bc51958`.
- D38K32.COM: `066cf0ef1651edcbceec8c6d1b052eca7b9f3dd9e406a12415e338a9ab1f1f22`.

Build flags are KEYBOARD_INPUT/KEYBOARD_DISCARD/KEYBOARD_STATUS; CODE32
also defines CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET. All 17 text-gated
product routes subsequently pass on this candidate, including EDIT return,
under `O:/winnt/logs/s38-keyboard-zf-product-r1-*`. This run also exercises
the updated observer's default COMMAND-prompt input path. The candidate
remains isolated and unpublished. Combined DIV-283/284 normalized delta in
int21map.c versus HEAD is +11/-1 including comments, not a diff reduction.

### Protected buffered keyboard input

The boundary extension passes twelve more real routes on 5c82fb2d...:
empty input and over-capacity input (eight routes total), plus queued-key
discard (four routes). Each covers 16-bit/CODE32 and direct/twice-nested.
Empty input is only CR and must return length zero and CR at data[0].
Over-capacity input is ABCDEFGHIJK plus CR against capacity 8; it must
return length 7, ABCDEFG plus CR, unchanged capacity and intact trailing
guard. Logs `O:/winnt/logs/s38-keyboard-{empty,overflow}{16,32}-r1-*`
contain the corresponding EMPTY_OK/CAPACITY_OK markers and I/O cleanup.

KEYBOARD_DISCARD first completes ordinary buffered input, then uses the
standard INT16/05 store-key API to queue Z (CX=2C5A). INT21/0B must report
AL=FF before INT21/0C00; after that original flush-only call, AL must be zero
and another /0B must report no input. This follows original
dos/v86/doskrnl/dos/cpmio2.asm $STD_CON_INPUT_FLUSH: an unsupported follow-up
function clears the queue and returns AL=0. No BIOS/private queue write is
performed. All four logs `O:/winnt/logs/s38-keyboard-discard{16,32}-r1-*`
contain S38_INT21_QUEUED_KEY_DISCARDED_OK. This proves actual pending input
was discarded, not merely that an initially empty queue stayed empty.

NASM artifacts below build/M0-T420/S38:

- int21-keyboard-bounds-r1/D38E16.COM: `b4787f0a250abcf5a56489bf86a082f2770ab8b57e2df983aebf5794b15ce99c`.
- int21-keyboard-bounds-r1/D38E32.COM: `67370b58b3c5de64d86a1a34d167a7751972ff366217beef4d4b750553523e97`.
- int21-keyboard-bounds-r1/D38O16.COM: `55ca702952a7e64279d8037313833e84ac0a33e24c06acdbb4a58be4a86a24c5`.
- int21-keyboard-bounds-r1/D38O32.COM: `00796b5e47103daa708bed9bb85eb8dd8538244f21f888286bdf878518233f2a`.
- int21-keyboard-discard-r1/D38K16.COM: `d404acfae211b106c6cf6baf6ad57a83c68841d56185524ea21a03123324b90c`.
- int21-keyboard-discard-r1/D38K32.COM: `91d15421eeec20556908f91326ce4de66557669899fef7f646d36cfc37092d04`.

All define KEYBOARD_INPUT plus KEYBOARD_EMPTY, KEYBOARD_OVERFLOW or
KEYBOARD_DISCARD respectively; 32-bit variants retain CLIENT32/CODE32/
HIGH_BUFFER/HIGH_OFFSET. The marker-ready observer remains 90edb54e...;
V: was removed. These results supersede the empty/capacity/discard gaps
below, not untested editing keys or other /0C follow-up function results.

KEYBOARD_INPUT invokes original INT21/0A with an eight-byte capacity and
requires returned length 4, exact ABCD followed by CR, unchanged capacity
and DX, intact trailing A5 guard, preserved segments/stack and subsequent
normal I/O/exit. KEYBOARD_FLUSH selects AX=0C0A instead, exercising original
FlushBuffReadKbd's BufferedKeyboardInput branch with the same assertions.
Neither test writes BIOS/private keyboard state or changes original media.

The Console observer now optionally waits for an exact visible probe marker
instead of COMMAND's prompt before using ordinary WriteConsoleInput records.
For these tests the marker is S38_KEYBOARD_READY and input is ABCD plus CR;
the runner also requires scripted-console-input=delivered. Default COMMAND
prompt behavior remains selected when no marker is supplied. The x86 /MT
/O2 observer (kernel32/user32/dbghelp) is
`build/M0-T420/S38/keyboard-observer-r1/observer.exe`, SHA-256
`90edb54e0afa47e45b017038ceaf7db9541eafb4c7f4913f3c8dc40bbf5dfcfe`.

All eight real routes pass on candidate 5c82fb2d...: each function in both
16-bit/CODE32, direct/twice-nested execution. Logs are
`O:/winnt/logs/s38-keyboard{16,32}-r1-*` and
`O:/winnt/logs/s38-keyboard-flush{16,32}-r1-*`. NASM `-f bin` artifacts:

- int21-keyboard-r1/D38K16.COM: `74936759247cce8eb36b6507ce140668ab5aa82c189d6e17fcf3d00cd9cf3b29`.
- int21-keyboard-r1/D38K32.COM: `a2f865364aab661115ad30695bf7569a01ad10361685409e4a70fd7fff73ecb1`.
- int21-keyboard-flush-r1/D38K16.COM: `fe77e696ea98264db52223b954ad5aae444fe7a610e0ed8932407d3a4dd09e95`.
- int21-keyboard-flush-r1/D38K32.COM: `81e9d3ea0e1b23ea104fd7383d0d6e13c6cdec1d3a3a9dc6cefd4211d561c6b0`.

Paths are below build/M0-T420/S38. Both variants define KEYBOARD_INPUT;
the second defines KEYBOARD_FLUSH; CODE32 adds CLIENT32/CODE32/HIGH_BUFFER/
HIGH_OFFSET (keyboard buffer itself remains a low offset in high memory).
No product changes were necessary and temporary V: mappings were removed.
These prove translated post-flush input, **not** discarding pre-existing
queued keys, overflow editing, empty input or every other 0C subfunction.
Those boundaries remain distinct acceptance work.

### Drive-data and device-parameter pointer family

DRIVE_QUERY exercises all four selected table entries: INT21/1B, /1C and
/1F through original GetDriveData, and /32 through GetDevParamBlock.
For the current drive, the independent probe first captures each successful
real-mode AL and DS:BX linear address, then requires the protected call to
return the same AL and the same address after INT31/0006 selector-base
resolution. Stack and ES must be preserved; DS is intentionally returned
and the probe restores its own DS afterward. This queries pointers only;
it neither writes the disk/DPB nor claims every DPB field is validated.

The r2 probe adds /1C and /32 with DL=FF, requiring AL=FF for invalid drive,
preserved stack/ES and successful subsequent normal I/O/cleanup. It makes
no claim about the unspecified pointer on failure. All four 16-bit/CODE32
direct/twice-nested routes pass on candidate 5c82fb2d..., with both
S38_INT21_DRIVE_POINTERS_REAL_PM_EQUAL_OK and
S38_INT21_INVALID_DRIVE_REJECT_OK in captured guest output. Logs:
`O:/winnt/logs/s38-drive{16,32}-r2-*` (r1 retains positive-only evidence).
Artifacts under `build/M0-T420/S38/int21-drive-r2`, NASM `-f bin -DDRIVE_QUERY`:

- D38D16.COM: `e745ca47fa523e25f39e359b69a814bb1fced956410aedcaa46964893534cd85`.
- D38D32.COM: `3b8208b311c2cfb6b8b43b9220e1ec15e06b47b2e9671fd32f998f1147c61356`,
  also CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET.

The original host bodies required no edits. No original guest or production
code changed in this test extension; V: was removed after each run. This
closes these current-drive pointer translation and invalid-drive cases,
not arbitrary media/remote-drive profiles or malformed selector contracts.

### DIV-283 original pointer translator selection

The candidate now routes only AX=6300 through existing original ReturnDSSI
before the unchanged dispatch table. Other /63 subfunctions retain their
original selection. This is two executable selection lines in the original
dispatcher, registered as DIV-283; no new translation algorithm/provider,
global DBCS definition or guest change. The normalized mirror delta for
int21map.c is +5/-1 including its three-line rationale; original CRLF is
retained. Source recovery uses the already compiled original body; the only
required binding is this owner-local selection. Enabling DBCS globally would
change unrelated profile behavior and is unnecessary. External adopted code
and newly authored pointer-translation semantics are therefore rejected.

The formal Win32/x86 incremental compile/archive/link and VdmTib gate pass.
New worker SHA-256 is
`5c82fb2d1b33f2db95fa353363da1affe3a482c43fca32cec542c76d6b1eac43`,
retained at `build/M0-T420/S38/int21-dispatch-fix-r1/ntvdm.exe`; the same
directory retains pre-fix-ntvdm.exe with the earlier eb4a402c... identity.
The initial sandbox Ninja completed no link/log tail and was terminated by
its exact verified PID before rerunning the same graph with host permission;
the second invocation recompiled the owner and completed successfully.

The formerly failing unchanged D38B16.COM now passes direct/twice-nested;
D38B32.COM (`8470c77ff8f112952448a1d066cece2ed8d85467800cd3f3dc5bbf8a1a9317e7`)
also passes both routes. Logs `O:/winnt/logs/s38-pointer-fixed{16,32}-r1-*`
contain all three InDOS/list/DBCS pointer equality markers plus I/O cleanup.
The candidate is deployed only in the isolated D38TRACE package, not the
official package root. All 17 text-gated product routes now pass on this
candidate, including EDIT return, under
`O:/winnt/logs/s38-pointer-fixed-product-r1-*` (summary JSON retained).

The strengthened probe additionally calls 6301, 6302 and 63FF on the selected
non-DBCS guest, requiring the original clear-CF result and unchanged
segment/stack state rather than pretending to implement DBCS mode-setting.
All four direct/twice-nested routes pass with
S38_INT21_NON_DBCS_SUBFUNCTION_STATE_OK and the three pointer markers;
logs `O:/winnt/logs/s38-pointer-preserve{16,32}-r1-*`. Artifacts in
`build/M0-T420/S38/int21-pointer-preserve-r1`:

- D38B16.COM: `4c23331297b4d125ea79327ab609950c54764e561d02d513321a91fc92988f27`.
- D38B32.COM: `b13e407f33d0e8ebe1456a6037a0eabf81b81d2ec047c8662abf1cc2275fe940`.

These close the reproduced pointer-dispatch defect, not all S38 acceptance.
Historical failures below are retained and superseded by these results.

### Returned-pointer family: list success and DBCS mismatch

Root-cause update: the selected non-DBCS build does **not** dispatch /63 to
ReturnDSSI. Both pinned OpenNT editions select NotSupportedBetter without
DBCS; that function warns only under DBG and calls NoTranslation. The
test-only original-int21map wrapper in dpmi_pointer_trace.c records the
real return and protected entry as AX=6300, DS=00A7, SI=0CE7, with no
segment-conversion call. The same trace records /34 and /52 converting
00A7 to 01C7. Thus the real DOS result is correct; the untranslated route
restores the caller's DS while retaining returned SI. This is a selected
host dispatch gap for /6300, not a CCPU instruction defect or immutable
guest defect. getset.asm's non-DBCS branch explicitly implements /6300.

Trace r2 artifact: `build/M0-T420/S38/pointer-trace-r2/ntvdm.exe`, SHA-256
`2566ad3e4ec1daa57892f76819d6fc157ff4f48d72f7baf03993fef4105b771d`;
events `O:/winnt/logs/s38-pointer-trace-r2.events.txt`, guest failure report
`s38-pointer-trace-r2-direct*`. This diagnostic binary is not published;
the isolated package was restored to eb4a402c... after execution. Recovery
must route the implemented /6300 contract through original ReturnDSSI,
without globally enabling DBCS or changing unrelated /63 subfunctions.
That source-shaped selection and its real 16/CODE32 regressions remain to
be implemented and reviewed; the current failing test is deliberately kept.

The POINTER_QUERY extension checks INT21/52 alongside /34 by capturing each
real-mode linear address and comparing the protected returned ES:BX after
INT31/0006 base resolution. Four direct/twice-nested 16-bit/CODE32 routes
pass on eb4a402c... under `O:/winnt/logs/s38-int21-pointer-list{16,32}-r1-*`.
Artifacts in `build/M0-T420/S38/int21-pointer-list-r1`:

- D38L16.COM: `7293dcc9987905e45be872e327e6dc6aaf1f4453e496113dd9c52362ffa9b712`.
- D38L32.COM: `740774ca3ddd57327f9da0067978f802d57c2ffc5b108859f2c1f06b65d63d32`.

The subsequent INT21/6300 ReturnDSSI query is **not passing**. The 16-bit
direct probe returns normally but fails the linear-address equality check:
real-mode address 00001757, protected address 00007A77, returned offset
00000CE7 and selector 019F. Its base therefore resolves to 00006D90 rather
than the expected 00000A70. The current reproducer is
`build/M0-T420/S38/int21-pointer-dbcs-r5/D38B16.COM`, SHA-256
`1c190ce83356a267a50d4eebb032a401ed9202bdb3033333d362b72cad6d05c4`;
logs `O:/winnt/logs/s38-int21-pointer-dbcs16-r5-direct*` retain failure stage
C and those four hex values. The runner rejects this nonzero guest result.

Original getset.asm $ECS_call supplies DS:SI even in its non-DBCS branch;
int21map.c ReturnDSSI converts DS through DpmiSegmentToSelector and restores
SI width. Causality is not yet established: trace the real returned DS,
the conversion input/output and descriptor base before deciding whether the
test comparison, guest state, or host transition is wrong. Do not register
this as an original-guest limitation or fix it speculatively. The diagnostic
r2 attempt incorrectly wrote through protected CS; r3 corrected test-owned
stage writes to SS. That diagnostic fault is separate from the reproducible
address mismatch in r1/r3/r4/r5. No product or original guest was changed;
V: mappings were removed. CODE32 DBCS remains untested after this failure.

### Source disposition of intentionally untranslated services

The current formal graph's cc_dpmi rule compiles int21map.c with
dpmi_cflags and without NT_ALLOC_DOS_MEM. A source/build-script scan finds
no definition of that macro. Its AllocateMemoryBlock, FreeMemoryBlock and
ResizeMemoryBlock bodies therefore do nothing; DosxTranslated is likewise
an intentional empty body. This is not evidence of missing host policy.

Original dpmi/486/dxmain.asm PMIntrDos invokes XlatInt21Call and explicitly
continues at NotXlated when the host has not simulated IRET. Its dosentry
dispatches AH=48/49/4A to pmallc/pmfree/pmmodb, retaining AllocateXmemBlock,
FreeXmemBlock and ModifyXmemBlock respectively. The same original guest
path handles AH=25/35 through DFSetIntrVector/DFGetIntrVector and AH=00
through DosExitCall. int21map.c's empty owner entries preserve that handoff;
filling them with new host implementations would duplicate original logic.
The DOS_MEMORY independent probe now supplies runtime evidence for the
memory handoff. It requests 16 paragraphs through INT21/48, stores a DWORD
and last-byte sentinel, grows to 32 paragraphs through /4A, verifies both
sentinels and writes the new last byte, shrinks to 16 paragraphs and verifies
the surviving data. INT21/49 must succeed and return ES=0 as original pmfree
specifies. Free and resize of that null selector must then return CF/error 9.
The probe restores its previous ES; normal file I/O and task exit afterward
must still pass. Segment/stack checks surround ordinary calls; successful
free checks DS and stack separately because its ES change is intentional.

All four 16-bit/CODE32 direct/twice-nested routes pass on eb4a402c..., with
S38_INT21_MEMORY_GROW_SHRINK_FREE_REJECT_OK in the captured guest output.
Logs: `O:/winnt/logs/s38-int21-dos-memory{16,32}-r1-*`.
NASM `-f bin -DDOS_MEMORY` artifacts in
`build/M0-T420/S38/int21-dos-memory-r1`:

- D38M16.COM: `17890816ecf5e1bd5568c2a53a5e9a0ea7453e21cc849cc4f3f9327e404e8255`.
- D38M32.COM: `226dacec708e04657a36d1e471683b0b3c3c60c4bd6dc277f023a01573498715`,
  additionally CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET.

No product or original guest changed, and V: was removed after each run.
This verifies normal memory lifecycle and invalid-selector failures, not
allocation exhaustion, failed-growth preservation, implicit cleanup or the
separate vector/old-exit handoffs. Those are not inferred from these passes.

### InDOS returned-selector verification

POINTER_QUERY captures the real-mode INT21/34 ES:BX linear address before
entering DOSX. In protected mode it calls the same service, resolves the
returned ES with INT31/0006, and requires base plus BX to equal that captured
address. It verifies unchanged DS and stack, then restores ES. It neither
writes InDOS nor claims validation of the pointed-to byte's contents.
The original int21map.c ReturnESBX owns this conversion; no product change
was needed. Four direct/twice-nested routes pass on worker eb4a402c...:

- 16-bit: `O:/winnt/logs/s38-int21-pointer16-r1-*`, probe
  `build/M0-T420/S38/int21-pointer-r1/D38R16.COM`, SHA-256
  `cebb297f19c8743fb5e95573292d6a4e40ad341c9f85edd35d5a027865365aad`.
- CODE32/high-buffer/high-offset: `O:/winnt/logs/s38-int21-pointer32-r2-*`,
  probe `build/M0-T420/S38/int21-pointer-r2/D38R32.COM`, SHA-256
  `8cc83799cedf401e57e338d522e21e8dba210915e93706b0dfad4a25770f2a4b`.

All four Console captures contain S38_INT21_INDOS_SELECTOR_LINEAR_OK and
the ordinary I/O cleanup marker. The first CODE32 r1 failure is retained:
the new test saved state through high-memory DS but compared through original
CS. Correcting test-owned saved-state writes to SS fixed the mismatch;
this was not a product failure or original guest limitation. Other returned
pointers, including INT21/52, /63 and drive parameter blocks, remain separate
obligations. Existing summary JSON omitted several newer test switches;
the runner now records every family switch for subsequent runs, without
rewriting historical logs or treating metadata as a substitute for markers.
The unchanged probe binaries were rerun with the corrected report writer:
`O:/winnt/logs/s38-int21-pointer{16,32}-r3-*`. All four routes pass again;
new JSON records PointerQuery=true and the other family switches explicitly.
The isolated worker hash is unchanged and V: was removed afterward.

### Combined INT21 state-interaction regression

The same-process combined matrix now enables FILE_FAMILY, FIND_FAMILY,
DIRECTORY_NAV, PSP_FAMILY, BPB_REFUSAL, PARSE_FAMILY, FCB_FAMILY,
EXTENDED_FCB, EXTENDED_OPEN, TEMP_FILE and COUNTRY_QUERY together. The second
binary additionally enables CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET. Both
are built from dpmi_int21_state.asm with NASM `-f bin`; the runner enables
all corresponding switches, so it requires every family marker rather than
only the final exit. This catches shared DTA, current-directory, buffer and
selector-state interference across the tested families in one client.

All four direct/twice-nested routes pass on eb4a402c..., recorded under
`O:/winnt/logs/s38-int21-combined{16,32}-r1-*`. Artifacts in
`build/M0-T420/S38/int21-combined-r1`:

- D38X16.COM: `ff0232cabb577f34bea8754b8b8b5f32bdb35b044f313b2579a56f15b7f78a71`.
- D38X32.COM: `e3d2055f95d1239f661088f538b1bdb9b08bd7a4e9854b2ff568c328f785f6f9`.

This is combined runtime evidence for those named cases, not acceptance of
the remaining EXEC/TSR, input, pointer-return, IOCTL or network families.
No product or original guest changed; the temporary V: mapping was removed.

### Country query real/protected comparison

The r2 variant also queries INT21/6501 with BX=DX=FFFFh and CX=41 before
and after protected-mode entry. It requires equal returned CX, all 41 output
bytes equal, preserved DI and an intact trailing guard. All four direct/
twice-nested 16-bit/CODE32 routes pass on eb4a402c..., with the additional
S38_INT21_EXTENDED_COUNTRY_EQUAL_OK marker and logs
`O:/winnt/logs/s38-int21-country{16,32}-r2-*`. This verifies the selected
GetExtendedCountryInfo copy-back for subfunction 1, not all 65xx subfunctions.
Artifacts under `build/M0-T420/S38/int21-country-r2`:

- D38C16.COM: `daee97c35ba4d3f60239c00c5957913e3a3b675071cf96ee216f85bbabfe52bc`.
- D38C32.COM: `4feffa1e6c1ba0f185fa0e7d7bc1563a956c5c71b615bc72f29a8083b6e16d76`.

No settings or original media changed; test-package mapping removed.

COUNTRY_QUERY captures INT21/3800's original 34-byte output and BX in real
mode before entering DOSX, then queries through protected-mode GetSetCountry
and requires exact byte equality, equal country ID, preserved DX and an
intact trailing guard. This avoids assuming a particular installed locale;
it proves translation fidelity, not correctness of every country's data or
callability of the returned case-map pointer. Both client widths, including
high-memory CODE32 buffering, pass direct/twice-nested execution on eb4a402c...
with S38_INT21_COUNTRY_REAL_PM_EQUAL_OK. Logs:
`O:/winnt/logs/s38-int21-country{16,32}-r1-*`.

Artifacts under `build/M0-T420/S38/int21-country-r1`:

- D38C16.COM: `484d80ee93b5916f18304e194d4364d5be0f391d8f677defecbadd5f9d0ba648`.
- D38C32.COM: `359849a79ef6db1eb69dcd98890787ad33e93c6e835fe94ea89ba9c3d8b876e1`.

The test only queries; it does not change guest or host country settings.
Product and original media remain unchanged. Extended-country subfunctions
and query failure contracts are still separate pending coverage.

### Temporary-file translation lifecycle

TEMP_FILE exercises original CreateTempFile via INT21/5A with `tests/` input
and the exact prefix-length-plus-13 output capacity. It requires a returned
nonempty filename, unchanged DX and intact trailing guard, closes the returned
handle, reopens/closes that returned path, deletes it and requires missing-file
error 2. Segment/stack checks and the existing I/O lifecycle also pass.
Four direct/twice-nested 16-bit/CODE32 routes pass on eb4a402c..., logged at
`O:/winnt/logs/s38-int21-temp{16,32}-r1-*`; the runner requires
S38_INT21_TEMP_CREATE_REOPEN_DELETE_OK. High-memory CODE32 uses a low buffer
offset. Invalid directory/allocation failure is not inferred from success.

Artifacts in `build/M0-T420/S38/int21-temp-r1`:

- D38T16.COM: `ff984a6bab53272f1eb46842e9b307d17a2fe16ac04dc0481c2e73d9ff0fde6d`.
- D38T32.COM: `e1af70ef6daa4e473a985ca5862ff849efb5a05f5d6816f633f42c62726a6cf8`.

No product/guest-media changes; temporary V: mapping removed after the runs.

### Production diff review checkpoint

Reviewed normalized working-tree production delta against HEAD 6eb3fbb82:
nine existing MVDM files total +96/-14 lines (including the 72-line mirror
ledger), and the existing worker debugger binding adds 65 lines. This is the
uncommitted delta against that HEAD, not the whole S38 admission delta and not
a claimed reduction. No new mirror file or overlay.

The line-by-line residual review is complete. `dpmidata.h` and `dpmi386.c`
select the existing original debug BOP only for CPU40; `dpmiint.c` prevents a
failed allocation from publishing the impossible zero-base VDM TIB; and
`int21map.c` contains only the nine witnessed pointer, flags, EXEC and IOCTL
repairs DIV-283 through DIV-290. The CCPU files contain only the DIV-282
new-event latch, reset and consume points. `dbg_dispatch.c` contains the
single CPU40 boundary: descriptor/page/span preflight plus a copied six-DWORD
lease, followed by original `MOV_DR` order and masks. The old effective-address
and guest-copy adapters cannot replace that preflight because neither validates
the descriptor limit/access and page span before this service must reject an
invalid request without raising a guest fault. Thus there is no semantic-empty
production diff to delete in this selected package; the source-only ledger and
all fixtures remain outside the shipped graph.

DIV-282 sets the event latch on the original data/instruction match and TSS
T-bit owners; reset clears it. The second DR6 B-bit store inside the data
match loop is already dominated by the initial matched-event assignment, so
it does not need another latch write. TF delivery still invokes Int1_t
directly before the pending-data check; the change does not replace that
original single-step path. The textual c_cpu_continue call-site sweep within
selected SoftPC/worker sources finds its callers only in c_xcptn.c exception
handling, consistent with clearing the event on exception entry completion.
This review supports the retained latch placement; RF, fault cancellation,
T-bit and reset have the separate executed native witnesses recorded above.

DIV-281 retains original MOV_DR ordering/masks behind the copied six-DWORD
input. The new worker helper preflights descriptor/payload readability and
segment bounds before copying; it is not a new breakpoint engine. Existing
native tests cover ordinary, missing descriptor, segment access/bounds,
expand-down, overflow and failed page translation cases. Mock binding tests
are not represented as proof of every real paging layout. No additional
production edits are justified by this review checkpoint.

### Extended file open through DSSI

EXTENDED_OPEN verifies `MapASCIIZDSSI` through INT21/6C00: open the existing
test file read-only with action 1 (no create), require CX=1 and preserved SI,
close the returned handle, delete the test file, and require CF/error 2 from
the same extended-open request afterward. Segment/stack checks remain active.
All four 16-bit/CODE32 direct/twice-nested routes pass on eb4a402c... with
S38_INT21_EXTENDED_OPEN_OK, under
`O:/winnt/logs/s38-int21-open{16,32}-r1-*`. CODE32 uses a high-memory selector
but a low filename offset; no high-ESI claim is made.

NASM artifacts under `build/M0-T420/S38/int21-open-r1`:

- D38O16.COM: `30a218ec54c6f2be7f44e714ee46cc3fa46959bbc499292869831cef30658e4f`.
- D38O32.COM: `9851b80a325d75e6e666390682a4f00ea97caf0bc913ab106d71bbfa07286c46`.

This closes the tested DSSI extended-open success/failure row below, not
every creation/action mode. No product source or original guest changed;
the temporary mapping was removed and the runner checks test-file absence.

### Consolidated INT21 acceptance ledger

This table is the current function-family disposition; chronological notes
below retain the scope of their individual runs. A passing representative
does not imply every subfunction, attribute or malformed-input case passed.

| Original owner family | Current evidence | Remaining obligation |
| --- | --- | --- |
| DisplayString, NoTranslation, ReadWriteFile, MoveFilePointer | Real output plus low/high, CODE32, above-64K offset/count, EOF, missing-file and cleanup probes | Do not generalize NoTranslation to all DOS services |
| MapASCIIZDSDX, RenameFile, MapASCIIZDSSI, CreateTempFile | Create/open/delete, attributes, directory create/remove/change, two-name rename; extended-open existing/missing; temporary create/reopen/delete and guard | Temporary-file failure and untested extended-open action modes |
| GetCurDir | Directory entry, result validation and restoration in both client widths | No blanket malformed-output-span claim |
| SetDTA/GetDTA/SetDTAPointers/SetDosDTA, FindFirstFileHandle/FindNextFileHandle | Low/high DTA, exact-name result, exhaustion, guard and restoration readback | Not arbitrary wildcard multiplicity |
| ParseFilename | Ordinary and wildcard FCB contents, AL distinction, SI advancement, DI preservation and guard | Not untranslated FCB read/write support |
| FindFileFCB/MapFCB/RenameFCB | Ordinary/extended find and delete, exhaustion, rename round trip, missing-delete, high-memory CODE32, DTA/directory restoration | Repeated-buffer lifetime stress; no speculative repair of the original equal-pointer copy-back |
| SetPSP/GetPSP | Identity equality, re-publication, high-memory refusal preserving identity, normal subsequent I/O and exit | CreatePSP and protected-mode LoadExec/TSR are distinct paths, still unverified |
| TranslateBPB | Real CF refusal in both client widths/direct/nested | Original unavailable contract verified, not BPB implementation |
| BufferedKeyboardInput/FlushBuffReadKbd | Real 0A/0C0A content/count/CR/guard/state, empty/capacity cases, queued key discard by 0C00; DIV-284 proves 06 empty/key and 0C06 empty ZF in both widths/direct/nested | Other 0C follow-up results and untested editing keys are not implied |
| GetDriveData/GetDevParamBlock/ReturnESBX/ReturnDSSI | INT21/1B, /1C, /1F, /32, /34, /52 and /6300 real/protected address equality in both widths/direct/nested; invalid drives rejected; DIV-283 retains other /63 behavior | No arbitrary media/remote-profile or pointed-to-structure integrity claim |
| GetSetCountry/GetExtendedCountryInfo | INT21/3800 and /6501 real/protected exact output comparison, guards and preserved input offsets | Other subfunctions and query failure boundaries |
| IOCTLMap2Bytes/MapDSDXLenCX/IOCTLBlockDevs/IoctlReadWriteTrack | IOCTL/00 and real /02,/03,/0C invalid-handle state/guard cases pass | Controlled success-path verification and repair of offset/length hazards; no physical-disk writes or /00-as-track proof |
| Func5Dh/MapDPL, Func5Eh/GetMachineName/GetPrinterSetup/SetPrinterSetup, Func5Fh | Source dispatch and mapping identified | Protected-mode guest requests through original network boundaries, with explicit existing external-service dispositions |
| NotSupportedFCB/Bad/Better | Original code warns only in DBG then forwards | No false claim of deterministic refusal; identify original caller limitations separately from host integration |
| AllocateMemoryBlock/FreeMemoryBlock/ResizeMemoryBlock, DosxTranslated | Original guest handoff identified; real /48 allocation, /4A growth/shrink with data preservation, /49 free clearing ES, and null-selector error 9 in both client widths/direct/nested | Memory exhaustion/failed-growth and implicit cleanup; vector/old-exit handoffs are distinct |

The remaining rows stay in S38's selected-host contract review. This ledger
does not transfer unfinished work to S39 or add a new product capability.

### Residual INT21 source disposition

The remaining cells above are not an unbounded test backlog. The selected
`ApiXlatTable` has been read entry-by-entry against the existing source bodies:

| Class | Selected disposition |
| --- | --- |
| Pointer-owning translations | The mapped families (path strings, DTA/FCB, buffers, packets, return pointers, EXEC and PSP) have a named real or controlled witness. Their outstanding DOS submodes do not introduce another protected-pointer owner. |
| `NoTranslation` | These entries deliberately preserve register/segment state around the real-mode call. The selected body neither maps a protected pointer nor owns a new resource, so a separate host implementation would be semantic expansion, not recovery. |
| `NotSupported*` | The original code's DBG warning/forwarding contract is retained. It is not reclassified as a deterministic rejection and it is not a missing standalone provider. |
| `DosxTranslated` | Vector/termination handoff stays with original DOSX. The real vector and ordinary task-exit witnesses cover the selected boundary; Ctrl-C delivery and arbitrary resident callbacks are not implied. |
| IOCTL physical services | The host translation/mapping contract is complete as stated in the reconciliation table. Media/device success depends on the existing DOS/DEM provider and actual hardware; S38 neither fabricates a provider nor treats a mock as hardware acceptance. |

This completes the selected INT21 source disposition. It deliberately does
not convert every DOS subfunction into a claim of universal device, network,
or guest-program compatibility.

### Final candidate regression and observer cleanup

Formal candidate `debug-binding-r1/ntvdm.exe` is
`3a4b7d3b36f322c014d0aed583cbee81502da9b39f5a1dc0ffb25145cef21a03`.
Its four native CPU40/DPMI fixtures rebuild with no work and pass their
debug-match, checked-span/order, fault/RF/reset/T-bit and descriptor/page
markers. The same isolated `O:/winnt/tests/D38TRACE` package passes all 17
established product routes, split only to keep each Console observation below
the terminal control window: `s38-product-basics-r1`, `s38-nested-cleanup-r2`,
`s38-nested-mem-r1`, `s38-direct-command-r3`,
`s38-interactive-tail-r1` and `s38-mem-r1`. The six JSON summaries contain
exactly 17 unique cases and every actual exit equals its expected exit.

During this repeat the observer was corrected, not the product: nested COMMAND
input now waits one second after each line, and cleanup retains only observer-
recorded direct child PIDs whose executable path exactly equals the isolated
package. This avoids a reparented broker/worker escaping test cleanup without
ever selecting a user process. Final verification found no D38TRACE process
and no temporary `V:` mapping. The official `O:/winnt` package was not
replaced and guest media were not modified.

### FCB operation source review and retained test obligation

EXTENDED_FCB now selects the original 44-byte branch: FF prefix, five reserved
bytes and attribute zero precede the ordinary FCB for find and delete. The
probe requires the returned extended DTA prefix FF and exact filename at
offset 8; the guard is immediately after the 44-byte DTA. It retains next
exhaustion, ordinary-format rename round trip, extended delete/missing-delete,
DTA/directory readback and host-side no-residual-files gates.

All four direct/twice-nested 16-bit/CODE32 routes pass on eb4a402c... under
`O:/winnt/logs/s38-int21-fcb-extended{16,32}-r1-*`, including required marker
S38_INT21_EXTENDED_FCB_OK. Artifacts under
`build/M0-T420/S38/int21-fcb-extended-r1`:

- D38E16.COM: `fe5cfef2d0d715c28ca491b1d0e60b20f4df7332c4295e123746091b16a4a009`.
- D38E32.COM: `802ea683fd4286ac149dd9d530d0404bd294020f36cd5fc47bb399a882d14202`.

This supersedes the extended find/delete coverage gap below, not all possible
attribute combinations or the untranslated FCB read/write family. No product
or immutable guest changes; the temporary V: mapping was removed.

The r2 follow-up additionally reads back INT21/2F after restoring DTA and
requires the exact original selector/offset. It captures the initial current
directory with 47h and compares its bounded NUL-terminated string with the
post-restore result. All four routes pass on eb4a402c... under
`O:/winnt/logs/s38-int21-fcb{16,32}-r2-*`; the runner also requires
S38_INT21_FCB_DTA_DIRECTORY_RESTORED_OK. This closes the r1 readback gap,
not extended-FCB or long-duration stress coverage. Artifacts under
`build/M0-T420/S38/int21-fcb-r2`:

- D38F16.COM: `c676ba906ac1d4a0fe2c2aa174a44c6e66f62bae746aba62fb0e9adada033709`.
- D38F32.COM: `596a22d9bf660531597c7245c051b093f33c1666f4bcd9bdc899947f9821e94d`.

No production/guest-media changes were needed; temporary mapping removed.

The first real FCB_FAMILY matrix now passes all four direct/twice-nested
16-bit/CODE32 routes on candidate eb4a402c... . It enters the package's tests
directory after creating and closing its unique D38IO.TMP, installs a saved/
restored test DTA, checks 11h exact-name output and 12h AL=FF exhaustion,
renames via 17h to D38FCB.TMP, verifies old-name open error 2 and new-name
open/close success, renames back, deletes via 13h, and requires AL=FF on a
second deletion. The DTA guard and segment/stack assertions pass; the runner
requires S38_INT21_FCB_FIND_RENAME_DELETE_OK and no residual test files.
The high-memory CODE32 case exercises bounced FCB/DTA inputs at low offsets.

Logs: `O:/winnt/logs/s38-int21-fcb{16,32}-r1-*`. Artifacts under
`build/M0-T420/S38/int21-fcb-r1`:

- D38F16.COM: `a890830be2dd3157e72beee8fa5f5e5445e58aef6de944f9a2398cd1432628c0`.
- D38F32.COM: `199eb20485c3796c0b60a700175d2b2b76031d82e357b2f4f6d16d301cd7de95`.

This verifies the observable ordinary-FCB operations despite the suspicious
RenameFCB copy-back arguments; no mirror repair is justified by that line
alone. Extended FCBs, a long repeated-call stress and readback of restored
DTA/directory remain outside this first matrix. The probe calls the restore
operations but does not yet independently assert their readback here.
Original media and production sources remain unchanged; V: is removed.

Original INT21 FCB operations have three different contracts:
FindFileFCB (11/12) translates an ordinary 37-byte or extended 44-byte FCB
and handles DTA bounce/copy-back; MapFCB (13) maps deletion input and copies
back; RenameFCB (17) always maps a special 37-byte rename record. In contrast,
NotSupportedFCB forwards without translating selector pointers. No common
"FCB unsupported" disposition can represent these bodies.

Both pinned OpenNT editions and the current byte-exact int21map.c contain
`DpmiUnmapAndCopyBuffer(FcbAddress, FcbAddress, 0x25)` in RenameFCB. The helper
returns immediately when those addresses are equal. This is suspicious but
not yet a demonstrated semantic defect or persistent leak: the outer
DpmiXlatInt21Call executes DpmiFreeAllBuffers, resetting both allocation
indicators, and original DOS `$FCB_Rename` in `dos/v86/doskrnl/dos/fcbio.asm`
saves/restores the temporarily inserted destination drive word. Its documented
result is AL=0/FF, not a returned modified FCB. Do not change the mirror solely
to make the argument names look consistent.

Required real verification remains an isolated test-owned directory with
exact-name find/next exhaustion, rename to a unique name, missing-old/present-new
checks, FCB deletion, failure on a missing source, repeated high-buffer calls,
and restored DTA/current directory. Ordinary and high-memory CODE32 cases
must distinguish returned AL from CF and preserve the existing state gates.
This is a bounded next test obligation, not a pass, guest-bug disposition,
or justification to waive original host defects.

### Filename-to-FCB translation

The r2 follow-up adds `D38*.TMP`: AL must be 1 rather than the ordinary
filename's 0, SI must advance eight bytes, DI stays unchanged, and the FCB
name/extension must be exactly `D38?????TMP`. The output guard and subsequent
I/O/cleanup still pass. All four routes pass under
`O:/winnt/logs/s38-int21-parse{16,32}-r2-*`; the runner now requires the
additional S38_INT21_PARSE_WILDCARD_OK marker. New binaries under
`build/M0-T420/S38/int21-parse-r2` have hashes:

- D38A16.COM: `11383e444a845baa9d9d22e93997245a2f42f998df23904aa297bd657661d177`.
- D38A32.COM: `c90301f0bb9dc5bf255efa0eb30bcca6071532dcbdee812ecd4fed57bc4ac3ab`.

This supersedes the r1 wildcard-parsing coverage limitation only; extended
FCBs and actual FCB file operations are not thereby accepted.

PARSE_FAMILY exercises original `int21map.c::ParseFilename` through INT21/2900.
It supplies a 20-byte string input and a test-owned ordinary FCB, then checks
AL=0, SI advancement by nine bytes, preserved DI, default drive zero, exact
space-padded `D38IO   TMP` name/extension and an output guard. The existing
segment/stack assertions and subsequent create/read/write/delete/exit checks
also pass. This reaches the source's string and FCB mapping/copy-back contract;
the CODE32 variant places both buffers in its high-memory selector, with low
offsets. It does not claim high-offset parsing, wildcard/extended FCB support
or FCB file enumeration/read/write coverage.

All four direct/twice-nested routes pass on eb4a402c... with required marker
S38_INT21_PARSE_FCB_OK. Logs: `O:/winnt/logs/s38-int21-parse{16,32}-r1-*`.
NASM outputs under `build/M0-T420/S38/int21-parse-r1`:

- D38A16.COM: `8a2d50b84d58c26fcbf8e2aeea74dce0076629dcf59985243f336720d01c7efd`.
- D38A32.COM: `ca8f41bd8db1740a0b43564f0fecb8982b5869ca0e77f9e401ed4f38ee68bf98`.

Only independent probe, runner and evidence changed. Product and immutable
guest media are unchanged; temporary V: mapping was removed in finally.

### Explicit BPB translation refusal

Original `int21map.c::TranslateBPB` deliberately sets CF and returns through
DpmiSimulateIretCF; it does not translate a BPB on NT. The BPB_REFUSAL probe
variant clears carry before INT21/53h and requires carry afterward, preserved
segments/stack, and successful subsequent PSP, file, directory and exit
checks. This verifies the original unavailable result, not BPB support and
not a newly excluded capability. It does not generalize to the differently
implemented NotSupportedFCB/Bad/Better forwarding functions.

Four direct/twice-nested 16-bit/CODE32 routes pass on candidate eb4a402c...
with logs `O:/winnt/logs/s38-int21-bpb{16,32}-r1-*`. The runner explicitly
requires S38_INT21_BPB_REFUSAL_OK. Artifacts in
`build/M0-T420/S38/int21-bpb-r1` have SHA256:

- D38B16.COM: `9f382b47891e04c0dbe7af61e3d98fa79a1ccc947edb3353aa9054e79696510a`.
- D38B32.COM: `cce4562b8f46f6a054295aef7618c8e29d8ebd376e1a4a3942af668738747c49`.

Only test source/runner and evidence changed; original media and production
code remain unchanged. The temporary V: mapping was removed after execution.

### PM projection consumer ownership review

#### Selected retail consumer disposition

The selected CPU40/486 consumer audit is now complete at the source-defined
boundary. `486/dxboot.asm` is a publisher: after BOP 53:11 it creates the
SEL_VDMTIB descriptor from CX:DX. `486/dxstrt.asm::AllocateExceptionStack`
is the second publisher/caller and consumes only the successful return. The
only direct SEL_VDMTIB read in the selected DOSX source family is the
`dxbug.asm` DEBUG_TRACE_PROC read at offset zero; it is enclosed by `if
NTDEBUG`, while gendefs defaults NTDEBUG to zero and the selected ASM486 rule
does not define it. The selected `486/dxintr.asm` contains no SEL_VDMTIB
access. The alternative non-486 dxintr monitor reads are CPU30/kernel-monitor
source, not CPU40 product composition.

The retained DOSX binary/source-map examination is consistent with that
selection: it contains the two known 53:11 publishers, while the exact
`mov ax, SEL_VDMTIB or STD_RING` encoding for the debug reader is absent.
This is not a claim that arbitrary bytes can never be treated as an indirect
pointer; it is the required disposition of every selected direct original
consumer. Current CPU40 stack and return machinery uses its separately
published portable globals, as proved by the real IRQ/fault matrix. Therefore
the stale projected Flags observation has no selected consumer and no
source-shaped synchronization repair is warranted. The projection remains:
the selected dxboot publisher still needs it to construct the original
SEL_VDMTIB descriptor. A future NTDEBUG or CPU30 profile would require a new
admission and consumer audit, not reuse this retail conclusion.

The retained original `486/dosx.exe` and deployed DOSX were hash-compared
again: both are c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff.
The adjacent original map has SHA256
`778d93671de368b158b2140b719d1e5e0e7c053acc3af3b476eefed74399e4c7`.
It identifies PMINTHANDLERIRET at 1778h and PMINTHANDLERIRETD at 177Ch,
consistent with the runtime publication witnesses. It lists none of the
explicit public DebugTrace/DebugTraceRm/DebugTraceBuffer/RegisterTraceBuffer
symbols declared inside `dxbug.asm`'s NTDEBUG region. `gendefs.inc` defaults
NTDEBUG to zero, and the retained ASM486 options do not enable it.

This supplies converging build/symbol evidence that this specific historical
debug consumer is not part of the deployed retail composition. A read-only
whole-file scan also finds no B8 DB 00 sequence for its explicit
`mov ax, SEL_VDMTIB or STD_RING`; that scan is supporting evidence only,
not a decoder, unpacker, or proof against arbitrary indirect accesses.
No new synchronization or guest patch follows from this finding. Remaining
consumer review must distinguish binary runtime reachability from these
source-selection and symbol facts; the map alone cannot establish all field
accesses or authorize deleting the positive publication contract.

The original `dpmi/makefile` resolves a previously insufficient directory-only
search: its dxintr.obj rule selects `486/dxintr.asm`, whereas dxbug.obj selects
the parent `dxbug.asm`. The latter contains a SEL_VDMTIB read in its
WOW_x86 DEBUG_TRACE_PROC macro, enclosed by `if NTDEBUG`. Consequently,
absence of that spelling beneath `486/` alone is not proof of no consumer.
The non-486 `dxintr.asm` hardware-reflector reads/writes are not selected by
this makefile's 486 rule. These are source-composition facts, not proof of
the flags used to produce every retained immutable binary.

For the selected portable host, `data.c` owns LockedPMStackCount,
PMLockOrigEIP/SS/ESP and return entries as independent globals; `dpmidata.h`
aliases them to VdmTib.PmStackInfo only under i386. Original
`BeginUseLockedPMStack`/`EndUseLockedPMStack` operate on those globals, while
Frame32 reads CurrentAppFlags. `DpmiInitApp` updates CurrentAppFlags before
`DpmiInitRegisterSize`; its VdmTib flag assignment is i386-only. Therefore
a later client flag change is not, by itself, proof that CPU40's actual
stack/return machinery uses stale projection flags. The real mixed-width,
nested-fault and timer tests remain the separate runtime evidence.

No speculative projection synchronization is added: it would create a second
state-maintenance path without an established current consumer. Equally, the
projection is not deleted: immutable DOSX has the positively identified
SEL_VDMTIB publication caller. Complete deployed-binary consumer disposition
remains open; the NTDEBUG macro and any indirect consumers must be accounted
for before making a stronger absence or equivalence claim.

### PSP identity and high-memory refusal

PSP_FAMILY extends the independent INT21 probe with 51h/62h identity equality,
50h re-publication of that same original PSP, and another identity query.
The high-memory CODE32 variant submits its valid test-owned selector above
1 MiB to 50h, requires CF refusal and preserved BX, then proves the original
PSP identity is unchanged. It does not overwrite PSP bytes or substitute a
fabricated PSP. The existing segment/stack gates apply to every call; file,
DTA, directory and normal exit operations must still pass afterward.

Original `int21map.c::SetPSP` rejects a selector base greater than ONE_MB
before `DPMI_EXEC_INT` and before changing CurrentPSPSelector. `GetPSP`
converts the real DOS PSP segment back to its cached selector. The mirror
body remains unchanged. The test proves these observable contracts; it does
not prove CreatePSP, a new child EXEC, TSR, or arbitrary invalid-selector
handling merely because they belong to the process family.

All four direct/twice-nested routes pass on candidate eb4a402c..., recorded
under `O:/winnt/logs/s38-int21-psp{16,32}-r1-*`. Both require
S38_INT21_PSP_IDENTITY_OK; CODE32 additionally requires
S38_INT21_PSP_HIGH_REJECT_PRESERVE_OK. NASM artifacts in
`build/M0-T420/S38/int21-psp-r1`:

- D38S16.COM: `efb43994f5efd275d9dcaebeea25d513679557234100804cc1ae4aea4cf85696`.
- D38S32.COM: `e50e0eaeaa16a36ff3a16003a1add57bd25c6707bcc8d2f702387e79e0a4b22b`.

The isolated-package mapping was removed after testing. No production source,
official package or original guest media changed.

### Directory translation round trip

The independent INT21 probe's DIRECTORY_NAV variant now tests original
`MapASCIIZDSDX` (3B) and `GetCurDir` (47): save the current directory,
enter the newly created test-owned `tests/D38DIR`, require the returned
final component `D38DIR`, return two levels to the original package directory,
compare the restored directory string, check the 64-byte output guard and
remove the test directory. Every call also checks the existing segment/stack
contract. The host runner requires `S38_INT21_DIRECTORY_RESTORE_OK` and
absence of the temporary directory and files; exit zero alone cannot pass.

All four direct/twice-nested routes pass on formal candidate eb4a402c...:
`O:/winnt/logs/s38-int21-directory{16,32}-r1-*`. The 32-bit variant selects
CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET; directory output itself is at a low
offset in the high-memory selector, not an above-64K directory-output test.
NASM artifacts under `build/M0-T420/S38/int21-directory-r1`:

- D38N16.COM: `d8b880602892204e88385d9e783fe7b87dee92e5c963f5cb796614e65e031441`.
- D38N32.COM: `99b7755cba0e9c6ad6dffe6c9bdef08cdca3da36fbe0e4818a2728af47e0e77d`.

No product source or original guest media changed. The isolated package
used the temporary V: mapping, removed in finally. This closes the tested
current-directory translation/restoration case, not FCB, PSP/EXEC, network
translation or every malformed-buffer case. Original `TranslateBPB` is an
explicit CF refusal; `NotSupportedFCB/Bad/Better` instead forward through
`NoTranslation`. Those distinct contracts must not be collapsed into one
unsupported category during the remaining audit.

This checkpoint supersedes historical pending statements below only for the
explicitly verified items. S38 is still open and uncommitted.

- Formal candidate SHA256 is
  `eb4a402c817e3e7ec442c03cf66f6f168f2bdf7163562f95fb6f102ed52be9bc`.
  The retained 17-route `s38-pm-failure-fixed-regression-r1-summary.json`
  was re-read; every expected exit matches. This is a result review, not a
  new runtime run; the earlier transcript-gated evidence remains below.
- PM allocation-failure result records were re-read: direct Terminate and
  nested Ignore both report the exact memory-error dialog and worker exit.
  Diagnostic binaries are distinct from the formal product candidate.
- The current `dpmi-debug-tests` target was rebuilt, then four binaries
  were actually executed: `ccpu-debug-match-test`, `debug-register-binding-test`,
  `ccpu-halt-reset-test`, and `cpu40-descriptor-domain-fixture`. All exited 0;
  debug-match, checked-span/register order, fault cancellation, RF and T-bit
  witnesses were emitted. The descriptor fixture succeeds silently.
- Publication layout and five-client reuse evidence below are established;
  complete projection-consumer semantics are not. Remaining INT21 family
  dispositions, final source/diff review and delivery remain mandatory.

No original guest bytes or production source were changed for this checkpoint.

## Owner Disposition Of Immutable DOSX Debug Limitations

The owner explicitly approved registering the three demonstrated original
DOSX limitations in TODO and continuing remaining acceptance: status-query
inversion/staleness, slot retention after invalid registration, and absence
of automatic watchpoint release at task exit. Explicit release remains the
verified path. This decision preserves immutable guest media and the failing
reproducers; it neither converts failures into passes nor authorizes a guest
patch. It does not waive host debug-binding correctness, 0500 output-span
protection or PM-stack projection/failure/lifecycle acceptance. S38 remains
open until its remaining evidence and delivery gates are satisfied.

Subsequent owner disposition separately accepts the demonstrated INT31/0500
invalid-buffer limitation for registration without repair; its negative
probe is retained and is not a pass. The owner also grants standing authority
to register future proven original-guest limitations without repeated
approval. The source policy records that rule; unproven attribution and
host/adapter defects remain outside it. The earlier sentence withholding
0500 acceptance is superseded by this explicit decision, not by a test pass.

## Fatal Exception Source Contract And Real DOS Dialog Verification

Ignore follow-up also passes all four client-frame/route combinations under
`O:/winnt/logs/s38-ignore{16,32}-{direct,nested}-r1.*`, on worker SHA256
`56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554`.
The harness opens a SYNCHRONIZE-only handle to the original dialog thread,
sends original Ignore button ID 102, and waits for that thread's exit. It
then requires a different dialog thread belonging to the same worker and
the same exact GP instruction address, before selecting Terminate and
observing worker/launcher exit. Merely rediscovering the first window cannot
pass. For example, 32-bit nested evidence records thread 40900 followed by
3292, with X#=0D at IP=00000141. This proves original Ignore returns through
the exception frame and the persistent fault is delivered again, not that
Ignore repairs the fault. No guest code/state is patched by the observer.
This covers the DOS user-choice branch; WOW hung-task notification and
injected host allocation/dialog creation failures are not claimed tested.

Runtime follow-up: all four combinations of 16/32-bit client frames and
direct/twice-nested COMMAND pass the Terminate branch under
`O:/winnt/logs/s38-fatal{16,32}-{direct,nested}-r3.*`.
`Verify-T420S38DpmiFatal.ps1` scopes dialog enumeration to the single worker
of an initially unused isolated package. It requires X#=0D and the exact
probe fault IP (00000140 for 16-bit, 00000141 for 32-bit, confirmed by
NDISASM), sends original button ID 100 and observes worker termination and
launcher exit before its finally cleanup. Remaining isolated broker state
is cleaned by the harness, not claimed as natural broker-exit evidence.
The fixture retains the dialog text, probe/worker hashes and observer report.
At this stage Ignore was untested; the subsequent evidence above closes
that user-choice gap for the selected DOS profile.

The initial r1 test observed the expected dialog and normal termination but
failed its final Console-marker gate: the Console buffer contained no guest
text. An r2 read during the modal dialog likewise found no marker; its
temporary AttachConsole observation disturbed the PowerShell console and
prevented that shell's subst cleanup. A fresh shell verified the test
processes gone and removed/recreated only the known V: test mapping.
The intrusive observation code was removed. The final gate does not equate
an empty transcript with guest success: the exact intended exception and
instruction address are its positive guest-execution evidence instead.
The initial failures remain retained; no product change was made to conceal
them, and no generic dialog or zero exit alone can pass the final gate.

The selected graph compiles `host/src/nt_error.c` and `nt_reset.c`, not
the historical alternative `nt_term.c`. Source review establishes this DOS
path: `DpmiUnhandledExceptionHandler` dispatches exception 6 or exceptions
above 7 to `DpmiFatalExceptionHandler`; the latter formats the fault number,
CS and IP and calls `RcErrorDialogBox(EG_BAD_FAULT, ...)`. `ErrorDialogBox`
suspends the timer and launches the original ERRORPANEL dialog thread.
`obj.vdm/resource.rc`, retained by the resource generator, supplies that
dialog. `nt_uis.h` defines message control 104, Terminate button 100 and
Ignore button 102.

For DOS, Terminate returns RMB_ABORT to ErrorDialogBox, then calls
`TerminateVDM -> terminate -> host_applClose -> host_terminate`.
The selected `nt_reset.c` calls `ExitVDM(FALSE,0)` and exits with VdmExitCode;
there is no contract here requiring a special nonzero fatal exit code.
Ignore returns to the original fatal handler, which simulates RETF through
the DPMI exception frame; it does not repair the offending instruction or
skip its bytes. A persistent fault therefore cannot be claimed recovered
merely because Ignore was accepted. Dialog creation failure maps to Abort;
DOS dialog-thread creation failure also terminates the VDM.

Independent `tests/observation/dpmi_fatal_exception.asm` enters the unchanged
DOSX service, prints S38_FATAL_GP_READY, leaves the original exception-13
handler installed and attempts to load DS=FFFFh. Any fallthrough exits 99,
not success. NASM produced both client-frame variants under
`build/M0-T420/S38/fatal-exception-r1`:

- D38F16.COM SHA256
  `af42aefd959384a8105d7ccce9dc4050704bb24cf406046203f8447c71330376`.
- D38F32.COM SHA256
  `bc7af7c707c0d9627890b8edfc6add8449a337f4305ba48a7fba743fd83e4976`.

Assembly alone was not acceptance. The runtime follow-ups above capture
X#=0D and fault CS/IP, exercise Ignore and Terminate on the test worker only,
and observe worker/launcher exit. Neither a zero exit code nor an observer
timeout substitutes for that evidence. No product or original guest-media
change is made for this probe.

## Checked-span Candidate Verification (Still Open)

### Additional INT21 file-family runtime evidence

FIND_FAMILY follow-up adds original 1A/2F DTA set/get with exact selector and
offset comparison, 4E exact-name enumeration with returned name/size checks,
4F exhaustion requiring CF/error 18, a guard after the 43-byte DTA, restoration
and readback of the prior DTA, and 39/43/3A test-directory create/attribute/
remove. The original SetDTA/GetDTA/SetDTAPointers/FindFirstFileHandle/
FindNextFileHandle owners remain unchanged. The high-memory variant forces
the source's DOSX DTA bounce condition; the DTA offset itself remains 16-bit,
matching CurrentDtaOffset, and is not claimed to support high EDX.

Both variants pass direct and twice-nested execution with the same formal
worker 56ba3839...; the harness requires S38_INT21_DTA_FIND_DIRECTORY_OK,
the file-family and prior I/O markers, and absence of the temporary directory
and files. Initial r1 passed four routes; r2 additionally checks readback of
the restored DTA and again passes four routes. Final artifacts in
`build/M0-T420/S38/int21-find-r2` are D38F16.COM SHA256
`c1336588215fca50dea3d4d0f0b146308ded7054186b033ee6567e5cb16c7194`
and D38F32.COM SHA256
`5e9ded3777f7ccd44f948e81e74519f4c9afff7d272c548597075d7c3a679e30`.
Logs: `O:/winnt/logs/s38-int21-find{16,32}-r2-*`; runner flags add
`-FindFamily` to the file-family command below. These cover handle-based
enumeration, not FCB enumeration, wildcard multiplicity or every directory
translation. No product or original guest changes were needed.

The independent int21_state probe's FILE_FAMILY variant adds 4400 disk-file
device information (bit 7 clear), 45 duplicate handle, close of that duplicate,
68 commit on the still-live original, 4300 ordinary-file attributes, and 56
rename with both DS:DX and ES:DI. Opening the old name must return CF/error 2;
the probe renames back and retains its original write/read/delete checks.
Every added call also checks the existing segment/stack preservation gate.
The observer requires the new S38_INT21_FILE_FAMILY_OK marker, prior I/O
success markers and absence of both test files. No product behavior changed.

NASM outputs in `build/M0-T420/S38/int21-file-family-r1`:

- D38F16.COM SHA256
  `0e90a11ca8b7187c987b0f8abe37b8a640ab3ee235645b62ce0024bd762c5a92`.
- D38F32.COM SHA256
  `9099d74887d16aa1f22765ba994efce8318d23f824b311f688653b2fbb292ba7`;
  CLIENT32/CODE32/HIGH_BUFFER/HIGH_OFFSET select actual 32-bit code and
  protected backing with the existing above-64K I/O offsets. The additional
  filename pointers themselves remain below 64K.

`Verify-T420S38DpmiInterrupt.ps1 -Int21State -FileFamily`, adding
`-ClientBits 32 -Code32` for the second variant, passes all four direct and
twice-nested routes on formal candidate 56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554.
Logs: `O:/winnt/logs/s38-int21-file-family{16,32}-r1-*`.
Runs used the isolated V:/D38TRACE package; the temporary V: mapping was
removed afterward. No official package or immutable guest was replaced.

Original owner `dpmi32/int21map.c` remains byte-exact to pinned OpenNT,
SHA256 `b0a8632ddb1bb0ed609115bcb113c7a43a0b042930cd281d333540089589c7e1`.
IOCTL/00, duplicate, close and commit use NoTranslation; attributes use
MapASCIIZDSDX; rename uses RenameFile. Source review also confirms
NotSupportedFCB, NotSupportedBad and NotSupportedBetter only diagnose in DBG
and call NoTranslation: their names must not be reported as explicit API
refusals. DosxTranslated is an empty host entry for guest-owned operations,
not proof of successful host translation. These tests do not close the
remaining DTA/find, directory, FCB, network and process translation families.

### PM-stack publication: two distinct original callers

The new candidate eb4a402c... now passes all 17 established product routes
under `O:/winnt/logs/s38-pm-failure-fixed-regression-r1-*`, using the rebuilt
lifecycle-observer-r2 and unchanged guest media in V:/D38TRACE. Passing rows:
empty, native-zero, missing, native-seven, native-streams, native-eof, mem,
nested-empty, nested-mem, mem-repeat, direct-mem, command-c,
command-c-seven, guest-seven, command-c-mem, direct-seven and edit.
The harness checks actual Console witnesses and original per-case exit
semantics (including 1 for interactive COMMAND/EDIT and 7 for the independent
exit probe); no blanket zero-exit criterion. This supersedes the pending
regression statement below. It does not close other S38 package gaps or
authorize claiming the whole task delivered; changes remain uncommitted.

The complementary allocation-dialog Ignore branch also passes through
twice-nested COMMAND. Injection worker
`08408f87b001f8765b81e6734393438fd8197ef2d7cb6c7621c3278729642866`
(`pm-allocation-failure-r3`) logs the failed 36-byte allocation. The harness
selects button 102, observes worker 18076 and launcher exit, and verifies no
publication-return marker. Evidence is
`O:/winnt/logs/s38-pm-allocation-ignore-r1.{txt,result.json,events.txt}`.
Unlike the separate GP Ignore test, this branch must terminate rather than
re-fault: cmdmisc.c's required-allocation pattern explicitly calls
TerminateVDM after the dialog returns. Both user-choice failure branches
now have runtime evidence. The isolated worker was restored to eb4a402c...
and the full 17-route product regression is in progress; no full-regression
pass is claimed by this paragraph.

Allocation-failure runtime verification now passes. The r2 injected worker
logs exactly `S38_PM_ALLOCATION_REFUSED size=36 address=0`, displays original
resource text "The memory resources needed by NTVDM could not be allocated.",
and exits after original Terminate button 100. The harness observes worker
and launcher completion before cleanup and rejects any
S38_FAIL_PM_PUBLICATION_RETURNED marker. Evidence is
`O:/winnt/logs/s38-pm-allocation-failure-r2.{txt,result.json,events.txt}`;
worker 19240, SHA256 33e0c4a... . This verifies explicit Terminate; the dialog's
Ignore-return-to-TerminateVDM branch still needs its own check.

New formal candidate eb4a402c... then passes direct/twice-nested 32-bit
descriptor tests and CODE32 file/DTA/directory tests, recorded under
`s38-pm-fixed-descriptor-r1-*` and `s38-pm-fixed-files-r1-*`. The old D38I.COM
was still locked on two copy attempts before execution. No process or file
was removed to bypass that lock: the runner now accepts a validated 8.3
ProbeName and these runs use D38P.COM and D38Q.COM. All four runs have guest
success markers. The isolated package now contains eb4a402c...; the official
package is unchanged. Full product regressions and closure review remain.

The test-only allocation-failure build now composes the actual dpmiint.c
owner with only its DpmiAllocateVirtualMemory call redirected to a failing
provider. A wrapper records an unexpected return from publication; no guest
memory is patched and no fault switch enters production. Initial build r1
failed linking because CRT fputs pulled fwrite alongside original SoftPC
fprt.c's same-named owner. The test logger now uses CreateFile/WriteFile
directly, leaving original fprt.c unchanged. This confirms a test linking
conflict, not the cause of all earlier diagnostic runtime failures.

`Build-T420S35EnvironmentTrace.ps1 -Boundary pm-allocation-failure` produces
`build/M0-T420/S38/pm-allocation-failure-r2/ntvdm.exe`, SHA256
`33e0c4a57233a593bcce7242965a0ef461dcc98e9488f14981df0439b127d16c`.
Runtime failure-dialog/termination acceptance remains pending; a successful
injection build is not proof the failure branch has executed. Selected log
path is `O:/winnt/logs/s38-pm-allocation-failure-r2.events.txt`.

Allocation-failure repair is implemented but not runtime accepted: DIV-226
replaces setCX(0)/setDX(0) with original cmdmisc.c's memory-error dialog and
TerminateVDM. No new DOS error code or guest state machine is introduced.
The dpmiint.c semantic delta versus HEAD is +4/-2, including two comments;
its CRLF is retained. Formal debug-binding-r1 recompiles this owner and links
successfully, including the VdmTib storage gate. New worker SHA256 is
`eb4a402c817e3e7ec442c03cf66f6f168f2bdf7163562f95fb6f102ed52be9bc`.
Earlier runtime passes belong to 56ba3839..., not this changed artifact.
Neither isolated nor official packages were updated. Fault injection and
normal-path regression remain mandatory before deployment and closure.

Allocation-failure source review identifies a usable existing fatal-memory
owner path instead of a new DOS status policy: original
`dos/command/cmdmisc.c` handles a required allocation failure with
RcErrorDialogBox(EG_MALLOC_FAILURE,NULL,NULL), followed by TerminateVDM.
Other original host initialization callers use host_error with ERR_QUIT.
By contrast, original DpmiPassPmStackInfo's i386 branch publishes preexisting
VdmTib storage and cannot fail an allocation at that point. CPU40 DIV-226
introduced the allocation and its current zero-address return; that added
failure rule is not an inherited guest defect and is not waived by the
standing guest-limitation policy. The source-shaped repair candidate is to
reuse the existing required-memory fatal path, with fault-injection evidence
that no zero-address descriptor is published and the worker terminates.
This is a design finding, not an implemented or verified repair; copying
the adjacent policy without a failure test would not close the boundary.

Focused lifecycle observation now completes all five clients. The trace
builder's test-only DpmiLifecycleOnly switch records only slots 05, 0D, 11
and 13; other dispatches still execute the original body without file I/O.
`pm-lifecycle-trace-r1/ntvdm.exe` SHA256 is
`89f06e2e75090c833cf56aec2bb73a385f3f182849116bf9c1436a14ee733ba9`.
`s38-pm-lifecycle-only-r1.txt` contains five descriptor-success pairs and
S38_PM_FIVE_CLIENTS_OK. Its separate events log contains five completed
slot-05 calls and five slot-0D calls. Every observed projection uses address
00146800; later publications update selector 017F to 01C7 and retain the
same return entries. This supplies positive five-client publication/reuse
evidence, not an allocation-failure test or proof every projected field is
consumed. Full-dispatch trace failures remain unresolved; reduced logging
is a test observation choice, not a product workaround or a causal diagnosis.

At launcher completion the observer still sees live worker/broker children
(WAIT_TIMEOUT/STILL_ACTIVE), even though all five guests passed. Therefore
that immediate sample by itself cannot prove erroneous early completion.
The first restoration encountered delayed image release; a follow-up found
no live isolated process, restored candidate 56ba3839... with hash verification,
and removed the verified V: mapping. No formal package was changed.

Process-completion observation now records direct children before observer
Console teardown. The x86 test tool `lifecycle-observer-r2/observer.exe`
has SHA256 `8bd5fe38f292a860d3146538c8da949f979c70c07d85d3e86766827b025644ec`.
It uses a read-only process snapshot and QUERY_LIMITED_INFORMATION/SYNCHRONIZE
handles, without changing product completion rules. A missing process row
alone is not proof of its earlier exit cause.

`s38-pm-child-state-r1.txt` on trace worker r4 reports a real 15-second
launcher timeout, not a normal launcher exit: before cleanup, broker 44520
and worker 55500 both have wait=258 and exit=259 (STILL_ACTIVE). Console
contains one successful descriptor-client pair, not five. This proves the
latest failure is a still-running request after one client and prevents
misclassifying it as completed worker teardown. Earlier zero-exit/empty
runs remain separate observations. Initial immediate EXE restoration raced
process/image release; a subsequent read-only process check found no test
process remaining and restoration succeeded with verified candidate hash
56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554.
No official package or guest modification occurred.

Diagnostic link audit: formal worker_link and the trace builder select the
same subsystem, OPT:REF, export DEF and system libraries; the trace builder
does omit the post-link Verify-VdmTibStorage gate. Running that gate manually
against r4's map and the reused mvdm_vdm_tib.obj passes: owner is the real
object, storage is 4208 bytes at 007D76D0, with no overlapping symbol. The
known undersized/COMMON VdmTib failure is therefore not supported here.
No native-exception marker appears in the r4 trace. After the completed
slot-0D, further table publications and repeated mode transitions occur;
the final events are repeated 53:01 calls, not an observed worker-exit hook.
The external observer waits for run16, not directly for worker completion.
Consequently `result=exited` alone establishes launcher exit, not the cause
or timing of worker termination. Next attribution must capture the worker
and broker completion state before test cleanup; inferring a guest exit
from this log would be unsupported.

Diagnostic-wrapper audit found two attribution/lifetime risks: an open
FILE spanned original dispatch (including possible nested execution), and
post-call labels used global Index, which nested dispatch can change.
The test-only wrapper now captures the entry subfunction, closes the log
before invoking original code, reopens afterward and preserves LastError
around normal observation. No production source or guest state is changed.
Prior global-index counts remain historical observations, not guaranteed
outer-call counts. This cleanup is not a proven root-cause repair.

Build `pm-stack-trace-r4` reuses manifested debug-binding-r1 inputs and
produces worker SHA256
`d362858d6901078346b17a0e263d90ec39673e18041f64b3338b42101a0bc99e`.
`s38-pm-five-trace-r4.txt` still exits zero without Console success markers;
the separate `s38-pm-stack-trace-r4.events.txt` contains one completed app
init and one no-longer-in-use call. The five-client diagnostic run therefore
still fails acceptance: log lifetime/index cleanup did not resolve early
completion. The isolated formal candidate was restored after the test.

Repeatability correction: restoring the uninstrumented CRLF BAT to exact
SHA256 c2d0f196771c73faf5a32a2d2efa3590bc112ac3e18f73cb6873df3b5a66376b
passes three consecutive formal-candidate /c runs, recorded as
`s38-pm-batch-repeat-r{1,2,3}.txt`. Each contains five complete descriptor
success pairs and the final marker. Therefore the earlier ECHO correlation
does not establish causality; input/output or runtime nondeterminism remains
unresolved. The fixture no longer contains the experimental ECHO stages.

The same script on trace worker 2de06c0f... fails its completion evidence in
`s38-pm-five-trace-r3.txt`: exit zero, empty Console, only two slot-05 and
two slot-0D events. The appended trace begins at line 7137 of
`s38-pm-stack-trace-r2.events.txt`. It proves the second client's slot-11
republishes the same address 00146800 with stack selector 01C7 instead of
017F and Flags=0001; a later slot-11 reuses that address again. Thus the
projection is reused and reset at observed client stack publications, not
allocated anew per observed client. It does not prove five traced clients
or explain diagnostic-worker interference. The formal candidate was restored
afterward. Do not treat diagnostic exit zero as passing lifecycle evidence.

BAT comparison with the rebuilt observer and formal candidate reproduces
early completion: `s38-pm-five-batch-formal-r1` exits zero with empty Console;
`s38-pm-batch-direct-r1` times out; `s38-pm-batch-interactive-r1` delivers
input but times out after the command prompt launches the BAT. These are
failed runs, not evidence of five client completions or an attributed guest
defect. Both staged/source CRLF BAT hashes were
`c2d0f196771c73faf5a32a2d2efa3590bc112ac3e18f73cb6873df3b5a66376b`.

Adding first-client before/after ECHO markers makes the same /c route pass
in `s38-pm-batch-stages-r1`: five descriptor-success pairs and the final
marker are visible. This sensitivity to the test input is diagnostic, not
a repair or proof the uninstrumented BAT now works. The first experimental
after-ECHO preceded its ERRORLEVEL check; the tracked fixture now checks
ERRORLEVEL first, and that reordered fixture has not yet been rerun. Retain
the earlier failures and investigate source/order/layout rather than use
ECHO as a product workaround. No original guest or product code changed.

Same-worker client reuse now has positive runtime evidence on formal
candidate 56ba3839..., independent of the unresolved BAT attempt below.
`s38-pm-five-interactive-r1` failed in the old S34 observer while entering
the first backslash: the Console contained only `tests`, and its report
said scripted-console-input=failed. This is not a guest execution failure.
The current console_startup_observer.c already contains the Set-1 backslash
mapping. Rebuilding it with MSVC x86 /MT /O2 (kernel32/user32/dbghelp) into
`build/M0-T420/S38/lifecycle-observer-r1/observer.exe` produces SHA256
`d964016753739083a50b87b600f925b388245d587a665b1164b32589091dfe8a`.
Only inherited fopen deprecation warnings were emitted; no source change
was needed for this tool recovery.

`O:/winnt/logs/s38-pm-five-interactive-r2.txt` records input delivered,
result=exited and original interactive COMMAND exit 1. Its Console has five
separate D38I.COM commands, each followed by both
S38_DESCRIPTOR_CLIENT32_OK and S38_DESCRIPTOR_REJECTION_PRESERVATION_OK,
then S38_PM_FIVE_CLIENTS_OK and exit. Probe SHA256 is
`e99b051b25d916610a618880c5ffe93f11f3329c3721fce51ca6824f39581d84`.
This proves five actual protected-mode client lifecycles and a usable
parent in one worker; it does not prove PM-projection consumption or
allocation-failure handling. The test used the formal candidate, not the
trace worker. BAT early completion remains unresolved and is not attributed
to an original guest limitation without further evidence.

The five-client batch attempt remains failed, not lifecycle acceptance.
`s38-pm-five-clients-r1.txt` returned zero but showed batch text without the
required final marker. Its newly authored BAT used LF; r2 uses DOS CRLF but
still returns zero with an empty captured Console and no success marker.
Thus line endings alone were not the cause and no five-client success is
claimed. The next investigation must attribute the early completion rather
than infer it from exit zero. Diagnostic tracing appended to the existing
r2 event log. Test-owned processes were stopped, and the isolated worker was
restored to candidate SHA256 56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554;
official package and guest media remain unchanged.

Consumer follow-up: pinned `v86/monitor/i386/int.c` reads PmStackInfo.Flags
to choose Frame32. That kernel-monitor profile is not selected by CPU40.
The selected host uses CurrentAppFlags and the portable locked-stack globals.
Read-only NDISASM of deployed DOSX finds the D8 selector publication at file
2A82 but no MOV AX,00DB sequence used by the alternative dxintr/dxbug
consumers. This is evidence against those exact consumer bodies, not a proof
that no indirect guest access is possible. No Flags synchronization patch
is admitted merely from the stale projection observation.

Lifecycle follow-up uses the same read-only wrapper at slots 05, 0D and 13,
as well as 11. Diagnostic `build/M0-T420/S38/pm-stack-trace-r2/ntvdm.exe`
has SHA256
`2de06c0f71fb4e2818e3dd1c25800ea52376ab964500f01053d1ed263b2cdd0f`.
Both 32-bit descriptor-contract routes pass under
`O:/winnt/logs/s38-pm-stack-lifecycle-r2-*`; field observations are in
`O:/winnt/logs/s38-pm-stack-trace-r2.events.txt`. The isolated package was
restored to the formal candidate after observation; official media and
executables were not changed.

In both routes, slot 05 changes CurrentAppFlags to 0001 while the projected
Flags remains 0000. Slots 13 and 0D leave the cached address 00146800 and
its fields present. This matches the source: DpmiInitApp updates the native
VdmTib field only under i386, whereas the CPU40 projection is initialized
only by slot 11. It establishes a stale copy, not yet a malfunctioning
consumer. The 486/dxintr.asm source contains no SEL_VDMTIB accesses, while
the alternative dpmi/dxintr.asm and dpmi/dxbug.asm contain such consumers.
Their presence elsewhere in the source tree does not prove selection by
the immutable deployed binary. No synchronization patch or deletion is
justified until that consumer/profile boundary is established. Successful
descriptor tests do not close this field-lifecycle or allocation-failure
gap, and the retained allocation is not a demonstrated per-task leak.

Field-trace execution is now complete after the owner expressly permitted
ending official worker PID 5088 and broker PID 58960. Their executable paths
were rechecked before termination. The diagnostic worker was copied only to
the isolated D38TRACE package; the formal candidate was restored afterward.
Both descriptor-contract routes pass under `s38-pm-stack-observed-r1-*`.
`s38-pm-stack-trace-r1.events.txt` records the same sequence in both workers:

- First publication, PE=0, ES=00B7: address 00146800, size 36, lock/flags
  zero, stack selector 00B7, all saved-state/return/reflector fields zero.
- Second publication, PE=1, ES=017F: same address and size, stack selector
  replaced by 017F; int16/int32 become 00C71778/00C7177C and fault16/fault32
  become 00C71770/00C71774. Remaining logged fields are zero.

This establishes actual lazy allocation and replacement of fields at the
two original publication stages; it does not establish that guest execution
reads every field, that allocation failure is handled, or that repeated
applications in one worker release/reinitialize all state. Initial zero
return entries precede their later publication, and cannot alone be called
a broken return path. The earlier pending-run note below is historical and
superseded by this run. Official O:/winnt binaries were not replaced.

Compiled-layout check now passes in the existing
`cpu40_descriptor_domain_fixture.c`: the actual VDM_DPMIINFO header type is
36 bytes; LockCount/Flags/SsSelector/SaveSsSelector are at 0/2/4/6,
SaveEsp/SaveEip at 8/12, four return entries at 16/20/24/28 and
DosxRmReflector at 32. Thus the original DOSX 20h-byte PM-stack prefix does
not overlap the additional reflector member. This is a compiled ABI check,
not evidence of runtime field consumption. The existing GDT/LDT and IDT
publication assertions remain and also pass.

Formal x86 target `cpu40-descriptor-domain-fixture.exe` rebuilt in
debug-binding-r1 and exited 0; SHA256
`90b2dd02dbc0b076d94e0e134af24f527d39b7bd916b54995d8eccebb1437683`.
Run capture: `O:/winnt/logs/s38-pm-layout-r1.txt` (silent successful fixture).
Compiler emitted inherited anonymous-union and constant-condition warnings;
no warning-free claim. This native fixture neither connects to BaseSrv nor
changes a product process, so it safely ran while official sessions remained
active. The diagnostic guest field-trace run is still pending.

Existing runtime evidence, `O:/winnt/logs/s38-watchpoint-bound-dispatch-r1.events.txt`,
already proves both slot-11 calls: line 38 returns in real mode at
D1FA:255A with ES=00B7 and CX:DX=0014:6800; line 634 returns in protected
mode at 00C7:0486 with ES=017F and the same 0014:6800 result. This establishes
execution and address reuse for that historical diagnostic run, not all
field contents or current-artifact acceptance.

The existing test-only dpmi_dispatch_trace wrapper now additionally logs the
VDM_DPMIINFO fields immediately after slot 11 without writing guest state.
`Build-T420S35EnvironmentTrace.ps1 -Boundary dpmi` reused the formal
debug-binding-r1 inputs (manifest recorded by the builder) and built
`build/M0-T420/S38/pm-stack-trace-r1/ntvdm.exe`, SHA256
`0a497da71aee7a1d45840a3da72e9efb2afa0153f2e024ad4e657466dbdd638f`.
Its selected log path is `O:/winnt/logs/s38-pm-stack-trace-r1.events.txt`.
Execution did not start: preflight found live official O:/winnt basesrv.exe
and ntvdm.exe. No process was terminated and no package EXE was replaced.
This is a compiled observation tool, not a new runtime pass; isolated
execution must wait for a non-conflicting broker environment.

Immutable binary audit confirms both callers are present, not merely two
alternative source possibilities. Official `O:/winnt/system32/DOSX.EXE`
and isolated `O:/winnt/tests/D38TRACE/system32/DOSX.EXE` both have SHA256
`c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`.
Read-only scanning finds exactly two C4 C4 53 11 byte sequences. Disassembly
of their surrounding instructions identifies:

- File offset 2A7Bh: push ES, load ES=B7h and BX from [4700h], invoke BOP,
  then AX=20h, push D8h/CX/DX/zero/AX/F2h and call NSetSegmentDscr. This
  matches the dxboot WOW_x86 SEL_VDMTIB construction, including the original
  32-byte PM-stack structure prefix.
- File offset 332Fh: the AllocateExceptionStack sequence supplies its stack,
  invokes BOP, then immediately POP ES, CLC, RET, matching dxstrt.

NDISASM does not recognize the private BOP: its displayed LES spanning bytes
C4 53 11 is not an actual guest instruction interpretation. Boundaries above
use the known four-byte BOP convention and resume decoding after it.
Binary presence alone does not establish invocation order or every field's
runtime consumption. It does rule out deleting the projection on the premise
that the selected DOSX contains only the ignoring caller. Next observation
can reuse the existing test-only dpmi_dispatch_trace wrapper to record both
53:11 entry/return states without adding production tracing or guest writes.

Review of slot 11 finds two original InitializePmStackInfo callers with
different contracts. `dpmi/486/dxstrt.asm::AllocateExceptionStack` allocates
the 4-KiB exception stack, supplies ES and BX, invokes the BOP and ignores
returned CX:DX; it reaches aes_ok and clears carry. In contrast,
`dpmi/486/dxboot.asm` under WOW_x86 supplies its hardware interrupt stack
and uses returned CX:DX to construct SEL_VDMTIB. The non-486 dxboot source
contains the same second contract. Successful ordinary fault/IRQ return
tests therefore prove stack use, not consumption of the published structure
by every historical profile.

Current DIV-226 in `dpmiint.c::DpmiPassPmStackInfo` initializes original
LockedPMStackSel/count, then lazily allocates a guest-linear VDM_DPMIINFO
projection, zeros it and publishes flags, selector and four return entries.
The allocation is direct through DpmiAllocateVirtualMemory/SAAllocate, not
registered in XmemHead. DpmiFreeAllXmem only walks XmemHead, so that function
does not free this projection. The sole cached address is worker-global and
is reused on later publications; this is not evidence of an allocation per
task, but neither is it proof of complete teardown/reinitialization.

Allocation failure currently returns CX=DX=0. The first original caller
does not inspect those values and clears carry; the second can construct a
zero-base selector. No end-to-end failure proof exists yet. There is also no
justification to remove the projection solely because the first caller
ignores it. Required next evidence is the selected immutable binary's actual
caller/profile identity, layout consumption, repeated publication and
allocation-failure disposition. This is a concrete retained-adaptation gap,
separate from the inherited 0500 buffer-validation omission; no production
change or passing verdict is made by this source review.

### Descriptor rejection and state preservation

Independent `tests/observation/dpmi_descriptor_contract.asm` exercises
original INT31/0000,0001,000B,000C. After allocating and reading an unused
descriptor, it attempts to install a system descriptor and a descriptor
whose DPL differs from the original. Both must return CF set, and a fresh
Get Descriptor must return all eight original bytes unchanged. It then
frees the selector and requires Get Descriptor to reject that released
selector, as well as out-of-table FFF8h, without changing the eight-byte
sentinel output. These are actual DOSX validation paths, not host mocks.

Both 16/32-bit clients pass direct and twice-nested COMMAND on candidate
56ba3839..., logs `O:/winnt/logs/s38-descriptor{16,32}-r1-*`, using the
dedicated `-DescriptorContract` marker gate. NASM products under
`build/M0-T420/S38/descriptor-contract-r1`:

- D38D16.COM SHA256
  `621ffc9f9603508836fd9c3c4f54ca44ab91c31b2cd347acad407229a608aabc`.
- D38D32.COM SHA256
  `e99b051b25d916610a618880c5ffe93f11f3329c3721fce51ca6824f39581d84`.

Original `dxint31.asm::i31_GetSetDescriptor` owns table range/allocation,
system-bit and DPL checks before NMoveDescriptor. These failures do not
need a replacement host policy. Together with existing valid data/CODE32
execution and descriptor-domain fixtures, this proves named validation
branches, not every malformed descriptor or pointer-span combination.
No production change was required.

### Memory information client-width and span audit

Contract/source-recovery disposition (not a waiver): the DPMI 0.9 primary
specification, section 13.1, requires a 30h-byte caller buffer and describes
CF clear/set for success/failure, but does not name an invalid-buffer error
code or mandate a particular fault for this service. Reference:
[DPMI 0.9 specification](https://www.phatcode.net/res/262/files/dpmi09.html).
Our reduced-limit diagnostic deliberately violates that input precondition;
it proves missing protection, not that valid 0500 calls are broken or that
the specification requires a particular new exception at the BOP location.

Both pinned OpenNT `base/mvdm/dpmi/486/dxint31.asm` and the non-486
`base/mvdm/dpmi/dxint31.asm` have the unconditional `i31_done` continuation.
Pinned `base/mvdm/dpmi32/dpmimemr.c` already obtains selector-base pointer,
adds GetDIRegister and writes the host structure without span validation;
the native-x86 dpmimem.c has the same construction. Neither is a ready-made
recovery source for invalid-span rejection. The current CPU40 effective-
address binding checks selector table bounds and decodes the descriptor,
but does not check segment limit/access rights or base-addition overflow;
the caller supplies offset zero and subsequently performs pointer
arithmetic, so that binding cannot infer the omitted offset/span.

The recovery ladder therefore currently has no identified original rejection
body (rung 1), and a same-shaped pointer facade cannot derive information
discarded before it is called (rung 2). Changing immutable DOSX continuation
is forbidden, not an admitted external-code exception (rung 3). A novel
fault/termination policy (rung 4) is not justified by this audit and has not
been implemented. Keep the reproduced gap open for an explicit boundary
decision; do not silently convert its diagnostic into a passing requirement.

Negative-span finding: the independent LIMIT_PROBE variant keeps its full
128-KiB backing allocation, but sets the output selector limit to 1000Fh
before querying at ES:10000h. LSL must confirm exactly 1000Fh; only sixteen
output bytes are therefore within the published segment. After the call
returns with CF clear, the test restores its own descriptor limit before
reading the complete owned buffer. All nine DWORD field checks and both
outer guards pass, demonstrating that the host wrote 36 bytes, including
20 beyond the selector limit, while preserving the reserved tail.

`O:/winnt/logs/s38-memory-info-limit-r2-direct.txt` reports exit 1 and its
console capture contains S38_FAIL_MEMORY_INFO_WROTE_PAST_ES_LIMIT. This is
a reproduced negative-contract failure, not an acceptance pass. Probe source
is `dpmi_memory_info.asm` with CLIENT32/HIGH_OFFSET/LIMIT_PROBE; binary is
`build/M0-T420/S38/memory-info-limit-r2/D38M.COM`. The preceding r1 reproduced
the same observation without the extra LSL confirmation. Only test-owned
memory is involved; no original media, private guest state or other task is
patched. Current worker remains the isolated 56ba3839... candidate.

The source-shaped omission is present in the historical pointer construction;
both `mvdm/inc/softpc.h` and `softpc.new/host/inc/sim32.h` discard the Size
argument in the Sim32GetVDMPointer macro. It is not evidence for blaming the
new debug-register binding. Recovery still requires an original exception/
failure-contract decision; the reproduced failure cannot be closed by a
CF-only host change or silently waived because normal queries pass.

High-EDI follow-up passes both direct and twice-nested COMMAND. The probe
allocates a 128-KiB data block and descriptor through original INT31
services, initializes guards at offsets FFFCh/10030h, queries at ES:10000h,
then copies the returned 56-byte guarded region to its low data segment for
the existing field/accounting assertions. Both descriptor and allocation
are explicitly released. Thus a truncated DI cannot satisfy the result
checks: the actual high buffer would retain A5 sentinel data.
`build/M0-T420/S38/memory-info-high-r1/D38M.COM` SHA256
`e7f0f118863eb76307acb2459e636991b101bfb3c8ebc1241ef3df50e6f174da`;
logs `O:/winnt/logs/s38-memory-info-high-r2-*`, using the dedicated
`-MemoryHighOffset` marker gate on the unchanged 56ba3839... worker.

The surrounding source audit also rules out a guessed CF-only fix:
`PMIntr31` dispatches via EnterIntHandler; `i31_GetFreeMem` unconditionally
jumps to `i31_done`, which clears carry in the saved caller frame. The host
DpmiDispatch only validates the BOP function index; it does not validate
this output buffer. SIM32 converts the supplied selector/base address and
its historical ExpSim32GetVDMPointer does not enforce the size argument.
A host BOP merely setting CF therefore would not provide a faithful visible
INT31/0500 failure response. Invalid-span acceptance remains open, and must
not be papered over with that ineffective change or a guest-media patch.

INT31/0500 now also passes with CLIENT32 selected at original DPMI entry,
using initialized full EDI for the output offset. Both direct and twice-
nested COMMAND routes pass field relationships, exact 36-byte host write
extent, unchanged reserved tail and surrounding guards, and 64-KiB
allocation/free accounting. Probe
`build/M0-T420/S38/memory-info32-r1/D38M.COM` SHA256
`eb0cefe2465aa5472e5d736501f185b45a0b506e6ffda99c3ff5704778cba528`;
logs `O:/winnt/logs/s38-memory-info32-r1-*`, with the dedicated
S38_MEMORY_INFO_CLIENT32_OK gate. This uses a low offset and does not prove
EDI above 64 KiB, invalid selectors or a buffer crossing the segment limit.

Source review distinguishes the unresolved contract: original
`dpmimemr.c::DpmiGetMemoryInfo` requests Sim32GetVDMPointer for selector base
with size 1, then adds GetDIRegister and writes sizeof(DPMIMEMINFO) directly.
The original native-x86 alternative in `i386/dpmimem.c` has the same pointer
construction. Original `dxint31.asm::i31_GetFreeMem` loads caller ES/DI and
issues the BOP without an explicit whole-output-span check in that body.
These facts do not prove safety of invalid output buffers or establish an
error code. Before changing behavior, complete the surrounding dispatch and
selected SIM32 lease review; do not assign a guessed failure convention or
use normal guarded-buffer success as a negative-span test.

### 32-bit client INT21 verification

Large-count follow-up passes: `HIGH_COUNT` requires CLIENT32/HIGH_OFFSET,
allocates 320 KiB through original INT31/0501 and publishes a 4FFFFh-limit
data selector. It generates 65,561 bytes at offset 10000h, requests that full
ECX count in a single INT21/40h call, seeks to zero, and requests count+17
at offset 30000h via INT21/3Fh. Full EAX count, byte-for-byte payload,
untouched A5 tail, subsequent EOF, failed-handle behavior, saved state and
explicit file/selector/memory cleanup are all checked. This is an application
single request split by original `int21map.c::ReadWriteFile`, not a test loop
making smaller DOS requests. `register.c` selects getECX/setEAX for CLIENT32;
the original translator uses ULONG aggregate counts and 4 KiB DOS chunks.

Both true CODE32 and 16-bit code with 32-bit client frames pass direct and
twice-nested COMMAND. The gate requires S38_INT21_COUNT_ABOVE_64K_OK and, for
CODE32, its distinct marker. Logs:
`s38-int21-count-code32-r2-*`, `s38-int21-count-code16-r2-*` under O:/winnt/logs.
Probe paths below `build/M0-T420/S38/int21-count32-r1` and SHA256:

- D38L.COM (CODE32):
  `228336e313a96bb5f423bdb313b39db252fa3b2390103297d3f2b98950d573bc`.
- D38S.COM (16-bit code, CLIENT32):
  `c957bc31a24a054b67979b8b12a23572d99c6bed0bd7fc94964b76288f0700fe`.
- D38B.COM (ordinary 16-bit regression):
  `a7c3d6121f5bf230a85bbdaa9daa6ad7c07c50beadd7c2dc0bfc47f38a5f5ace`,
  both routes pass under `s38-int21-baseline16-r5-*`.

All use unchanged candidate worker 56ba3839...; no product repair is needed.
This closes the count-width coverage gap, not every INT21 function family.

True CODE32 follow-up now passes direct and twice-nested COMMAND routes.
The independent I/O probe uses original INT31/000B/000C descriptor services
to copy the current code descriptor, set its default-size bit and far-jump
to a `bits 32` body. The checked INT21 macro accounts for the DWORD PUSHF
frame (eight bytes including saved EAX), checks ESP and SS/DS/ES/FS/GS,
and uses full EAX results. The same high-memory/high-offset multi-chunk I/O,
byte comparison, short-read/EOF, bad-handle, missing-file and explicit
file/memory cleanup assertions execute in that code segment. Image copying
uses explicit 32-bit address-size and initialized ESI/EDI/ECX in both modes.

Build: NASM `-DCLIENT32 -DCODE32 -DHIGH_BUFFER -DHIGH_OFFSET -f bin`,
`build/M0-T420/S38/int21-code32-r1/D38C.COM`, SHA256
`96939bfc4c8b8ed3cd7edc9ce7c39bcb35e3ae6025002a1865e40d06cfbee290`.
Logs `O:/winnt/logs/s38-int21-code32-r2-*` pass the runner's explicit
`-Int21State -ClientBits 32 -Code32` gate, including S38_INT21_CODE32_OK.
The preceding r1 run passed the shared assertions before that dedicated
marker gate was added; r2 is the stronger acceptance record. No production
change was required. The subsequent large-count evidence above closes the
separate count-above-64-KiB dimension.

High-offset follow-up: `HIGH_OFFSET` requires CLIENT32 and HIGH_BUFFER,
allocates 128 KiB, publishes limit 1FFFFh, and places write/read buffers at
DS:00010000h and DS:00014000h. Independent 32-bit address-size copy/compare
instructions initialize and validate these locations; successful comparison
is against the original low-offset payload, not a possibly misaddressed copy.
The existing chunk, short-read/EOF, invalid-handle and explicit cleanup
assertions remain active. This distinguishes real EDX high-word handling
from passing tests whose offsets happen to fit DX.

Both routes pass, with `S38_INT21_EDX_ABOVE_64K_OK` and all other I/O markers,
under `O:/winnt/logs/s38-int21-offset32-r1-*` on worker `56ba3839...`.
Probe `build/M0-T420/S38/int21-offset32-r1/D38O.COM` uses NASM defines
CLIENT32, HIGH_BUFFER and HIGH_OFFSET, SHA256
`cb4b8dd1a71994f2c7b52283ceb3acbfc4838f176d9a4a3509bfac475443eb0e`.
Original register.c selects getEDX for this client and ReadWriteFile adds
the full returned offset. No product change was required. Transfer counts
remain below 64 KiB and the code segment remains 16-bit; those are distinct
coverage dimensions, not implied by this address test.

The existing I/O probe now selects a 32-bit DPMI client via AX=1 at original
entry when built with `CLIENT32`. It supplies complete ECX/EDX count/offset
arguments and checks full EAX for successful read/write counts and EOF,
alongside its existing SS/DS/ES/FS/GS/ESP preservation, byte comparisons,
CF/DOS-error and explicit file/descriptor/memory cleanup assertions. Original
`register.c::DpmiInitRegisterSize` owns this client-width selection; no new
product branch or register provider was added.

Both low and HIGH_BUFFER variants pass direct and twice-nested COMMAND on
candidate `56ba3839...`: four real routes. Logs and identity summaries are
`O:/winnt/logs/s38-int21-client32-low-r2-*` and
`s38-int21-client32-high-r2-*`. Each requires both the common I/O marker and
`S38_INT21_CLIENT32_STATE_OK`; high transcripts also report their chunk/EOF
marker. Build inputs use NASM `-f bin -DCLIENT32=1`, plus `-DHIGH_BUFFER=1`
for high memory, from `tests/observation/dpmi_int21_state.asm`.

| Probe under build/M0-T420/S38/int21-client32-r2 | SHA256 |
| --- | --- |
| D38F.COM | bdb91fa4eb1c048461b8db90bb71e5d811c6a09c20ade1eb55bec61919e939f4 |
| D38H.COM | e96e0e0949115586c9f3d2c1d901eeb5a5959bd2392ded98528dc4a9158ee61b |

This is a 32-bit client running 16-bit code, not a true CODE32 I/O probe.
Its offsets/counts remain below 64 KiB; these runs cannot establish high-word
offset/count behavior merely because ECX/EDX are initialized explicitly.
The earlier r1 counted only AX and is superseded by these full-EAX runs.

### Real INT31 memory-information verification

`tests/observation/dpmi_memory_info.asm` invokes original INT31/0500
(`486/dxint31.asm::i31_GetFreeMem` -> BOP 53:0B ->
`dpmimemr.c::DpmiGetMemoryInfo`). It checks a nonzero largest free block,
MaxUnlocked=LargestFree/4096, FreeAddressSpace=FreePages and sufficient total
free pages. Allocating 64 KiB through 0501 reduces FreePages by exactly 16;
0502 restores both the previous page count and largest free block.

The original provider deliberately publishes compatibility values MaxLocked
=0B61h, UnlockedPages=0B68h, and AddressSpaceSize/PhysicalPages=4096. The probe
asserts those values but does not misrepresent them as actual modern-host
locked/physical-page measurements. PageFileSize is checked for the range of
the original DWORD GlobalMemoryStatus result divided by 4096; exact equality
to a simultaneous host query is not claimed.

The source-defined DPMIMEMINFO contains nine DWORDs (36 bytes), although the
guest entry documents a 48-byte buffer. Canary checks prove no overwrite
outside that buffer and preserve the twelve A5h tail bytes. This records the
actual original write extent, not a newly implemented reserved-field policy.
Invalid guest output spans and 32-bit EDI addressing remain separate boundary
coverage; this positive test must not be cited as proving them.

NASM `-f bin tests/observation/dpmi_memory_info.asm -o
build/M0-T420/S38/memory-info-r2/D38M.COM` yields SHA256
`bc9d0587cb25a8fbb18837805646b994e17c576aa4ffb0f86c4e2b7dadb5d265`.
The retained S38 runner's `-MemoryInfo` mode passes direct and twice-nested
COMMAND against candidate worker `56ba3839...`: exit zero and
`S38_MEMORY_INFO_ALLOC_FREE_FIELDS_OK`. Logs and full identities are in
`O:/winnt/logs/s38-memory-info-r2-*`. No product source or guest-media change
was needed. This supersedes the prior absence of focused slot-0B evidence.

### Current per-slot closure audit

This table supersedes the initial grouped inventory for closure planning.
Numbers are the exact `dpmi32.c::DpmiDispatchTable` indices, not coverage
counts inferred from compilation. "Behavior proved" refers only to the
named real workload; it does not claim a per-slot trace or every branch.
All providers below are selected in the candidate graph; slot 0E is the
newly recovered `i386/dpmi386.c` body, the others retain their existing owner.

| Slot | Original provider | Current evidence and remaining scope |
| --- | --- | --- |
| 00 | DpmiSetDescriptorEntry | Real high-memory data selectors and CODE32 descriptors execute; descriptor-domain fixture covers GDT/LDT publication. Real 16/32-bit clients reject system/wrong-DPL descriptor updates without mutation, and reject freed/out-of-table selectors without output writes. Arbitrary malformed pointer spans are not implied. |
| 01 | switch_to_protected_mode / selected CPU40 binding | All real protected-mode probes enter/return; selected original mode state, not kernel execution. |
| 02 | DpmiSetProtectedmodeInterrupt | Real software and timer-vector install/restore in 16/32-bit variants. |
| 03 | DpmiGetFastBopEntry | Selected original portable body returns zero BX/DX/ES. Fast kernel entry is excluded by the approved profile; no claim of fast execution. |
| 04 | DpmiInitDosx | Real first-entry and translated I/O consume shared DOSX data. Malformed internal publication is not independently injected. |
| 05 | DpmiInitApp | 16/32-bit client frames and repeated tasks exercise register-size/DTA initialization. |
| 06 | DpmiXlatInt21Call | Low/high I/O, error and cleanup pass with 16-bit and 32-bit clients, including true CODE32, EDX offsets above 64 KiB and ECX count 65,561. Not blanket acceptance of every INT21 translation family. |
| 07 | DpmiAllocateXmem | Real allocation, forced relocation and exhaustion baseline plus new high-memory I/O allocation. |
| 08 | DpmiFreeXmem | Real explicit release, reuse and high-memory I/O teardown. |
| 09 | DpmiReallocateXmem | S36 real growth/forced relocation and failed-growth data preservation. |
| 0A | DpmiSetFaultHandler | Real divide/debug handler install/restore and invalid-vector refusal. |
| 0B | DpmiGetMemoryInfo | Real INT31/0500 fields, original constants, bounded write extent and 64K allocate/free accounting pass direct/nested for 16/32-bit clients, including ES:00010000 output. Reduced-limit diagnostic reproduces an unchecked write; plain host CF cannot survive original i31_done. No invalid-span safety claim. |
| 0C | DpmiDpmiInUse | Original hook activation supports actual timer and exception returns; not an independent state-bit trace. |
| 0D | DpmiDpmiNoLongerInUse | Original final-client teardown and repeated DOS task baseline; hook-state clearing is source attributed. |
| 0E | DpmiSetDebugRegisters | Real delivery groups and invalid-span zero rollback pass. Explicit release works. Original DOSX status query, rejected-request slot retention and missing implicit task-exit release are reproduced, owner-accepted TODO limitations, not passes; their disposition is complete and does not waive host defects. |
| 0F | DpmiPassTableAddress | GDT/LDT separation and IDT re-publication fixture plus real descriptor execution. |
| 10 | DpmiFreeAppXmem | S36 real same-worker task-exit memory reuse; original owner is xmem.c and DOSX TerminateApp. |
| 11 | DpmiPassPmStackInfo | Original DOSX AllocateExceptionStack calls InitializePmStackInfo, not a WOW-only future caller. Real fault/IRQ stack behavior, field publication, five-client address reuse and allocation-failure Terminate/Ignore refusal pass. Selected CPU40/486 direct-consumer audit is complete: dxboot/dxstrt publish, NTDEBUG-only dxbug is excluded, and CPU30 monitor readers are unselected; no unsupported synchronization is added. |
| 12 | DpmiVcdPmSvcCall32 | Real discovery/version/port-range and unsupported-operation refusal; no physical serial-transfer claim. |
| 13 | DpmiFreeAllXmem | Original final-client teardown and repeated allocation baseline; not an independently traced count of releases. |
| 14 | DpmiIntHandlerIret16 | Real timer interrupt chain returns with preserved stack. |
| 15 | DpmiIntHandlerIret32 | Real 32-bit client/frame and CODE32 timer return variants. |
| 16 | DpmiFaultHandlerIret16 | Real divide, nested divide and debug-handler returns. |
| 17 | DpmiFaultHandlerIret32 | Real 32-bit frame/CODE32 nested divide returns. |
| 18 | DpmiUnhandledExceptionHandler | Real 16/32-bit divide reflection passes with DIV-280. Real DOS fatal GP dialog, Ignore/repeated fault and Terminate pass both client frames and direct/nested routes. WOW notification and injected host-resource failures are not covered. |

Concrete remaining closure work is: (1) retain the completed original
debug-service and spool TODO dispositions without treating limitations as
passes; (2) finish the bounded IOCTL and other-INT21 source dispositions,
rather than generalizing from representatives; and (3) run matching-artifact
gates followed by reviewed commit/push. These are remaining obligations, not
new S packets or a transfer of unfinished work to S39. Original DOSX
implementation defects cannot be repaired by adding a second policy inside
launcher/broker.

### Reproducible native debug test targets

The formal generator now exposes `ccpu-debug-match-test.exe` (links the
selected original `obj/ccpu/c_debug.obj`) and
`debug-register-binding-test.exe` (compiles the actual binding with explicit
test-only endpoint mocks). `dpmi-debug-tests` builds both plus the existing
real-CPU `ccpu-halt-reset-test.exe`. None enters a product link or default
product packaging. The seven graph statements plus one comment are test
composition, separately from the one-line production debug-source selection.

Regenerate with `New-T310OriginalSoftpcNinja.ps1 -Architecture x86
-BuildRoot build/M0-T420/S38/debug-binding-r1 -ParallelJobs 4`, using the
recorded Node/Ninja toolchain, then run that root's
`run-ninja-parallel.cmd dpmi-debug-tests`. Configure/build logs are
`debug-test-configure-r1.log` and `debug-test-build-r1.log` in the build root.
All three executables complete normally with exit zero and their expected
assertion markers. Logs are `O:/winnt/logs/s38-formal-<target>-r1.stdout.txt`
and `.stderr.txt`:

| Target basename | SHA256 |
| --- | --- |
| ccpu-debug-match-test | 9e7fcc6bb550f316462589309e5c9a68f4355dc9bef66c1fe6debc1d0b77805c |
| debug-register-binding-test | e8af370bb832f25862384435baba39a571bf5c0476aca7b60f49675fed79ef88 |
| ccpu-halt-reset-test | 50b610b55081c0c731efcc5f6cccc83f263e7871c878c5df0cf9ef1f6920994c |

The boundary fixture retains visible historical-header type/macro warnings
and unreachable tails after its deliberately aborting unrelated endpoints;
this is not a warning-free-build claim. The real-CPU and guest tests, not
the mocked endpoints, prove decoder, paging and exception execution. This
step changes test composition only and does not replace the official worker.

### Mirror formatting and candidate diff review

The six modified mirror sources were checked against their pinned OpenNT
counterparts: `dpmi32/dpmidata.h`, `dpmi32/i386/dpmi386.c` and
`softpc.new/base/ccpu386/{c_debug.c,c_debug.h,c_main.c,c_tsksw.c}`.
All six upstream files use CRLF throughout. The worktree contained LF or
mixed endings; these six files now consistently use upstream CRLF. This is
a mechanical formatting correction, not a semantic repair or code reduction.
The existing HEAD stores these files with LF, so byte-preserving mirror Git
attributes correctly expose the format change as full-file replacement.

Against HEAD `6eb3fbb82`, these six files total raw +7096/-7061 lines, but
`git diff --ignore-space-at-eol --numstat` reports only +41/-6:

| Mirror file | Added | Removed |
| --- | ---: | ---: |
| dpmi32/dpmidata.h | 2 | 0 |
| dpmi32/i386/dpmi386.c | 22 | 1 |
| ccpu386/c_debug.c | 4 | 0 |
| ccpu386/c_debug.h | 2 | 0 |
| ccpu386/c_main.c | 8 | 5 |
| ccpu386/c_tsksw.c | 3 | 0 |

The register README adds 15 documentation lines, separately from source.
The worker-local checked debug binding adds 65 lines; the build selection
changes one line (+1/-1). There is no new mirror file or overlay. These are
current uncommitted candidate-versus-HEAD counts, not total project/upstream
diff counts or a claimed reduction. Tests/evidence are outside production
implementation accounting.

The original debug setter remains selected as an original translation unit;
its original failure path still clears six registers and sets CF. The CPU
change retains original matching tables and exception delivery, adding only
pending-event state separate from sticky DR6. The checked input binding
retains original descriptor/page translators rather than reimplementing them.
Full selected-package acceptance and original guest limitation disposition
are still required before this candidate can be delivered.

Formal x86 rebuild in `debug-binding-r1` completed all 25 affected Ninja
edges (`newline-build-r1.log`), including VdmTib ownership verification.
New worker SHA256:
`56ba3839fac527b8b452df1fd5cc1f54f4f11aec5e6144040a9085f806f51554`.
It is deployed only to the isolated D38TRACE test package. CPU fixture
SHA256 `b0e6a2720400beda56c9acf3ec44f726389e86db7e7232a5c0da7967c51b6df5`
passes reset, fault cancellation, RF, paged-span and T-bit tests, exit zero;
logs are `O:/winnt/logs/s38-newline-cpu-r1.*.txt`.

The same worker subsequently passes all 17 transcript-gated established
routes (`s38-newline-product-r1-*`). Watchpoint scalar/RMW/string/stack/code
delivery, invalid-span rollback, and high-memory I/O each pass direct and
twice-nested routes under `s38-newline-groups-r1-*`,
`s38-newline-rollback-r1-*` and `s38-newline-high-r1-*` respectively. The
specialized summary JSON files record the same full worker hash above.
These are passing candidate regressions, not resolution of the independently
recorded failing DOSX status query and rejected-request allocation behavior.

### Protected-mode INT21 state and file lifecycle

High-memory follow-up: `HIGH_BUFFER` allocates 64 KiB through original
INT31/0501, requires its base above 0010FFFFh, binds an original descriptor,
and copies the independent probe's data there. DS/ES/GS then refer to that
high allocation while SS retains the original low stack. Saved test state
uses SS and comparisons use readable CS, so restoration is not accidentally
checked against the translated data buffer. The low test now asserts its
initial SS/DS identity explicitly and uses the same checker.

The high test transfers 8,217 bytes, requiring three original 4 KiB chunks.
It requests 17 extra read bytes and checks the short-read count, data and
untouched A5h tail; a subsequent EOF read returns zero without changing the
tail; an invalid-handle buffered read returns CF/AX=6 with the tail intact.
The test closes/deletes its file, clears segment references, and explicitly
frees its descriptor and DPMI allocation before reporting success.
This exercises the original `ReadWriteFile` complete/short/zero/error branches
and `buffer.c` high-memory copy/unmap behavior, not only the low-address
identity path. It does not prove allocator exhaustion or arbitrary nested
buffer allocation depths.

Latest high probe: `build/M0-T420/S38/int21-high-r2/D38H.COM`, SHA256
`c8bd3e68e945ea17bc88984ccab072d7b31005fcbea192a725dd9bcadd9d1e51`.
Latest low probe: `build/M0-T420/S38/int21-state-r3/D38F.COM`, SHA256
`2143a3697046cfd3495af1cb4797dac3a1011e0455505b7f87c91c52b2d874e0`.
Both direct and twice-nested routes pass for both variants on worker
`36d0ee67...`. Logs are `O:/winnt/logs/s38-int21-high-r2-*` and
`s38-int21-state-r3-*`; both high transcripts additionally contain
`S38_INT21_HIGH_BUFFER_CHUNKS_EOF_CLEANUP_OK`. No product source changed.

`tests/observation/dpmi_int21_state.asm` is an independent 16-bit DPMI client.
It uses original INT21 create-new (5B), write, seek, read, close and delete,
then requires missing-file CF/AX=2 when opening the deleted path. Binary data
including NUL and FFh is compared byte-for-byte. Every checked call must
preserve SS/DS/ES/FS/GS and full ESP; FS is null while ES/GS use valid DS.
The checker preserves returned AX and FLAGS rather than masking DOS errors.
Only `tests/D38IO.TMP` is created, and create-new refuses preexisting content.

The original guest `486/dxmain.asm` dispatches XlatInt21Call; the host table
in `dpmi32/int21map.c` selects MapASCIIZDSDX, ReadWriteFile,
MoveFilePointer and NoTranslation for these calls. Their save/restore owner
is the unchanged `savestat.c`, byte-identical to pinned OpenNT, SHA256
`76864b5f6c852820722d123395e49da43fd206f2420dd38d00d1ce911b1d7e42`.
This is source attribution plus real caller behavior, not instruction-level
trace proof of every internal branch or high-memory bounce-buffer coverage.

Build with NASM `-f bin tests/observation/dpmi_int21_state.asm -o
build/M0-T420/S38/int21-state-r2/D38F.COM`; probe SHA256 is
`4d299574c8eefe4b48cc8a9141e604bae82ebf22131eb3b8ccfe513054c9282d`.
`Verify-T420S38DpmiInterrupt.ps1 -Int21State` runs direct and twice-nested
COMMAND routes against isolated worker `36d0ee67...`. Both exit zero and
emit `S38_INT21_IO_STATE_FAILURE_CLEANUP_OK`; the harness independently
requires absence of the temporary file. Reports and identities are under
`O:/winnt/logs/s38-int21-state-r2-*`, including the summary JSON and both
Console captures. No product or original-media change was needed.

The r1 probe erroneously wrote its saved state through CS in protected mode.
It exited zero without the required marker; the transcript gate rejected
it. R2 writes through the writable data selector and only reads saved state
through CS. R1 is a rejected test defect, never a product pass. These results
cover the listed 16-bit I/O family; they do not close all INT21 translations
or the open debug-service limitations.

### Rejected guest watchpoint request consumes a slot

The independent lifetime probe now has an `INVALID_REQUEST` negative mode.
On a fresh worker it calls INT31/0B00 with BX:CX=7FF00000h and DX=0103h
(invalid three-byte write watchpoint), requires failure CF, then makes a
valid one-byte request and requires handle zero. This is a strict assertion,
not a diagnostic success or a product workaround.

NASM `-f bin -DINVALID_REQUEST=1 -DEXPLICIT_RELEASE=1` builds
`build/M0-T420/S38/debug-invalid-r1/D38N.COM`, SHA256
`ba2f45184525ff0c30adc0594dbad1409e28976bcf3832ce52337f449f2a895d`.
The direct original-guest route on isolated worker `36d0ee67...` fails with
exit 1: `S38_DEBUG_EXIT_HANDLE=1`, followed by the probe's generic
`S38_DEBUG_EXIT_ALLOCATION_FAILED` failure marker. The valid allocation did
succeed; the failed assertion is its leaked-slot result. Logs are
`O:/winnt/logs/s38-debug-invalid-r1.txt` and its `.console.txt` capture.
Only the identified test worker/broker were terminated afterward; the V:
mapping was removed. No official product binary or original media changed.

The byte-exact pinned `486/dxint31.asm` owner explains the result:
DD_I31_SW_Found_One sets the slot's DD_DR7 enable bits before validating the
client size/type. DD_I31_Error sets failure CF but does not undo that slot.
Read-only disassembly of the deployed DOSX (SHA256 `c5af29a2...`, full hash
below) confirms file offset 7B7Ch sets the bits, 7BA9h tests length 3 and
7BACh branches to error 7CD7h; the host-register BOP is reached only by the
later success call at 7BF5h. Thus this rejected request never calls the
host debug-register setter. Adding host rollback cannot repair the original
guest-local allocation failure without prohibited guest mutation or a new
interception policy. This is a separate proven original-media defect, not
the host's checked-span rollback (which passes), and remains failed coverage
pending explicit limitation disposition. It is not an S38 closure claim.

### Original TSS T-bit delivery verification

The formal x86 `ccpu-halt-reset-test.exe` now executes an independently
authored far-JMP task switch through the linked original CCPU providers.
Its private GDT contains original-format code/data and available/busy 386
TSS descriptors; its IDT contains a 32-bit exception-1 interrupt gate. No
original guest media or production exception callback is replaced.

With the incoming TSS T bit set, the handler asserts that the new task has
not executed its first instruction, counts one exception, and returns via
IRETD. The new task then executes and returns the BEEF sentinel. Acceptance
also requires retained DR6.BT, a cleared pending-event latch, and exactly one
handler entry. Repeating from CPU reset with T clear requires no exception,
no DR6.BT and successful task execution. This exercises original
`c_tsksw.c::switch_tasks`, `c_main.c` scheduling and `c_xcptn.c` delivery,
not a mocked task-switch or breakpoint provider.

Build: `build/M0-T420/S38/debug-binding-r1/run-ninja-parallel.cmd
ccpu-halt-reset-test.exe`, compiler/link log `tbit-build-r2.log` in that root.
Fixture SHA256:
`ca7d7bc83402c9376db369b240e58db26598fee093d0d4853e4f8eabd0ecb961`.
Runtime logs: `O:/winnt/logs/s38-ccpu-tbit-r2.stdout.txt` and
`s38-ccpu-tbit-r2.stderr.txt`. Exit is zero with
`S38_CCPU_TSS_TBIT_SINGLE_DELIVERY_OK`; the existing reset, pending-event
fault cancellation, RF and real paged-descriptor/payload checks also pass.
The run completed normally, without timeout or forced termination.

This closes the debug profile's tested T-bit branch, not complete task-switch
architecture acceptance or the still-failing original DOSX status-query
contract. It adds test coverage only; no product source or published binary
changed in this verification step.

### Actual watchpoint status values

The independent probe's optional `STATUS_OBSERVATION` mode reports the actual
INT31/0B02 AX before the first watched write, after one delivered exception,
and after successful INT31/0B03. On the candidate below, both direct and
twice-nested COMMAND runs report `0001`, `0001`, `0001`, with
`S38_DEBUG_WRITE_HIT_OK` between the first two queries. Logs are
`O:/winnt/logs/s38-watchpoint-status-r1.txt` and
`O:/winnt/logs/s38-watchpoint-status-r1-nested.txt` plus their Console captures.
Exit zero means the observation completed, not that status semantics passed.

The diagnostic binary is `watchpoint-status-r1/D38S.COM`, SHA256
`0b28c0981df3de6691c5f03ecd6a5bc1b2bf2d06b7b83de4be327d6bdd71783b`.
The no-define strict probe remains byte-identical, SHA256
`2ccecda10747b2770c3f6db622c999fd1afb91789319b91fc08146cbfab66ff4`.
No original guest image was modified. The strict reset failure must not be
weakened: even the pre-hit query reports a hit, so its post-hit success alone
was not proof of correct publication.

Source corroboration in `dpmi/486/dxint31.asm`: DD_DR6 starts at zero;
WOW_x86 excludes Store_DBG_Regs at debug-service entry; Get_Status executes
`test [DD_DR6],edx; jne ...; inc al`, returning one for a clear bit; reset
clears that bit and republishes registers. These facts match the observations,
The following binary attribution establishes the reversed predicate; the
separate status-publication contract must not be inferred from a setter call.

### Original guest binary attribution

Read-only disassembly now confirms the inverted query predicate in the actual
deployed image, not merely a related source edition. Official and isolated
`system32/DOSX.EXE`, mirrored `dpmi/486/dosx.exe`, and pinned
`O:/repos.external/OpenNT-4.5/nt/private/mvdm/dpmi/486/dosx.exe` all have SHA256
`c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`.
The mirrored and pinned `486/dxint31.asm` are also byte-identical, SHA256
`36add8e440b1f51a7353828d2af46ab23f503557649a670205981bc78b023aa1`.

NASM ndisasm, 16-bit mode, at file offset 7C6Dh shows the query entry. Its
relevant file offsets are 7C95h `xor eax,eax`, 7C98h
`test [7B90h],edx`, 7C9Dh `jnz 7CA1h`, 7C9Fh `inc al`, and 7CA1h
`mov [ebp+12h],ax`. The reset body at 7CCEh clears the same memory bit,
then calls the routine at 7CDDh, whose bytes at 7CE3h are `C4 C4 53 0E`.
Those bytes are the original DPMI BOP, not ndisasm's ordinary LES decoding.

Consequently the query's reversed zero/nonzero interpretation is present in
the immutable original binary and is not introduced by our CPU or adapter
diff. This establishes that specific original guest defect against its own
documented bit-0 result contract. It does not settle the separate missing
DR6-to-guest publication mechanism, authorize guest patching, or close S38.
The strict lifecycle test and the immutable-media constraint remain intact.

The current formal x86 candidate from `build/M0-T420/S38/debug-binding-r1`
has SHA256
`23ce8fbd6eeff3534521495e00fdd7f2fe74db672bac1afa0eef15abf388e4c4`.
It was copied only to `O:/winnt/tests/D38TRACE/ntvdm.exe`. No package process
was present before the copy. The official `O:/winnt/ntvdm.exe` retains SHA256
`55214ce4a67d08716432b83b54e061eef37fc238eb2bc8c9ccc9bfe0307f11ec`.

`Verify-T420S38DpmiInterrupt.ps1 -DebugGroups` with the independent
`watchpoint-groups-r5/D38G.COM` passes both direct and twice-nested routes,
recorded under `O:/winnt/logs/s38-debug-span-groups-r1-*`. These assert scalar,
RMW, string, stack and instruction-breakpoint delivery, including no new hit
from stale DR6 after removal. They do not assert the full status-query/reset
contract of the ordinary watchpoint probe.

`Verify-CommandExitStatus.ps1`, prefix `s38-debug-span-product-r1`, passes all
17 established routes on that same isolated candidate, including MEM,
nested COMMAND, native streams, guest exit status and EDIT return. Both
runners used temporary `V:/D38TRACE`; the V: mapping was removed in finally.
The existing native `debug-match-r1/match.exe` and formal
`ccpu-halt-reset-test.exe` also pass again, reporting
`S38_CCPU_DEBUG_MATCH_STATUS_SEPARATION_OK` and
`original-CCPU HALT RESET: AX=beef producer=0` respectively.

Read-boundary review found a remaining proof obligation: original
`ccpu386/c_bsic.c::read_descriptor_linear` calls `spr_read_dword` and
explicitly permits a page fault. Checking selector table limits and the
decoded target span does not prove that an unavailable descriptor backing
returns FALSE through the adapter. Negative descriptor access, original
rollback, RF/task-switch/fault cancellation and the full DPMI status/reset
contract remain open. These passing regressions are not S38 closure or
permission to publish this experimental worker.

## Admitted Watchpoint Recovery Design

### Repeated-task debug-handle lifetime observation

Version/positive-control follow-up: the probe now checks INT31/0400 returns
AX=005Ah (major 0, minor decimal 90). With `EXPLICIT_RELEASE`, it calls the
original INT31/0B01 before termination. Five sequential copies all report
handle zero and reach the final batch marker, exit zero, in
`s38-debug-lifetime-release-r3.txt`. Binary SHA256 is
`ef8010d02a6006e3b84dc23b2c82f4472c4528e38f3b9d8c86f96daa9ae4ad44`.
Thus explicit original release/reuse is proved; the no-release observation
must not be described as a failure of the explicit free service.

The reproduced [DPMI 0.9 specification](https://www.phatcode.net/res/262/files/dpmi09.html)
section 6 requires protected-mode INT21/4C but does not enumerate automatic
watchpoint cleanup. The later specification's
[client termination comparison](https://www.delorie.com/djgpp/doc/dpmi/ch4.2.html)
lists watchpoint cleanup while distinguishing the narrower 0.9 obligations.
Do not silently elevate this 0.90 product to the later contract. Original
`486/dxmain.asm::TerminateProcess` installs ChildTerminationHandler;
`486/dxstrt.asm` frees client memory and invokes TerminateApp, while
`dpmi32/int21map.c::Terminate` explicitly leaves translation to DOSX.
No newly invented broker/launcher cleanup is justified by this observation.
Implicit cleanup remains a documented limitation/contract-review item,
separate from the proved original inverted status-query defect.

The independent `dpmi_debug_lifetime.asm` enters DPMI, allocates a byte write
watchpoint at untouched linear address 7FF00000h, reports its handle and
exits via INT21/4C without an explicit 0B01 release. Its BAT runs five copies
sequentially inside one COMMAND/worker. This observes implicit task cleanup;
it does not replace a debugger application or modify original guest media.

Probe `build/M0-T420/S38/debug-lifetime-r1/D38L.COM` has SHA256
`e5f05159751766c3a26ec48fde004033fbd19da061f67693102c1393e480fdd0`.
On isolated worker `094f24cb...`, `s38-debug-lifetime-r2.txt` and its Console
capture report handles 0, 1, 2, 3, then
`S38_DEBUG_EXIT_ALLOCATION_FAILED`, followed by the batch end marker.
The fifth task does not get a free slot after the previous four exit.
This is failed implicit-cleanup coverage, not a passing lifecycle result.
Whether this is original process-global debugger policy or a missing selected
task-cleanup integration still needs original-owner contract attribution;
do not add launcher/broker resets or guest-table writes from this observation.

The first run's LF-only test BAT merely echoed the lines after `echo off`;
it did not execute the probes. The test BAT now uses DOS CRLF and the r2
transcript, not the r1 launcher exit, is the evidence. Test package processes
were cleaned and the temporary V: mapping removed after both runs.

### Checked binding unit evidence

#### Complete descriptor and payload preflight

The same private bounded preflight now covers both the eight-byte descriptor
and the 24-byte register payload. Descriptor translation uses supervisor-read;
payload translation retains the selected original
`ccpusas4.c::bios_read_byte` access-request expression used by the copied
lease, rather than imposing a different privilege interpretation. Existing
`xtrn2phy` still owns page-table lookup and failure. No shared memory manager,
mirror decoder or guest-state policy is added.

Actual CCPU/SAS tests additionally cover a valid descriptor with absent payload
PTE, and a payload straddling a present and absent page. Both return FALSE
with all output DWORDs unchanged. Installing the second PTE then reads all
24 bytes correctly. Runtime log: `s38-ccpu-debug-payload-r1.txt`; fixture
SHA256 `1a27ab3316fca70f28f95a4d7f6fbefe7d3101d8f22a0947e3191bf576e3922d`.
The actual-binding mocked unit also passes (`debug-boundary-r1/build-r5.log`).

Formal worker SHA256
`36d0ee67d9b27348d9738ea86f00faaa28bcbb452cf1573dd5918ffb2146e191`
passes direct/nested delivery groups and original error rollback under
`s38-debug-payload-groups-r1` and `s38-debug-payload-rollback-r1`. All 17
established product routes pass under `s38-debug-payload-product-r1`.
The isolated package alone was updated; the official worker hash remains
55214ce4a67d08716432b83b54e061eef37fc238eb2bc8c9ccc9bfe0307f11ec.
Temporary V: mapping was removed. This does not close T-bit delivery,
the original guest status defect or remaining whole-package dispositions.

Descriptor preflight now reuses original `c_tlb.c::xtrn2phy`, explicitly
documented there as non-faulting external translation. The adapter checks
only eight descriptor bytes and physical SAS bounds before calling unchanged
`read_descriptor_linear`. No page-table or descriptor decoder is copied.
The fixed read is worker-local on the executing CPU thread; no asynchronous
lease or guest pointer is retained. Earlier direct decoder use could raise
#PF instead of returning the binding's failure result.

The extended binding unit passes missing-map, overflow and backing-bound
negatives without entering its decoder endpoint. More importantly, the
formal CCPU/SAS fixture sets an actual GDT descriptor, CR3 and page tables:
out-of-backing descriptors, missing PDE and missing PTE return FALSE normally;
valid flat and paged reads return all six 5A5A5A5Ah values. No page translator,
decoder or memory binding is mocked in that fixture. Evidence:
`O:/winnt/logs/s38-ccpu-debug-real-preflight-r1.txt`, including
`S38_CCPU_REAL_DESCRIPTOR_PAGING_REFUSAL_OK` and the still-passing reset,
DIV cancellation and RF assertions. Formal worker link also succeeds in
`debug-preflight-build.log`. Full candidate runtime regression must be rerun;
this does not settle paged target-payload refusal or T-bit lifecycle.

### Real invalid-span rollback

The `ROLLBACK_GROUP` independent guest mode first proves one write hit,
then uses LSL to assert its DS limit is FFFFh. It calls the original
BOP 53:0E with SI=FFF8h (a 24-byte request crossing that limit), requires CF
failure, writes the watched byte again and requires the hit count to remain
one. It removes the DOSX-local handle and restores its exception handler.
No original guest code/data is patched.

`build/M0-T420/S38/debug-rollback-r2/D38R.COM` passes direct and twice-nested
routes via `Verify-T420S38DpmiInterrupt.ps1 -DebugRollback`, with prefix
`s38-debug-rollback-r2`. Both transcripts have the initial hit and
`S38_DEBUG_INVALID_SPAN_ROLLBACK_OK`; the summary records probe and worker
hashes. Worker SHA256 is
`094f24cbe9b499f2bd8e2ee1903fa5901908dff7424fba5589497e31aea95aa8`.
The earlier r1 diagnostic passed without explicitly checking the DS limit;
r2 supplies that missing precondition and is the acceptance evidence.

This verifies the selected original DpmiSetDebugRegisters error/zero-reset
tail through the actual guest BOP and CCPU binding. It does not prove malformed
descriptor backing/paging failures or the independent status-query contract.
The official O:/winnt worker remains unchanged; this artifact is isolated.

`tests/mvdm-host/debug_register_binding_test.c` compiles the actual
`dbg_dispatch.c` binding with the formal graph's x86 /MT cflags. Only its
descriptor/memory/MOV_DR endpoints are mocked; unrelated dispatcher endpoints
abort if called. `build/M0-T420/S38/debug-boundary-r1/build-r3.log` records
the successful build and the executable reports
`S38_DEBUG_BINDING_CHECKED_SPAN_REGISTER_ORDER_OK`.
It checks the exact 24-byte read, null/table rejection, present/system/code
access flags, inclusive limit, expand-down lower/upper bounds, address
overflow, copy failure and DR0/1/2/3/6/7 order. It does not test actual
descriptor decoding, page faults, original DPMI rollback or CPU delivery.

The first compile exposed missing original declarations in the new binding;
including `c_bsic.h` removes implicit declarations of the original descriptor
functions. The second link lacked unrelated dispatcher endpoints and the
formal non-inline CRT printf carrier; these are test-composition failures,
not product runtime failures. The final test links legacy_stdio_definitions
and supplies abort-only unrelated endpoints. The formal worker subsequently
rebuilds and passes its VdmTib link check (`debug-header-build.log`); this
newly linked artifact is not yet runtime-accepted or deployed.

Original `v86/monitor/i386/thread.c::ThreadSetDebugContext` reads six DWORDs
into InitialContext and calls NtSetContextThread for each monitor thread.
It does not retain the input guest pointer or write DR6 back to it. Therefore
adding private guest-memory publication here would invent an undocumented
contract; the checked input binding must not quietly acquire that policy.

Reuse the existing mirror `dpmi32/i386/dpmi386.c` translation unit with only
its original DpmiSetDebugRegisters body selected for CPU40. Its other bodies
require NT process LDT, passive monitor context and kernel fast-BOP machinery;
they remain excluded, without defining the retired CPU30 profile. Preserve
CF initialization and original all-zero rollback on ThreadSetDebugContext
failure. Replace the zero-length guest alias request with a checked 24-byte
read and handle a rejected pointer before dereference.

Original `v86/monitor/i386/thread.c::ThreadSetDebugContext` walks native
monitor threads and calls NtSetContextThread. That entire translation unit
cannot implement software-CPU state: host DR registers are not guest DR
registers. The existing worker debugger binding will preserve its six-DWORD
interface and register order through original CCPU MOV_DR, including its
reserved-bit masks and breakpoint-table rebuild. No new breakpoint policy,
emulator, mirror file or overlay is admitted. This is recovery-ladder rung 2
for the unavailable mechanism; original DPMI policy remains in its owner.
External-code intrusion and a newly authored breakpoint engine are rejected
because the original CCPU implementation already supplies the required work.

Verification must include the real traced caller, write delivery, error and
cleanup behavior, formal x86 link and established regressions. This design
is not a passing implementation or permission to modify immutable guest code.

## Admission And Baseline

### Required CPU40 debug-delivery dependency

#### Executed fault cancellation and RF control

The existing `ccpu_halt_reset_test.c` now runs two further synthetic-memory
programs through the formal original CCPU/SAS graph, without a DOSX provider,
replacement decoder or original-media mutation. A byte DIV reads a watched
zero divisor and faults. Its real-mode #DE handler advances the saved IP and
returns by IRET. The test requires exactly one #DE, normal BEEF completion,
retained DR6.B0 and no pending event; a spurious #DB has a DEAD sentinel.

The RF case uses IRETD with RF set to resume at a watched INC. That first INC
must execute once; a jump back must then enter #DB before a second INC.
A never-cleared RF reaches DEAD instead. The actual #DB handler produces
BEEF, and the test checks the counter, DR6 and consumed pending state.

Both assertions and the original asynchronous HLT/reset test pass. Formal
target `ccpu-halt-reset-test.exe` SHA256 is
`f5ed7b411ca1cba1bf6f91b89f33d7d836d5b77c80183e95ff67010afa3f6c6b`;
runtime evidence is `O:/winnt/logs/s38-ccpu-debug-cancel-rf-r1.txt`, with
`S38_CCPU_DIV_FAULT_CANCELS_PENDING_DEBUG_OK` and
`S38_CCPU_RF_SUPPRESS_ONCE_THEN_BREAK_OK`.
These are actual CPU execution tests, not the mocked binding fixture. They
do not prove task-switch T-bit delivery or paged descriptor failure handling.

The original CCPU already owns matching and exception delivery. Its existing
DR6 status-based scheduling predicate cannot distinguish an old recorded hit
from a newly matched access. The S38 recovery retains that complete owner:
introduce only one internal pending-event latch, set by the original data,
instruction and task-switch match sites, consumed before exception delivery,
and reset/cancelled by original CPU reset and exception entry. Do not clear
architectural DR6 as a shortcut or write DOSX private data. No opcode
executor, breakpoint table or DPMI policy is replaced.

This bounded dependency uses the already selected CPU40 profile. Its required
mechanical groups are scalar reads/writes, RMW/string accesses, stack accesses,
and instruction breakpoints/RF/control flow. Acceptance additionally requires
repeat hits, retained-status non-events, exception cancellation and reset;
the ordinary DPMI probe and product regressions remain mandatory. Direct
original reuse was attempted and has the same scheduling expression; the
smallest external facade cannot supply the missing per-instruction event
identity without replacing CPU ownership. A registered original-owner repair
is therefore required, not an autonomous debugger adapter.

Intel SDM Volume 3B sections 18.2.3 and 18.3.1.2 distinguish recorded DR6
status from matching/access-triggered exceptions and describe pending
breakpoint cancellation on exception delivery:
[Intel manual](https://cdrdv2-public.intel.com/774491/253669-sdm-vol-3b.pdf).
This is specification evidence, not imported implementation code.

### Watchpoint binding experiment: not accepted

The fresh x86 graph `build/M0-T420/S38/debug-binding-r1` links the original
debug-register service to CCPU MOV_DR. Worker SHA256
`672db115721f4d1ff61a70de2c29b32b35423d408875c6863089f04a2fec514b`
passes the direct/nested VCD probe, but the watchpoint probe has **not** passed.
Both `s38-watchpoint-bound-r1.txt` and `s38-watchpoint-bound-r2.txt` report
launcher exit zero with no guest success text. Exit zero is not acceptance.

The independent test wrapper `dpmi_exception_trace.c` observes the original
`nt_inthk.c::host_exint_hook` without modifying registers or guest memory.
Its first 32 callbacks in `s38-watchpoint-exception-r1.events.txt` establish:

- The watched write really triggers exception 1 at `01BF:01A6`, DR6=1.
- The original DPMI handler dispatches to the probe at `01BF:022A`.
- After the handler's first instruction, another exception 1 arrives at
  `01BF:022E`, with DR6 still 1. The same handler is entered again.
- SS remains 017F while SP descends FD0, FA0, F70 and onward by 30h per
  entry. No completed exception-return/lifecycle assertion is demonstrated.

This proves repeated nested debug delivery rather than a missing first hit.
It does not by itself prove the final worker termination cause. The selected
`c_main.c` tests DR6 status bits between instructions; `Int1_t` enters the
original exception hook, and `DpmiFaultHandler` does not consume that status.
The corresponding exception and status-query ownership must be resolved
before accepting the new binding. Clearing DR6 blindly, hot-patching DOSX's
DD_DR6, or calling this an original guest defect would be premature.

The initial observer used common compile flags. Repeating with exact
`host_cflags` exposed a test-output issue: the original host CRT redirects
fprintf, producing an empty r2 event file. The wrapper now undefines only
that output macro after including the original TU. The r3 observation uses
formal host flags and independently reproduces the same 32 nested entries
in `s38-watchpoint-exception-r3.events.txt`; observed worker SHA256 is
`b2cee065ee380895e748d22f19efbeb1e58377765f47b377f1aa94b2ece6ed08`.

The test-only observed worker is confined to `O:/winnt/tests/D38TRACE`;
the temporary V: mapping is removed after each run. `O:/winnt/ntvdm.exe`
has been restored to the previously verified formal worker SHA256
`55214ce4a67d08716432b83b54e061eef37fc238eb2bc8c9ccc9bfe0307f11ec`.
The unaccepted binding remains source/build WIP, not a released repair.
Its copied-read helper also still needs descriptor-limit/access validation:
checking the two translated linear addresses alone does not establish those
properties. S38 stays open.
The previously LF-indexed `dpmi386.c` WIP is mechanically normalized to its
pinned original CRLF layout. Its raw +407/-386 count therefore includes
format restoration; ignoring line endings gives +22/-1. Neither figure is a
delivered semantic reduction, and the experiment is not included in the
observation-only commit.

The owner authorizes sequential automatic S admission after S37 closure
`2e56df9eb`. S38 verifies the selected DPMI32 host package; S39 retains the
guest DOSX package. Guest media remain immutable. Product baseline is
`6b80471d9`; S18 source recovery and S36 real DPMI allocation/cleanup evidence
are reusable inputs, not proof of every interrupt, fault or stack path.

## Source And Dispatch Inventory

The selected fifteen original bodies are `debug.c`, `dpmi32.c`, `dpmiint.c`,
`dpmiselr.c`, `buffer.c`, `data.c`, `int21map.c`, `modesw.c`, `register.c`,
`savestat.c`, `stack.c`, `vxd.c`, `xmem.c`, `dpmimemr.c` and `dpmimscr.c`.
The associated headers are `dpmi32p.h`, `dpmidata.h` and `dpmiint.h`.
The portable CPU40 composition uses the original non-monitor memory bodies;
it must not silently substitute kernel-monitor assumptions.

Source inspection of `src/mvdm/dpmi32/dpmi32.c` identifies 25 BOP slots:

| Slots (hex) | Contract requiring disposition |
| --- | --- |
| 00, 0F | Descriptor updates and table publication: domains, base, limit, rights and invalid requests. |
| 01, 04, 05, 0C, 0D, 11 | Mode switch, DOSX/application initialization, in-use state and protected-mode stack lifecycle. |
| 02, 0A, 14-18 | Interrupt/fault registration, 16/32-bit returns and unhandled exceptions. |
| 06 | INT21 translation, real DOS results and failure propagation. |
| 07-09, 0B, 10, 13 | Allocation, reallocation, memory information, explicit free and task cleanup. |
| 03, 0E, 12 | Fast-BOP, debug-register and VCD entries: selected-profile reachability or original unavailable behavior. |

## Verification Obligations

Every selected body and slot needs a source/compiled/reached disposition.
Real guest probes must check data, registers, FLAGS, invalid requests,
interrupt/fault return and repeated-task cleanup. Boundary mocks supplement
but do not replace real guest evidence. S36 allocation/stress/lifecycle
probes are baseline coverage only. Required final checks include formal x86
build and all 17 established transcript-gated product routes.

All new build/probe products belong under `build/M0-T420/S38`; deployed test
artifacts and observations belong under `O:/winnt/tests` and `O:/winnt/logs`.
Any repair must follow the four-rung original-source recovery audit and
report mirror and non-mirror changes separately.

## Current Result

S38 remains open; no production repair or complete dispatch coverage is claimed.

## Initial Runtime And Source Checks

`Verify-T420S36DpmiGuest.ps1` ran against the deployed x86 CPU40 product with
the S34 Console observer and retained S36 independently authored probes:

| Run prefix under O:/winnt/logs | Result |
| --- | --- |
| s38-baseline-lifecycle-r1 | Direct and twice-nested COMMAND routes pass; four live-allocation children, restored task capacity and worker exit after broker loss are asserted. |
| s38-baseline-stress-r1 | Harness failure: the 39-column Console wrapped the success marker across rows 3 and 4. Guest exit was zero and the complete marker is present in captured text. |
| s38-baseline-stress-r2 | Both routes pass after joining observer-prefixed physical rows for marker matching; forced relocation, failed growth, data preservation and free are asserted. |

The harness preserves captured characters and spaces, removes only observer
row prefixes and row separators, and retains exit/failure-marker checks.
This is a test parsing correction, not a product or guest change.

Source inspection distinguishes the special dispatch slots: original
`dpmimscr.c::DpmiGetFastBopEntry` returns BX/DX/ES zero; non-i386
`dpmidata.h` maps debug-register BOP to `DpmiIllegalFunction`; original
`vxd.c` implements VCD version and host serial-port enumeration and sets CF
for unsupported operations. They must not be classified together as absent.
The non-monitor interrupt returns retain actual guest IRET execution, whereas
fault returns restore saved frames directly. Both still require focused real
guest coverage, including 16/32-bit cases, before closure.

## Real 16-bit Interrupt And Fault Return

The independent `tests/observation/dpmi_interrupt_return.asm` probe follows
original `dpmi/486/dxint31.asm` vector contracts and the 16-bit fault frame in
`dpmi32/dpmiint.c`. It uses INT 31h to install and restore software interrupt
60h and divide-error handlers; it never modifies original guest code. The
divide handler advances the saved IP past the probe's own DIV instruction.
Assertions check one handler invocation, restored SP, preserved AX and carry,
and rejection of exception-vector number 11h by original DOSX.

Build: NASM `-f bin tests/observation/dpmi_interrupt_return.asm -o
build/M0-T420/S38/interrupt-guest-r1/D38I.COM`. The retained runner is
`tools/audit/Verify-T420S38DpmiInterrupt.ps1`, using the S34 observer,
that probe and `-LogPrefix s38-interrupt16-matrix-r1`. Both direct and
twice-nested COMMAND routes exit zero and emit `S38_INT16_RETURN_OK` plus
`S38_FAULT16_RETURN_NEGATIVE_OK`; hashes and reports are in
`O:/winnt/logs/s38-interrupt16-matrix-r1-summary.json`.

This proves selected 16-bit guest-observable return behavior, not all BOP
slots or 32-bit frames. Hardware interrupt nesting, 32-bit clients, complete
descriptor coverage and the remaining package audit are still open. No
product diff or immutable guest-media change was made in this delivery.

## 32-bit Client Frame Verification

The same independent probe now has `CLIENT32`: AX=1 at DPMI entry selects
32-bit client frames, vector offsets retain EDX, and the handlers use IRETD
and a 32-bit far return with the original DWORD exception frame. Its code
segment remains 16-bit. This distinguishes frame-width coverage from a
claim of complete 32-bit instruction/code-segment coverage.

NASM builds under `build/M0-T420/S38/interrupt-guest-r2`: `D38I.COM` without
defines and `D38J.COM` with `-DCLIENT32`. The retained verifier accepts
`-ClientBits 16` or `32` and requires the corresponding guest markers.
`s38-interrupt32-matrix-r1` and `s38-interrupt16-matrix-r2` both pass direct
and twice-nested routes with zero exits and matching markers. Each summary
under `O:/winnt/logs` retains the exact probe hash.

The unchanged deployed product also passes all 17 established routes using
`Verify-CommandExitStatus.ps1`, the S34 observer and `G7.COM`, with prefix
`s38-product-baseline-r1`. This includes direct/nested/repeated MEM, COMMAND,
native streams/EOF, exit codes and EDIT return. Expected historical nonzero
codes are asserted by that matrix rather than misclassified as failures.

## Current Mirror Comparison

Byte/hash and CRLF-normalized comparison of the fifteen bodies and three
headers against `O:/repos.external/OpenNT/base/mvdm/dpmi32` found five byte-exact
files: debug, register, savestat, vxd and dpmimscr. Buffer, int21map and stack
are normalized-exact but not byte-exact: their format restoration remains a
closure obligation. The other ten files have retained non-newline changes.
In particular, xmem's BOOL failure check is the S36 verified repair, not an
unnecessary type-cleanup difference; removing it would reintroduce failure
misclassification. No mirror edit has been made during this initial review.

## True 32-bit Code And Exact Formatting Recovery

The probe's `CODE32` variant uses original INT31 descriptor get/allocate/set
services to clone its code descriptor, set the default-size bit and far-jump
to independently assembled 32-bit code. Its exception handler consumes the
original 32-bit frame. Bootstrap failures retain a separate 16-bit exit path.
All variants now verify the complete EAX value rather than AX alone.

NASM outputs in `build/M0-T420/S38/interrupt-guest-r3` are D38I (16-bit
client), D38J (`CLIENT32`, 16-bit code) and D38K (`CLIENT32` plus `CODE32`).
Prefixes `s38-frame16-r3`, `s38-frame32-r3`, `s38-code32-r3` record six passing
direct/nested routes on the preceding deployed product. This closes these
specific code/frame combinations, not hardware interrupt nesting or all APIs.

The three normalized-exact mirror files buffer.c, int21map.c and stack.c
were mechanically restored to their upstream bytes after checking normalized
equality. Git attributes already preserve mirror bytes (`-text`). Byte hashes
now match for eight of eighteen selected files. Raw diff is +3731/-3731
solely from line endings; ignoring end-of-line differences produces no diff.
Semantic diff reduction and autonomous production-code reduction are zero.

The retained formal x86 S36 graph was intentionally reused for unchanged
inputs. Its three affected objects and DPMI library rebuilt, followed by the
explicit run16.exe/basesrv.exe/ntvdm.exe targets and successful VdmTib storage
verification. New worker SHA256 is
`9afae9d7903b39b607247c646f36d9ac8867919e739201fc2e81491130a5dc4c`.
It was deployed after checking that the package was unused. On that worker,
`s38-code32-formatted-r1` passes both routes. The full product regression
prefix is `s38-format-product-r1`: all 17 routes completed and passed,
including EDIT return. Original guest media remain unchanged.

## Nested Locked-stack Exception Verification

The fault probe now raises a second divide exception inside the first
handler, while retaining the outer frame. It preserves EAX/EDX/EBX around
the inner trigger, advances each saved IP past its own DIV and requires
exactly two handler invocations before checking the outer client state.
This exercises nested locked-stack return rather than inferring it from
a single exception. No original guest or product source changes are involved.

NASM outputs under `build/M0-T420/S38/nested-fault-r1` retain the three
variants. Prefixes `s38-nested-fault16-r1`, `s38-nested-frame32-r1` and
`s38-nested-code32-r1` each pass direct and nested COMMAND on the deployed
worker recorded above; verifier summaries retain probe hashes. These are
six passing real-guest routes, not hardware-IRQ proof.

The formal x86 `cpu40-descriptor-domain-fixture.exe` target was also rebuilt
and executed successfully. Its inspected assertions cover GDT versus LDT
selection, rejection before GDT publication, final DOSX IDT descriptor,
replacement publication rather than first-address latching, and invalid IDT
limit. This is explicitly host fixture evidence; the true CODE32 guest probe
provides separate end-to-end descriptor publication and execution evidence.

Source review distinguishes `DpmiSwIntHandler` (direct client IRET frame)
from `DpmiHwIntHandler` (locked stack plus DOSX return-hook frame). Consequently
software INT success alone does not verify BOP 14/15 hardware-return hooks.
That hardware path remains a required follow-up before S38 closure.

## Real Timer IRQ Return

The probe now saves protected-mode vector 08h, installs its own small
counter-and-chain handler and spins with interrupts enabled until two actual
timer interrupts occur. It never executes software INT 08h. The handler
far-jumps to the original vector, preserving timer/PIC handling. The probe
checks the returned SP, restores vector 08h and requires the additional
`S38_HARDWARE_IRQ_RETURN_OK` marker before successful exit.

Builds under `build/M0-T420/S38/hardware-irq-r1` use the same three variants.
Prefixes `s38-hardware16-r1`, `s38-hardware-frame32-r1` and
`s38-hardware-code32-r1` each pass direct and twice-nested COMMAND: six real
routes. The verifier now requires the IRQ marker as well as interrupt and
nested-fault results. Product and guest-media bytes remain unchanged.

Source attribution: `ccpu386/c_main.c` invokes `host_hwint_hook` for accepted
hardware interrupts; the selected DPMI hardware path constructs its locked
stack and DOSX return-hook frames in `dpmi32/dpmiint.c`. These runs establish
observable hardware return behavior. They are not an independently captured
per-BOP trace, nor proof that all possible IRQ devices or nesting cases pass.
The earlier hardware-coverage follow-up is superseded for this timer case.

Remaining audit includes the unhandled-fault-to-interrupt-chain behavior,
special fast/debug/VCD service dispositions and complete selected-body/slot
coverage. A registered exception handler passing does not prove unhandled
exception behavior; `DpmiUnhandledExceptionHandler` has distinct reflection
and fatal branches that must not be silently marked passed.

## Unhandled Exception Reflection And Original Frame Correction

An independent `REFLECT` variant restores the default divide fault handler,
installs a protected-mode interrupt 0 handler and triggers DIV-by-zero. The
default DOSX fault entry must transfer through the original unhandled-fault
reflection into that interrupt handler. The handler skips only this probe's
DIV instruction and returns; EAX, carry, SP and one reflection are asserted.
Reflection and timer cases are selected separately, avoiding ambiguous
attribution of a failure in the preceding timer wait.

Both pinned `OpenNT/base/mvdm/dpmi32/dpmiint.c` and
`OpenNT-4.5/nt/private/mvdm/dpmi32/dpmiint.c` write FrameCS to DWORD-frame
offset 4 and then overwrite it with FrameFlags at offset 4. The required
EIP/CS/EFLAGS layout is offsets 0/4/8, as the same file's other IRETD frame
construction demonstrates. DIV-280 changes only that flags destination to 8.

Recovery ladder: the original translation unit already composes and remains
the implementation owner. A facade cannot fix a wrong store offset inside
that owner without changing its algorithm or replacing it; no external-code
intrusion or autonomous provider is needed. The minimal registered mirror
correction preserves original ordering, reflected-handler selection and
failure flow. Product source delta is +3/-1 including two comment lines;
adapter/overlay delta is zero. This is an upstream layout defect, not a
newly invented exception policy.

Evidence chronology: `s38-reflect16-r1` and the two r2 16-bit cases emit their
success markers. The first 32-bit combined attempt times out before an IRQ
marker, so it cannot identify reflection as the failing operation. The
isolated `s38-reflect32-r2` also times out with no captured success text;
that absence is not a per-instruction fault trace. With the single offset
correction and the same r2 binary, `s38-reflect32-fixed-r1` passes direct and
nested routes. `s38-reflect16-fixed-r1` and `s38-hardware-fixed-r1` also pass
both routes. This source/layout plus same-probe contrast supports the fix;
it does not claim that every earlier timeout has been conclusively attributed.

One 16-bit run additionally exposed test cleanup racing normal broker-loss
exit after success. The runner now tolerates a process already gone while
still requiring any looked-up process to finish within five seconds.

Formal x86 compile/link and VdmTib verification passed using the retained
S36 graph. Deployed worker hash is
`55214ce4a67d08716432b83b54e061eef37fc238eb2bc8c9ccc9bfe0307f11ec`.
The full product gate `s38-reflection-product-r1` completed with all 17 routes
passing, including EDIT return. S38 remains open, including the distinct
fatal branch and remaining service/coverage dispositions.

## VCD Guest Service And Special-slot Source Attribution

The independent `tests/observation/dpmi_vcd_service.asm` enters DPMI,
discovers VCD via INT2F/1684 and BX=0Eh, calls the returned original DOSX
entry and asserts version 030Ah, a nine-bit port array and CF refusal for
FFFFh. Original `486/dxfunc.asm::VCD_PM_Svc_Call` performs BOP 53:12 into
the existing `vxd.c` body; no test-specific BOP or provider is introduced.

NASM output `build/M0-T420/S38/vcd-guest-r1/D38V.COM` passes direct and
twice-nested routes with the S38 runner's `-VcdService` switch and prefix
`s38-vcd-r1`. Both transcripts contain `S38_VCD_VERSION_PORTS_REFUSAL_OK`.
This proves discovery, version, bounded response and refusal, not physical
serial I/O or equality to the exact host port bitmap. Product and guest
media remain unchanged.

Source attribution: Fast BOP discovery in `486/dxboot.asm` is WOW_x86-only;
portable `dpmimscr.c` returns a zero fast-entry address. The non-WOW_x86
debug path in `486/dxint31.asm` uses guest MOV DRx in Store/Load_DBG_Regs,
not BOP 0E. Zero hits at that slot cannot prove missing debug capability.
Actual watchpoint behavior remains unverified; these source routes are not
runtime success claims.

## Watchpoint Admission/Execution Gap (Open)

The independent `dpmi_watchpoint.asm` probe installs exception 1, calculates
the linear address of its byte, sets a write watchpoint, writes that byte,
then checks delivery, status/reset and removal. r1 mistakenly used 0Axx from
stale internal function comments; `s38-watchpoint-r1` exits 1 at setup. The
actual dispatch table selects 0B00-0B03. r2 corrects those numbers and adds
setup attribution; `s38-watchpoint-r2` exits 1 at stage 2 after successful
watchpoint registration but without the required one-handler result. Later
status/reset/removal assertions were not reached and are not passing evidence.
The r2 binary lives under `build/M0-T420/S38/watchpoint-r2`.

Correction to the preceding source-route interpretation: immediately before
the dispatch entries, `486/dxint31.asm` sets NO386=1 without WOW_x86 and
includes 0Bxx only when NO386=0. The portable MOV DRx source alone therefore
does not prove a selected guest entry. Current CPU40 `dpmidata.h` maps
DpmiSetDebugRegisters to DpmiIllegalFunction, which does not enforce failure
CF. This is a concrete contract-gap candidate: guest registration succeeds
without proven CPU breakpoint installation. It remains to verify the loaded
guest-to-BOP path and bind the original service to existing CPU debug state;
do not label the watchpoint capability passed or silently exclude it.

Original `dpmi32/i386/dpmi386.c::DpmiSetDebugRegisters` and CCPU `c_debug.c`
are the next bounded source owners to compare. This discovery does not admit
a replacement debugger, new execution profile or guest-media modification.

## Watchpoint BOP Attribution

The existing test-only DPMI wrapper and trace builder now accept an explicit
log path, leaving product sources unchanged. Build root
`build/M0-T420/S38/watchpoint-trace-r1` records reused formal input hashes;
observed worker SHA256 is
`2174e1e52e51f8729ca96e7ccc9c693d13aaf39960bf0c464c011d6bd06b4c2c`.
It runs only in the independent `O:/winnt/tests/D38TRACE` package with
byte-copied media. The official package worker was not replaced.

The first isolated-package attempt exited before task completion: its long
root exceeds the original 64-byte SHELL-value layout checked by
`product-package/package_layout.c`. No DPMI trace was produced. A temporary
unused V: mapping to `O:/winnt/tests` shortened the same package root to
`V:/D38TRACE`; that mapping was removed in finally and its absence checked.

`s38-watchpoint-trace-r2.txt` then exits 1 with `S38_FAIL_STAGE=2`.
`s38-watchpoint-dispatch-r1.events.txt` line 690 records
`after index=0E PE=1 CS:IP=00C7:4EF1`, DS=00B7 and SI=7B80. This confirms
the real watchpoint request reaches the selected BOP 0E route; it is not
lost in CLI admission or guest dispatch. The current macro maps that route
to DpmiIllegalFunction rather than publishing debug registers.

Bounded original owners identified for repair design are
`dpmi32/i386/dpmi386.c::DpmiSetDebugRegisters`, external comparison
`v86/monitor/i386/thread.c::ThreadSetDebugContext`, and CCPU
`ccpu386/mov.c::MOV_DR` plus `c_debug.c::setup_breakpoints`. MOV_DR already
applies valid-bit masks and rebuilds CPU breakpoint tables. A new breakpoint
engine is neither needed nor admitted. The service restoration still needs
source-shaped composition, pointer-span and rollback/lifecycle verification.
