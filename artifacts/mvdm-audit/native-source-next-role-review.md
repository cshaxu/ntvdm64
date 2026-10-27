# Next native source integration checkpoint

This main review applies to the 180 proposed paths at frozen pending indices
30–89 in each of shards 1, 2 and 3. Its ledger base is `536087158`; the cohort
itself remains defined by `ecd4a604a`, so the earlier 90-path checkpoint cannot
silently change which files are reviewed.

## Decisions and remaining contracts

The [48 per-file dispositions](native-source-next-role-dispositions.csv)
accept 47 **partial integrations** and one empty original source file role.
The other 132 paths remain pending with their original per-file obligations.
No row becomes full and no new guest behavior is accepted here.

The partial rows have deliberately different evidence strength. Selected CPU
helpers and declarations have bounded arithmetic, descriptor or stack fixture
evidence; other CPU, printer and generated C-VID providers have selected source,
object-map and original dispatch/publication evidence only. The checker now
independently requires each C provider's object in the selected product map
and its source in the current worker graph, or each declaration in the captured
selected dependencies. That establishes integration, not correct guest opcode
decoding, memory/privilege faults, planar/latch calculations, printing or device
lifecycle. Those exact gaps remain in each disposition and the S11/S13 owner
worklist. Historical statements saying a capability remains pending are not
upgraded to behavioral passes by its file's partial-integration classification.

`softpc.new/base/comms/prt_scrn.c` contains only the original comment that its
function moved to Intel BIOS4.ROM. It is compiled by the current graph. The
actual selected COFF object has no executable/code section with raw contents;
it is not an unselected runtime provider. Only the empty physical file's
implementation role is excluded. BIOS4.ROM identity, INT5/Print Screen dispatch,
output and failure behavior remain S13 obligations. This exclusion neither
removes Print Screen from the T nor counts it as restored.

All 48 sources retain 100 percent of the selected original bytes. That number
is text retention, not functional coverage. Each partial row explicitly states
why a restored executable-code percentage remains unmeasurable: map selection
and bounded fixtures do not supply a denominator for guest execution, faults
and alternative paths; declarations and an empty translation unit have no
independent executed-body denominator. No semantic percentage is inferred from
matching lines or fixture case counts. Mirror-difference estimates remain
separate and no changed mirror file is made full.

## Verification

The [input observations](native-source-next-role-observations.json) bind the
180 proposals, original/current/frozen hashes, selected build/dependency inputs,
actual product map and empty COFF object. The existing checker, extended at its
native-source boundary, verifies the exact 48-row ledger delta and all four
shards, leaves unrelated rows unchanged, retains all 52 full rows, and rechecks
the six existing source drifts. The original graph/dependency snapshots are
reused only with unchanged hashes; this is not a new runtime test.

Run the checker from the repository root:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/native-source-next-role-verification.json --base 536087158 --prefix native-source-next-role- --expected-count 48
```

The [result](native-source-next-role-verification.json) reports 1,804 pending,
319 partial, 52 full and 2,509 excluded paths, totaling 4,684. The earlier
180-proposal verifier is retained as evidence of its then-unmerged checkpoint;
the current merged state is verified by the command above. No production source,
guest, product build input, installed binary or release asset changes here.
