# M0 T417 — private-component elimination closure

T417 closes the private-component retirement candidate without changing any
unreviewed project-component boundary.

## Delivered

- S1 froze all private-root ownership, callers and formal build edges.
- S2 retired `src/mvdm-softpc-patch` under the strict mirror policy.
- S3--S4 retired `src/mvdm-overlay` and `src/opennt-host-overlay`; neither
  `src/mvdm` nor `src/opennt-host` gained a file.  Original-owner-local
  changes stayed in existing mirror files; C-VID ABI composition and the
  monitor printer bridge are named adapters.
- S5 completed the required non-mutating audit of app, broker, session and
  every adapter family.  It identifies a 387-line legacy broker deletion
  candidate and safe adapter/session boundary work, without treating archive
  linkage as proof that an implementation is live.

## Verification retained

S3--S4 used a fresh x86 formal graph, affected-owner compilation, three
product artifacts, the CCPU fixture, C-VID contract verifier, root-elimination
test and documentation-governance checks.  The complete owner and verification
record is [the S3--S4 evidence](../etc/evidence/m0-t417-s3-s4-overlay-owner-recovery.md).
S5 is source/build-relationship analysis only; its result is
[the component relationship audit](../etc/evidence/m0-t417-s5-component-relationship-audit.md).

## Handoff

The follow-up is the queue-head
[component-boundary normalization proposal](../proposals/proposal-component-boundary-normalization-001.md).
It must prove removal of the dead legacy broker archive before deleting it,
then partition adapter build ownership and extract private adapter state only
with the specified runtime regressions.  It may not introduce a mirror file or
move modern RPC/Console/ABI mechanics into an original mirror.
