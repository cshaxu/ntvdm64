# M0 T404 owner-directed closure

## Decision

On 2026-09-12 the owner ended S5 and T404 before further implementation,
requesting an audit of autonomous replacements, including working COMMAND
and EDIT paths. This administrative closure does not establish complete
DPMI32/WOW32 recovery or WRITE start/run/return. No new numeric task is active.

## Preserved results and limitations

- S3 retains the owner-accepted [mouse result](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md).
- S4 retains [positional bootstrap](../etc/evidence/m0-t404-s4-positional-wow-bootstrap-run-001.md), committed at `05f3859da`.
- S5 retains [provider and diagnostic evidence](../etc/evidence/m0-t404-s5-wow32-matching-provider-build-001.md) and the [DEM ingress correction](../etc/evidence/m0-t404-s5-dem-chmod-ingress-correction-001.md).
- The latest recorded WRITE observation reaches COMMAND's fatal allocation
  stall with a malformed MCB chain. The environment-size discrepancy is a
  lead; neither its writer nor a successful fix has been established.
- Same-version provider builds are not proof of guest acceptance.

## WIP disposition

The dirty source, imported declarations, build tools and research artifacts
are retained without reset, deletion or promotion to an accepted product.
At closure inspection, tracked changes totalled 934 additions and 190 deletions
across 38 files, excluding untracked inputs. These are worktree-versus-HEAD
counts, not OpenNT divergence counts or removable-code counts.
S5 product P review/commit/push remains pending; this closure must not be
reported as a clean workspace or a delivered S5 product P. This document
records that limitation, not an acceptance waiver.
Research builds stay under `build/M0-T404/S5/`; runtime logs stay under
`O:\ntvdm64\logs`. No executable is deployed by this closure.

## Audit handoff

### Owner-authorized snapshot delivery

The owner subsequently authorized commit/push and a clean worktree, with an
OpenNT MVDM/non-MVDM source-diff audit placed first in Queue. The retained
source, declarations and build-tool changes are committed as an explicit
unaccepted research snapshot, superseding the pending-save state above.
They are audit inputs, not certified repairs or a new runtime publication.
No source is discarded and no guest test is claimed by this preservation
delivery. Ignored build products and runtime logs remain local.
The queued audit has one dedicated proposal and no numeric task admission.
T404/S5 remain closed by owner direction; complete WRITE recovery remains debt.

Preservation checks: documentation governance, relative links and queue
valid/missing-link cases pass. The staged default whitespace check reports
only extra terminal blank lines in seven retained original ABI headers;
these bytes are preserved for provenance, not reformatted as a source fix.
The whitespace check excluding only `blank-at-eof` is required for this
snapshot. No fresh product build or runtime acceptance is claimed.

The [initial replacement audit](../etc/evidence/opennt-replacement-audit-20260912.md)
separates confirmed substitutions, unequal contracts and unresolved suspects.
Working COMMAND/EDIT are regression baselines, not exemptions. Full source
recovery remains unfinished; subsequent implementation requires an active
packet following the owner's audit decision.
