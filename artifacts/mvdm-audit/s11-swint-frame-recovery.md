# S11 protected software-interrupt frame candidate

Status: published as a tested partial baseline, not S11 closure. Source
commit `f1a641bd7`; the isolated candidate was published to both release
destinations with all 20 hashes verified. The installed `run16 cmd /c ver`
smoke exited successfully; see [publication](s11-swint-frame-evidence/publication.json)
and [installed smoke](s11-swint-frame-evidence/installed-smoke.txt).

The final candidate passed the 16 Full gates, the 105-case CCPU boundary
suite, Winmine/SOL/WRITE and four SOL Options cycles, and Win3.1 standard
and enhanced mode with both DOSONLY settings. The automated mouse probe
passed its injected-motion checks. These checks do not reproduce or resolve
the separately reported Win386 freeze after sustained physical mouse motion.
The immutable guest binaries in the two Win3.1 trees remained unchanged.
See [Full](s11-swint-frame-evidence/full-timings.json),
[games](s11-swint-frame-evidence/game-runs.json),
[Win3.1](s11-swint-frame-evidence/win31-matrix.json),
[guest integrity](s11-swint-frame-evidence/win31-integrity.json), and
[mouse](s11-swint-frame-evidence/mouse-observations.txt).

Original NT4 `base/ntos/ke/x86/instemul.asm:OpcodeINTnn` calls `PushInt`,
which uses the registered `VDM_INT_32` handler flag for the three-word or
three-dword frame independently of the application's default width. It uses
the current stack descriptor to select SP versus ESP, commits the complete
new ESP and target EIP, and, in its 32-bit frame, writes the CS slot from EDX
after replacing only DX in the saved EIP. The existing CPU40 path reaches
the selected original non-i386 `DpmiSwIntHandler` through
`INTx -> host_swint_hook`; its frame width instead followed `CurrentAppFlags`.

The [same final fixture against the old selected source](s11-swint-frame-evidence/comparable-baseline-stderr.txt)
fails on the expected 12-byte frame for a 16-bit application with a 32-bit
handler. The [candidate](s11-swint-frame-evidence/final-fixture-results.txt)
passes all eight combinations of application width, handler frame width and
stack address width, checks the return EIP/CS/EFLAGS bytes and full target
EIP, and checks the six-versus-twelve-byte underflow boundary. It also passes
the existing absent-handler, V86, fault-return and interrupt-return checks.
The fixture compiles the actual selected source with test-only function
sections and controlled register/memory seams; it is not an immutable guest
or full runtime integration test.
The selected test object and the current worker production object have
identical extracted instruction bytes for both `DpmiSwIntHandler` and the
shared stack helper ([comparison](s11-swint-frame-evidence/selected-object-instructions.txt)).
Object relocation values remain unresolved in that comparison; this is not
proof that all linked dependencies or guest behavior match.

`MVDM-HOST-DIV-481` keeps the existing stack-size calculation and passes its
frame width explicitly for the CPU40 software-interrupt call. Other callers
and non-CPU40 profiles keep their preexisting width rule. This repairs the
observed frame discrepancy, not every NT4 `PushInt` semantic: target
descriptor admission, virtual IF / interrupt-gate state, fault ordering and
handler reentry remain separately unproved. In particular, do not infer their
equivalence from these eight fixture cases.

The retained `BuildStackFrame` also obtains the stack descriptor from the
original DOSX `Ldt` source table, while the selected CPU40 protected-mode
adapter separately projects the NT process LDT. The fixture supplies one
synthetic table and cannot prove these views agree after independent
process-LDT updates. This remains a descriptor-identity boundary; the frame
width repair alone does not resolve it.

A [subsequent underflow control](s11-swint-frame-evidence/failure-baseline-stderr.txt)
also showed that the selected carrier cleared TF before rejecting the frame.
The original `OpcodeINTnn` only commits its flags after `PushInt` succeeds.
For CPU40 protected mode, flag changes now follow successful stack
validation; V86 and other CPU profiles retain their existing order. The
same selected-source fixture then passed. This does not prove recovery after
a later fault during target-selector loading, or the virtual-IF rules above.
An earlier package's background tests were already running before this
additional source change; their results do not certify this final candidate.

## Candidate: original PushInt stack-limit decision

`SsToLinear` expands a granular limit as `(limit << 12) | 0xfff`;
`PushInt` first subtracts the frame size, then rejects a normal segment when
the new SP is greater than or equal to that limit, or an expand-down segment
when the new SP is less than or equal to it. The selected non-i386 helper
instead incremented before expanding the limit, tested the old SP for a
normal segment, and accepted equality on an expand-down segment. Those
decisions differed in controlled selected-source fixtures.

The candidate keeps the prior helper behavior for its other callers and
non-CPU40 profiles. Only the CPU40 protected software-interrupt call requests
the `PushInt` decision; it continues to use the existing DOSX source table and
mapping service, whose identity and fault boundaries remain unproved above.
The actual selected source before the change fails the new normal-limit
positive case; the candidate passes two normal, two expand-down and two
granular cases. These use controlled register, memory and descriptor seams,
not a live guest, and do not prove selector admission or failed native-access
delivery. The production and fixture objects have identical extracted
instruction bytes for the handler and helper, with relocations unresolved:
[comparison](s11-swint-limit-evidence/selected-object-instructions.txt).
The candidate passed the [16-gate Full suite](s11-swint-limit-evidence/full-timings.json);
the separate final package from ten current build outputs also passed
[Full](s11-swint-limit-evidence/latest-full-timings.json),
[games](s11-swint-limit-evidence/latest-game-runs.json),
[four Win3.1 cases](s11-swint-limit-evidence/latest-win31-matrix.json),
and the [synthetic mouse witness](s11-swint-limit-evidence/latest-mouse.txt).
The original and isolated installations retained all 86 monitored guest
binary hashes. This does not cover physical mouse input or every exception,
selector-identity or callback-reentry boundary. Source commit `a40d83287`
was published as a coherent ten-component baseline; both destinations match
all 20 hashes. The installed `run16 cmd /c ver` smoke exited zero:
[publication](s11-swint-limit-evidence/publication.json),
[installed smoke](s11-swint-limit-evidence/installed-smoke.txt).
The first Full invocation rejected mismatched sealed/unsealed cache identities
before product tests; its [preflight result](s11-swint-limit-evidence/full-first-preflight.txt)
is retained. The rerun uses hash-matched sealed runtime inputs and is separate.
Afterward, comparison with the latest cached build outputs showed that the
first tested package's nine unchanged inputs came from the previous sealed
package and two differed in hash from the just-built outputs. That package
is retained as a preliminary result, not the latest-build delivery. A new
package is sealed from all ten actual current build outputs, with each input
hash checked; its full and product tests must pass separately.

## Candidate: rejected SIM32 stack mapping

