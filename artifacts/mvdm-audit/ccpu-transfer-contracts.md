# NOT, XCHG and LEA selected original contracts

The original NOT leaf complements one IU32, XCHG swaps two IU32 values and
LEA copies the supplied effective-address value into the supplied output.
They do not calculate guest addresses, acquire resources, invoke callbacks or
alter flags. Their pointer arguments refer to the decoder's valid local
operand storage, not arbitrary modern host objects or live guest pointers.
The original decoder owns memory/address validation, normalization and writeback.

`c_main.c` calls NOT from F6/F7 extension 2, XCHG from 86/87 and 91--97, and
LEA from 8D after operand/address decoding. The selected files are unchanged
from their frozen original identities. The [image evidence](ccpu-transfer-image.json)
matches complete pure-function object bodies without masked internal fields,
then matches all non-relocation bytes of the selected 55,972-byte `_ccpu`
decoder and resolves every reference to the three named functions. Multiple
identical emitted bodies are not treated as a unique symbol location: the
actual decoder target must land on a matching complete body.

The independently authored `tests/observation/ccpu_transfer_probe.asm` has
[actual guest evidence](ccpu-transfer-guest.json) from unchanged package-c.
It runs all 65,536 low-word values twice with complementary upper sentinels:

- NOT: 8/16/32-bit register, aligned memory and unaligned memory, including
  unaffected upper bits, adjacent byte guards and unchanged arithmetic/DF flags.
- XCHG: 8/16/32-bit register pairs, aligned/unaligned memory, both values,
  unchanged source/upper bits/guards/flags and same-register alias forms.
- LEA: all four operand/address-size combinations, base/index/displacement,
  16-bit wrap and 32-bit scaled arithmetic, no dereference of the calculated
  out-of-range effective address, unchanged flags/base/index registers.

The probe computes expectations with the separately verified integer ALU
operations before restoring the tested flags; it does not call a host-replaced
MVDM provider. Its completed counter is 3,276,800 checks. A separately assembled
negative expectation fails at NOT phase 1, prints no success marker and returns
guest exit one. The normal run returns zero. Each run records its medium-token
modern pwsh host, private-desktop observer, exact input/runtime hashes and owned
short-stage cleanup. No original guest is modified or rebuilt.

Reproduce using `tests/observation/verify-ccpu-transfer-guest.ps1` with the
existing package, observer, logon helper and a fresh build-owned output root;
add `-NegativeOracle` for the wrong-expectation control. The image checker
`tests/observation/verify_ccpu_transfer_image.py` takes `--image`,
`--worker-build` and `--output` and only reads the inputs.

Independent shard review accepts full classification for each complete pure leaf contract
and its corresponding single typed declaration. Wider 32-bit input pairs are
not exhaustively enumerated, and these results do not close address decoding,
memory faults, XCHG atomicity, illegal/LOCK/repeat prefix behavior, protected
default-width modes, branch helpers or service lifecycle. The simple bitwise,
swap and copy algorithms have no independent allocation/failure state to test.

The separate [486 profile audit](shard-3/cpu486-profile-boundary.md) establishes
that current SPC386 selection compiles six guarded source files without their
486 definitions. Their declaration and implementation gates use different macro
names. This remains a finite source-recovery question, not an excluded capability
or permission to flip a macro without its register/reset/TLB dependencies.

The [checkpoint verifier](ccpu-transfer-checkpoint.py),
[result](ccpu-transfer-checkpoint-result.json) and
[background record](ccpu-transfer-checkpoint-runner.json) cover the 4,684-path
inventory, six new full transitions, 26 source/declaration provider pairs,
source/object/runner/image hashes, positive and negative guest completion,
[wrong-image rejection](ccpu-transfer-image-negative.json), owned-stage
removal and unchanged ten-image publication identities. Documentation and
whitespace checks passed. These are bounded contract results, not a complete
product regression, an ISA completeness claim or S10/T closure.
