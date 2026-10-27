# Selected CCPU leaf declaration contracts

The 23 full original instruction leaf sources have 23 corresponding headers.
Each complete header contains only its named typed IMPORT declaration, with
no additional interface or algorithm silently omitted. The disjoint reviews
record exact selected hashes, signatures, original types and actual compiler
consumers:

- [Seven shard-0 headers](shard-0/header-abi-closure.csv),
  [method](shard-0/header-abi-closure.md).
- [Eight shard-1 headers](shard-1/header-abi-closure.csv),
  [method](shard-1/header-abi-closure.md).
- [Four shard-2 headers](shard-2/header-abi-closure.csv),
  [method](shard-2/header-abi-closure.md).
- [Four shard-3 headers](shard-3/header-abi-closure.csv),
  [method](shard-3/header-abi-closure.md).

The selected ANSI expansion is `IMPORT` to extern and IPT0/2/3 to typed
prototypes. IU32 is four bytes, IUM8 one byte, and the selected x86 pointer
four bytes. The actual CCPU compile flags use default cdecl. Existing real
guest and emitted-code/call-binding evidence proves those declarations reach
the corresponding original providers; mere source inclusion is not that
behavioral proof. Extra consumers, such as a conditionally disabled CMPXCHG
body including CMP's declaration, do not gain runtime coverage here.

`tests/mvdm-host/ccpu_leaf_declarations.c` includes the actual original headers
and types, asserts widths and initializes four explicitly typed function-pointer
groups without casts: no-argument, binary write, binary read and unary write.
`tests/observation/verify-ccpu-leaf-declarations.ps1` uses the actual selected
worker flags and existing x86 compiler driver. Normal syntax compilation
passes; the negative ADD first-argument mismatch is specifically rejected as
C4113 under /WX. The inherited NTVDM macro redefinition C4005 is the only
suppressed warning; no calling-convention or type diagnostic is suppressed.
The fixture and runner replace no product declarations, implementation or ABI.

The header files are full/100 for their complete selected declaration
contracts. They are counted separately from the 23 executable leaf files:
this does not claim 46 instructions or new execution coverage. Other compiler
profiles, protected default-size decoding, invalid prefixes, faults,
interrupts and lifecycle remain with their actual open owners. No product or
guest source changed or required republication in this checkpoint.

The [compile evidence](ccpu-leaf-header-abi-result.json) retains original
include paths, selected graph/source/driver/runner hashes and both compiler
results. The [checkpoint verifier](ccpu-leaf-header-checkpoint.py),
[result](ccpu-leaf-header-checkpoint-result.json) and
[background record](ccpu-leaf-header-checkpoint-runner.json) validate all
4,684 paths, disjoint partitions, original identities, the 23 new declaration
classifications, full-leaf/declaration provider pairing and compile evidence.
Documentation governance and whitespace verification passed. S10 still has
2,948 pending rows and the separate external-provider relevance audit; these
declaration results cannot close either obligation.
