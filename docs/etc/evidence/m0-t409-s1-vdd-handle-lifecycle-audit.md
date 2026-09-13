# M0 T409 S1 — DEM/VDD handle lifecycle audit

## Baseline

The selected `demfile.c` preserves the original D31--D33 bodies at lines
891--1211 but disables them under `#if 0`.  Its active replacements delegate
to the 425-line `mvdm_vdd_sft_shadow.c` adapter.  The original lifecycle slice
is approximately 321 source lines.

## Per-operation disposition

| Original operation | Current state | S2 disposition |
| --- | --- | --- |
| PDB/JFT lookup and free JFT search | Reimplemented in shadow using copied lease data. | Restore original order in `demfile.c`; bind guest reads through scoped leases. |
| SFT-chain walk and free-SFT selection | Reimplemented in `select_sft`. | Restore original selection loop and source failure values. |
| JFT/SFT reserve and initialization | Reimplemented in `shadow_create`. | Restore original mutation order; acquire a bounded write lease only for publication. |
| associate mode/flags/handle fields | Reimplemented in shadow. | Restore original field writes in `VDDAssociateNtHandle`; preserve host-handle opacity at binding boundary. |
| retrieve and release | Reimplemented in shadow. | Restore original validity/refcount/JFT order; scoped returned views require explicit caller commit/discard. |
| session registry/teardown and lease acquire/release | No original user-mode equivalent. | Retain as finite adapter mechanics, reduced to view/commit lifetime only. |

## Caller audit

WOW32 open/create commit after the original final SFT mutation; their error
paths publish the original JFT reset and reference-count decrement.  Close
commits JFT reset and decrement before host-handle close.  No order mismatch
was found in these reached paths.  S2 must retain this sequencing while
removing shadow-specific algorithm ownership.

## Recovery-rung result

1. Original source exists and is usable as the algorithm owner.
2. A finite guest-memory view/commit binding is required because the original
   returned durable flat aliases cannot cross the standalone boundary.
3. No external-code intrusion is needed.
4. New lifecycle algorithm is not justified.

S2 begins with a source-shaped scoped-view facade and removes the duplicate
SFT/JFT traversal, mutation and registry algorithm from the shadow provider.

