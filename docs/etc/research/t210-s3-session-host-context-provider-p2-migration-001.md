# T210 S3 P2 — DEM/COMMAND host-context migration

## Question

Can existing DEM CWD users and COMMAND current-directory publication consume
the one T210 session provider without retaining a second state store or a
synthetic-root success path?

## Change

`bx_ntvdm_dem_cwd_context_v1` is now a compatibility name and thin API
wrapper over `bx_ntvdm_session_host_context_v1`; it owns no independent
profile, overlay or per-drive state.  Existing DEM path/file callers retain
their source-compatible type while using the shared provider.

The boot-namespace composition binds COMMAND to the same DEM-owned session
context when its DEM mutation profile is admitted.  CLI admission of an
already copied COMMAND context seeds the provider without performing a BOP or
host operation.  Later direct BOP CWD changes retain the provider's validated
`SetCurrentDirectoryW` contract.

`cmd_current_dir_service_v1` no longer invents a root directory when its
copied context is absent or names another drive.  It returns a bounded
carry/error result.  COMMAND `54:04` dynamically creates its copied context
from the shared provider, while `54:02` and `54:0F` retain the existing
fixed-width copied consumer ABI.

## Procedure and observations

- `Invoke-T210S3SessionHostContextProbe.ps1` r6 passed under MSVC x64 `/MT`.
- `Invoke-T202S2DemCwdServiceProbe.ps1` r2 passed after the compatibility
  migration.
- The full source closure
  `Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture boot-namespace` r2
  compiled, linked and ran with `passed: true`.  It proves composition retains
  the CLI-admitted `C:\\NTDOS64` response through the shared projection.
- A focused MSVC x64 `/MT` current-directory service build at
  `artifacts/build/t210-s3-cmd-current-dir-r2` passed, including the new
  admitted-but-different-drive negative case.

The same full source closure with `-Fixture dem-package` compiled and linked
but returned 197.  That fixture deliberately installs no direct host namespace
while expecting a successful direct `50:1A`; its own comment calls that an
unavailable CWD backend.  This is a real remaining package-integration
admission gap, not a provider pass, and is not counted as success.

## Interpretation and follow-up

The duplicate CWD state and COMMAND synthetic-root workaround are migrated.
S3 remains active because a direct namespace admission fixture and the full
`13/18/1A` plus `54:02/04/0F` family regression still need to run against one
actual CLI-admitted host context.  No native guest trace was used.
