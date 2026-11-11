# T478 S37 DDEML and WINMM investigation

Parent 4a8a74a24, finding40. Historical checkpoints are retained below; the final S37 bounded closure and frozen results supersede their active status.
Admission governance passed in background PID17040. No product or original
guest changes, publication or push.

## Reconstructing the actual route

The current S34 package has original MMSYSTEM.DLL and TIMER.DRV, but contains
no DDEML.DLL. Earlier shard2 evidence distinguishes an app-local Win16 DDEML
implementation from the unselected wddeml32.c thunk. Enabling that thunk merely
because it exists would not establish a caller or replace the guest owner.
The original guest DDEML sources and binary lineage remain distinct evidence.

The pinned OpenNT debug DDEML.DLL is present under
src/mvdm-guest/wow16/ddeml/debug, SHA256
4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443.
S37 copies it unchanged into a disposable app-local test directory; it is not
added to product packaging. The old W31 O:/winnt/tests/w31p path did not resolve
in this workspace check; that single observation does not establish general
unavailability and no writes to O:/winnt were made.

## Current real DDEML observations

The two existing independently authored shard2 init/register probes were copied
byte-for-byte to test/observation/win16_ddeml_{init,register}_probe.asm. The new
verify-win16-ddeml-probe.ps1 preserves their receipt assertions and uses the
current proven physical-stage, fresh package ID, medium-token private desktop
and original logon environment. It records the DLL actually returned by guest
GetModuleFileName, verifies its hash, and retains failed stage evidence.

Background supervisor18672 ran init then register on the unchanged S34 package.
Arguments use Observer build/t478-s34/observer.exe, LogonHelper
build/t478-s34/logon-env.dll, DdemlImage above and its exact hash; roots are
build/t478-s37/ddeml-init and ddeml-register. Both exit0.

- Init: invalid reserved parameter4006h, init0, instance147072, reinit0 with
  identical instance, uninit1 and second-uninit0.
- Register: init0, nonzero service49357, register1, unregister1, free1,
  uninit1. **Callback count is zero.** This is not a callback or transaction
  pass. Earlier W31 registration failures cannot be declared repaired by a
  different binary profile passing these API returns.

[Progress receipt](../../../artifacts/mvdm-audit/shard-2/ddeml-s37-progress.json)
freezes actual results and hashes. The direct SELF_CALLBACK_CONTROL variant
already present in the probe was started as supervisor30260 in
build/t478-s37/ddeml-self-register to distinguish callback entry/probe behavior
from the original module's notification path. It completed exit0: callbackCount1, first/last type0, exactly the direct all-zero call; register/unregister snapshots remain1. Thus the callback thunk can enter and record, but registration adds no observed callbacks.
The mirror source says registration sends local notifications, but a source
expectation is not proof of this binary's exact route or callback delivery.

## WINMM environmental evidence

Existing verify-wow32-multimedia-feasibility.ps1, with explicit RepositoryRoot
and fresh BuildRoot build/t478-s37/mm-inventory2, completed under bounded
supervisor9800: wave_out2, wave_in2, midi_out1, midi_in0, aux0, mixer4,
joystick16, timer_min1, timer_max1000000. This is native endpoint enumeration,
not Win16 asynchronous/device/callback acceptance; it prevents presuming that
all hardware is absent. The original MMSYSTEM timeSetEvent contract requires
its callback to reside in a DLL, which the independent test must respect.

The first supervisor30780 omitted the mandatory RepositoryRoot argument and
its child26580 waited for input. That owned child tree was explicitly stopped;
mm.exit1 is retained. The corrected launch is noninteractive and passed. This
was a test invocation failure, not product or multimedia failure.

Remaining work: resolve actual DDE callback/transaction owner and behavior;
verify applicable original WINMM async operation, cancellation and retirement.
Do not import a new DDE manager, callback ring or disabled converter family.

## Window-word failure isolated

