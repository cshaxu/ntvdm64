# Original NT port routing and actual-object witness

This S10 audit/test checkpoint compares against `e94f3fb48`. It changes no
runtime, mirror, guest or release image. The [six dispositions](ios-owner-dispositions.csv)
partially verify the selected IOS source and declaration, and the CCPU IN/OUT
source/declaration pairs. None becomes full.

## Source ownership and profile

The original `softpc.new/base/support/ios.c` owns per-port read/write adapter
maps and per-adapter callbacks. Empty callbacks return all-one input values.
Byte registration supplies generic word/dword/string fallbacks; optional
callbacks replace their named slots. Words split into adjacent byte ports;
fallback dwords split into two words. Port arithmetic wraps at 16 bits.

Read and write mappings can have different owners. Original NT disconnect
semantics are deliberately whole-port: an adapter owning either side clears
both mappings, while a nonowner clears neither. A string fallback snapshots
its callback for that invocation; redefining the callback during the first
iteration affects the next invocation, not the remaining iterations.
These are original rules, not proposed independent-owner detach semantics.

Both the BIOS-facing `ind/outd` wrappers and the **selected NT non-SFELLOW**
CCPU `IN32/OUT32` helpers split into word operations. The direct dword callback
branch in the latter is guarded by `SFELLOW`, which the current graph does
not define. The callback arrays themselves can be tested directly; that
does not prove the selected CPU route uses them for dword instructions.
The first two exploratory fixture versions assumed direct dword dispatch
for these wrappers/helpers and failed two assertions each. Source review
corrected those expectations; no runtime was changed to satisfy the test.
This audit does not admit or revive SFELLOW.

BIOS-facing wrappers use the original CPU virtualisation envelope. CCPU
helpers avoid recursively entering that envelope; instruction validation
belongs to their actual decoder/caller. The controlled envelope in this
test proves only routing around that boundary, not guest privilege checks.

## Executable evidence

The [fixture](../../tests/mvdm-host/ios_object_contract_fixture.c) directly
includes original IOS and CCPU declarations and links actual selected cached
`ios.obj`, `in.obj` and `out.obj`. It supplies a controlled CPU envelope and
callback spies, not copied routing algorithms. The [final receipt](ios-owner-fixture-result.json)
binds all 15 source/header/object/graph/runner inputs before and after testing.

All **70 assertions pass**, with 78 callbacks: defaults, byte/word/dword
order and values, string lengths/guards and zero count, optional registration,
callback replacement, separate read/write connect, whole-port/nonowner
disconnect, slot identity, wraparound, map reset/reconnect and the two CPU
entry routes. Compiler PID 21352 and fixture PID 31876 exited 0, with no
compiler warnings. Deliberately declaring a four-byte word is
[rejected with C2118](ios-owner-abi-negative.txt), compiler PID 9996 exit 2.

The [shared runner](../../tests/observation/verify-softpc-object-contract.ps1)
owns compiler/test processes and input receipts for both contracts. The
existing [quick-event CLI](../../tests/observation/verify-qevnt-object-contract.ps1)
remains a thin compatible wrapper. Its [fresh regression receipt](ios-owner-qev-control-result.json)
passes all 37 assertions/eight callbacks under the shared runner; build PID
30288 and fixture PID 21208 exited 0, wrong-width compile PID 32836 exited 2.
This removes duplicated test-process/receipt plumbing without introducing
runtime retries, polling or lifecycle deadlines.

## Status and bounded handoff

The [retention metrics](ios-owner-code-retention.csv) are 100% exact original
source retention for these six files, **not** functional completeness or
coverage. Their source bytes and original identities remain unchanged; no
new mirror divergence/exception is introduced. Ledger totals become 2,139
pending, 236 partial, 2,257 excluded and 52 full.

Remaining evidence is finite: S11 owns real instruction decode, permission,
mode and fault behavior; S13 owns device registration, concurrent access,
reset/retirement and actual guest dispatch; S17 owns applicable VDD callback
retirement. Published-image/object equivalence is still unproved by this
cached-object fixture. No full DOS/WOW/Win3.x compatibility or default
publication qualification follows from these controlled tests.

## Reproduction

Run through the required owned background process context:

```text
tests/observation/verify-softpc-object-contract.ps1 -Contract ios -BuildRoot build/t473-s10/ios-object-contract-r3
tests/observation/verify-qevnt-object-contract.ps1 -BuildRoot build/t473-s10/qevnt-object-contract-r6
python -B -X utf8 artifacts/mvdm-audit/ios-owner-checkpoint.py build/t473-s10/ios-object-contract/checkpoint.json
```

Receipts retain these particular run identities; a new run has a new receipt
and executable hash. The [ledger check](ios-owner-verification.json) checks
the exact six-row delta, all unrelated shard rows, original/frozen identities,
source retention, both actual-object receipts, selected graph and live drifts.
It does not close S10. Other sessions' setup/video changes remain preserved
and outside this audit-only checkpoint.

## S11 decoder permission-map source follow-up

