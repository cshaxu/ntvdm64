# T221 S3 P54 — Virtual profile work deferral

## Decision

The active S3 implementation scope is narrowed by owner direction to the
Direct, Readonly and Overlay mutation profiles. Existing Virtual source,
focused fixtures, package-session evidence and historical records remain
preserved and must not be deleted, weakened or silently redirected to another
profile.

No new Virtual-only BOP/provider/session/CLI behavior is admitted during this
S3 pass. The remaining Virtual closure is tracked in `docs/TODO.md` and must
be re-admitted as a package-level work item before implementation resumes.

## Rationale and boundary

The provider architecture remains four-mode capable. Pausing Virtual avoids
multiplying every remaining DEM owner-family regression by a private
boot-volume dimension while Direct/Readonly host integration and Overlay COW
semantics are brought to complete component-package closure.

This is a schedule and admission decision, not a semantic change:

- Virtual's existing default-off behavior and no-host-I/O boundary remain
  tested by its existing evidence;
- Direct, Readonly and Overlay must not use Virtual as a fallback;
- no claim of four-profile or Virtual-package closure may be made from T221's
  narrowed exit criteria; and
- a later resumption must revalidate the original OpenNT source/ABI/failure
  map and the retained Virtual code before adding any behavior.

## Consequence

T221 now seeks a 29-identity Direct/Readonly/Overlay package regression,
while retaining the existing Virtual implementation as frozen prior work.
This is not deletion, abandonment, or permission to make Virtual silently
unavailable where it is already implemented.