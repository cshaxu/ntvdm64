# T484 S3 P25: native CPU contract producers

Four inline compiler runners are consolidated into the CMD cpu-contracts
group and test/observation/verify-cpu-contracts.ps1. The assertion runner never
constructs compiler commands, extracts flags from Ninja, or emits source.
Negative compile targets are separate and excluded from product/all.

NEG64 and leaf declaration checks inherit the actual c_neg64 object profile.
Current original NT/SDK headers caused the first /WX build to fail on known
C4201, C4273 and C4028 diagnostics. Those header diagnostics and the existing
C4005 suppression are explicit; all other warnings remain fatal. Both negative
pointer signatures failed specifically with C4113, not an unrelated error.
This preserves the typed ABI requirement, not a claim that original headers
are warning-free. No production source or warning policy changed.

The notification projection contains the same five masks and four functions
as the previous extractor. Its only difference after CRLF normalization and
terminal-newline removal is one empty line after CPU_HW_INT_MASK. Source bytes
inside the functions are unchanged. This bounded fixture proves event-map
atomic operations, not actual guest IRQ delivery or scheduling.

Final runtime evidence:

- NEG64: 631410 cases, both pointer orders, unsigned modulo-64 results,
  unchanged guards and involution; deliberate wrong oracle rejected at case0.
- Two wrong ABI declarations: compiler exit2 with the required C4113.
- Notifications: 4096 rounds,8192 raises,4096 single-winner/later-raise cases.
- Host x86 INT-prefix reference: five expected exception codes and IP offsets.

The [receipt](../../../artifacts/build-migration/t484/s3-cpu-contracts/result.json)
records seven build/test subprocesses and expected failures separately.
These are native/helper tests, not a new guest CPU acceptance run.
Cached full build and separate 11 raw/22 published identity checks are
recorded with this batch. Unchanged product images need no recopy.
The containing commit delivers P25; S3 and subsequent S4/S5 remain open.