The selected `c_main.c` decoder checks the TSS I/O bitmap before calling
`IN8/16/32` or `OUT8/16/32` for string IN/OUT when CPL exceeds IOPL or VM is
set. It passes the operand width into `check_io_permission_map`; that helper
checks task-register presence/type, map-base and map-word limits, then tests
one, two or four permission bits. The string memory operand is acquired only
inside the subsequent iteration. This is source ordering, not a test of fault
precedence, privilege transitions, zero-count REP or guest dispatch.

The helper stores the bitmap offset in `IU16`. Thus `map_start_offset +
port/8` wraps at 64 KiB. This is not sufficient evidence for a repair: Intel's
[system programming manual](https://www.intel.com/content/dam/support/us/en/documents/processors/pentium4/sb/25366821.pdf)
distinguishes the 486's TSS wrap behavior from later P6/Pentium behavior for
large I/O-map bases. The code's individual two-byte read and limit rules have
not been compared on real 386/486 hardware, and the selected guest's use of
such a map has not been established. Do not generalize a current-host result
to the original 486 target or modify the mirror on this static observation.
The existing S11 instruction/exception and S13 device/mode boundaries remain
open; source retention is not a runtime pass.

For a selected-decoder witness, execute immediate-port and DX-port IN/OUT at
byte, word and dword widths against the actual IOS object with counting device
callbacks: ring 0 normal dispatch, ring 3 with no usable TSS (general-protection
fault before any callback), and ring 3 with a controlled 386 TSS permitting or
denying individual bitmap bits. String cases additionally need zero and nonzero
REP counts, memory fault ordering and the original interrupt boundary. The
fixture must report the decoded instruction, fault vector and error code,
callback counts, register/memory side effects and selected object hashes;
hardware-model-specific edge cases require separate evidence and must not be
silently rewritten to match the host CPU. The full matrix is not yet claimed.

A first bounded selected-decoder witness executed a real byte OUT followed
by byte IN using DX against the linked original IOS object and a counting
test device. Ring 0 performs exactly one write of 5Ah and one read. With CPL 3
and no usable TSS, the first OUT raises #GP(0) before either callback. The
runner verifies link-map owners for `c_cpu_simulate`, `IN8`, `OUT8`, IOS and
the exception fixture. Its captured fault IP was 8005h, the OUT instruction,
not a preceding setup instruction. The later expanded fixture changes the
instruction address and supersedes that initial two-case result. Both use
`/force:multiple` and a controlled exception hook.

The selected-decoder witness now also covers byte, word and dword DX-port
IN/OUT in seven scenarios each: ring 0; ring 3 without a TSS; a controlled
386 TSS allowing access; denial of the first and last relevant permission
bit; a truncated permission map; and a 286 TSS. All 21 cases passed. Allowed
cases verify the returned accumulator value, byte-split device callback
counts and first output byte; rejected cases assert #GP(0) at the OUT
instruction and zero callbacks. An initial extended fixture mistakenly
overlapped its TSS with the fixture's page-table storage and failed with
#PF at startup; the corrected fixture reserves a separate TSS page. The
earlier 21-case run covered only DX scalar operands; its conclusions do not
extend to other forms. Guest delivery and 386/486 edge semantics remained
unverified.

The 84-case selected-decoder run superseded the 21-case receipt: cases passed for DX and immediate
scalar IN/OUT and DX REP INS/OUTS across the same seven privilege scenarios
and three widths. String cases use counts zero and two and check device callback
counts, input destination edges and a guard byte. In the selected original
decoder, even zero-count REP checks I/O permission before entering the loop;
this is recorded as existing behavior, not proven processor equivalence. The
first string fixture failed because it did not reload CX after REP OUTS; the
corrected fixture reloads CX before REP INS and passed. The per-case receipt
still cannot establish real guest exception delivery, all bitmap boundaries,
memory fault precedence, asynchronous interrupts, or original 386/486
hardware behavior.

The subsequent run added 12 controlled memory-order cases, for 96 total. With access permitted, REP
INS to a present read-only destination raises #PF with write/user/present
error bits before a device read; REP OUTS from an absent source raises #PF
with user/read/not-present bits before a device write. If both an INS target
page is absent and the TSS bitmap denies the port, #GP precedes the page
fault. A read-only OUTS source is a successful control. Each case covers all
three widths, checks fault IP and CR2 where applicable, counts callbacks and
checks physical source/destination contents and guards. An initial guard check
incorrectly read the absent page through the guest-access helper; the corrected
test inspects physical backing and passes. These are controlled selected-object
results, not proof of real guest exception delivery or all crossing-page and
interrupt interleavings.

The [current receipt](host-ch-scope/ccpu-port-privilege-result.json) additionally
tests DX port 2207h, whose permission bit is at the end of a bitmap byte. All
117 cases pass, including denial of the last byte/word/dword port bit across
the bitmap-byte boundary. This does not test TSS segment wrap or replace the
unverified guest and interrupt boundaries above.
