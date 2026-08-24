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
| `HOST-DIV-010` | DEM uses the NT4 VDM process drive/current-directory environment. | CLI admission is session-owned and must not mutate global process state merely to assemble a guest. | Retain the bounded include/exclude-drive snapshot as a host capability; original DEM callers receive its existing failure before a host-volume query. | `dem/bx_ntvdm_host_drive_policy.c`, `dem/bx_ntvdm_host_drive_policy.h` |
| `HOST-DIV-011` | DEM calls NT4 host namespace, find and file providers. | The historical VDM namespace/product shell is not independently composable. | Retain the bounded direct-host namespace capability with OpenNT-shaped OEM/find facades at its callers; it owns no BOP decode or guest-memory mapping. | `dem/bx_ntvdm_host_namespace.c`, `dem/bx_ntvdm_host_namespace.h` |

The pre-T260 PIF original and other uncompiled historical inputs are evidence,
not a second provider. They reside under `docs/etc/legacy_code/opennt-host/`.
