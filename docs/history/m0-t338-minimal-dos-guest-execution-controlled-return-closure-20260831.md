# M0 T338 — Minimal DOS guest execution and controlled return closure

T338 does not claim execution of a declared DOS child, `EXEC`, parent PSP
restoration or COMMAND `54:0B` result return.

It closed the reopened configuration-layout question with an exact original
source predicate. `config.nt` is normally processed through
`cmdGetConfigSys -> ExpandConfigFiles`; the former fixed container failed only
because its executable-relative `mvdm` root occupied 64 characters and
violated the original 64-byte `achSysRoot` capacity. A byte-identical stage
whose root is 38 characters crosses the configuration cohort and reaches the
already known original `50:3D -> demExitVDM` boundary.

The next vertical package therefore starts from that DEM/guest-control owner,
using the short-root fixed container. It must not replace COMMAND, skip
`config.nt`, invent a guest loader, or treat configuration BOPs as a leaf
patch queue. Long executable-relative package-path disposition remains a
separate release/package concern.

Evidence: [S2 continuation attribution](../etc/evidence/m0-t338-s2-post-configuration-continuation-attribution-001.md)
and [S3 path-capacity transfer](../etc/evidence/m0-t338-s3-original-configuration-path-capacity-transfer-001.md).