The unchanged OpenNT debug DDEML binary's actual DMGWNDPROC (1777:8A61)
receives WM_CREATE and UM_REGISTER/UM_UNREGISTER (04C8/04C9) successfully;
build/t478-s37/ddeml-window-register records those original entries. Its window
word0 is zero. Read-only disassembly of this exact DLL confirms WM_CREATE
stores PAPPINFO using SetWindowWord; ProcessRegistrationMessage reads it with
GetWindowWord, and this binary's DoCallback explicitly returns for NULL pai.
This explains why successful registration returns do not prove callback delivery.

Independent WORDPROB avoids DDEML entirely. Supervisor26424, window-word,
received creation value5A39 but immediate and post-create reads were0.
Supervisor16972, window-word2, additionally proves class extra size2 and a
second post-create write/read also returns0. Destroy and unregister both1.
Both strict guest cases fail; their reports remain failures, not waived passes.
No original guest binary was edited.

Native x86 USER32 control supervisor17512 (native-word) passes full-HWND
write/read for extra sizes2/4/6. Supervisor39784 (native-word2) repeats this
and supplies the exact original HWND32 conversion, (INT)(SHORT)h16. For all
three sizes SetWindowWord(short,0,6B4A) fails with error1400, while
GetWindowWord(short,0) succeeds and reads the previous5A39; full-HWND read
also remains5A39. Thus modern USER32 supports the storage, but its setter
rejects this original short-handle convention. Native test exit0 means its
full-handle controls passed; the short-handle failure is explicitly observed.

Original walias.h defines HWND32 through that sign-extended short conversion;
WU32SetWindowWord's default branch passes it directly to imported USER32.
The selected map confirms native SetWindowWord/GetWindowWord imports. Existing
project long-word/native operations already have full-identity bindings.
Next: audit and reuse that identity owner for the smallest setter binding,
then rerun WORDPROB and actual DDEML notifications/transactions. No production
change or repair claim yet. WINMM async acceptance also remains outstanding.

## Admitted setter binding

Extend the existing ADAPTER-WOW-078 short-HWND compatibility boundary to
SetWindowWord. The original WU32SetWindowWord keeps its offset conversion,
special offsets and return semantics unchanged. A same-signature redirect in
wow_user_private_access.h selects the existing WOW_NATIVE_MAPPED_CALL owner in
wow_user_task_lifecycle.c, which resolves the published full HWND and preserves
native callout/return ownership and LastError. USER32 still owns storage.
No new handle table, DDE state, storage copy, retry or guest change is admitted.
Direct native reuse was tested and fails specifically for short setter handles;
existing full-handle machinery is reusable, so no mirror intrusion is needed.
Budget: one declaration, one redirect, one existing macro instantiation and
explanatory comment. Acceptance requires exact guest round-trip, DDEML recovery,
applicable lifecycle checks and coherent package/Full/local publication.

## First candidate results

Build supervisor27008 completed exit0 for x86 worker/WOW32 and selected x64
components. Recovery supervisor37988 staged build/t478-s37/package-final.
WORDPROB now passes: creation5A39, previous0, immediate/post-create5A39,
class-extra2, second-write previous5A39/read6B4A, destroy1/unregister1.
This directly verifies the setter repair on the actual guest route.

The subsequent unchanged-debug-DDEML window control did NOT pass: its observer
reports timeout45000ms, exit53504354. Original UM_REGISTER and UM_UNREGISTER
both return1; the last guest stage is6 (after unregister, before free/uninit
completion). Owned test processes were cleaned by the runner, and the failed
physical stage O:/tmp/n941 was retained. No callback recovery or whole DDEML
success is claimed. Add independent report capture after unregister and after
string release to distinguish the remaining boundary; do not suppress the
failure or alter original DLL behavior. Full/publication/commit remain pending.

