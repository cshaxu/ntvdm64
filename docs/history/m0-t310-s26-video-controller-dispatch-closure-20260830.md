# M0 T310 S26 — video-controller dispatch closure

S26 closes the selected original SoftPC `base/video` controller package for
Win32/x86 CCPU40.  Nineteen original translation units now compile directly;
the discarded generated wrapper carrier is not part of production composition.

The sole state-carrier overlay preserves the original `Gdp`, `Cpu`, and
`Video` declarations while preventing a duplicate `Sas` global already owned
by selected CCPU40.  Direct controller selection keeps original table ordering
and uses a same-shaped `effective_addr` facade over the selected CCPU address
provider.  Formal forced-link review has no unresolved S26 controller, C-VID,
or effective-address edge.

This is not a host display or guest-video claim.  The unresolved original
`InvalidateConsoleDIBits` endpoint belongs to S27; end-to-end machine behavior
remains mandatory S49 work.  Evidence: [S26 P1](../etc/evidence/m0-t310-s26-p1-video-controller-direct-source-boundary-001.md)
and [S26 P2](../etc/evidence/m0-t310-s26-p2-video-controller-formal-assembly-001.md).