The CPU40 SIM32 adapter returns NULL when its selected descriptor walker
rejects a selector. The software-interrupt stack helper formerly added SP to
that NULL alias, cleared TF, and wrote a frame; a controlled selected-source
fixture caught a host access violation and observed changed flags. NT4
`SsToLinear` instead returns failure when its selector lookup fails. The
candidate checks the alias before changing flags or writing a frame and
returns unhandled to the existing CPU interrupt path. Identical fixture
inputs report `host_fault=0`, `handled=0` and unchanged registers and flags;
see [baseline](s11-swint-null-evidence/baseline-stderr.txt) and
[candidate](s11-swint-null-evidence/candidate-stderr.txt).

This is a controlled NULL-return seam, not a live stale-selector witness.
It does not establish that SIM32's NULL result covers every NT4 selector
failure, resolve nonlocal descriptor-read faults, or unify the source and
projected LDT identities. Other stack-frame callers retain their prior path.

The first NULL guard is not eligible for publication as written: source
review found that `GetVideoMemory` can also return NULL when allocation of
its video scratch buffer fails. Treating every NULL as an invalid selector
would conflate distinct errors and change the existing failure path. The
first candidate passed Full, but its results do not certify the narrower
candidate. The latter calls the original CCPU `selector_outside_GDT_LDT`
only after a NULL alias and returns unhandled only for a null or out-of-table
selector. For a selector still in the table it retains the old failure path,
rather than claiming the NULL was caused by the selector. The controlled
fixture passes both cases; it stubs the descriptor-owner result, and does not
prove concurrent table mutation, live selector failure or complete fault
delivery. A direct CCPU declaration is used because including `insignia.h`
in the DPMI translation unit redefines its `BOOL` and caused new type
warnings; the declaration's 16-bit selector and 32-bit address types were
checked against the original CCPU declarations. The production link map and
object relocation identify the original CCPU provider:
[symbols](s11-swint-null-evidence/narrow-production-symbols.txt).
The narrower ten-component package passed [Full](s11-swint-null-evidence/narrow-full-timings.json),
[game](s11-swint-null-evidence/narrow-game-runs.json),
[Win3.1](s11-swint-null-evidence/narrow-win31-matrix.json) and
[synthetic mouse](s11-swint-null-evidence/narrow-mouse.txt) checks; all 86
monitored guest binaries remained unchanged in both trees. These results do
not prove live selector failure, concurrent table mutation or physical input.
Source commit `222b0e5a1` was published as a coherent ten-component package;
both destinations matched all 20 hashes and installed `run16 cmd /c ver`
exited zero. [Publication](s11-swint-null-evidence/publication.json),
[installed smoke](s11-swint-null-evidence/installed-smoke.txt).

The interrupt-gate flag boundary is a separate unresolved group: NT4
`PushInt` clears the shared virtual interrupt bit for interrupt gates, while
CPU40's `setFLAGS` changes IF only when CPL is no greater than IOPL. CPU40's
normal interrupt entry uses a direct IF update, and `IRET` returns through
the privilege-sensitive flag setter. DOSX chooses interrupt gates for real
vectors in `dxfunc.asm`, while `InitialiseDosEmulation` selects the software
CPU's FCLI/FSTI fallback. A one-sided IF clear without tracing the return and
shared-flag lifecycle could leave interrupts disabled; the null-alias guard
does not attempt that change or prove gate equivalence.

### Selected CCPU entry/return comparison

A test-only extension of the existing CCPU monitor fixture links the real
selected DPMI handler and CCPU providers. It uses an installed 16-bit
software interrupt handler, a controlled valid LDT and CPL 3, and captures
both CPU flags and the fixed-state word. With an interrupt gate, IF initially
set and IOPL 0, the current handler returns with CPU IF and the fixed virtual
IF still set. The test then **deliberately forces both off** and calls the
real CCPU `IRET`; CPU IF and the fixed bit both remain off despite the saved
frame containing IF=1. With IOPL 3, CPU IF is restored but the fixed bit
remains off. A trap-gate control keeps IF set, and an initially disabled
interrupt-gate control remains disabled. [Matrix](s11-gate-return-evidence/matrix-results.txt),
[fixture diff](s11-gate-return-evidence/matrix-fixture.patch),
[identities](s11-gate-return-evidence/matrix-inputs.csv).
The stored fixture diffs remove trailing whitespace from added blank lines;
the identity hashes describe the original compiled temporary files.

A second controlled path writes an actual CCPU interrupt-gate descriptor and
uses the original `do_intrupt` and `IRET` directly. Native entry clears CPU
IF but does not clear the fixed virtual bit. At IOPL 0, IRET leaves CPU IF
off; at IOPL 3 it restores CPU IF. [Results](s11-gate-return-evidence/native-results.txt),
[fixture diff](s11-gate-return-evidence/native-fixture.patch).
Thus neither clearing IF on the existing DPMI entry alone nor bypassing it via
CCPU's IDT is a complete restoration of the selected NT4 virtual interrupt
contract. The experiments use synthetic vector 31h and controlled descriptors;
they do not establish an actual immutable DOSX guest call, guest reentry or
task-switch lifecycle.

The same original-linked CCPU fixture also directly invoked the selected
`DpmiHwIntHandler` with a controlled installed IRQ-8 handler and locked
stack. With initial IF=1, entry retained CPU IF at CPL 3 / IOPL 0 despite
the handler's `setEFLAGS(... & ~IF)`; at IOPL 3 the same code cleared CPU
IF. The fixed virtual bit remained set in both cases. [Hardware entry
results](s11-gate-return-evidence/hw-results.txt),
[fixture diff](s11-gate-return-evidence/hw-fixture.patch). This is a direct
function invocation with controlled CPU and descriptor state: it does not
prove a real PIC acknowledgment, a live DOSX entry, nested delivery, or
successful return. It does show that reviewing software INT alone misses a
separate hardware-entry owner; a complete repair must include both paths.

The hardware stack-failure route cannot be equated to a generic rollback.
NT4 `vdm/x86/vdmints.c:PushPmInterrupt` increments the lock count, records
the prior stack at the first nesting level, switches the trap frame to the
locked stack, and deliberately publishes the computed invalid ESP before
returning `STATUS_ACCESS_VIOLATION`; `VdmDispatchInterrupts` then dispatches
the exception. The selected `DpmiHwIntHandler` instead attempts to end its
locked-stack use and returns FALSE, causing `ccpu_dispatch_accepted_irq` to
enter CCPU's ordinary `do_intrupt` fallback; it also attempts to clear flags
before constructing the stack. Merely restoring registers on FALSE, or
raising a synthetic guest fault, would not establish the original debugger,
stack, exception or locked-stack semantics. This remains an S11 execution
and failure-ownership gap, not a claim that the controlled entry fixture
proved a live failure.

