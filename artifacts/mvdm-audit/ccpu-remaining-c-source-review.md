# Remaining CCPU C sources: complete source-review cohort

At ledger base `4f6da139d`, exactly 22 pending files beneath
`softpc.new/base/ccpu386/` have a `.c` suffix. The
[per-file table](ccpu-remaining-c-source-review.csv) covers all 22, not a
sample. Every current source hash equals its named original OpenNT source.
The [input identities](ccpu-remaining-c-review-inputs.json) bind the actual
decoder, build generator, generated worker graph, selected map and shared
C-VID accessor source inspected in this review.

This is a source/caller and map-owner investigation. It does not change
the ledger, qualify runtime behavior or establish a code-coverage percentage.
Each file retains a concrete remaining-evidence obligation in the table.

## Selected source contracts

Twelve instruction-source files plus the original PROD diagnostic carrier
have 19 public function entries in the current map. Their contracts are
reviewed with the actual decoder, not just the leaf helper:

- CLI and CLTS permission checks execute before the helper. Clearing a flag
  in a native fixture would not prove privilege faults preserve guest state.
- DIV's zero/overflow paths call the original nonreturning exception machinery
  before register writeback. A stubbed Int0 that returns would invalidate a
  fault test and could execute an actual host divide by zero.
- JCXZ/JECXZ selection follows address size in `TYPEE0`. Operand size still
  participates in the shared IP update; test those dimensions separately.
- LIDT rejects register operands and checks privilege before fetching the
  six-byte memory object. SIDT uses the original store path and preserves
  the source's full cached base, including the original 16-bit-form comment.
- BTC, RCL and SHLD depend on decoder operand normalization/writeback and
  policy-specific undefined flags. Read the pointer-assignment lines as code;
  removing every line beginning with `*` is not a valid comment filter.
- WAIT is not an unconditional successful instruction merely because its leaf
  is empty: the decoder checks MP/TS and can raise Int7 before calling it.
- The original RSRVD leaf intentionally returns for specifically selected
  reserved encodings. Do not replace every such route with a generic Int6.
- `stubs.c` is the original PROD diagnostic carrier, not a project-authored
  behavior replacement. The selected map contains `check_I`, `force_yoda`
  and the condition flag; it does not prove `check_D` or debug-profile use.

Functions with identical empty code share a map address. That is consistent
with linker folding and is not proof of separate executions, dispatch or
independent lifecycle coverage.

## Nine other physical source roles

Six original files (`bswap`, `cmpxchg`, `invd`, `invlpg`, `wbinvd`, `xadd`)
are compiled as files but their SPC486 function bodies are inactive in the
selected SPC386 profile. The earlier [486 boundary review](shard-3/cpu486-profile-boundary.md)
already identifies the declaration/implementation gates and the coupled
control, TLB, flags and reset obligations. This review neither enables that
profile nor excludes those capabilities merely because current functions
are absent.

The remaining three need different handling:

| File | Actual source/build result | Remaining decision/evidence |
| --- | --- | --- |
| `nop.c` | Compiled original empty helper is absent from the selected public map; the original decoder handles opcode 90 with a direct `break`. | Verify actual object/decoder correspondence and guest NOP before excluding only an unused helper-file role. The instruction itself is in scope. |
| `ntstubs.c` | The generator deliberately omits this alternate mixed CPU/FPU/SAS host carrier because the selected real providers would conflict. | Resolve its applicable symbols and defaults to the real selected owners; never exclude all CPU/FPU/SAS functionality from the filename. |
| `vglob.c` | Its bytes equal selected `base/cpu/src/evid/vglob.c`; the actual compile edge uses that shared source with the C-VID header/profile. | Prove selected GDP fields, pointer ownership, video-device state and lifecycle; byte equality alone does not establish the active layout. |

## Follow-up and limitations

The table is now the complete per-file source contract for this cohort.
S10 still owes selected-object/decoder correspondence and formal dispositions;
S11 owns CPU guest behavior/fault recovery, with C-VID lifecycle shared with
S13. No pending entry is removed from the T's objective. Source-text retention
is 100 percent; behavioral restoration cannot be measured from this review.
No production, guest, archive, build graph or release file changes here.
