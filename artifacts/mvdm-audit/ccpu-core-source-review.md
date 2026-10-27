# Selected CCPU foundations: source and NEG64 witness

This S10 checkpoint concerns the original CPU40/SPC386 implementation, not a
new CPU profile. The [nine source identities](ccpu-neg64/source-identities.json)
match both frozen original editions and the current mirror. No production,
guest, library, build graph, release or ledger bytes change. Original source
text retention is 100 percent; this is not a semantic restoration percentage.
All nine roles remain pending until their required evidence is reconciled.

## Source review and finite outstanding evidence

Paths below are relative to `src/mvdm/softpc.new/base/ccpu386/`. These conclusions
come from the original function bodies and callers, rather than basename or
link selection alone. None is a newly established runtime defect.

| File | Retained original responsibility | Evidence still required for functional disposition |
| --- | --- | --- |
| `c_addr.c` | `d_mem`, address wrapping, ModRM/SIB default segments, POP displacement and segment-limit checks | Decoder must bypass memory decoding for register operands; real guest address-size/segment/POP-ESP combinations and page/segment fault order. |
| `c_bsic.c` | Descriptor decoding, selector checks, FLAGS/EFLAGS privilege rules and cached undefined-shift-OF policy | CPL/IOPL/PE/VM combinations, descriptor page-boundary failure and cold versus cached environment policy. `/P` alone cannot prove them. |
| `c_intr.c` | Real IVT entry and protected interrupt/trap/task gates, privilege stack transition, V86 extra frame and flag changes | IRQ versus software INT, 16/32-bit gates, nested faults, task transition and normal/failed return with original state recovery. |
| `c_neg64.c` | Two-word complement with carry; actual original object now exercised below | Guest signed multiplication/division callers, decoder reachability and arithmetic exception order remain separate. No null or aliased-pointer contract is inferred. |
| `c_prot.c` | SS access/type/privilege/presence checks, 16/32-bit TSS stack lookup, data-segment invalidation and TSS validation | Valid and invalid selectors, exception precedence, mixed TSS/stack sizes and failure before partial state commit. |
| `c_stack.c` | SP/BP wrapping, push/pop/temporary-pop and old/new stack limit/page validation | Caller prevalidation, exact read/write/fault ordering, expand-down limits and supervisor/user page checks. `spush16` with 32-bit operand spacing intentionally writes only the low word; do not replace it with zero fill. |
| `c_neg64.h` | Original typed two-pointer declaration | Native compatible declaration and deliberately incompatible declaration are checked below; guest caller contracts remain separate. |
| `c_prot.h` | Original protected-mode helper declarations | Actual caller/provider ABI and the corresponding protected-state tests above. |
| `c_stack.h` | Original stack helper declarations | Actual caller/provider ABI and the corresponding stack/fault tests above. |

Stack helpers are not an isolated exception engine. In particular `spush`
updates SP before its write, whereas `spop` reads before advancing SP. Correct
exception behavior depends on the original caller's validation and recovery.
A fixture with invented CPU state would not establish that boundary.

## Executed NEG64 evidence

The independently authored [native fixture](../../tests/mvdm-host/ccpu_neg64_test.c)
links the actual cached `obj/ccpu/c_neg64.obj`, without source copying, CPU-state
stubs, object rewriting or recompiled original code. The
[runner](../../tests/observation/verify-ccpu-neg64.ps1) obtains the actual production
CCPU flags and original headers. Only the existing duplicate `NTVDM` macro
warning is suppressed; other warnings are errors.

The [result](ccpu-neg64/result.json) passes 631,410 cases, each checked against
unsigned modulo-64 arithmetic and checked again after a second negation.
Boundary combinations, a 65,536-element carry sweep, deterministic random
pairs and both output-pointer storage orders are covered. Guard words are
checked after **each** call, so a second call cannot hide the first overwrite.
The contract uses distinct valid aligned `IS32` words, with no CPU resources
or lifecycle state. It is broad sampled evidence, not exhaustive enumeration
of all 64-bit inputs.

The wrong-signature fixture produces the expected
[C4113 rejection](ccpu-neg64/wrong-abi-rejection.txt), and a deliberately wrong
oracle produces the expected [first-case failure](ccpu-neg64/wrong-oracle.stdout.txt).
The fixture passes on the final source revision, after adding the first-call
guard assertion; the earlier run is not substituted for that result.

The [image checker](../../tests/observation/verify_ccpu_neg64_image.py) reuses the
existing COFF/PE parser. Its [result](ccpu-neg64/image-result.json) proves that
the 32-byte, relocation-free original function body occurs exactly once in
both the tested native fixture and the current source-built candidate worker.
It also checks the selected original `mul64` and `div64` object bodies and
resolves all four emitted direct NEG64 calls to that same worker function.
No arbitrary relocation masking is used for NEG64; caller matching masks only
declared COFF relocations and then verifies each relevant call destination.
This does not prove guest decoder execution or qualify the candidate package.

## Reproduction and delivery limits

All compiler, linker, behavior and image checks ran as owned hidden background
processes with recorded PIDs, arguments and completion. The 60-second child
budgets are diagnostic bounds, not product timeouts. Hashes and logs are bound
by the [observation manifest](ccpu-neg64/observations.json). Cached worker,
original library and NEG64 object hashes are unchanged before/after the test.

```powershell
pwsh -NoProfile -File tests/observation/verify-ccpu-neg64.ps1 -BuildRoot <fresh-repo-build-directory>
python -B -X utf8 tests/observation/verify_ccpu_neg64_image.py --worker-build build/.ninja/msvc-x86-release/worker --image build/t473-s10/candidate-validation/package/system32/ntvdm.exe --fixture <test-output>/fixture.exe --output <result.json>
```

The first image invocation used the wrong package-relative path and failed
before inspecting an image. The corrected `system32/ntvdm.exe` invocation
passed. This was a test-launch error, not a runtime regression.

No mirror diff is introduced and no file is promoted to full on these native
results. S10 still owns the per-file reconciliation; the approved S11 CPU
stage owns the behavioral obligations above. The candidate SOL Options
failure remains unexplained and publication remains unqualified.
