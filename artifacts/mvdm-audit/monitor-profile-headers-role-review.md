# Profile and host declaration checkpoint

The [monitor and HUNTER review](shard-3/monitor-legacy-interfaces-role-review.md)
and [video diagnostic review](shard-3/video-diagnostics-role-review.md)
identify four historical profile-specific headers. Their file roles are
excluded in the captured product profile, not the corresponding memory,
video, CPU or debugging capabilities.

The [DOS host declaration review](shard-2/dos-host-root-headers-review.md)
identifies three headers selected by the current worker build. These roles
are partial; BOP routing, reset and thread lifetime remain to be tested.
The [merge](monitor-profile-headers-role-merge.py) replays these observations
and verifies the frozen-ledger delta without changing guest or product code.
