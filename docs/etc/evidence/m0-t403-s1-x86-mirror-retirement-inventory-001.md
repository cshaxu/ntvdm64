# M0 T403 S1 x86 mirror-retirement inventory

## Question

Which reached product differences exist only for native-x64/cross-width
closure or unnecessary project-authored substitution, and which remain needed
by the selected Win32/x86 product?

## Inputs and procedure

- The active M0 T403 S1 packet and the owner request of 2026-09-10.
- `git show 371be986f^:src/session/session.h` and
  `git show --stat 371be986f`, which establish that the generic mapping
  manager was project-authored after the earlier session shape.
- Exact source/build reference searches for `mapping_manager`, session mapping
  accessors, `mvdm_host_identity`, and `mvdm_softpc_physical_mapping`.
- The generated `build/M0-T403-S1/r011` Win32/x86 CCPU40 graph and its
  `original-softpc-process.exe` link, invoked through the MSVC x86 developer
  environment rather than a host-shell Ninja invocation.

## Disposition

| Surface | Disposition | Reason |
| --- | --- | --- |
| `src/session/mapping_manager.*` and three session instances | Removed | They were a project-authored generic token registry introduced for cross-width closure; the selected x86 process has original 32-bit direct carriers. |
| Mapping-manager test | Removed | It tested only the removed registry. |
| 32-bit host identity facade | Removed | All reached callers now use their original Win32/x86 pointer or `HANDLE` carrier; no facade or build/test input remains. |
| OEM pointer-width overlay | Removed | The original OEM source path is selected directly for the sole x86 product; the overlay and its x64-only fixture/build branch were unused by that product. |
| XMS A20-state lease overlay | Removed | The selected one-process x86 path directly restores `pHimemA20State` from the original AX:BX carrier and preserves the original state-byte write order. |
| Dialog pointer-width adapter | Removed | Its sole `nt_error.c` caller restores the original x86 `DWL_USER` pointer carrier. |
| SoftPC config-scalar widening | Removed | Original `ULONG`/`SHORT` scalar conversions are restored for the selected x86-only configuration path. |
| UMB numeric-address codec | Removed | SoftPC, XMS and VDD restore their original x86 `PVOID`/`DWORD` carrier conversions; the codec had no independent x86 role. |
| COMMAND native-length helpers | Removed | The original x86 `DWORD`/`WORD` length carriers are restored in `cmdconf.c` and `cmdenv.c`; both helpers existed only for x64 `size_t` narrowing. |
| COMMAND invalid-file sentinel widening | Removed | `cmdconf.c` restores its original `(HANDLE)0xFFFFFFFF` comparison; the native-width sentinel substitution only served x64. |
| C-VID native-word varargs and bit-literal edits | Removed | The selected x86 generated sources restore their original diagnostics and `int` shift literals; the former x64-only formatting/width annotations no longer remain. |
| Bounded guest-memory lease | Retained | This is a bounded read/copy/commit lifetime, not a pointer-identity manager. Reached XMS copy, COMMAND state, debugger probe, DEM fallback, WOW callback, and Redirector copy paths require an explicit owner/commit boundary because their selected user-mode source has no kernel VDM alias service. The lease contains only numeric guest addresses and temporary byte buffers. |
| SoftPC physical-page adapter | Retained | `VdmSetPhysRecStructs` and DOS-page aliases are reached source contracts, but their original kernel VDM backing service is absent. The reduced adapter owns only per-session page bind/alias records and direct x86 address storage; it no longer assigns opaque identities. |
| DEM fast-read fallback | Retained | The selected original dispatcher reaches `SVC_DEMFASTREAD`; the kernel-only worker is absent. The small overlay safely performs a synchronous disk-file read or returns carry so the unchanged original slow path retries. |
| CCPU local state/vector binders | Retained | The selected CCPU40 and C-VID source manifests omit their historical generated binding unit and conflict if the full alternate owner is linked. The overlays declare source-shaped state and bind the retained provider slots; they introduce no native-width or identity transport. |
| PIC EOI bridge | Retained | `SoftPcEoi(int,int*)` and `ica_eoi(ULONG,LONG*,int)` are distinct 32-bit C types even under x86 MSVC. The bridge is a checked ABI adaptation rather than a cross-width conversion. |
| 16-bit Redirector mailslot adapter | Removed | The original `vrmslot.c` already owns a process-local WORD bitmap and list lookup. Its standalone adapter only existed to route that original mechanism through the removed mapping manager. |

## Observations

The regenerated x86 graph completed 434 actions and linked
`build/M0-T403-S1/r011/original-softpc-process.exe` after the generic manager,
identity facade, OEM pointer-width overlay, XMS A20-state lease overlay,
dialog pointer-width adapter, SoftPC config-scalar widening, UMB numeric
address codec, and Redirector mailslot adapter were removed.
The PE header reports `14C machine (x86)`.
Existing compiler warnings are pre-existing historical-source warnings; no
mapping-manager or identity unresolved symbol, or direct-carrier compile
failure, occurred.

The governed staging tool re-read the r011 PE header and published the sole
product name at `build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe`.
Both staged files have SHA-256
`479f38247f9640272a24e5518247d2185dea9a93c6975ab5412f12a44370f54c`.
An owner runtime observation is required before closure.

The owner tested `O:\ntvdm64\ntvdm32.exe` successfully on 2026-09-10.

## Interpretation and follow-up

Confidence is high that the generic manager, its three namespaces, and the
identity facade are removed from the selected product graph. The next review
audits the remaining x64/cross-width candidates, retaining only a
source-comparable x86 seams; the mailslot mechanism now restores its original private bitmap.
