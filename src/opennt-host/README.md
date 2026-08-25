# opennt-host

Independently composable OpenNT host-capability components: host algorithms and
their smallest source-derived replacements where a historical host facility no
longer exists. It does not own BOP dispatch or Bochs machine mechanics.

It may use declared same-shaped interfaces from `adapter-win32` and
`adapter-softpc`; it must not access `adapter-bochs` or `bochs-core` directly.

## Local intrusion register

Every changed imported OpenNT component and every new source-derived host
component is recorded here with original source identity, exception identifier,
unavailable dependency, retained observable semantics and focused verification.
The future static library name is `opennt-host`; its public include surface is
the explicitly declared OpenNT host ABI consumed by `opennt-bop` and `app`.

## Production layout and local divergence register

The live source is grouped by the OpenNT owner that composes it. Direct host
imports retain their original `softpc.new/host/...` path; remaining recovery
work is grouped under `dem/`, `redir/`, `top_level/` and `config/`. Dormant
original VDD, debugger and FAX packages do **not** belong in this component:
they remain in the external OpenNT baseline until a reached provider is
admitted for composition. The T263/S5 bulk preservation admission was
withdrawn by
`docs/etc/evidence/m0-t263-s7-unconnected-host-package-withdrawal-001.md`.
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
| `HOST-DIV-014` | `nt_error.c` formats its prompt from private `ntvdm.exe` resources. | The NT4 resource table is not an unpack-and-run input. | Preserve the original two-`LoadString`/`sprintf` control flow through a public-Win32 resource façade supplying the reached original `D_A_*` strings; the historically absent `D_A_OLDPIF` retains the original fallback result. | `softpc.new/host/src/nt_error.c`, `adapter-win32/facade/opennt_error_dialog_facade.c` |
| `HOST-DIV-026` | `nt_error.c` transports a `DWORD` direct-access category bit mask through `LPVOID` with 32-bit casts. | Those casts truncate or warn under the supported x64 build. | Use `ULONG_PTR` only at the two transport casts; the original `DWORD` mask and decision ordering remain unchanged. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-027` | `nt_error.c` also contains general SoftPC error UI, WOW dialog and reset/timer product-shell routines. | The admitted route reaches only `host_direct_access_error`; the other complete routines require unadmitted NT4 product facilities. | Keep the original direct-access function and its failure decision as a true subset; do not synthesize the unconnected product-shell paths. | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-015` | `vdmredir/vrnmpipe.c` registers each synchronous `OVERLAPPED_PIPE_IO` with the NT4 VDD cancellation list. | The standalone product has no VDD product-global cancellation table, and a native pointer may not leave its bounded call. | The retained original synchronous I/O order omits only the registration/unregistration calls; cancellation remains unavailable until its asynchronous Redirector/VDD owner is admitted. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-016` | `vdmredir/vrnmpipe.c` closes a timed-out named-pipe `HANDLE` directly. | Native handle lifetime belongs to the session-owned handle manager; an original provider cannot invalidate its opaque owner without corrupting the manager. | Preserve issue/wait/result/error order and report timeout; bounded session teardown performs the sole native close and metadata retirement. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-017` | `vdmredir/vrinit.c` exports `VrInitialized` from the separately loaded VDMREDIR DLL. | Static linking would collide with the directly imported DEM `VrInitialized` function-pointer import. | Preserve the original pointer ABI in `opennt-bop`; give only the statically linked provider body a private link name declared at its BOP composition use site. | `vdmredir/vrinit.c`, `opennt-bop/dem/opennt_demfile_composition.c` |
| `HOST-DIV-018` | `vdmredir/vrputil.c` includes NT4 product-composition headers and optional `VR_DIAGNOSE` logging. | The reached utility routines do not call those product services; admitting empty facades would misstate their availability. | Keep only the public Win32/LAN Manager/character interfaces actually used by the provider and crop the uncalled include/diagnostic block at its original location. | `vdmredir/vrputil.c` |
| `HOST-DIV-019` | `VrpTranslateDosNetPath` uses an assignment expression in its loop test. | Current warning-as-error builds diagnose the unparenthesized form, while `toupper` also requires a defined unsigned-char input. | Parenthesize the identical test and cast only the `toupper` argument; traversal and failure order remain unchanged. | `vdmredir/vrputil.c:49` |
| `HOST-DIV-020` | `VrConvertLocalNtPipeName` narrows flat pointers before subtraction, assigns `strlen` directly to `DWORD`, and uses an assignment condition. | Those spellings are invalid or warning-as-error failures on the required x64 build. | Retain the identical prefix-length/result values with defined pointer arithmetic, an explicit length cast and a parenthesized assignment test. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-022` | OpenNT `VrInitialize`/`VrUninitialize` compose VDD hooks, NetBIOS, DLC, ICA queues and VDM load-info guest state. | Those coupled NT4 product services have no admitted helper-level recovery. | Delete those whole unadmitted functions and their dedicated product data; retain only the original `VrInitialized` provider required by the current DEM import. No static success gate remains. | `vdmredir/vrinit.c` |
| `HOST-DIV-023` | `VrReadNamedPipe` leaves its local byte-count uninitialized until `GetOverlappedResult`. | The required `/W4 /WX` build correctly identifies a possible read after a failed provider result. | Initialize only that local to zero; successful-transfer result/error ordering remains OpenNT's. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-024` | `vrnmpipe.h` selects DLL-export declarations under `VDMREDIR_DLL`. | The recovered provider is statically linked, rather than loaded as the historical VDMREDIR DLL. | Define the original selector only in the static named-pipe provider/consumer translation units; retain the imported header byte-for-byte and keep project binding declarations outside it. | `inc/vrnmpipe.h`, `vdmredir/vrnmpipe.c` |
| `HOST-DIV-025` | `vrnmpipe.c` obtains a small set of path/allocation/assertion manifests from broad NT4 CCPU/VDM product headers. | Those headers would import unconnected guest-pointer, asynchronous Redirector and CCPU contracts into a synchronous host provider. | Retain only the exact pure manifests used by the reached named-pipe bodies, locally and visibly, rather than supplying empty product APIs. | `vdmredir/vrnmpipe.c` |
| `HOST-DIV-028` | `vrnmpipe.c` contains named-pipe state/transact/call/wait services and asynchronous I/O, interrupt, termination and cancellation machinery. | The current host provider has only the independent synchronous open/read/write/path/list route; the omitted services require separate Redirector/VDD lifecycle admission. | Retain the complete synchronous block and leave omitted BOP entry disposition to `opennt-bop`; do not provide local success stubs. | `vdmredir/vrnmpipe.c` |

The exact OpenNT `nt_pif.c` parser was transferred to
`opennt-bop/softpc.new/host/src/` by T269/S3 because its only current consumer
is the COMMAND PIF translation unit and its historical `PIF_DATA` layout is
not an independently composable host ABI. The source body remains byte-for-byte
original; the transfer removes the former cross-component `.c` inclusion.
## T265 S5 private-overlay disposition

The following registered exceptions retain only a one-include boundary in the
OpenNT mirror; their previously in-file multi-line binding/declaration blocks
now live in `opennt-host-overlay`, which has no external consumer.

| IDs | Private overlay fragment | Mirror boundary |
| --- | --- | --- |
| `HOST-DIV-012`, `HOST-DIV-014` | `../opennt-host-overlay/softpc.new/host/src/nt_error_bindings.h` | `softpc.new/host/src/nt_error.c` |
| `HOST-DIV-017` | `../opennt-host-overlay/vdmredir/vrinit_bindings.h` | `vdmredir/vrinit.c` |
| `HOST-DIV-024`, `HOST-DIV-025` | `../opennt-host-overlay/vdmredir/vrnmpipe_product_seams.h` | `vdmredir/vrnmpipe.c` |