Second diagnostic supervisor9360 also times out. Its stage8 and interim receipt
prove DdeFreeStringHandle returns1; DdeUninitialize does not return. Callback
count remains0 despite successful register/unregister returns. The first setter
fault is real but does not alone account for all DDEML behavior. The second
probe reused DPRE for post-operation data, so its derived window-word.json is
invalid (those slots now contain callback counters). Preserve the raw receipt;
subsequent probe source writes a separate DPOST.BIN, retaining DPRE's original
window-word meaning. No guest binary or product workaround is introduced.
Candidate package8389208d-e0cd-4b48-a965-ba1f588dd98b remains un-published.

## Corrected callback ABI and causal comparison

The registration probe omitted the standard MakeProcInstance callback prologue:
original kernel31/ldaux.asm emits MOV AX,instance then a far jump, not MOV DS.
The independently authored callback must save DS, set DS from AX and restore DS
before RETF28. Its former counter writes used DDEML's DS and corrupted DLL data;
direct self-callback passed only because the caller already had application DS.
The timeout is therefore not evidence of a production DdeUninitialize defect.
Both register probes now follow that ABI; original guest DLLs are unchanged.
The independent window-word callback adopts the same complete prologue.

Supervisor32600 (ddeml-fixed4) passes: instance147072, actual window word16000
(the instance low word), register1/unregister1/free1/uninit1; callback count2,
register snapshot1, unregister snapshot2; types80A2/80D2. Strengthened runner
asserts those exact registration semantics, rather than API success alone.

Supervisor22668 runs the corrected same probe on unchanged S34: API lifecycle
returns success but callback count0, types0 and window word0; the strengthened
callback assertion fails as expected. Candidate supervisor5556 then passes the
corrected window-word and strict DDEML cases (word-final/ddeml-final), and starts
Full against the exact candidate8389208d-e0cd-4b48-a965-ba1f588dd98b. Full remains
running at this checkpoint, not yet passed. Prior failed diagnostics remain.
This establishes restored original registration callbacks and teardown; it is
not general DDE transaction/device acceptance or S37 closure.

## Multimedia route correction

The earlier T466 S3 table attributes timeSetEvent to TIMER.DRV, but selected
original mmsystem/time.c packages TIMEREVENT and stack.asm::timeMessage calls
resolved tid32Message through MMCALLPROC32. Existing shard1 guest-timer-review
already distinguishes these routes. TIMER.DRV presence cannot establish the
active WOW scheduling owner. Real asynchronous verification must observe this
WINMM route and original callback bridge, with a valid fixed DLL callback;
no new scheduler or resurrection of disabled converter families is justified.

## Independent multimedia callback witness

Added win16_multimedia_timer_library.asm and win16_multimedia_timer_probe.asm,
with verify-win16-multimedia-timer-probe.ps1 using the established private-desktop
physical-package runner. The DLL has fixed preload code/data segments and an
exported callback, following original mmsystem/time.c's explicit callback
requirements. It uses internal segment relocation to select its own data,
not ambient DS or MakeProcInstance. The callback only increments owned data
and checks dwUser11223344; it makes no forbidden system calls.
The application requests a20ms periodic event, yields for300ms, observes a
callback, cancels, allows300ms for already-delivered completion, then verifies
that the counter stays stable through another300ms. The test does not promise
modern synchronous-cancel semantics absent from the original contract.
Original ordinals602/603/607 are from mmsystem.def. No original guest source or
binary is changed or rebuilt. Supervisor12352 waits for the existing Full
supervisor5556 before running this case, so real guests remain serial.
This is an unverified new probe until its actual receipt and route are checked;
a malformed probe or missing callback is not an original-library limit.

## P1 bounded delivery

Full supervisor5556 completed exit0: all16 selected gates passed,377452ms,
identical ten-image package. Window-word and strict DDEML registration callbacks
are its focused acceptance. Supervisor10548 published the same package to this
worktree's assets/release; all ten hashes match, with previous release preserved
under build/t478-s37/release-before. No O:/winnt write or push. This P delivers
only the verified setter binding and registration recovery, not S37 closure.
The multimedia probes are subsequent uncommitted investigation until verified.

