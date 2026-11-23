# T484 S3 P20: original heartbeat and ICA producers

Two historical PowerShell generators are removed. The native worker target
`host-thread-boundaries`, exposed through tools/build-fixtures.cmd, links the
current product's obj/host/nt_timer.obj and obj/host/nt_eoi.obj with the existing
fixtures. Their runtime assertions remain unchanged. No parallel source list,
macro profile, original algorithm or guest binary is introduced.

The old ICA graph referenced an absent overlay bridge; current nt_eoi.c
already owns InitializeIcaLock/host_ica_lock/host_ica_unlock. The real current
object links and passes directly, without resurrecting that bridge. Historical
x64 syntax graphs are not treated as a supported CCPU product configuration;
the new entry explicitly accepts the actual x86 worker profile only.

`verify-host-thread-boundaries.ps1` builds via CMD and runs two bounded cases.
The heartbeat case observes the worker still waiting, suspends it, observes
it still waiting, then resumes it with STATUS_SUCCESS. It does not test the
DelayHeartBeat STATUS_TIMEOUT or STATUS_ALERTED branches. The ICA case proves
another thread cannot enter until unlock and rejects unexpected outbound
calls. Existing unselected external seams remain explicit test boundaries.
[Final result](../../../artifacts/build-migration/t484/s3-host-boundaries/result.json)
records both exit0 results and executable identities.

The adjacent cached full build and identity receipts record successful host
verification and11 raw/22 published hashes unchanged under package
d8983d2b-3fe1-4d18-8ea9-64613c84346c. No changed product image requires
republication. There is no new Full runtime or guest acceptance claim.
The containing commit delivers P20. Four literal Ninja generators remain;
inline test compilation and S4/S5 remain pending.
