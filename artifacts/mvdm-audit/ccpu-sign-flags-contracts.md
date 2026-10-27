# CCPU sign-extension and flag leaf contracts

This S10 batch evaluates eleven unchanged original leaf files: CBW, CWDE,
CWD, CDQ, LAHF, SAHF, STC, CLC, CMC, STD and CLD. Their exact source identities
and two isolated guest runs are retained in [guest evidence](ccpu-sign-flags-guest.json).
The test is an independently authored COM program, not a modification or
rebuild of an original guest. It ran against package-c NTVDM SHA-256
`01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`.

## Original ownership and emitted code

`c_main.c` explicitly calls CBW/CWDE at opcode 98, CWD/CDQ at 99, LAHF at 9F,
SAHF at 9E, STC at F9, CLC at F8, CMC at F5, STD at FD and CLD at FC.
The size-dependent pairs select USE16/USE32 before entering the leaf. These
files contain only their named leaf bodies and ordinary declaration includes.
Their complete state writes remain the original ones; no adapter replaces them.

The [image correspondence](ccpu-sign-flags-image.json) compares actual selected
COFF bodies with the tested published PE. Cached-map addresses alone are not
valid: the cached worker differs from the tested package. Two unique emitted
bodies identify the published register/flag relocation targets; every function
then matches with all absolute relocations resolved. Decoder call-site windows
also match, and their actual E8 destinations reach the corresponding complete
body. CLC has two identical code copies, so its actual decoder call identifies
the applicable target rather than claiming a unique masked-body match.
The [wrong-image control](ccpu-sign-flags-image-negative.json) rejects WOW32.DLL,
an x86 image without these selected CPU bodies.

Reproducer: `tests/observation/verify_ccpu_sign_flag_image.py --image
build/package-uuid/package-c/system32/ntvdm.exe --worker-build
build/.ninja/msvc-x86-release/worker --output <build-owned-result.json>`.
The checker is read-only with respect to both binaries and objects. It never
patches code or assumes modern private object layouts.

## Actual guest behavior

`tests/observation/verify-ccpu-sign-flags-guest.ps1` reuses the proven modern
pwsh, medium-token, private-desktop observer and short physical staging helper.
Its two retained runs record exact host, parent PID, script, assembler, COM,
observer, helper and runtime hashes. Both stages were cleaned by the existing
owned helper; no shared product process or original guest was changed.

- All 65,536 AX values run twice, with complementary upper-word sentinels,
  through CWDE, CBW, CWD and CDQ: 524,288 checks. This exhausts CWDE's AX input,
  CBW's AL and old AH combinations, CWD's AX input, both CDQ sign classes,
  upper-word alias preservation and the arithmetic/DF flags captured immediately
  after each instruction. Eight additional CDQ boundaries include zero,
  7FFFFFFF, 80000000 and FFFFFFFF.
- All 64 arithmetic-flag combinations and all 256 AH byte values run through
  LAHF, SAHF, STC, CLC, CMC, STD and CLD: 114,688 instruction checks. DF starts
  both clear and set. LAHF must preserve AL/upper EAX and supply its fixed AH
  bits; SAHF ignores AH bits 1/3/5 and preserves OF/DF. The five single-flag
  operations must preserve EAX and all checked non-target flags.
- STD plus backward REP MOVSB copies sixteen bytes in reverse-address order,
  checks final SI/DI and compares destination bytes after CLD. Both the success
  and failure paths clear DF before DOS calls.
- The normal run prints every required marker and exits zero. A separate
  compile-time negative oracle flips the first expected CWDE result, prints
  failure phase A, emits no success marker and exits one. The runner requires
  that exact negative outcome; a missing or blank report is not a pass.

Reproducer: run the guest script with `-PackageRoot`, `-Observer`,
`-LogonHelper` and a fresh `-BuildRoot` under the declared S10 temporary root;
add `-NegativeOracle` for the failure-control run. Exact paths and commands
are in the retained records. The legacy E30 PIF producer supplies only the
isolated test launch profile; these CPU assertions do not depend on EMS.

## Classification boundary

Independent shard review accepted the [eleven full classifications](full-selected-leaf.csv)
for each complete, selected leaf source
contract, not the whole ISA or CPU executor. The sign-extension bodies depend
only on the indicated sign bit/input alias; the flag bodies read/write only
canonical architectural flag slots. The finite input partitions, original
source, emitted bodies, direct decoder targets and real register/flag aliases
are covered together. These functions own no allocation, callback, cancellation
or independent lifetime requiring an invented failure/reentry test.

Protected-mode default-size decoding, invalid/LOCK/repeated-prefix behavior,
privilege checks, interrupts, exception delivery and other CPU profiles remain
with their actual decoder/execution owners. They are not declared passed by
this real-mode/66h witness. Normal COM completion proves this workload's return,
not idle worker/service retirement after helper cleanup. Full leaf ownership
must not be used to close those broader outstanding source rows or the T goal.

The [checkpoint verifier](ccpu-sign-flags-checkpoint.py) and its
[result](ccpu-sign-flags-checkpoint-result.json) check all 4,684 paths, frozen
identity, disjoint ownership, the eleven pending-to-full transitions and the
current source/object/test/image hashes. They also check that the helper-owned
stages are absent and all ten unchanged package images still match release
assets and the installed system32 files. The
[background record](ccpu-sign-flags-checkpoint-runner.json) records successful
documentation and whitespace checks. These checks supplement the actual guest
and image tests above; they are not a full-product regression or T closure.