The original WOW_x86 `riret`/`riretd` macros in `inc/intmac.inc` are
distinct from `FIRET`: they copy the saved IF to the fixed word before IRET,
and execute STI/CLI only when `RI_BIT_MASK` is set. The WOW hardware
reflector in `dpmi/dxintr.asm` uses this return form, whereas other source
paths contain ordinary IRET. The synthetic direct-IRET cases above do not
exercise the macro. In particular, they cannot prove an actual reflector
fails or justify applying a global IRET override. The original CCPU IRQ
loop checks CPU IF, while the protected monitor and WOW_x86 return macro
also use the fixed word; both ownership and every path that changes those
representations must be reconciled together before changing either one.

IOPL 0 is not merely a synthetic possibility: an earlier immutable-DOSX
fault observation recorded a protected-mode saved EFLAGS of `00000006`
while executing the STI fallback. That observation was a fault-frame sample,
not an observed interrupt-gate entry or evidence that every task uses IOPL 0.
See [actual DOSX fault attribution](s11-foundation-evidence/code32-prebuilt-search/review.md).

Original NT4's protected dispatch table does not select an IRET emulator,
while the original DOSX `FIRET` macro takes different paths based on the
software-CPU flag: on the native path it restores the fixed interrupt bit,
while on the software-CPU path it executes IRET. DOSX installs interrupt
gates for vectors 08h–0fh and 70h–77h. Any fix must reconcile this source
choice with the selected CCPU's actual CPL/IOPL state and return path; it
must not globally reinterpret unrelated protected-mode IRET or introduce an
unowned parallel task/stack state. No runtime change or semantic concession
is claimed by this diagnostic.

## V86 software interrupt routing candidate

NT4 `emv86.asm:OpcodeINTnnV86` checks `VDM_INT_HOOKED`: an unhooked
vector uses the real-mode interrupt table, while a hooked vector uses
`VDM_DPMIINFO.DosxRmReflector`. Selected CCPU `INTx` calls the host software
hook whenever PE is set, including VM=1; the current DPMI handler instead
interpreted that state as protected mode and also applied the WOW INT 2Ah
signature. A controlled selected-CCPU baseline observed both interceptions
with VM=1 and an unhooked vector. It is not evidence of an immutable guest
failure. The original DOSX source also contains an IVT-based reflector, so
the presence and ownership of the separate NT4 hooked reflector cannot be
assumed from its name.

An early candidate returned unhandled for an unhooked V86 interrupt. The
selected source-object and direct hook checks passed, but the end-to-end
`INTx` test failed: selected CCPU `do_intrupt` selects its protected IDT
whenever PE=1, including VM=1. Thus the fallback does **not** perform the
required V86 IVT dispatch. The candidate was removed. The controlled test
fixture patch and logs are retained as diagnostic evidence; they are not a
passing regression or an immutable-guest failure. Original `ReflectV86Int`
already builds a real-mode frame, but its flag updates, stack failure and
hooked-reflector ownership still need to be reconciled with the NT4 monitor
before it can be used as a complete replacement. No guest change or release
is implied.

The baseline and rejected candidate are retained in
[V86 baseline](s11-gate-return-evidence/v86-baseline-results.txt),
[fixture patch](s11-gate-return-evidence/v86-candidate-fixture.patch), and
[candidate results](s11-gate-return-evidence/v86-candidate-results.txt).
The unhooked tests set PE and VM together, install the production DPMI
software hook, and compare registers before and after a direct hook call;
the third case calls the original CCPU `INTx` with a controlled IVT entry.
Cases 121 and 122 passed only with the rejected candidate; case 123 terminated
with a diagnostic exception before reporting its assertion. Source tracing
confirms `do_intrupt` selects the protected IDT on PE alone. The test is a
controlled host fixture, not an end-to-end DOSX or product verdict.

NT4 `trap.asm:V86Intnn` supplies a further constraint: on the default
virtualized-IF path, the saved IF comes from VDM fixed state, the virtual IF
is cleared, and the saved frame presents IOPL 3 while current NT and TF are
cleared. Its direct-IOPL path clears CPU IF instead. Therefore simply calling
the existing `ReflectV86Int`, which pushes the caller-provided EFLAGS and
changes SP, IP and CS, would not by itself establish the original flag or
failure contract. This is part of the same interrupt ownership investigation,
not proof that either controlled path was observed in a real guest.

The selected CCPU's V86 exception-to-IRET case 140 executes an interrupt-gate
divide exception, advances the saved IP in the guest handler and executes IRET.
With the historical software-CPU selector bit set, execution returns to the
expected instruction with CPU IF set while fixed state 0x714 retains IF clear
(`flags=23202 fixed=a400`). The initial assertion requiring both bits to agree
was incorrect for this path: original `vint.h` selects real IRET for the
software CPU, and selected CCPU `c_main.c` gates hardware delivery on CPU IF.
NT4 `V86Iret` updates fixed-state IF on the separately trapped IRET path; direct
IOPL execution need not take that trap. Case 140 is now a fixture gate
for this direct return, not proof of pending-interrupt delivery or equivalence
of every fixed-state consumer. A separate case 141 raises a real PIC IRQ after
the exception gate has disabled CPU IF; after IRET, the selected CCPU accepts
it once, after executing the first returned instruction (`eip=108`). This
matches the existing CCPU dispatch placement and its explicit IRET comment;
it does not establish that NT4's trapped IRET has identical interrupt timing.
Both cases pass in the 123-case fixture. No new IRET implementation was introduced.

Case 142 then executes that exception/IRET sequence with A20 wrapping enabled,
SS=FFFF and SP=0020. The former single `ReflectV86Int` host pointer wrote the
frame above 1 MB while the CPU read wrapped low memory; the return failed
(`cs=6 sp=1e`). The working DIV-485 candidate writes the same separately
wrapped 16-bit offsets in the same flags/CS/IP order through CCPU supervisor
access and reads the IVT through that access layer. The same case now returns
to the original CS:IP and SP. Case 143 repeats the same stack with A20 enabled;
it also returns correctly. All 125 boundary cases and the selected DPMI
object fixture pass. Original `instemul.asm:PushException` confirms the
flags/CS/IP store order and individually wrapped 16-bit offsets. This proves
the controlled unpaged A20 cases, not paged
fault/reentry semantics or full product compatibility.

The A20 candidate subsequently passed the Full selected product gate, all
four Win3.1 standard/enhanced × DOSONLY combinations, and independent
Winmine, SOL, WRITE and four-round SOL Options probes. All ten staged image
hashes still match after testing; 86 monitored guest binary hashes remained
unchanged in both source and isolated installations. [Recorded results and
package identities](s11-v86-origin-evidence/a20-package/) establish these
particular outcomes, not the excluded paged or virtual-IF exception paths.
The tested ten-component package was subsequently published to both release
destinations; all 20 installed hashes matched. This is a partial S11 baseline,
not S11 closure or a claim that the open exception paths are restored.

