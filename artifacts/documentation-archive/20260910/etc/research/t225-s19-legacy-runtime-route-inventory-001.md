# T225 S19 Legacy Runtime Route Inventory 001

## Question

Which remaining DEM routes are live in the formal CLI/native product graph, and
which are retained only in pre-governance adapter-runtime derivative tooling?

## Inputs

- Formal module manifest: `tools/build/t225-s7-full-module-manifest.json`.
- Product installation: `src/bx-mantle/bx_ntvdm_engine_run_v1.c` and
  `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`.
- Product BOP composition: `src/bx-vdm/bx_ntvdm_boot_namespace_composition_v1.c`.
- Historical runtime: `src/bx-vdm/bx_ntvdm_adapter_runtime.c` and retained
  `tools/build/Configure-Bochs26NativeContainer.ps1` / T188--T190 derivative tools.

## Observations

1. The formal manifest includes `bx_ntvdm_composition_runtime_v1.c`,
   `bx_ntvdm_boot_namespace_composition_v1.c`, and
   `bx_ntvdm_native_bop_composition_v1.c`; it does not include
   `bx_ntvdm_adapter_runtime.c`.
2. The engine installs composition runtime from copied CLI input. That runtime
   captures the admitted drive snapshot and real host namespace, binds the DEM
   package session and native BOP composition, then arms the mantle entry.
3. The live BOP handler routes ingress to the DEM package session before
   COMMAND/Redirector/Debugger fallbacks. It has no exact DEM service-number
   recognizers.
4. `bx_ntvdm_adapter_runtime.c` still contains legacy exact-gate branches for
   DEM `00,02,0d,0f,12,16,1b,32,3b,42,46` and historical host-session
   transactions. Its only current references are retained pre-governance
   derivative/configure tooling, not the formal product manifest.

## Disposition

| Surface | Status | S19 disposition |
| --- | --- | --- |
| Formal composition runtime + boot namespace composition | Live product route | Retain; it is the only DEM package-session owner. |
| `bx_ntvdm_adapter_runtime.c` | Not formal-product linked | Do not migrate new behavior into it; classify as historical compatibility/deletion candidate. |
| T188--T190 derivative build tools | Retained evidence/tooling | Do not use as S19 acceptance inputs. |
| `Configure-Bochs26NativeContainer.ps1` copy of adapter runtime | Historical container path | Audit separately before removal; no product behavior depends on it. |

## Interpretation

S19’s 73-slot Direct/Readonly result applies to the actual package route. The
old runtime must not be used to justify another selector-specific repair. Its
later retirement can be a build/governance cleanup only after all retained
derivative records have a successor or a documented historical-only status.

## Follow-up

Proceed to the PDB/JFT/SFT owner gap (`50:3C`) and the remaining source-rung
reviews inside the live DEM package session. Native trace remains a final
package validation, not a routing-discovery mechanism.