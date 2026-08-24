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
work is grouped under `dem/`, `redir/`, `top_level/` and `config/`.
COMMAND BOP/session composition is intentionally owned by `opennt-bop`, not
this component.  The former generic `capability/` staging directory is not a
product category.
Every source-derived provider remains in its owner group, with each necessary
departure from an identified OpenNT definition marked in code as
`DIVERGENCE:` and registered below.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `HOST-DIV-012` | `nt_error.c` uses the NTVDM product's `TlsDirectError` slot to suppress repeated direct-access warnings per thread. | The standalone session has no NT4 product TLS initialization. | Compiler TLS retains the same per-thread category bitset; it installs no host state. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-013` | `nt_error.c` shifts a direct-access category into that bitset. | An arbitrary modern guest AX must not invoke an undefined C shift. | Preserve the original 0..6 result and leave other categories observable without a bitset index. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-014` | `nt_error.c` formats its prompt from private `ntvdm.exe` resources. | The NT4 resource table is not an unpack-and-run input. | One source-order message construction point supplies the same category to the public-Win32 dialog façade. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-015` | `vdmredir/vrnmpipe.c` registers each synchronous `OVERLAPPED_PIPE_IO` with the NT4 VDD cancellation list. | The standalone product has no VDD product-global cancellation table, and a native pointer may not leave its bounded call. | The re-rooted helper retains OpenNT's overlapped issue/wait/result/error order; its session ingress owns cancellation outside this source call. | `vdmredir/vrnmpipe.c` |

The pre-T260 PIF original and other uncompiled historical inputs are evidence,
not a second provider. They reside under `docs/etc/legacy_code/opennt-host/`.