Multimedia supervisor12352 failed assembly before guest execution due to NASM
parsing `%512`; spaces corrected this independent test expression. Supervisor8244
then assembled but its guest launcher exited042B without a receipt. This is an
unclassified startup failure, not evidence of unsupported original multimedia.
Supervisor37452 separately probes LoadLibrary return values, with the DLL's
reserved automatic-data prefix made explicit. Its result is pending here.

P1 governance supervisor14820 passed, including relative links; git diff --check
passed. The declared temporary CURRENT size exception remains below32KiB.
The seven-line product binding reuses the existing full-identity and native
return owner; mirror files and original guest binaries are unchanged. P1 is a
local delivery only, with S37 still active. Unverified multimedia probe sources
remain outside this P commit for continued diagnosis.

P1 committed locally as860ac2d91. The staged whitespace check additionally
reported a blank EOF line in each newly copied init/register probe (the earlier
unstaged check did not include untracked files). These two text-only lines are
removed in the following working change; they do not affect assembled bytes.

The LoadLibrary-only control supervisor37452 timed out. Supervisor26856 adds
an interim receipt: stage1 is written before LoadLibrary("MMSYSTEM"), but stage2
never appears. Therefore this failure precedes loading the independent callback
DLL and cannot yet be attributed to its NE format. The selected dispatch trace
shows repeated WK32FileOpen failures (FFFF0002) and WU32SysErrorBox on the guest
task. Original MMSYSTEM imports KERNEL/USER/KEYBOARD, all present in the package.
Next run enables the already existing file-failure trace to identify the actual
requested path, rather than guessing a missing multimedia export or driver.

Supervisor33276's original WK32FileOpen failure trace names literal MMSYSTEM
without an extension throughout the search. The independent LoadLibrary-only
probe supplied that incomplete filename. It now requests MMSYSTEM.DLL and
MMTPROBE.DLL explicitly; the prior diagnostic cannot establish an original
loader defect or unsupported library. Static-import startup failure remains a
separate observation until the correct explicit-load control completes.

Explicit-load supervisor36296 passes: MMSYSTEM module5942, test DLL5911,
stage3, normal launcher exit0. Both files can load unchanged on this candidate;
missing-library or unsupported-module claims are rejected. The next run returns
to actual periodic timer/callback/cancel acceptance after the test-DLL data
prefix correction; library-load success alone is not async proof.

Periodic supervisor40136 still exits042B before a receipt. Its file trace names
MMSYSTEM.DLL under tests, runtime root and runtime/system, while the selected
file resides in runtime/system32. This narrows the remaining startup question
to actual module resolution; it is not a timer-callback failure. The successful
explicit-load control did not capture GetModuleFileName, so its numeric handles
alone do not prove which MMSYSTEM copy loaded. Next verify actual module path
and hash, and distinguish guest search/profile configuration from host package
wiring before drawing an async-capability conclusion. All failed stages remain.

Supervisor39336 verifies explicit-load provenance: actual path
O:/tmp/nbae/system32/MMSYSTEM.DLL, SHA256
080432D0CAD660B163D0D231D37603010F7D9366F58570AC7C74D171C518211A,
matching the selected package original. App-local static control11208 still
exits042B even after eliminating MMSYSTEM file-not-found, so search alone is
not a proved root cause of that startup. Static-entry test construction remains
under review rather than a declared production repair target.

Dynamic control35692 loads both DLLs, resolves explicit ordinals and reaches
original MMCALLPROC32 repeatedly; timeGetTime values advance. It ultimately
fails before a final receipt. Additional application-side phase snapshots will
distinguish timer submission, first wait, callback observation and cancellation.
No new production modification has been made after P1.