An additional odd-SP diagnostic, case 144, places a 16-bit flags write across
the A20 boundary itself. Its initial test accidentally let the wrapped write
change the INT 0 IVT target; after controlling that alias the fixture still
reports `STATUS_ACCESS_VIOLATION`, instruction address zero. A direct call to
`spr_write_word(0xfffff, ...)` reproduces the same null call before guest
execution; the real SAS type query reports ROM then RAM. The original physical
word path classifies that as a cross-type write and falls through its default
write-handler dispatch. The fixture does not establish whether the full
machine provides that handler. A temporary adapter-only byte-split candidate
avoided the immediate null call but the guest failed to complete: the first
byte lies in ROM and cannot simply be assumed writable. That candidate was
removed. This diagnostic remains outside the passing gate; it is not evidence
that a physical Win3.1 workload hits this boundary or that an unrelated CCPU
change is justified. The interaction of ROM-write and real exception delivery
remains unresolved.

Case 145 isolates the original CCPU memory provider without exception
reflection: it marks both boundary pages RAM, enables A20 wrapping, and calls
`spr_write_word(0xfffff, 0xa55a)`. The observed bytes are `fffff=5a`,
`zero=66` (unchanged), `100000=a5`; expected bytewise wrapping would write
`a5` at zero. `ccpusas4.c:phy_w16` masks the starting address once before a
word write, which explains the result for this controlled setup. This is an
original CCPU physical-access boundary, not proof of a real workload, and
the fixture remains outside the pass gate. Fixing only the new exception
writer would not repair original IRET's corresponding word read. Per S11's
scope it is not an authorization to change the unrelated core memory path.

A second candidate used `ReflectV86Int` for unhooked VM=1, IOPL=3 and
preserved the 16-bit frame and cleared IF/TF/NT. The selected DPMI fixture,
two selected-CCPU `INTx` cases and the 107-case CCPU suite passed; see
[candidate fixture](s11-gate-return-evidence/v86-reflect-candidate.patch)
and [results](s11-gate-return-evidence/v86-reflect-candidate-results.txt).
Review then found that `ccpu386/zfrsrvd.c:DoNpxPrologue` also calls `INTx(7)`
for EM/TS, whereas `c_main.c` calls `INTx` for decoded software INT and INT3.
The shared software hook receives only the vector, so that candidate cannot
distinguish an x87 device-not-available path from software `INT 7`; its
passing tests do not prove the exception contract. It was removed before
publication. Reconnection must preserve instruction origin as well as the
hooked-reflector, IOPL and flag ownership boundaries.

The next candidate moves only the unhooked, IOPL-3 V86 `CD imm8` route to
the CCPU instruction decoder after its normal IP advance. The host adapter
accepts it only while the registered software handler is the DPMI handler;
`DpmiV86SoftwareInt` reuses `ReflectV86Int`. This keeps the common `INTx`
entry unchanged for the x87 caller and preserves other registered software
handlers. The selected DPMI fixture covers the real-mode frame and rejects
hooked, virtualized-IOPL and protected-mode inputs. The selected CCPU fixture
executes actual `CD 31` and `CD 2A` instructions, verifies target and saved
frame, calls `INTx(7)` with an independent handler, checks a decoded `INT 7`
with that independent handler, and exercises original `ZFRSRVD` with CR0.EM
set. Two further decoded cases verify 16-bit IP and SP wraparound. All 112
CCPU boundary cases and the selected DPMI fixture passed in the candidate
build. This direct x87 entry is not a decoded guest instruction.
These tests do not prove a real DOSX guest, hooked V86, trapped IOPL, stack
fault or interrupt reentry behavior. The first Full attempt stopped at its
control-fixture setup because the selected cache lacked
`frontend-request-client-test.exe`; its prior gates are not a full pass.
At that point the candidate had not passed the full product gate or been published.

The subsequent actual-CCPU case 128 adds a V86 stack spanning two linear
pages whose physical pages are non-contiguous. It **fails** for the current
candidate: the return IP is correct, but saved CS and flags appear at the
old contiguous host addresses rather than at the second mapped physical
page ([result](s11-v86-origin-evidence/mapped-stack-candidate.txt)). The
cause is that `ReflectV86Int` obtains one `Sim32GetVDMPointer` for SS and
adds offsets to that single host pointer. `Sim32pGetVDMPointer` translates
only that starting address through `c_GetLinAdd`; it does not establish an
alias for every subsequent guest page. Case 128 is now included in the CCPU
gate and currently fails. Therefore the earlier 112-case and Full results
do not certify this newly exposed boundary, and the candidate is not eligible
for publication at that point.

Original CCPU `spr_write_word` translates each linear page and sets A/D bits,
but its page-fault delivery can reenter the CPU. It must not be substituted
without tracing and validating its fault/partial-write behavior against the
NT4 V86 monitor. Additionally, `trap.asm:V86Intnn` computes a wrapped
starting SP then writes adjacent words, while `emv86.asm:OpcodeINTnnV86`
subtracts each 16-bit stack offset separately. The earlier SP-wrap test alone
does not establish which original entry owns each actual instruction path.

The current experimental candidate replaces the single SIM32 alias with
original CCPU `spr_write_word`/`spr_read_word`, and delays flag/SP/register
commit until the accesses return. The previously failing discontiguous case
now passes and observes accessed/dirty bits on both mapped stack pages and
the IVT read. Controlled cases 129/130 repair an initially absent first or
second stack page in a **test-only** fault callback; case 131 repairs an
absent IVT page. Their restart preserves the original IF, SP and return frame
and records the respective partial writes. The earlier early-flag-commit
candidate lost IF on retry ([before](s11-v86-origin-evidence/mapped-stack-fault-before.txt),
[after](s11-v86-origin-evidence/mapped-retry-cases.txt)); the 116-case CCPU
suite passed with those test callbacks. These callbacks are not production
exception delivery and do not prove NT4 equivalence.

When the same controlled fault is instead passed to the actual selected
`DpmiFaultHandler` with no installed DPMI fault vector, it returns unhandled
after clearing IF; the fixture then observes a double fault and terminates
with a host access violation ([trace](s11-v86-origin-evidence/original-fault-diagnostic.txt)).
The synthetic fixture's IDT and DPMI fault-vector state are deliberately not a
real DOSX setup, so this does not prove a production crash. It does establish
that the successful callback-injected retries cannot certify the actual fault
owner. Real failure delivery and the NT4 fast/slow wrap distinction remain
open and the experimental runtime is not eligible for publication. The Full,
Win3.1 and game results below belong to the **earlier** single-alias candidate,
not this changed runtime; they must be rerun after the boundary is resolved.

