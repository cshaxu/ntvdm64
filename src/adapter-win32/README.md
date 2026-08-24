# adapter-win32

Same-shaped recovery of unavailable historical Win32/NTDLL interfaces using
modern public Win32 APIs. A facade retains the original reached name,
parameters, calling convention and observable failure semantics as far as the
current platform permits.

It owns neither BOP/provider policy nor Bochs interaction. Its public headers
are the declared historical API facades consumed by `opennt-host` and
`opennt-bop`; the future static library name is `adapter-win32`.

## T260/S6 component register

`facade/` contains the pre-existing `opennt_rtl_compat` and
`opennt_error_dialog_facade` implementation. Their source spelling and
observable contract remain the historical OpenNT-facing surface; T260/S6 only
relocates the files out of the former generic BOP shim directory.

`include/compiler/` and `include/legacy-api/` are production ABI input
closures: they preserve the historical declarations consumed by active
OpenNT-derived source. They are not comparison-only source trees. Any
unreachable historical declaration is externalized to `docs/etc/legacy_code/`
rather than retained merely as reference material.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `WIN32-DIV-001` | The NTVDM resource DLL/string table supplies hard-error text. | That resource closure is not a composable CLI input. | Use the resource ID in the same terminal host-notification contract. | `facade/opennt_error_dialog_facade.c:54` |
| `WIN32-DIV-002` | The OpenNT hard-error dialog exposes an editable text control. | `MessageBox` has no public equivalent control. | Keep call shape and deterministic terminal reply; do not pretend an edit happened. | `facade/opennt_error_dialog_facade.c:80` |
| `WIN32-DIV-003` | `softpc.new/host/src/nt_pif.c` calls the NTVDM product's OEM path/environment/title helpers. | Those private exports are unavailable to a standalone program. | Retain the reached helper signatures and results through public OEM/ANSI Win32 APIs. | `facade/opennt_pif_oem_facade.c:1`, `include/legacy-api/opennt_pif_oem_facade.h` |
| `WIN32-DIV-004` | Recovered COMMAND callers link selected RTL conversion, environment and NTSTATUS mapping routines through NT4 host composition. | That product import closure is not an independently linkable application contract. | Preserve the reached names, argument layouts, allocation/failure rules and status mapping through public Win32 plus bounded host-private storage. | `facade/opennt_rtl_compat.c:12`, `include/legacy-api/opennt_rtl_types.h` |
| `WIN32-DIV-005` | OpenNT DEM calls OEM directory exports supplied by the historical NTVDM host. | Those private product exports are unavailable to a standalone process. | Retain the original names, parameters and last-error behavior through OEM/ANSI public Win32 calls. | `facade/opennt_dem_oem_facade.c:1`, `include/legacy-api/opennt_dem_oem_facade.h` |
| `WIN32-DIV-006` | OpenNT DEM calls OEM file exports supplied by the historical NTVDM host. | The original TEB static-Unicode buffer and product exports are not public modern API. | Retain original function spellings, parameter and retry order through public OEM conversion and Win32 file APIs. | `facade/opennt_dem_file_oem_facade.c:1`, `include/legacy-api/opennt_dem_file_oem_facade.h` |
| `WIN32-DIV-007` | OpenNT DEM queries OEM host computer and disk information through NTVDM product composition. | Those product exports do not form an independent API closure. | Retain original names, argument and Win32 error behavior through public ANSI Win32 calls. | `facade/opennt_dem_gset_facade.c:1`, `include/legacy-api/opennt_dem_gset_facade.h` |