Supervisor26404 stops at phase10: the original timeSetEvent call returned0010,
and the first300ms wait did not complete. Its interim receipt preserves the
actual runtime/system32/MMSYSTEM.DLL path. The later query/cancel phases were
not reached. This narrows the unresolved boundary to the first timed wait /
asynchronous delivery, but does not yet prove callback entry or establish
whether the fault is the independent DLL ABI or host callback wiring.

## Import-ordinal correction and renewed acceptance

Original kernel31/kernel.def proves Yield is29 (11 is LocalHandle), and
USER/user.def proves InitApp is USER5 (KERNEL5 is LocalAlloc). These independently
written probes had incorrect imports. Corrected the timer wait and the InitApp
imports in the window-word and DDEML probes; this is a test repair, not a
production behavior change. Supervisor17148 re-ran window-word and strict
DDEML registration/teardown successfully on the released P1 package.

The corrected timer probe reaches phase12: timeSetEvent0010, first300ms wait
completed, callback count1 and bad-user-data0. It still exits042B later; the
prior attribution to first-wait failure is superseded for corrected inputs.
Supervisor22312's no-event control completes all three waits, queries and
normal exit with all counters0. Its recorded module path/hash again matches
the current package. Thus the wait/clock/query path works without the submitted
event; subsequent observation must resolve event cancellation/late delivery.
No unsupported-host or original-limitation classification is made yet.

Supervisor34872's additional cancellation snapshot reaches stop0 and settled
callback count1 after the second300ms wait; final wait/receipt still fails.
Thus original timeKillEvent returns success and no additional callback had been
observed at the settled snapshot. This is not yet complete cancel/exit proof.
Original stack.asm::DriverCallback pushes WORD hdev, WORD msg, then three DWORDs;
the independent callback's RETF16 and dwUser offsets14/16 match that layout.
No-event control22312 passed all waits and normal exit, so the submitted-event
case remains the only unresolved differential. No code fix is inferred yet.

### Corrected ABI comparison and native crash evidence

The corrected USER ordinal5 InitApp probe passes on P1 (ddeml-abi-register:
callbackCount2, types80A2/80D2, all teardown returns1). The identical corrected
probe against S34 fails the strict callback check: callbackCount0 although
init/register/unregister/free/uninit return success. Supervisor26056 exit1 is
the expected negative result, not a runner crash. The setter repair conclusion
therefore survives the independent test-import correction.

Supervisor34796 (mm-timer-fault) repeats the timed event failure with existing
CPU/termination tracing enabled; guest exits042B. No termination trace was
produced, and the CPU witness alone does not identify a fault. Windows events
1000/1001 do: the exact private runtime O:/tmp/n174/system32/ntvdm.exe faults
at native address00000010 with C0000005 (execute access), report identity
08a318ee-137b-4c79-a7f2-4c05ebc0b79c. Saved evidence is
build/t478-s37/mm-timer-fault/windows-crash-events.json. The earlier n5ee run
has the same exception/address. This is native process failure, not merely an
unread modal error dialog or a negative API result. WER archive retains only
Report.wer; no dump was available there.

Address10 also appears as the CPU40 inline-BOP sentinel, but equality alone
DOES NOT establish the call origin. Next diagnosis should use the existing
wow_exception_observer (owned process-tree debugger, original exception
disposition retained) to obtain the native stack/module at failure. Such a
debugger run is diagnostic only, never capability acceptance. No product
change, new tolerance, guest mutation, release replacement or S closure is
justified by the crash address alone. S37 remains active.

### Native interrupt callback ABI mismatch isolated

The original 32-bit diagnostic observer cannot launch the 64-bit console
observer (CreateProcess error50). A test-only x64 build now reads descendant
x86 state through Wow64GetThreadContext. Its first attempt forwarded the WOW64
initial breakpoint4000001F and terminated startup; that run is invalid as a
multimedia reproduction. The corrected observer consumes only this first-chance
debugger breakpoint and preserves access-violation disposition.