The subsequent coherent-package rerun passed all 16 Full gates using the
complete fixture cache; see [timings](s11-v86-origin-evidence/full-timings.json),
[package manifest](s11-v86-origin-evidence/package-manifest.json),
[producer hash comparison](s11-v86-origin-evidence/producer-hashes.csv),
[selected DPMI](s11-v86-origin-evidence/dpmi-selected.txt), and
[CCPU cases](s11-v86-origin-evidence/ccpu-cases.txt). The first failed setup
is retained above as a runner error. Independent Win3.1 and game checks,
final review and coherent publication are still in progress; Full is not
proof of hooked V86 or virtualized-IOPL behavior.

The independent [Win3.1 four-mode matrix](s11-v86-origin-evidence/win31-matrix.json)
and [game cases](s11-v86-origin-evidence/game-runs.json) passed; both Win3.1
trees retained all 86 checked guest binaries
([integrity](s11-v86-origin-evidence/win31-integrity.json)). The Win386 mouse
probe did not pass: `SetWindowsHookExW` reported 126 before input injection.
The same historical probe and unchanged prior package failed identically in
a same-session control ([candidate](s11-v86-origin-evidence/mouse-candidate-error.txt),
[baseline](s11-v86-origin-evidence/mouse-baseline-error.txt)). The hook DLL is
x64 and exports `MouseInputHook`, with USER32/KERNEL32 imports
([inspection](s11-v86-origin-evidence/mouse-hook-dependencies.txt)). This
does not prove why installation failed and does not establish mouse regression
or success; physical Raw Input was not exercised. The candidate remains
unpublished while that failure and the final delivery checks are resolved.
After both mouse-probe attempts, all 86 tracked guest binary hashes still
matched in the original and isolated Win3.1 trees
([integrity](s11-v86-origin-evidence/win31-post-mouse-integrity.json)).

Further self-thread diagnostics in the same desktop-capable execution context
loaded the probe module and resolved its export, but a non-NULL module
argument returned 126 from O: and from a local temporary copy; an untriggered
USER32 control also returned 126. A same-thread hook with a process-local
callback and NULL module succeeded. Results:
[probe module](s11-v86-origin-evidence/mouse-hook-self.txt),
[temporary copy](s11-v86-origin-evidence/mouse-hook-temp.txt),
[system module](s11-v86-origin-evidence/mouse-hook-system.txt), and
[local callback](s11-v86-origin-evidence/mouse-hook-inline.txt). The same
DLL callback also installed successfully on its own thread with a NULL
module ([result](s11-v86-origin-evidence/mouse-hook-null-module.txt)); that
self-thread control cannot replace the cross-process probe. Microsoft
[documents](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw)
different module-argument requirements for same-process and cross-process
threads, so these self-thread observations alone cannot establish that valid
cross-process module-backed hooks fail.

A conforming separate control created two test-owned x64 processes, initialized
the child's message queue, and supplied a loaded DLL and exported procedure for
the child's thread. Both the probe DLL and a separate untriggered USER32
export control returned 126 at hook installation. See the
[control source](s11-v86-origin-evidence/mouse-hook-cross-control.c),
[probe result](s11-v86-origin-evidence/mouse-hook-cross-control.txt), and
[system-module result](s11-v86-origin-evidence/mouse-hook-cross-system.txt).
This demonstrates failure outside NTVDM and the mouse DLL, not its Windows
root cause or a passing substitute for the Win386 mouse probe.
In the two-process control, Windows reported zero extension-point-disable
flags for both processes and zero binary-signature and image-load mitigation
flags for the child; all four queries succeeded
([source](s11-v86-origin-evidence/mouse-hook-cross-mitigation.c),
[result](s11-v86-origin-evidence/mouse-hook-cross-mitigation.txt)). This
rules out those specific process-mitigation bits in that control, not all
system or desktop policies.

Historical OpenNT USER source `client/clhook.c` resolves the module path and
`kernel/hooks.c:_SetWindowsHookEx` maps failure of `GetHmodTableIndex` to
126; `kernel/libmgmt.c` shows atom registration and an internal module table
as possible failure points. This is a hypothesis source, not proof that the
current Windows 11 implementation follows the same path. Direct controls
successfully resolved the probe module's full path, added and deleted fresh
global and local atoms, and registered and unregistered a fresh window class
([module](s11-v86-origin-evidence/mouse-hook-path-check.txt),
[atoms](s11-v86-origin-evidence/mouse-hook-atom-control.txt),
[class](s11-v86-origin-evidence/mouse-hook-class-control.txt)). These do not
inspect USER's internal hook-module table or establish its capacity.
An aggregate read-only global-atom inventory found 160 occupied entries out
of 16,384 string-atom slots and no DLL-path names
([result](s11-v86-origin-evidence/mouse-hook-atom-inventory.txt)); it did
not enumerate USER's internal module table and therefore does not rule out
that separate table's exhaustion. A fresh x86 DLL and two x86 test-owned
processes reproduced 126 as well ([hook source](s11-v86-origin-evidence/mouse-hook-cross-x86-dll.c),
[control source](s11-v86-origin-evidence/mouse-hook-cross-x86.c),
[result](s11-v86-origin-evidence/mouse-hook-cross-x86.txt)). The observed
failure is therefore not specific to the earlier x64 DLL or process bitness.
As a separate USER-namespace observation, `GetClipboardFormatNameW` could
resolve the test's freshly registered class atom before it was unregistered
([control](s11-v86-origin-evidence/mouse-hook-useratom-control.txt)). An
aggregate query returned 656 USER names, of which 13 ended in `.dll`; it
found no names ending in the probe DLL, USER32 or KERNEL32
([counts](s11-v86-origin-evidence/mouse-hook-useratom-inventory.txt)). No
clipboard contents or other names were recorded. This is not an enumeration
of the internal hook-module table and cannot establish its occupancy.
The current USER lookup includes the exact Windows `shell32.dll` path
([result](s11-v86-origin-evidence/mouse-hook-shell-atom.txt)). A separate
two-process control using that loaded system module still returned 126
([source](s11-v86-origin-evidence/mouse-hook-cross-shell32.c),
[result](s11-v86-origin-evidence/mouse-hook-cross-shell32.txt)). Its child
thread was suspended during installation and hook removal, so the exported
procedure was not invoked. This excludes a simple explanation that no USER
name exists for the chosen module; an atom entry is not proof that the
separate internal hook-module registration already exists.

