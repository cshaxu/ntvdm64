# M0 T458 S4 — consumer acceptance and bounded closure

**Status:** closed with explicit pre-provider environment non-passes; no
production source, package or guest-media change.

The existing bound `menu-resource` consumer suite was run against the formal
x86 provider and worker graphs in `build/t458-s4/menu-resource-r1`.  It
compiled the current production objects plus original menu resource/thunk
fixtures, then stopped before any provider call when its isolated page-domain
desktop creation returned `ERROR_ACCESS_DENIED` (5):
`WOW_PAGE_DOMAIN_DESKTOP_UNAVAILABLE error=5`.  It is therefore neither menu
nor resource acceptance evidence, and is not attributed to T458 code.

The same discipline applies to S2's retained private-desktop WINMINE observer:
that run stopped before worker registration.  In contrast, the directly
reachable selected boundaries pass their actual extracted-original caller and
alias cohorts: S2 resource conversion (`cursor-r16`) and S3 accelerator
conversion/retirement (`accelerator-r6`).  The cached formal x86 WOW32 build
is current and selects the same unchanged original providers.

No source-proven defect or competing implementation was found.  T447 remains
the exclusive MENU-01 graph/parser/mutation/owner-draw owner; its checked-in
bound resource-client test remains the applicable consumer implementation.
T458 adds no product source and therefore has no new package to publish.

This closes only Queue W09's residual resource and accelerator ownership
boundary.  It does **not** claim broad interactive application acceptance;
that remains in the separately queued whole-provider/usability packages.
