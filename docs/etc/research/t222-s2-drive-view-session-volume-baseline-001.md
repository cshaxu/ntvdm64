# T222 S2 Drive-View Session Volume Baseline

## Question

Can the normal Direct/Readonly package-session fixture receive the same
immutable admitted volume view required by DPB, media and free-space services?

## Procedure And Observation

The fixture now captures `bx_ntvdm_host_volume_snapshot_v1` from its already
admitted drive snapshot during session setup, then binds it through
`dem_package_session_v1_set_volume_snapshot` before dispatch.  The existing
CWD and DTA assertions were rerun with MSVC x64 `/MT` in
`build/M0-T222-S2/024-drive-view-volume-baseline`; the linked executable
exited zero.

## Interpretation

Future `0E/10/25/46` assertions will consume one immutable session input,
not fixture-only volume data or ambient host queries.  This changes no service
result and does not itself prove those identities.
