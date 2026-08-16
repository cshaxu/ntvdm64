# T221 S3 FCB Overlay lifecycle foundation

`bx_ntvdm_dem_fcb_overlay_backend_v1` now composes an admitted read-only base
file into a private Overlay token, then returns its COW size and DOS time/date
through typed session/backend validation. Its successful close uses the same
Overlay handle backend. It contains no BOP decode, guest memory access, or
host write operation.

Whole-provider MSVC x64 `/MT` evidence passed at
`build/M0-T221-S3/065`. This is a lifecycle prerequisite only; `50:2C..31`
remain unbound pending complete result-layout and checked-DTA integration.
