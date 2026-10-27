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