Supervisor25824/mm-timer-debug64b reproduces the native execute fault at10.
Supervisor19168/mm-timer-stack records ESP-64 and module ownership, showing:
WINMM RVA0ECdd (timer dispatch), RVA124A5 (return from interrupt publication),
NTVDM RVA5D0AA (host_ica_unlock return). The native stack still contains three
interrupt arguments 1,2,1; the following saved value/timer ID10 is subsequently
used as a return address. This is not evidence of a CPU40 sentinel escape.

Read-only dumpbin of the exact local SysWOW64/winmm.dll confirms:
- NotifyCallbackData RVA118F0 resolves NTVDM.EXE!call_ica_hw_interrupt.
- WOW32DriverCallback publishes the original ring, pushes three arguments,
  calls that pointer at RVA124A3, then immediately pops EDI/ESI/EBP and RET1C.
  It expects callee cleanup of those three arguments.
- The product export (ordinal84) aliases original ica_hw_interrupt; its
  selected original implementation is cdecl. Original wowmmcb.h explicitly
  declares call_ica_hw_interrupt __cdecl; VDMREDIR and WOW32 also consume it.

Thus the supported host's WINMM callback ABI and the original exported ABI
mismatch. This explains the first successful guest notification followed by
native stack corruption. No original queue replacement or new callback
scheduler is needed. However, changing export84 to stdcall without auditing
existing cdecl consumers would create another regression. Next work must
choose a narrow boundary adaptation that preserves those original consumers,
then test normal periodic callbacks, cancellation/exit and the existing
redirector paths. No product fix has been made yet.

Diagnostic inputs/logs remain under mm-timer-{debug,debug64,debug64b,stack};
local native disassembly and exports under build/t478-s37/winmm-*.txt. Debugger
results are diagnosis, never Full or real nondebug acceptance. No publication,
O:/winnt write or push occurred in this investigation.

### Original declaration correction and bounded implementation

Re-reading the divergence prefix changes the attribution: explicit __cdecl in
wowmmcb.h is project DIV-294, not original source. Original WOW32 uses /Gz;
its original declaration therefore uses stdcall. The earlier investigation's
phrase "original wowmmcb.h explicitly declares __cdecl" was incorrect and is
superseded here. The selected CPU40 provider and redirector build use cdecl.
This is a project ABI integration gap exposed by unchanged native WINMM.

The candidate keeps original ica_hw_interrupt's cdecl implementation and adds
only mvdm_public_ica_interrupt, a three-argument WINAPI forwarding boundary.
The generated public export retains name and ordinal84 and targets that
adapter. WOW32's original declaration is restored (remove this file's DIV-294
cdecl change). Redirector vrica.h explicitly uses the public stdcall contract
(DIV-884), because its selected CPU40 compilation default is cdecl. All known
in-tree public callers are rebuilt together; internal CPU40 PIC calls keep
the existing cdecl body. The prior package's incorrectly cdecl public boundary
is not a compatibility promise for independently compiled cdecl external VDDs.
No queue, interrupt order, failure handling or callback lifecycle is replaced.

First build supervisor27456 failed VDMREDIR linking: new generated import
library exports raw call_ica_hw_interrupt (no leading underscore), unlike the
old cdecl alias. dumpbin /linkermember evidence is interrupt-imports.txt.
Name-decoration bindings now use that observed raw name; both callsite and
target are stdcall. Supervisor15232 rebuilds the corrected candidate, with
results under build-p2b.*. Acceptance remains pending; no new release yet.

### P2 candidate verification checkpoint

Build-p2b supervisor15232 passes; governance21972 passes (existing documented
CURRENT size exception only). Candidate package77579397-8a7e-4d07-add4-9a0e3e2404ac
is staged at build/t478-s37/package-p2. It is not yet assets/release.
Supervisor15736's real nondebug timer run passes: timer16, 15 callbacks,
stop0, settled15, final15, bad-user-data0, normal exit. The no-timer control
also passes with every counter0. Both bind unchanged package MMSYSTEM.DLL
080432D0CAD660B163D0D231D37603010F7D9366F58570AC7C74D171C518211A.

