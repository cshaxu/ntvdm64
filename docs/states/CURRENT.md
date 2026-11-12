# Project Status

## Current Work

**No active M/T/S packet.** M0 T483 remains open; S1 is closed.

The owner authorized S1 closure after audit and verification. S2 has not been
admitted; no functional remediation is authorized by this closure.

## Latest S Closure

[M0 T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md) records the bounded
mirror-difference audit and P delivery. The new
[ordered register](../etc/operations/m0-t483-mirror-register.md) is the basis for
future repairs, in fixed file order and then line order. Historical IDs are
auxiliary provenance, not a second work queue.

- Fixed host C/H census: 1060 = 491 byte-identical + 259 exclusion-only +
  302 other differences + 8 deleted files. All 569 differing files classified.
- 3042 registered decisions: retain 1767, restore 76, delete 120, minimize 117,
  correct 207, pending evidence 755. Classification is not repair completion.
- 9596 lexical difference blocks: 9588 live plus 8 whole-file deletions;
  admission baseline 9636. These are not semantic-defect counts.
- 365 formatting restoration sites in 249 files (122 C/H, 127 other files).
  Eight proved unnecessary files deleted; selected build graphs reference none.
- No original guest edits or rebuild; assets/winnt.zip unchanged.

## Current Technical Baseline

Package `6ace4e9a-48ea-48ac-abd9-94e966b67c78`: cached x86/x64 build passed;
formal WOW graph and dynamic host API checks passed; all 16 selected Full gates
passed. The external DOS4GW/NESticle fixture was not supplied and was explicitly
skipped, not passed. All 11 images are published to assets/release and
O:/winnt/system32 with matching manifest SHA-256 hashes.

[Delivery evidence](../../artifacts/mvdm-audit/t483-s1/delivery-verification.json)
and [Full receipt](../../artifacts/mvdm-audit/t483-s1/full-timings.json)
distinguish raw compiler identity, package stamping and runtime acceptance.
No claim of binary equivalence to the old package is made.

## Remaining Scope

Functional recommendations and evidence gaps remain in the new register.
Modern Windows API ownership can be the correct original closure boundary;
original omissions alone do not justify restoring code. Subjective boundary
choices require owner review. Compat and product-component minimization are
later work under the
[admitted T scope](../proposals/proposal-original-source-implementation-minimization-review-001.md).

The DOS identity/wrong-waiter defect and Win95 exit-code boundary remain in
[TODO](TODO.md). Win95 restart-to-DOS belongs to the
[XP baseline-upgrade package](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md).

Parallel-session QUEUE.md edits and the win203 setup proposal are preserved and
excluded from S1 delivery.

## Recent M0 Closures

- [T483 S1](../etc/evidence/m0-t483-s1-closure.md): mirror audit, bounded cleanup and verified delivery.
- [T482](../history/m0-t482-closure.md): investigation transferred, not repaired.
- [T481](../history/m0-t481-closure.md): guest multitask delivery with explicit boundaries.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md). S1 closure preserves the
parallel-session queue and proposal changes.
