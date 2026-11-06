# T478 S37 DDEML and WINMM investigation

Parent 4a8a74a24, finding40. S37 is active; this is a checkpoint, not closure.
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
byte-for-byte to tests/observation/win16_ddeml_{init,register}_probe.asm. The new
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