On the candidate frontend's actual input path, `WM_MOUSEMOVE` is intentionally
ignored while captured movement comes from `WM_INPUT`; accepted mouse events
are copied into an NTCON-owned input queue before worker transport. Posting a
legacy mouse-move message, or writing directly to a worker console, would not
exercise the same path as the historical sink-hook probe and cannot be used to
claim this regression passed. No project or probe processes were found in the
read-only process inventory after the failed runs.
The enabled Code Integrity operational log reported no events in the observed
15-minute window ([record](s11-v86-origin-evidence/mouse-hook-code-integrity.txt));
absence of an event does not rule out another policy or loader failure.
The control host was medium integrity in session 3, and its loaded-module
inventory showed USER32 and win32u but no project hook module
([context](s11-v86-origin-evidence/mouse-hook-context.txt)). This eliminates
neither desktop policy nor other external hooks; it only records the tested
process context.

The original DOSX sources provide additional ownership context but not a
completeness proof: `dxboot.asm:InitIntrDscrTable`, `dxfunc.asm`'s WOW setup
and `dxutil.asm:PutIntrVector` pass gate-width/type flags (0, 1, 2 or 3) to
`SetProtectedModeInterrupt`; `dxmain.asm:DFSetIntrVector` also describes
installing its real-mode reflector in the IVT. Those source paths do not show
the calls publishing flag 4 or the NT4 `DosxRmReflector` value. This supports
the relevance of the IVT path without proving that hooked NT4 behavior is
unreachable in every guest or lifecycle.

The current unprefixed IOPL-3 candidate now follows the ordering in NT4
`V86Intnn`: a single dword IVT read precedes three word writes at the wrapped
final SP. With that exact implementation, the rebuilt CCPU boundary fixture
passed 116 cases and the selected DPMI fixture passed. In the controlled
page-fault cases, the fixture's handler repairs the mapping and retries; this
does not establish what the installed DPMI handler does in the same situation.
The earlier full-package results above predate this implementation and do not
validate it for release.

NT4 `VdmDispatchPageFault` reports a qualifying V86 memory access below 1 MiB
to its VDM monitor as `VdmMemAccess`, rather than unconditionally routing it
through a protected-mode DPMI fault handler. The current CCPU path invokes
`host_exint_hook` when PE is set, including V86, and the installed
`DpmiFaultHandler` selects its V86 reflection branch by testing PE rather than
VM. The diagnostic with no installed DPMI fault vector exposes a failure but
does not reproduce a configured DOSX guest. This difference requires an
end-to-end owner and fault-delivery comparison; a synthetic handler that fixes
the mapping is not evidence that the production path is equivalent.

The original `EventVdmMemAccess` calls `DispatchPageFault` in
`softpc.new/host/src/nt_msscs.c`, which searches registered VDD memory hooks
and raises a noncontinuable access violation if none matches. That event is
part of NT's host-address protection mechanism; it must not be assumed to be
identical to an emulated guest page-table #PF raised by `spr_write_word`.
The checked-in `DispatchPageFault` is still present, but this source reading
alone does not establish that the CCPU access path invokes it for the same
conditions or that it supplies the same address representation.

Additional original-path finding: a fault while the NT4 fast trap is in
`PcVdmAlert` is recovered by `Kt0eVdmAlert` and `V86TrapDRecovery`; the latter
re-enters `KtdV86Slow` and `_Ki386DispatchOpcodeV86`. If that slow dispatcher
returns failure, `KtdV86Slow2` asks `_Ki386VdmReflectException` to deliver
#GP. This is separate from both the general `VdmDispatchPageFault` route and
the current candidate's direct `spr_*` guest-paging exception. In particular,
the current synthetic callback retry does not demonstrate this fast-to-slow
transition. These are distinct fault origins, not justification to turn all
access failures into #GP; the slow dispatch and its own exception handling
must be traced before changing the candidate.

The slow path is materially different even before failure: NT4
`OpcodeINTnnV86` updates the saved flags, performs three individually wrapped
16-bit stack writes, commits SP, and only then reads the vector (or uses the
registered protected-mode reflector). Its trap exception handler
`Ktd_ExceptionHandler` goes through `Kt0d103`/`Kt0d105`, which dispatches
`STATUS_ACCESS_VIOLATION` for a user-mode fault. A failure returned by the
opcode dispatcher instead takes the explicit #GP reflection branch. Thus an
access exception and a dispatcher-returned failure are not interchangeable;
partial writes and updated saved flags also cannot be described as an atomic
rollback. This source evidence establishes why a blanket #GP fallback or a
generic guest #PF injection would be an unjustified replacement. It does not
yet establish how host page protection and emulated guest PTE faults correspond
in this project's CCPU process.

The selected-object owner fixture now separately checks the rejected hooked,
virtual-IF and non-V86 branches: each preserves registers and flags and none
performs an IVT read or stack write. The ordinary admitted case performs one
IVT dword read and three stack word writes. The rebuilt fixture passed
([output](s11-v86-origin-evidence/selected-owner-no-access.txt)). This checks
ownership at the adapter boundary only; it does not resolve the fast-to-slow
fault transition or exercise an installed DOSX fault handler.
The same selected object was subsequently checked with all three excluded
V86 IOPL values (0, 1 and 2), including unchanged state and no additional
memory accesses ([output](s11-v86-origin-evidence/selected-owner-all-iopl.txt)).

A separate test of the selected production DPMI object calls its actual fault
handler with PE and VM set and no installed fault vector. It returns FALSE but
has already cleared IF and TF while retaining CS, IP and SP
([output](s11-v86-origin-evidence/selected-uninstalled-fault.txt)). The test
uses mocked CPU registers, not a configured DOSX guest, and does not establish
the installed-vector case. The first build of this fixture failed because an
unnecessary `DpmiEmulateInstruction` stub duplicated the selected object;
removing that stub made the relink and test pass.

With a present vector and mocked CPU registers, the same selected handler
returns TRUE, constructs a 16-bit frame on the configured locked PM stack,
and leaves the saved VM flag set ([output](s11-v86-origin-evidence/selected-installed-fault.txt)).
This is a direct function-level observation, not proof that the CCPU can enter
that protected handler correctly while VM is set or that a DOSX guest installs
the same vector. The initial test exposed a fixture ABI error: its
`LockedPMStackCount` was declared USHORT even though `dpmi32p.h` declares
ULONG; correcting the fixture eliminated the test's false locked-stack state.
The two test-only fault-report replacements also now use the production
signatures and assert that both reports were reached. The selected-object
test passed after this ABI correction; these witnesses do not provide an
original exception dispatcher.

An additional real-CCPU diagnostic installs a synthetic fault vector and
enters the selected DPMI handler after an emulated stack page fault. It
reaches the handler with VM set and then exits with host access violation
before the handler returns ([receipt](s11-v86-origin-evidence/real-installed-fault-diagnostic.txt)).
The initial setup did not establish a valid DOSX locked-stack mapping, and the
fixture uses a force-linked host boundary. The subsequent exception record
located its failed write at `0xFDE`, consistent with a NULL SIM32 base plus
the frame offset. Installing a present LDT in a separately mapped page makes
the same real CCPU/handler dispatch return TRUE; the observed registers have
VM set, CS 0x1f, IP 0x555, and SP 0xfd0. The fixture deliberately stops before
executing that handler. Case 133 now joins the 117-case passing boundary
suite ([corrected output](s11-v86-origin-evidence/real-installed-fault-corrected.txt)).
It is not an installed DOSX guest test and does not prove that execution after
the dispatch, or the original fast-to-slow fault transition, is correct.

