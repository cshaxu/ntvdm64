# T198 S105 DEM fast-I/O disposition map 001

## Question

Should the observed DEM `50:42` boundary be a source-derived host byte-copy
provider, or does the original OpenNT composition prescribe another result?

## Original source map

| Selector | OpenNT definition | NTDOS caller and continuation | Historical DEM table | Historical outcome |
| --- | --- | --- | --- | --- |
| `50:42` | `SVC_DEMFASTREAD`, `src/opennt/base/mvdm/inc/dossvc.h:158` | `handle.asm:448-466`: establish BX:SI position and BP:AX NT handle, test `sf_nt_seek` into ZF, call service, then `jnc dor2`; CF set selects ordinary `SVC_DEMREAD` | `demdisp.c:165` is `demNotYetImplemented` | `demNotYetImplemented` at `demdisp.c:230-238` clears CF only; GPRs are untouched. |
| `50:43` | `SVC_DEMFASTWRITE`, `dossvc.h:159` | `handle.asm:477-485`: same `FastOrSlow` split, ZF transfer, `jnc do_w1`; CF set selects ordinary `SVC_DEMWRITE` | `demdisp.c:166` is `demNotYetImplemented` | same CF-clear, GPR-preserving no-op. |

`nt_bop.c:144-154` supplies the top-level historical mechanics: selector 50
reads exactly the service byte, calls `DemDispatch`, then advances IP by that
byte. Therefore the adapter equivalent must resume after the four copied BOP
bytes, not separately emulate the old `setIP(+1)` implementation detail.

## Current composition map

The generic bridge delegates to the active composition before its passive
recorder (`tests/bx-vdm/t198_s23_native_ntio_boundary_bridge.c:165`); the
recorder can therefore establish only a decline, not a provider result.

The current namespace plane invokes
`bx_ntvdm_dem_fastio_provider_v1_dispatch` for `50:42`
(`bx_ntvdm_boot_namespace_plane_v1.c:81`). That dispatch reaches
`bx_ntvdm_dem_readonly_file_v1_fast_read`, which performs host-namespace
seek/read and guest-memory bulk write preparation. This is a source-derived
byte-copy service and is inconsistent with the original two-entry dispatcher
table above. `50:43` currently has no corresponding active byte-copy route.

S104's one live event had token `00004005h`, ZF set and CF clear. Since it
declined before any outcome, it cannot authorize the current byte-copy route;
it only proves the historical no-op family is reached by the selected profile.

## Disposition

`50:42` and `50:43` form one historical DEM fast-I/O **no-op family**. Their
correct bounded replacement is a source-derived adapter plane that validates
only the copied BOP event and emits typed resume with CF clear and no GPR or
guest-memory effects. It must replace—not supplement—the active fast-read
byte-copy route. This conclusion is direct source evidence for the dispatcher
and caller branch, with high confidence; whether a later profile needs a
different independently evidenced fast-I/O implementation is explicitly
deferred.
