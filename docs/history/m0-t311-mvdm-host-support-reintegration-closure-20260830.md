# M0 T311 closure — MVDM host support reintegration

T311 restored the selected original MVDM support paths beneath
`src/mvdm-host`, eliminating the separate `mvdm-support` and
`mvdm-support-overlay` component roots. It preserved source history with
`git mv`, rehomed all divergence registrations, corrected duplicate register
entries, updated live include/build paths and completed the selected
Win32/x86 CCPU40 formal graph and forced-link audit.

Detailed provenance, verification and remaining unrelated link ownership are
recorded in
`docs/etc/operations/m0-t311-s2-s3-mvdm-host-support-reroot-closure-001.md`.
