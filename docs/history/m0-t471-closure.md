# M0 T471 — WOW32 whole-provider and three-application acceptance closure

## Scope

T471 accepted the queued whole-provider packet against one unchanged ten-image
package, without adding a provider, changing guest media, or changing a
runtime artifact.

## Evidence and result

- [S1 artifact freeze](../etc/evidence/m0-t471-s1-provider-artifact-freeze.md)
  reconciles W01--W19 and fixes the exact package identity.
- [S2 application progress](../etc/evidence/m0-t471-s2-application-integration-progress.md)
  records selected real WRITE, WINMINE and SOL routes.
- [S3 progress](../etc/evidence/m0-t471-s3-progress.md) records shared and
  separate WOW worker retirement, OEM, native CUI return, and abnormal
  DOS→WOW-worker-loss→DOS recovery.
- [S4 aggregate review](../etc/evidence/m0-t471-s4-aggregate-review.md)
  proves the installed and frozen package identities and confirms that T471's
  source footprint contains only test/evidence changes.

The frozen/installed ten-image package remains the T467 S3 release.  T471
commits `d9caa33f6`, `f769aea1c`, `de3072562` and `8886c4654` are all
documentation or test/evidence deliveries; no production package publication
was necessary.

## Retained limitation

The physical visible-desktop WRITE edit/save/common-dialog/resize sequence is
owner-deferred and an explicit non-pass.  The test host could not inject on
the active input desktop, and T471 did not replace it with message injection,
queued input, a guest change or a synthetic provider.  The limitation is
recorded in S2 and does not claim a product regression.

## Handoff

The next queue candidate, T472, owns only residual OpenNT error-dialog
response and termination semantics.  It must reuse T471's runtime evidence;
it may not reopen its accepted provider routes or absorb the deferred WRITE
physical-input item.
