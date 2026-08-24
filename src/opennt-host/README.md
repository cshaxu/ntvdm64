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

The live source is grouped by the OpenNT owner that composes it. Direct host
imports retain their original `softpc.new/host/...` path; the original
optional host extension packages `bde/`, `dbg/`, `fax/`, `ieuvddex/`, `vdd/`,
`vdmdbg/` and `vdmexts/` retain their original package paths. Their 138-file
source-identical import and non-link disposition are recorded in
`docs/etc/evidence/m0-t263-s5-opennt-host-extension-package-admission-001.md`.
Remaining recovery work is grouped under `dem/`, `redir/`, `top_level/` and
`config/`.
COMMAND BOP/session composition is intentionally owned by `opennt-bop`, not
this component.  The former generic `capability/` staging directory is not a
product category.
Every source-derived provider remains in its owner group, with each necessary
departure from an identified OpenNT definition marked in code as
`DIVERGENCE:` and registered below.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `HOST-DIV-012` | `nt_error.c` uses the NTVDM product's `TlsDirectError` slot to suppress repeated direct-access warnings per thread. | The standalone session has no NT4 product TLS initialization. | The public-Win32 dialog facade owns the session-resettable per-thread category state; `nt_error.c` keeps the original decision point and `VOID` result. | `softpc.new/host/src/nt_error.c`, `adapter-win32/facade/opennt_error_dialog_facade.c` |
| `HOST-DIV-013` | `nt_error.c` shifts a direct-access category into that bitset. | An arbitrary modern guest AX must not invoke an undefined C shift. | Preserve the original 0..6 result and leave other categories observable without a bitset index, inside the state-owning facade. | `softpc.new/host/src/nt_error.c`, `adapter-win32/facade/opennt_error_dialog_facade.c` |
| `HOST-DIV-014` | `nt_error.c` formats its prompt from private `ntvdm.exe` resources. | The NT4 resource table is not an unpack-and-run input. | One source-order message construction point supplies the same category to the public-Win32 dialog façade. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-015` | `vdmredir/vrnmpipe.c` registers each synchronous `OVERLAPPED_PIPE_IO` with the NT4 VDD cancellation list. | The standalone product has no VDD product-global cancellation table, and a native pointer may not leave its bounded call. | The re-rooted helper retains OpenNT's overlapped issue/wait/result/error order; its session ingress owns cancellation outside this source call. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-016` | `vdmredir/vrnmpipe.c` closes a timed-out named-pipe `HANDLE` directly. | Native handle lifetime belongs to the session-owned handle manager; an original provider cannot invalidate its opaque owner without corrupting the manager. | Preserve issue/wait/result/error order and report timeout; bounded session teardown performs the sole native close and metadata retirement. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-017` | `vdmredir/vrinit.c` exports `VrInitialized` from the separately loaded VDMREDIR DLL. | Static linking would collide with the directly imported DEM `VrInitialized` function-pointer import. | Preserve the original pointer ABI in `opennt-bop`; give only the statically linked provider body a private link name declared at its BOP composition use site. | `vdmredir/vrinit.c`, `opennt-bop/dem/opennt_demfile_composition.c` |
| `HOST-DIV-018` | `vdmredir/vrputil.c` imports NT4 private headers for its Win32 error/path utilities. | Those product headers are not independently composable. | Retain the original routine bodies using only the public Win32/LAN Manager declarations they require. | `vdmredir/vrputil.c:7` |
| `HOST-DIV-019` | `VrpTranslateDosNetPath` uses an assignment expression in its loop test. | Current warning-as-error builds diagnose the unparenthesized form, while `toupper` also requires a defined unsigned-char input. | Parenthesize the identical test and cast only the `toupper` argument; traversal and failure order remain unchanged. | `vdmredir/vrputil.c:49` |
| `HOST-DIV-020` | `VrConvertLocalNtPipeName` narrows flat pointers before subtraction, assigns `strlen` directly to `DWORD`, and uses an assignment condition. | Those spellings are invalid or warning-as-error failures on the required x64 build. | Retain the identical prefix-length/result values with defined pointer arithmetic, an explicit length cast and a parenthesized assignment test. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-021` | OpenNT `VrTerminateNamedPipes(DosPdb)` is an empty historical lifecycle hook. | The static single-session composition must release its private named-pipe metadata during teardown. | Retain the original `DosPdb` ABI; zero means composition teardown and otherwise retires only matching source-owned metadata. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-022` | OpenNT `VrInitialize`/`VrUninitialize` compose VDD hooks, NetBIOS, DLC, ICA queues and VDM load-info guest state. | Those coupled NT4 product services have no admitted helper-level recovery. | Retain only the source-derived static single-session provider gate; it does not report VDD/ICA/NetBIOS/DLC capability. | `vdmredir/vrinit.c` |
| `HOST-DIV-023` | `VrReadNamedPipe` leaves its local byte-count uninitialized until `GetOverlappedResult`. | The required `/W4 /WX` build correctly identifies a possible read after a failed provider result. | Initialize only that local to zero; successful-transfer result/error ordering remains OpenNT's. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-024` | `vrnmpipe.h` selects DLL-export declarations under `VDMREDIR_DLL`. | The recovered provider is statically linked, rather than loaded as the historical VDMREDIR DLL. | Define the original selector only in static provider/consumer translation units; retain the imported header byte-for-byte and keep project binding declarations outside it. | `inc/vrnmpipe.h`, `vdmredir/vrnmpipe.c`, `vdmredir/vrinit.c` |

The pre-T260 PIF original and other uncompiled historical inputs are evidence,
not a second provider. They reside under `docs/etc/legacy_code/opennt-host/`.
