# T211 S2 P1 — COMMAND bootstrap/environment/config provider boundary

## Decision

`cmddisp.c` services `54:01`, `54:02`, `54:04`, `54:05`, `54:0C`,
`54:0D`, `54:0F`, and `54:10` now have one adapter-owned state boundary:
`bx_ntvdm_command_bootstrap_provider_v1`.  It is a COMMAND owner-package
component, not a Bochs feature and not a selector recognizer.

The provider owns the copied COMSPEC/environment state, declared-command
delivery state, `CMDINFO` locator registration, and configuration input
state.  The COMMAND session remains responsible only for the fixed-width
guest-memory transaction and for invoking the provider-selected component.

## Source and disposition

The component membership follows the T211 S1 map and the original
`base/mvdm/dos/command/cmddisp.c` array:

| Service | Original handler | P1 disposition |
| --- | --- | --- |
| `01` | `cmdGetNextCmd` | declared CLI command/session provider state |
| `02` | `cmdComSpec` | copied COMSPEC provider state |
| `04` | `cmdGetCurrentDir` | T210 shared host-context projection |
| `05` | `cmdSetInfo` | checked `CMDINFO` locator registration |
| `0C`, `0D` | config/AUTOEXEC pathname handlers | T209 configuration input state |
| `0F` | initial environment | copied environment provider state |
| `10` | `cmdGetStartInfo` | explicit deferred member: original reads historical `DosSessionId`; no CLI session/console ABI is admitted to manufacture `AL` |

Thus `54:10` is owned and visible in the component but is not credited as
implemented.  Its historical dependency is an audit item, not a reason to
block the other seven members.

The previously shared host-child execution seam has also been narrowed to
`54:08 cmdExec`.  It no longer accepts `54:10 cmdExecComspec32`, so an
unadmitted host child cannot bypass the provider's explicit deferred
disposition.

## Migration

`bx_ntvdm_command_package_session_v1` no longer owns separate `bootstrap`,
`launch`, `get_next`, or `boot_input` fields.  They are fields of the one
versioned provider.  `bx_ntvdm_command_plane_v1` classifies all eight as
`BX_NTVDM_COMMAND_COMPONENT_BOOTSTRAP`; the old split between launch,
configuration, environment and session components is not used for this
owner-package boundary.

No OpenNT source is compiled into bx-vdm, and no Bochs source or BOP meaning
is added to bx-core/bx-mantle.

## Reproducible P1 evidence

Both builds use MSVC x64 `/MT` with the existing isolated bx-vdm composition
closure.

1. `Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture boot-namespace`
   at `artifacts/build/t211-s2-command-bootstrap-r1` passed.  It exercises
   the migrated COMMAND composition alongside its bounded guest-memory
   transaction paths.
2. `Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture command-bootstrap`
   at `artifacts/build/t211-s2-command-bootstrap-r2` passed.  Its provider
   fixture enumerates every `54:00..10` identity, verifies the exact eight
   memberships and rejects the dispatcher sentinel `54:11`.

This is P1 progress only.  S2 still needs an explicit component-level
success/failure/no-context/malformed-gather matrix and a final owner-package
regression before closure.  It does not authorize trace-led work.
