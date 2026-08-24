# opennt-host

Independently composable OpenNT host-capability components: host algorithms and
their smallest source-derived replacements where a historical host facility no
longer exists. It does not own BOP dispatch or Bochs machine mechanics.

It may use declared same-shaped interfaces from `adapter-win32` and
`adapter-softpc`; it must not access `bx-mantle` or `bx-core` directly.

## Local intrusion register

Every changed imported OpenNT component and every new source-derived host
component is recorded here with original source identity, exception identifier,
unavailable dependency, retained observable semantics and focused verification.
The future static library name is `opennt-host`; its public include surface is
the explicitly declared OpenNT host ABI consumed by `opennt-bop` and `app`.

## Production layout and local divergence register

The live source is grouped by the OpenNT owner that composes it:
`command/`, `dem/`, `xms/`, `redir/`, `top_level/` and `config/`.  The
former generic `capability/` staging directory is not a product category.
Every source-derived provider remains in its owner group, with each necessary
departure from an identified OpenNT definition marked in code as
`DIVERGENCE:` and registered below.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `HOST-DIV-001` | COMMAND uses `RtlNtStatusToDosError` directly from NT4 `ntdll`. | Direct historical import is not a stable application contract. | Resolve the same-named modern export once through `adapter-win32`. | `command/command_binary_shim.c:78` |
| `HOST-DIV-002` | `nt_lpt.c` clears its parallel-port open state. | This composition has no admitted LPT queue/controller model. | Do nothing; never claim a controller flush occurred. | `command/command_misc_shim.c:297` |
| `HOST-DIV-003` | COMMAND publishes console bytes through BaseSrv/CSR. | The NT4 product service is not independently composable. | Retain the copied multisz session-publication contract. | `command/command_misc_shim.c:307` |
| `HOST-DIV-004` | COMMAND probes a legacy 1 KiB DOS environment span. | That was an old adapter limit, not the provider contract. | Read the exact bounded multisz extent. | `command/command_misc_shim.c:866` |
| `HOST-DIV-005` | COMMAND materializes SAS pointers. | Raw/aliased host access is unsafe. | Use bounded copied spans with alias rejection. | `command/command_misc_shim.c:908` |
| `HOST-DIV-006` | DEM creates a VDD user hook. | It requires the NT4 VDD broker. | Preserve the call boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:60` |
| `HOST-DIV-007` | DEM emits VDD/debugger module notifications. | They are not DOS loader actions and their receiver is unavailable. | Preserve the boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:66` |
| `HOST-DIV-008` | DEM asks the debugger console for guest-visible input. | That needs a debugger-owned input lifecycle. | Preserve the boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:76` |
| `HOST-DIV-009` | DEM resets host floppy hardware. | FDC/DMA/CMOS is a machine owner responsibility. | Do not report a fabricated reset. | `dem/demmisc_shim.c:87` |
| `HOST-DIV-010` | FCB search terminates VDD user hooks. | The private VDD callback broker is absent. | Preserve the boundary as an explicit deferred no-op. | `dem/demsrch_fcb_shim.c:64` |
| `HOST-DIV-011` | `nt_pif.c` is composed from the historical global host tree. | Its `PIF_DATA` layout is not identical to COMMAND's copied `cmdpif.h` layout, so linking the historical global directly would cross incompatible declarations. | Keep `nt_pif.c` and its headers byte-identical at their re-rooted `softpc.new/host` paths; a separately named COMMAND composition bridge translates only the layout-identical result and does not replace parser logic. | `command/command_opennt_pif_parser.c:33`, `softpc.new/host/src/nt_pif.c`, `softpc.new/host/inc/nt_pif.h`, `softpc.new/host/inc/nt_uis.h` |

The pre-T260 PIF original and other uncompiled historical inputs are evidence,
not a second provider. They reside under `docs/etc/legacy_code/opennt-host/`.
