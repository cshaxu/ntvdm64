# S10 native source evidence checkpoint

This checkpoint compares against `ecd4a604a`. It reviews the first 30
then-pending paths from each existing native-owner proposal for fixed shards
[1](shard-1/native-pending-followup.csv),
[2](shard-2/native-pending-followup.csv) and
[3](shard-3/native-pending-followup.csv). These 90 paths are disjoint; this is
not a replacement for the remaining original CPU, BIOS or MVDM audit.

## Per-file conclusions

The [38 dispositions](native-source-role-dispositions.csv) record 36 partial
integrations and two historical test-source file-role exclusions. The other
52 reviewed paths remain pending. No row becomes full and no new guest
capability is accepted by this checkpoint.

The partial evidence deliberately has different strengths. Shard 1's ten
rows include selected-object/native-oracle arithmetic cases, extracted-source
BSF cases and stack/descriptor fixtures with controlled lower boundaries.
These prove portions of their helper contracts, not actual guest decoding,
all CPU state, prefixes or precise memory faults. Shard 3 includes similarly
bounded helper evidence and historical INT12/RTC observations, together with
selected-source-only cases. Shard 2's twelve rows establish selected providers
and original BOP/decoder/TLB call chains but lack direct behavior witnesses.
Their classification is **partial integration, without functional acceptance**.
Do not present the map or compiled instruction helper as a complete guest
instruction, BIOS service or restored runtime capability.

Each partial row retains a concrete missing contract: for example AAM's
zero-base divide exception, BOUND's signed limits and fault ordering, LTR's
descriptor mutation, POP's stack-width/segment effects, or TLB permissions,
accessed/dirty bits and invalidation. These belong to S11's execution and
translation foundations; the three BIOS rows belong to S13. Their stage
assignment records responsibility, not permission to defer a foundation
defect into a specific application merely because it manifests there.

`c_getset.c` explicitly describes complete CPU-state transfer for comparison
against the assembler CPU and places its implementation under PIG.
`ccpupig.c` is the original CCPU/Pigger comparison interface under the same
guard. The selected CPU40 production graphs do compile these translation
units, but do not define PIG. Actual selected COFF objects have only compiler
directives/checksum/debug sections and no executable sections; the captured
symbol tables likewise contain no executable provider. This corrects the
earlier non-consumption assumption rather than overlooking their inclusion.
Only those physical comparison-harness implementations
are excluded. Real CPU instructions, state access, recovery tests and the
separate applicable `c_getset.h` declarations are not thereby excluded.

## Proportion and identity

All 38 files are byte-identical to the selected original and frozen source:
their original-text retention is 100 percent, with no new mirror diff.
That is **not a 100-percent restored-code or behavior estimate**. The
dispositions explicitly mark restored executable extent unmeasurable from
the available fixture/selection evidence. The unchecked guest dispatch,
memory/fault paths and CPU configurations prevent a defensible coverage
denominator; a header also has no independent executed-body denominator.
No numeric functional percentage is invented from case counts or line
similarity. The existing mirror-difference estimates remain separate.

## Verification and remaining work

The [observation manifest](native-source-role-observations.json) binds the
three owner tables and previously captured selected worker/WOW input,
dependency and graph observations. The same unchanged graph identities
justify reusing those selection snapshots; this does not rerun or upgrade
their historical behavior evidence. Per-file original source, direct caller,
test scope and outstanding boundary remain in the tables.

The existing source-role checker verifies the exact 38-row delta, all 90
reviewed paths and fixed shards, selected original identities, original PIG
guards, the non-PIG CPU40 graph and actual empty COFF sections, all unchanged unrelated ledger rows,
the full-source invariants and the existing six live source drifts:

Independent review caught that these two edges use `ccpu_flags`, not just
the general `host_cflags`. The checker now verifies the actual `cc_ccpu`
rule, both source edges, `ccpu_flags` and `release_cflags` as well as the
empty COFF objects. The initial missing parameter assertion did not change
the observed objects, but is not left as an assumed configuration guarantee.

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/native-source-checkpoint-r3.json --base ecd4a604a --prefix native-source-role- --expected-count 38
```

The owned-background [result](native-source-role-verification.json) and
[run identity](native-source-role-verification-owner.json) are ledger,
identity and recorded-scope evidence, not a new behavior test. Counts become
**1,852 pending, 2,508 excluded, 272 partial and 52 full**. S10 remains active;
the separate candidate SOL Options failures are neither repaired nor waived.
