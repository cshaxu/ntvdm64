# M0 T280 S14 — DEM find-list host-resource binding audit

Inventory every original `demsrch.c` cast that persists a find-list pointer in
a 32-bit field. Design one same-shaped binding through the existing session
`host_resource` mapping instance: allocation, reverse lookup, stale handling,
teardown and failure must preserve the original caller's observable branches.
No raw pointer crosses the boundary; guest DTA addresses remain separate
`guest_memory` leases through adapter-softpc.
