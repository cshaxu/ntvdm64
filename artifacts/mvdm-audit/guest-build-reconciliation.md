# Immutable guest build and artifact checkpoint

This S10 checkpoint resolves 840 historical guest build-artifact file roles:
221 in shard 0, 191 in shard 1, 197 in shard 2 and 231 in shard 3. These files
are retained unchanged; only their classifications change to excluded as
current host/runtime source inputs. No guest capability, original source
algorithm or deployed runtime image is excluded through these decisions.

The exact per-file source hashes, original recipe references, format checks,
identity comparisons and remaining source/behavior boundaries are recorded in:

- [Shard 0 owner census](shard-0/guest-build-owner-reconciliation.csv),
  [221 carrier dispositions](shard-0/guest-build-carrier-disposition.csv) and
  [method](shard-0/guest-build-owner-reconciliation.md).
- [Shard 1 owner census](shard-1/guest-build-product-closure.csv),
  [intermediate formats](shard-1/guest-intermediate-format-review.csv) and
  [method](shard-1/guest-build-product-closure.md).
- [Shard 2 DDEML tests](shard-2/guest-ddeml-test-build-review.csv),
  [remaining owner groups](shard-2/guest-remaining-target-groups.csv),
  [intermediate formats](shard-2/guest-intermediate-format-audit.csv) and
  [method](shard-2/guest-remaining-target-groups.md).
- [Shard 3 carrier formats](shard-3/guest-build-carriers.csv) and
  [method](shard-3/guest-build-carriers.md).

Across the four disjoint groups, 628 object files have complete 16-bit OMF
record framing. Listings and recipes were inspected as text/build products;
symbol files require original recipe or format evidence. Eight shard-3
symbols and one shard-1 symbol lacking sufficient evidence remain pending.
Stored-zero checksum observations are distinguished from an inconsistent
nonzero checksum; none is silently repaired or declared corrupt.

Resources and libraries are not excluded merely because the host does not
directly compile them. Source-to-object-to-deployed-image lineage remains a
separate question where the original build cannot be reproduced under guest
immutability. Ancestor recipes and literal filename references provide
bounded owner clues, not that missing proof. Exact archive/package hashes
establish byte identity only, never function coverage. An original display
recipe explicitly copies DISPLAY.EXE to VGA.DRV; original SHARE and FASTOPEN
stub identities likewise must not be confused with service restoration.

The guest audit now retains 1,773 pending source/declaration/resource/binary
rows. The complete ledger has 23 full, 224 partial, 1,466 excluded and 2,971
pending rows. The wider original CPU, DOS, WOW and external-provider obligations
remain intact. This batch changes no product or original guest bytes and
does not claim additional runtime tests or S10 closure.

The retained [verifier](guest-build-checkpoint.py),
[result](guest-build-checkpoint-result.json) and
[background record](guest-build-checkpoint-runner.json) validate the 4,684-path
ledger, frozen identity, exact shard ownership, all 840 pending-to-excluded
transitions and the retained full/partial invariants. They also confirm no
excluded whole file is a current guest-ZIP or package-c runtime member.
[OMF results](guest-omf-format-result.json) cover 22,683 records across all
628 objects, with zero inconsistent nonzero stored checksums. Stored-zero
records are counted separately. Documentation governance and whitespace
verification passed. These are source/format checks, not guest execution tests.
