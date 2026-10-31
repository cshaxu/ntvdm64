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
