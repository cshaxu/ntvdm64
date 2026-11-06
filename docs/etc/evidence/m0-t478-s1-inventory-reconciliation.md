# T478 S1 inventory reconciliation — in progress

## Inputs pinned on 2026-11-05

Product baseline: 5ec7351c9. Both audits identify original OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Their current directories do not
expose Git metadata to the read-only query; these input SHA-256 identities
pin the actual ledger files instead of inventing repository revisions.

| Read-only audit input | SHA-256 |
| --- | --- |
| DOS16 management/seam-scope.json | 0efbde9140057115629ffceba6ecc8d58387b2a856d229f2913d3e06ddab075e |
| DOS16 management/seam-work.json | 536cf0f26c5a0a8461cb3810f18eb967bdf039aba01c6a110e186a1c8bc078e9 |
| DOS16 management/seam-records.json | aa0ec644b68c682afce8e48bfe0a1734af83aa078e95efec51e8c254f1fec4a8 |
| DOS16 management/module-handoffs.json | 12320abe30b5fee56869cdecb6d88511aa2c5715176f82e31f2a18d7737079b2 |
| WOW32 management/seams.csv | c6e08dd649a6c180f5e16aa9b99a525b6c22ebee41630f542c5d9be81c78e4a0 |
| WOW32 management/seam-sites.csv | cd4d95dded409fdfede3e2bdf8cc80f26340534eb8556a06482592a0768fbb97 |
| WOW32 management/seam-gaps.csv | de426fb8d896e295ac770d6f620fa5de0590e62380f8acbda87118e805eb5e2e |

## Owner scope clarification

S1 audits mvdm-host and necessary host-side integration. Guest source, guest
images, their packaging and configuration are read-only evidence only when
they demonstrate a concrete mvdm-host capability or missing host connection;
no standalone guest audit, guest packaging or guest repair is admitted. Any
older suggestions to treat guest-only payload presence as a work item are
superseded by this clarification. See the [host-scoped 69-row disposition](m0-t478-s1-classified-findings.md).

## Preliminary observations, not dispositions

WOW32 retains 1,238 seam records: 755 active OpenNT and 483 inactive
OpenNT-4.5 records; its site inventory contains 346,713 rows. These are
separate source-position and identity denominators, not a capability count.
The audit's implementation-validation labels describe what the audit itself
verified; they do not establish absence of current product implementation.
The DOS16 audit reports 24 bounded modules closed statically; its own status
explicitly distinguishes those source-contract conclusions from product and
runtime validation. Neither result proves complete product integration.

T473 S10 recorded 1,270 host C/H candidates: 53 full, 876 partial and 341
excluded, and identified 202 then-current host mirror differences. These are
historical dispositions at S10, not current counts. Its recovery worklist
contains independently qualified leads and must be reconciled against later
work before classifying a candidate as actionable.

T476 closure records 1,051 of 1,055 files classified, four pending compiler
verification and incomplete cross-context reconciliation; product-only lack
of hits did not establish exclusion. Thus an existing #error alone is not a
sufficient current eligibility or unsupported-behavior conclusion.

## Outstanding work

Reconcile every applicable inventory and inherited handoff against the current
product's selected source, dispatch, registered exclusions, local providers and
matching runtime evidence. Maintain separate selected-branch and excluded-
branch cases, identify exact original and autonomous owner paths, and mark
missing evidence unresolved. This record is preliminary and does not satisfy
the owner's complete-report request or close S1.

## S42 continuation after S41

The [stable-identity crosswalk](../../../artifacts/mvdm-audit/t478-final/README.md)
now accounts for308 DOS16/OpenNT,755 WOW32/OpenNT and483 inactive edition
identities. All seven original ledger pins still match. This is a denominator
and evidence-mapping checkpoint, not1063 missing features or a new source audit.
VDD registry startup is explicitly separated from S41's tested selected plugin;
WINMINE geometry/background remains a named inherited handoff until existing
repair and current evidence are reconciled. S42 is active; T completion is not
claimed. No changed product or regression rerun is required for this index.