To exercise actual ring wrap rather than a single notification, the final
probe waits750ms and requires at least17 callbacks across the original16-slot
ring. Supervisor12624's mm-p2-wrap passes: before38, stop0, settled38, final38,
bad0, normal exit. The same supervisor's word-p2 and ddeml-p2 pass; DDEML has
exactly the register/unregister callbacks and clean free/uninitialize returns.
Full runs serially afterwards under this supervisor in verify-p2-Full.
Its result is still pending at this checkpoint; no publication or P2 commit.

The compiled forwarding object disassembly (interrupt-adapter-disasm.txt)
shows three argument pushes, CALL original _ica_hw_interrupt, ADD ESP,12,
and RET12. interrupt-worker-exports.txt confirms name call_ica_hw_interrupt
remains ordinal84. Internal PIC calls still target the cdecl original body.
These witnesses establish the intended ABI correction rather than a coincidental
passing timer test. Full and final review remain mandatory before delivery.

## S37 bounded closure

P1 (860ac2d91) restores original window-word storage through the existing full
HWND mapping; P2 corrects the original public interrupt ABI at the CPU40
boundary. The selected DDEML is the immutable app-local original DLL, not the
unselected wddeml32.c manager. Registration and teardown now receive their
original notifications. Existing classic DDE transaction evidence is retained;
this does not assert every DDEML API or guest binary version has been tested.

The real unchanged MMSYSTEM timer drives native WINMM, the original16-slot
callback ring, original guest DriverCallback and the independently authored
fixed DLL callback.38 notifications, correct user data, successful explicit
cancel, stable subsequent counts and normal exit pass on the final candidate.
The original time.c contract explicitly requires periodic timeSetEvent to be
paired with timeKillEvent; this S adds no recovery for violating that contract,
no lossless full-ring promise, no forced-unload quiescence guarantee and no new
scheduler. Full-ring producer dropping/interrupt behavior and original queue
ownership remain unchanged. Device-specific wave/MIDI/MCI/fax verification is
finding54/S40, not an implicit claim from this common-boundary timer witness.

Final Full16/16 passes in388963ms under supervisor12624. DOS4GW/NESticle retains
the owner's established exclusion; it was not included in those16 gates.
Test closure maintenance changes old static interrupt aliases to the same
stdcall adapter. DEM115 assertions pass. The redirector test first lacked its
separate archive, then found a stale fixture archive and an existing missing
process-image link dependency; after rebuilding the fixture archive and
listing the current dependency, supervisor24380 passes all12 original-provider
assertions. These were test composition failures, not concealed product passes.
The debugger observation helper builds as x64 and now omits native64 module
addresses when attributing x86 context. No debugger run is acceptance evidence.

Publication supervisor25192 copies/hash-verifies all ten images and the
manifest into this worktree assets/release. Package:
77579397-8a7e-4d07-add4-9a0e3e2404ac. Prior release is retained in
build/t478-s37/release-before-p2. [Frozen results](../../../artifacts/mvdm-audit/shard-2/ddeml-winmm-s37-results.json)
contain package hashes, focused receipts, Full timing and diagnosis hashes.
No O:/winnt write or push. Governance and final staged review precede the
local P2 delivery; S38/finding48 follows under continuous authorization.

P2 mirror changes are limited to two declarations: wowmmcb.h removes the
project cdecl intrusion (net-4 lines); vrica.h names the public convention in
the cdecl build (net+2 lines, DIV-884). Non-MVDM is unchanged. The new
10-line executable-owned adapter only forwards three arguments across the ABI;
no original implementation is copied. Build name bindings and independent
tests are counted separately from production behavior. Existing excluded
branches and original guest/media bytes are unchanged.

Final governance supervisor11268 passes, including the S38 admission packet;
git diff --check passes. The temporary CURRENT size exception is documented.
