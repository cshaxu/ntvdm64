# T230 S15 P1 v1 DEM route-retirement map

## Finding

The active native engine currently installs
`bop-v1/bx_ntvdm_composition_runtime_v1` before beginning a machine stage.
That runtime is not a DEM-only object: it creates the startup image, host
namespace, command context, boot-namespace composition and native BOP
composition.  Deleting it or its directory would therefore remove unrelated
startup and non-DEM functionality, not merely retire DEM providers.

## Live route map

| Producer | Current dependency | Classification | S15 disposition |
| --- | --- | --- | --- |
| `bx_ntvdm_engine_run_v1` | `composition_runtime_v1_install...`, stage request/entry, reset | Startup image and lifecycle | Retain temporarily; it must not select DEM after P4. |
| `composition_runtime_v1` | `boot_namespace_composition_v1` | Startup, COMMAND and legacy DEM package | Retain only as non-DEM fallback until its startup/COMMAND replacement has its own task; block its DEM route first. |
| `composition_runtime_v1` | `native_bop_composition_v1` | XMS/DPMI/machine/non-DEM BOP composition | Retain temporarily; block DEM first. |
| `vdm_generic_ud_bridge_v1` | boot namespace handler, then native handler | Active generic-UD order | Insert Direct DEM session before both handlers. |
| `bop-v1` DEM package/facade/provider/result code | reached through legacy handlers after BOP classification | DEM-only route | Make unreachable for every valid `C4 C4 50 00..48` window, then remove from formal membership. |

## Required replacement seam

The existing `dem_native_session_shim` already consumes a copied generic-UD
event and accepts only `C4 C4 50 <service>`.  It lacks two composition inputs:
an active-machine checked-RAM callback pair and a direct host-handle/token
session.  These are P2/P3.  They must be injected before the v1 generic
bridge handlers, so a valid DEM event cannot fall through into v1.

## Non-findings

No Bochs selector/BOP recognition is needed.  The new mantle API is strictly
ordinary-RAM read/write; selector/service recognition remains in bx-vdm's
Direct DEM ingress.  No original OpenNT semantics are delegated to mantle.
