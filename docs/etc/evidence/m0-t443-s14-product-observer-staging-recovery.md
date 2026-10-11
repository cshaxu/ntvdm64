# M0 T443 S14 — product-observer staging recovery

## Scope

This is a test-only repair.  It does not alter a product image, the guest, the
OpenNT mirror, `SystemRoot`, or a user's drive mappings.

## Finding

The product observer had two staging defects:

1. `New-PhysicalRuntimeStage` created a 47-character generated physical root.
   The immutable WOW
   fixtures then exited `0x0000042b` before an NTVDM worker registered.
   Reducing only the component to nine or eight characters was insufficient.
   The same package starts at a proven ten-character real temporary root with
   a four-character leaf.
2. `observe-wow-frontiers.ps1` defaulted its guest executable directory to
   `system32`.  That directory is the carrier-binary directory.  The packaged
   WOW applications and their sibling resources belong at the package root;
   callers with another media layout can still pass `-GuestRoot` explicitly.

The revised stage leaf is `n` plus three random hexadecimal characters.  It
is real temporary storage, marker-authenticated before removal, and is not a
`subst` alias.

## Evidence

| Case | Physical root | Guest target | Result |
| --- | --- | --- | --- |
| r040 published/private | installed package | root `WINMINE.EXE` | live NTVDM at observer timeout |
| r042 old physical stage | 47 characters | root `WINMINE.EXE` | `0x0000042b`, no worker |
| r045 15-character stage | 15 characters | root `WINMINE.EXE` | `0x0000042b`, no worker |
| r046 corrected stage | 10 characters | root `WINMINE.EXE` | live NTVDM at observer timeout |
| r047 corrected three-fixture run | 10 characters | root WINMINE/SOL/WRITE | all three live at their 20-second observations |

The r047 run preserved private-desktop observation and wrote the original
per-application reports and window snapshots.  It did not turn a timeout into
application acceptance.

## Independent full-gate outcome

r048 used the same corrected staging and reached live NTVDM for all three
fixtures.  It then failed the retained `wow-frontiers` window-signature
assertion: the final samples contain the existing hidden NTVDM illegal-
instruction dialog rather than the retained application-window signatures.
This is an application/WOW frontier regression, not the former staging
failure.  The repair deliberately leaves that assertion intact.

## Cleanup

The focused and product runs used identity-scoped process cleanup.  The final
product gate left no process from its temporary stage alive.