Case 134 lets that synthetic dispatch execute one target instruction. The
fixture places distinguishable instructions at the V86 real-mode address and
the protected descriptor-base address; CCPU executes the V86 address and
retains VM ([receipt](s11-v86-origin-evidence/real-installed-execution.txt)).
This demonstrates an execution distinction in the constructed environment,
not that DOSX actually installs this vector or that NT4 would deliver the
same fault to it. Case 134 is a diagnostic, not a regression assertion that
the observed execution is the desired final semantic contract.

The original WOW build of DOSX does publish fault vectors: under `ifdef WOW`,
`dpmi/dxboot.asm:iidt25` loops over all 32 vector numbers, issuing the
`SetFaultHandler` BOP with a protected selector and successive
`PMFaultEntryVector` entries. `dpmi32/dpmiint.c:DpmiSetFaultHandler` stores
the supplied selector and offset in `DpmiFaultHandlers`. This includes vector
14 and establishes an original source route to a present vector for an
initialized WOW DOSX. It does not establish that the test's synthetic V86
paging configuration occurs in an installed guest, nor that NT4 forwards a
monitor-internal memory fault to this protected vector.

There is a further execution-profile distinction: the selected CPU40
`dpmi32/modesw.c:switch_to_real_mode` first calls
`mvdm_softpc_wow_page_domain_leave_protected`, which disables the WOW page
domain's CR0.PG when active, and then clears CR0.PE before loading the
real-mode segment caches. Thus this particular DOSX real-mode BOP is not
evidence that the standard WOW transition enters the synthetic PE=1,
VM=1, PG=1 state in cases 133/134. The converse also does not follow: a guest
VMM may establish V86 independently, and no claim is made that the synthetic
state is unreachable in a full Win386 system. The source finding narrows the
WOW interpretation of the test; it does not remove the monitor-internal
failure-handling obligation.

A retained earlier WOW mode trace provides a bounded runtime check of that
source reading: its 40 `bop-fd-real` samples all report CR0 `00000010`
(PE=0 and PG=0). The [extracted lines](s11-v86-origin-evidence/historical-wow-real-mode-transitions.txt)
come from `build/t473-s11/dr-context-study/remote-domain-reason.log`, SHA-256
`8273927855497AC106E841CCB4DD0264C8461AF5D877C2A90F3AB76B02E360D1`.
The historical runner names a WINMINE target and enables the DPMI-mode report,
but that path was reused and the exact original package/runner identity for
the retained log is not established. These samples do not cover all guest
mode changes, establish EFLAGS.VM, or validate the present candidate.

