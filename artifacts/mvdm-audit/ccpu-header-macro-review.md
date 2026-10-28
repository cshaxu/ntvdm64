# CCPU selected headers and operand macros

## Question and inputs

This S10 checkpoint follows `70985c440`. It reconciles the 22 remaining
CCPU headers against eight actual production preprocessing contexts, rather
than treating an include name or a successful compile as restored behavior.
The [per-file dispositions](ccpu-header-macro-role-dispositions.csv) bind the
original paths, hashes, retained text and remaining obligations. The
[observations](ccpu-header-selection-observations.json) retain selected tokens,
VALID dependency consumers and the expanded save/restore functions.

`tests/observation/observe-ccpu-header-selection.ps1` obtains the actual Ninja
commands and dependency records for c_div64, c_tlb, c_tsksw, c_xfer, c_xtrn,
c_main, c_reg and zfrsrvd. It changes only object emission to `/P /Fi`, using
the same compiler driver, definitions, forced includes and include order.
All eight preprocessing processes exited zero. Object, original CCPU archive
and linked-worker hashes were unchanged. This is preprocessing evidence, not
a new type-check, ABI proof or complete runtime acceptance.

## File conclusions and measurement

Seven headers emit declarations in these contexts: c_div64.h, c_tlb.h,
c_tsksw.h, c_xfer.h, c_xtrn.h, fpu_c.h and newnpx.h. c_oprnd.h and fault.h
provide expanded macros instead. These nine files become **partial**.
The original PIG-only comparison declarations in ccpupig.h emit no tokens
under the selected non-PIG profile; only that inactive historical test-header
role is **excluded**, consistently with the earlier PIG test-file decisions.
The generated `Pig` record constant is distinct from the `PIG` build flag.
General CPU diagnostics and guest instruction behavior are not excluded.
The other twelve headers remain pending; current nonselection alone is not
an exclusion for the six 486 declarations or the six model/schema headers.

All ten reconciled originals retain 100 percent original text. That number
does not estimate executable behavior. The restored-behavior percentage is
explicitly unmeasurable from declarations, expansion tokens and dependencies;
remaining ABI, fault, lifecycle and guest obligations are recorded per file.
The exact-delta checker records **1,668 pending, 417 partial, 52 full and
2,547 excluded**, preserving the frozen 4,684 paths and all unrelated rows.
No full classification, runtime image, mirror diff or guest byte is added.

## Demonstrated signed memory-bit-index difference

The original `c_oprnd.h` fetches a word register into unsigned operand storage;
its memory bit-operation adjustment right-shifts that value without sign
extension. For index FFFF the effective word displacement becomes +8190,
where x86 hardware selects the preceding word at -2.

The independent native [hardware oracle](../../tests/mvdm-host/ccpu_bit_memory_oracle.c)
uses operand-size 16 and address-size 32. Both possible locations are owned
storage. Positive index 15 succeeds. Index FFFF changes the preceding word,
leaves the far word zero, and returns carry one. The independently authored
[guest probe](../../tests/observation/ccpu_bit_memory_probe.asm) uses the same
operand/address sizes and owned locations. On both the accepted package and
the sealed candidate, positive control succeeds but the negative case reports
`near=8000 far=8000 carry=0` and exits one. This is a reproduced original-host
execution defect, not a test pass or a guest defect exemption.

The six-case recorded batch also reruns normal and deliberately wrong-oracle
Control and SignFlags cases. Those four regression contracts pass. Its two
expected diagnostic failures establish reproducibility; they do not establish
that BitMemory is repaired. S11 must correct the complete signed bit-string
addressing group, including register versus immediate indexes, word/dword,
address-size wrapping, read/write permissions, carry and fault commit order.
No instruction patch is selected by this source-audit checkpoint.

## x87 source inconsistency; runtime result inconclusive

Actual expanded `npx_frstor` and `npx_fsave` precheck spans are 94 bytes for
the 32-bit environment and 106 for the 16-bit environment, while transfers
use 108 and 94 respectively. The misleading NPX_ADDRESS_SIZE_32 name is set
from operand size by the original decoder. Source and selected expansion
prove a precheck/transfer inconsistency, but not its guest-visible outcome.

Exploratory DPMI startup attempts failed setup or exceeded their diagnostic
budget. A raw protected-mode enter/leave control passed, but a deliberate
out-of-limit access **without any FPU operation** exited FF too. Consequently
the probe's exception-delivery assumptions are unvalidated, and the full x87
probe's FF result cannot be attributed to x87. Neither DOSX failure nor an
x87 runtime fault is established. The [DPMI](ccpu-header-macros/dpmi-x87-probe-inconclusive.asm)
and [raw protected-mode](ccpu-header-macros/x87-direct-probe-inconclusive.asm)
sources are retained as inconclusive research, outside the regression runner.

S11 must first establish the original registered protected-mode fault route,
then test both operand sizes, exact-limit success, short-span rejection,
read/write direction, unchanged state on failure and normal return. FPU stack
initialization, OPERAND layout and reverse byte-copy helpers were examined;
none was established as the cause of these observations.

## Evidence and delivery boundary

`ccpu-header-macros/` retains process ownership, commands, input hashes,
preprocessor metadata, native-oracle output and bounded guest results. Full
preprocessed SDK bodies and generated executable test media are not imported.
The durable manifest binds captured bytes. The delivery verifier reruns the
ten-row ledger check, preserved-image checks and documentation gates.
Historical checkers belong to their matching ledger revision.

The [recovery worklist](recovery-worklist.md) owns the two finite S11 follow-ups.
This checkpoint does not qualify the separate runtime candidate, explain its
earlier SOL Options failures, publish binaries, close S10 or complete T473.
