# T194 S19 CLI drive-snapshot provider integration 001

Date: 2026-08-12  
Packet: M0 T194 S19  
Disposition: adapter selection path source-built; focused policy/provider tests
pass; broad retained runtime fixture is an unclassified limitation.

## Change

`bx_ntvdm_adapter_runtime.c` now routes `50:0F` through the existing
`bx_ntvdm_dem_drive_provider_v1` using only the already-installed immutable
`bx_ntvdm_host_drive_snapshot_v1`. The route still classifies through common
BOP ingress, provider registry and DEM plane records. It runs before the
retained static-profile inventory branch, so a normal CLI session cannot have
its include/exclude/default policy overridden by optional profile data.

No host query occurs at BOP time; capture remains at session installation.
No Bochs, profile grammar, filesystem, DPB, `50:46`, or guest input changed.

The CMake test graph now names the pre-existing drive-provider unit explicitly
and includes that provider in the runtime-test closure. This is a build-closure
correction, not a new runtime component.

## Verification

- `bx-ntvdm-host-drive-policy-test` passed, covering include-only,
  exclude-only, conflict precedence, default visibility and malformed input.
- `bx-ntvdm-dem-drive-provider-v1-test` passed, proving common
  ingress/registry/plane routing, four-byte resume, AX count and CF clear for
  snapshot-backed contiguous and gap cases.
- `bx-ntvdm-adapter-runtime-test` source-built with the integrated provider,
  but its broad default fixture returned `1` without a diagnostic marker. It
  was not a previously established passing test in this task sequence, and it
  contains unrelated legacy profile/namespace/observation assertions. It is
  therefore retained as a failed, unclassified check—not used as acceptance.

## Follow-up

T194 S20 must add one narrow runtime-routing fixture that installs a controlled
snapshot and proves `50:0F` consumes it rather than a static profile value.
It must not change provider semantics, run the engine, or repair unrelated
legacy fixture assertions.