The NT4 fast handler's `KeI386VdmIoplAllowed` branch is a kernel execution
policy, not itself the guest's saved two-bit IOPL value. In
`base/ntos/vdm/x86/strtexec.c` the kernel separately chooses VIF, the shared
virtual-interrupt state, or direct IF handling according to system features,
this policy, and V86 mode. Consequently the candidate's `(EFLAGS & 0x3000)
== 0x3000` admission test is an explicit selected-CCPU limitation, not a
source-proven equivalent of that NT4 policy. Existing tests establish that
the candidate does not intercept IOPL 0/1/2; they do not establish that their
fallback provides the original V86 interrupt semantics. The corrected
117-case suite has been rerun after adding the execution diagnostic, and
still does not certify those excluded modes.

An original NT4 source boundary is now explicit: `instemul.asm:PushException`
checks VM first. For V86 it refuses to reflect vectors above 7; vectors 0–7
use the V86 stack and IVT. The protected-mode DPMI locked-stack frame is
constructed only in the non-V86 branch. Therefore a successful invocation of
the current DPMI handler for synthetic V86 vector 14 is an observation of a
different path, not a compatibility pass. Case 133 was removed from the
passing CCPU suite, and the equivalent installed-vector assertion was removed
from the selected-object gate; both retained diagnostic results remain above.
This does not establish that a monitor-internal host fault equals a guest
page fault or identify the correct owner of each. A valid implementation must
distinguish these origins rather than route them all to vector 14 or #GP.

The working candidate now implements the independently source-proven first
boundary as `MVDM-HOST-DIV-484`: when VM is set and the vector exceeds 7,
`DpmiFaultHandler` returns FALSE before changing flags or selecting a PM
handler. The selected-object test checks vector 14 with and without an
installed handler, and a protected-mode negative control; all pass. The
rebuilt CCPU boundary suite passed 116 cases. Returning FALSE lets the
existing CCPU exception machinery continue; these tests do not prove that
its final delivery is equivalent to NT4's VDM host-memory fault path. V86
vectors 0–7, actual DOSX/VMM paging and fast-to-slow recovery remain open.
The selected-object test also checks vectors 8 and 31 and verifies that no
stack mapping or target dispatch occurs in the declined V86 paths. It does
not classify the existing vector-7 behavior as an original-semantics pass.

A separate opt-in selected-object baseline asserts source-defined NT4 V86
vector-7 reflection through the IVT and 16-bit stack with no PM vector
installed. The current candidate exits 1 at `DpmiFaultHandler(7, 0)` because
the handler returns FALSE. The fixture uses mocked CPU registers and memory;
this is a controlled implementation mismatch, not a live guest reproduction.
The assertion stays outside the passing suite until the reflection has
correct virtual-IF, mapping and failure semantics.

The unpaged IOPL-3 portion now has candidate `MVDM-HOST-DIV-485`: it reuses
the original `ReflectV86Int` stack/IVT function, then clears TF and applies
the registered interrupt-gate IF rule. The former vector-7 mismatch is now a
selected-object check over all eight vectors and both gate flags; it passes.
A real CCPU divide-by-zero instruction also reached its IVT target and left
the expected 16-bit fault frame; the expanded 117-case CCPU suite passed.
These are scoped results: unpaged CCPU IOPL-3 does not prove every NT4 kernel
IF-virtualization mode, safe host mapping failure, paged V86 access, or the
actual immutable guest path. Those remain open; the candidate is unpublished.
The selected-object vector-7 check invokes the fault handler directly.
`ZFRSRVD`'s own prologue has an `INTx(7)` call, but actual CCPU instruction
dispatch first calls `Int7()` when EM or TS is set, before reaching that
prologue. A real CCPU x87 instruction with EM set now exercises that earlier
exception hook and reaches IVT vector 7 with the original instruction IP in
the frame. This corrects the earlier inference from direct `ZFRSRVD` invocation;
the passing case is limited to unpaged IOPL-3 and is not a guest-media test.

The real CCPU divide control now also starts at SP=4 with an interrupt gate.
It observes individually wrapped stack words at offsets FFFE, 0000 and 0002,
then reaches the IVT target with IF clear. The trap-gate control keeps IF set.
Both and the x87 control are included in the passing 119-case boundary suite; neither exercises
paging, virtual IF or host-access failure.

The selected-object gate now explicitly checks that paging enabled or IOPL 2
does not enter the new low-vector frame/714h update path. Those cases retain
the pre-existing fallback; the test does not assert that fallback is correct.
Original `inc/vint.h` defines the 714h virtual-IF bit and shows that the
software-CPU `MIPS_BIT_MASK` makes its FCLI/FSTI macros execute CLI/STI;
`nt_msscs.c` sets that bit for CPU40. This supports treating guest CPU IF as
an actual execution input but does not by itself map every NT4 VIF/IOPL
policy to CCPU or prove all guest observers stay synchronized.

The current candidate was built through all eight cached build groups and
sealed into one ten-component package. Every staged input hash matched its
respective build producer; all ten sealed component hashes still matched
after testing. Its Full run passed all 16 gates, and isolated Winmine, SOL,
WRITE and four SOL Options rounds passed. Win3.1 standard/enhanced mode with
both DOSONLY values passed startup, mode and normal-exit checks; 86 selected
guest binary hashes matched in both the original and isolated trees. See the
[package manifest](s11-v86-origin-evidence/low-fault-package/package-manifest.json),
[full timings](s11-v86-origin-evidence/low-fault-package/full-timings.json),
[application runs](s11-v86-origin-evidence/low-fault-package/game-runs.json),
[Win3.1 matrix](s11-v86-origin-evidence/low-fault-package/win31-matrix.json),
[guest hashes](s11-v86-origin-evidence/low-fault-package/guest-integrity.json)
and [post-test component hashes](s11-v86-origin-evidence/low-fault-package/post-test-hashes.json).
These package tests do not prove the unresolved V86 paging, host-access-fault
and virtual-IF semantics; no default publication is claimed on this evidence.

The DIV-484 negative control now also installs a protected-mode vector 14 and
checks the selected original DPMI object's 16-bit locked-stack frame, saved
registers, error code and fault-report calls. It passes
([result](s11-v86-origin-evidence/protected-fault-control.txt),
[input hashes](s11-v86-origin-evidence/protected-fault-control-inputs.csv)).
This is an object-level test with mocked CPU registers; it does not prove
all protected exception return, nested fault or guest lifecycle paths.

After the package run, a direct CCPU interrupt-gate control initialized the
714h virtual-IF bit and demonstrated that DIV-485 cleared CPU IF but left the
shared bit set. The candidate now calls a shared, ICA-locked atomic IF
helper, also used by the existing protected CLI/STI monitor path. The
selected-object tests verify all 0–7 vectors with trap and interrupt gates;
the real CCPU test verifies that an interrupt gate clears the IF bit while
preserving unrelated fixed-state bits. The rebuilt 120-case CCPU suite and
selected-object gate passed. The Full, game and Win3.1 results immediately
above predate this change and do not validate the current bytes. Paged V86,
non-direct IF and host-access-fault behavior remain open.

The real CCPU fixed-state control additionally tests a trap gate with the IF
bit and other bits initially set: it preserves `0xa200` and CPU IF, while the
interrupt-gate control produces `0xa000` with CPU IF clear. Both controls and
the existing protected CLI/STI cases pass in the 121-case CCPU suite. This
changes test code only and does not establish virtualized-IF or paged behavior.

The real CCPU and selected-object tests were repeated with both low
pending-interrupt bits set in the 714h state. The interrupt gate changes
`0xa203` to `0xa003`; the trap gate retains `0xa203`. Both preserve those
bits, and the 121-case suite and selected-object gate passed. This establishes
stored-bit preservation, not every concurrent IRQ-delivery interleaving.

The IF-sync candidate was subsequently rebuilt, sealed and tested as a new
ten-component package. Its Full run passed 16/16 gates, and Winmine, SOL,
WRITE and four SOL Options rounds passed. All four Win3.1 mode/DOSONLY
combinations passed, with the 86 selected guest binary hashes unchanged in
both original and isolated trees. Source hashes recorded at test start and
all ten sealed component hashes were unchanged afterwards. Evidence:
[manifest](s11-v86-origin-evidence/if-sync-package/package-manifest.json),
[Full](s11-v86-origin-evidence/if-sync-package/full-timings.json),
[applications](s11-v86-origin-evidence/if-sync-package/game-runs.json),
[Win3.1 matrix](s11-v86-origin-evidence/if-sync-package/win31-matrix.json),
[guest hashes](s11-v86-origin-evidence/if-sync-package/guest-integrity.json),
and [component hashes](s11-v86-origin-evidence/if-sync-package/post-test-hashes.json).
These are product regression results, not a proof of the still-open V86
failure and virtualization semantics; this package is not published.

After that run, the new DPMI V86 branches were narrowed at compile time to
`CPU_40_STYLE && CCPU`: the shared IF helper exists only in the CCPU host
profile. The selected worker command defines both symbols. A separate
CPU40/non-CCPU compilation of the same DPMI source succeeded and its object
contains neither the new V86 helper nor the shared-IF reference; the selected
CCPU object fixture and 121-case boundary suite also passed. This compile-only
negative control does not certify any complete alternate runtime. The new
worker input hash differs from the sealed IF-sync package's input hash, so
the earlier Full/package results cannot be attributed to this latest source.
The non-CCPU compile and symbol check now run inside the existing selected
DPMI fixture rather than only as a one-off build script; the updated gate
passed together with the CCPU object controls.
An additional PE-section comparison found the selected worker's `.text`,
`.data`, detour sections and `.reloc` byte-identical to the tested sealed
image; `.rdata` and `.rsrc` differ. See
[section hashes](s11-v86-origin-evidence/profile-vs-tested-sections.txt).
The comparison is between a sealed image and an unsealed build input and does
not establish full-image equivalence or replace a same-package Full run.

Re-sealing all ten current producer images with the previously tested
package ID produced nine exact hash matches. NTVDM differs in four bytes:
two low bytes of the PE COFF timestamp and two low bytes of the
`IMAGE_DEBUG_DIRECTORY.TimeDateStamp`; both timestamp fields contain the
same old/new timestamp values. All NTVDM section bytes other than that debug
timestamp match, including `.text`, `.data`, `.reloc` and `.rsrc`. See the
[component comparison](s11-v86-origin-evidence/same-id-package-comparison.json)
and [exact differing ranges](s11-v86-origin-evidence/same-id-ntvdm-difference.txt).
This establishes a narrow timestamp-only relationship to the tested image,
not identical full hashes, complete V86 semantics or new publication.
