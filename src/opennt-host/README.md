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

The live source is grouped by the OpenNT owner that composes it.  Direct host
imports retain their original `softpc.new/host/...` path; remaining recovery
work is grouped under `dem/`, `xms/`, `redir/`, `top_level/` and `config/`.
COMMAND BOP/session composition is intentionally owned by `opennt-bop`, not
this component.  The former generic `capability/` staging directory is not a
product category.
Every source-derived provider remains in its owner group, with each necessary
departure from an identified OpenNT definition marked in code as
`DIVERGENCE:` and registered below.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `HOST-DIV-006` | DEM creates a VDD user hook. | It requires the NT4 VDD broker. | Preserve the call boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:60` |
| `HOST-DIV-007` | DEM emits VDD/debugger module notifications. | They are not DOS loader actions and their receiver is unavailable. | Preserve the boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:66` |
| `HOST-DIV-008` | DEM asks the debugger console for guest-visible input. | That needs a debugger-owned input lifecycle. | Preserve the boundary as an explicit deferred no-op. | `dem/demmisc_shim.c:76` |
| `HOST-DIV-009` | DEM resets host floppy hardware. | FDC/DMA/CMOS is a machine owner responsibility. | Do not report a fabricated reset. | `dem/demmisc_shim.c:87` |
| `HOST-DIV-010` | FCB search terminates VDD user hooks. | The private VDD callback broker is absent. | Preserve the boundary as an explicit deferred no-op. | `dem/demsrch_fcb_shim.c:64` |

The pre-T260 PIF original and other uncompiled historical inputs are evidence,
not a second provider. They reside under `docs/etc/legacy_code/opennt-host/`.
