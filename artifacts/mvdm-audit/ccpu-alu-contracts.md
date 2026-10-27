# Original ALU source contracts

This S10 group covers the complete unchanged ADD, ADC, AND, CMP, DEC, INC,
NEG, OR, SBB, SUB, TEST and XOR source bodies. It combines original source
and caller tracing with independently produced native x86 reference data,
actual packaged guest execution and published binary correspondence. This
record does not close the broader decoder, memory/fault or CPU lifecycle owners.

## Actual caller and provider

The original `_ccpu` interpreter assigns arithmetic helpers to local typed
function pointers (`instp32328`, `instp328`, `inst32328`) and invokes them in
shared TYPE operand-processing blocks; some CMP/TEST cases use direct calls.
Operand reads normalize 8/16/32-bit values before invoking these bodies.
Writeback, addressing, prefixes and exception delivery belong to the decoder,
not these pure arithmetic providers. CMP's CMPS/SCAS comments describe reuse
of its one comparison body, not additional independently implemented functions.

The [image record](ccpu-alu-image.json) matches each complete selected COFF body
to the tested published image, resolving its actual absolute flag/parity
references. It additionally matches all non-relocation bytes of the 55,972-byte
`_ccpu` function and checks every reference to these twelve providers against
the matched target address: absolute function-pointer assignments and relative
transfers are distinct. The 6,531 decoder relocations are not all interpreted
as calls or declared semantically verified. Only this group's bindings are
resolved by this check. The [wrong-image control](ccpu-alu-image-negative.json)
rejects an unrelated x86 WOW32 image.

## Native reference and real guest evidence

The optional x86 CMake target `ccpu-alu-oracle` compiles the independent
`tests/observation/ccpu_alu_oracle.c`. Native MSVC x86 instructions generate
results and immediately captured flags. BT supplies input carry immediately
before the operation; INC/DEC preserve it. Logical AF is architecturally
undefined, while the original four logical providers explicitly clear it;
the reference records therefore require AF=0 for those original contracts.
No MVDM instruction implementation is used to generate expected results.

Each reference record is fourteen little-endian bytes; the
[corpus manifest](ccpu-alu-records.json) records their file hashes and counts:
two normalized operands, result, and a 16-bit flag mask. All 65,536 ordered
8-bit pairs are generated for each variant. The 16/32-bit cases contain all
196 ordered pairs of fourteen width-normalized boundary values, including
FFFFFFFF, plus
1,000 deterministic LCG pairs. ADC/SBB/INC/DEC each cover input CF=0 and CF=1.
The 48 files contain 1,086,848 records; the wider input space is sampled.

`tests/observation/ccpu_alu_probe.asm` executes every record through three
actual guest forms: register destination, memory destination, and memory
source. Unary operations instead use an unaligned memory destination for the
third form. Upper register/memory sentinels, adjacent memory guards, source
preservation, result and all six arithmetic flags are checked. CMP/TEST must
preserve their destination. File sizes, record boundaries, EOF and actual
phase/record/check counters are asserted, with failure phase/form/index output.

The [guest record](ccpu-alu-guest.json) retains a normal completion after
3,260,544 instruction checks and a separate negative run. The latter flips
one expected-result bit in its own fresh ADD8 reference data, fails at phase
01/form 01/index 0000, prints no success marker and propagates guest exit one.
It changes no original guest or product code. Both runs used the established
medium-token modern pwsh/private-desktop observer and helper-owned short
physical stage. Their exact inputs, runner context, native producer, assembler,
observer and package hashes are recorded.

Reproduce with `tests/observation/verify-ccpu-alu-guest.ps1`, passing the existing
package, observer, logon helper, compiled `-Oracle` and a fresh build-owned
`-BuildRoot`; add `-NegativeOracle` for the deliberate reference error.
`tests/observation/verify_ccpu_alu_image.py` takes `--image`, `--worker-build`
and `--output`. Neither checker changes the image or loaded runtime.

## Classification and remaining boundaries

Independent shard review accepts full classification for each complete selected leaf contract:
its normalized inputs, width-dependent result/flag algorithm, pointer
writeback or no-write comparison behavior. The byte-wide space is exhaustive;
wider arithmetic uses source reasoning, boundary and sampled actual execution.
The functions own no allocation, callback or independent resource lifetime.

This does not establish protected-mode default-size decoding, illegal/LOCK/
repeat prefixes, fault/interrupt delivery, all possible memory addresses,
string instruction sequencing or CPU/service teardown. Those remain with
their original owners and open audit rows. No production source, guest or
release image changed. The native producer is optional test infrastructure;
it is excluded from the default product build and release package.

Boundary values can coincide after width normalization; the stated wider
counts are executed records, not a claim of 196 distinct pairs at every width.

The [checkpoint verifier](ccpu-alu-checkpoint.py),
[result](ccpu-alu-checkpoint-result.json) and
[background record](ccpu-alu-checkpoint-runner.json) cover the 4,684-path
partition, source hashes, twelve new full transitions, complete 23-file full
index, native record normalization/known vectors, real guest/image identities,
owned-stage cleanup and unchanged ten-image release/installed hashes.
The producer used the existing stable CMake/Ninja tree and repository-routed
ccache; a second build reported no work. Documentation and whitespace checks
passed. This is not full-product regression or S10/T closure.
