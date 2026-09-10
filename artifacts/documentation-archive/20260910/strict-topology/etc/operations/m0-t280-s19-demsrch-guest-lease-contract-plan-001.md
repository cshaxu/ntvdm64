# M0 T280 S19 — DEM `GetVDMAddr` guest-lease contract audit

Audit the complete original `demsrch.c` `GetVDMAddr`/segmented-address use
surface before adding any source-facing `adapter-softpc` implementation. The
audit must prove each source dereference's access direction, minimum span,
call lifetime, error/failure path and exact relationship to DTA, FCB and DOS
filename data.

The result chooses one same-shaped bounded synchronous lease contract over the
already existing `session.guest_memory` instance. It must not turn guest
addresses into opaque host-resource tokens, expose a raw Bochs pointer, retain
a pointer in a worker/async record, or enable a DEM BOP provider. Actual
lease implementation and source admission remain subsequent work after this
whole-source contract is complete.
