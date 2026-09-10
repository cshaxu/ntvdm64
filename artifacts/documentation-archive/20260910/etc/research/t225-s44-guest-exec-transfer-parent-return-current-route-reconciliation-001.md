# T225 S44 — Guest EXEC Transfer And Parent-Return Current-Route Reconciliation 001

## Question

After S43 established that native execution reaches original NTDOS `$Exec`'s
`50:36` pre-transfer notification, what is the complete source-owned guest
multi-program path, how is every presently relevant cross-layer crossing bound
in the formal CLI graph, and which one owner-scoped package must follow?

## Inputs

- Original OpenNT NTDOS and COMMAND sources below `src/opennt/base/mvdm/`.
- T225 S19 DEM package, S20 COMMAND package, S21/S26/S27 source/current-route
  records, S42 paired observation, and S43 attribution record.
- Current formal graph authority:
  `tools/build/t225-s7-full-module-manifest.json` (MSVC x64 `/MT`, CPU5/P-MMX).
- Current live composition:
  `src/bx-vdm/bx_ntvdm_boot_namespace_composition_v1.c`, its DEM and COMMAND
  package sessions, and `src/cli/ntdos64_native_cli.c` through the manifest.

## Original lifecycle and crossing map

| Order | Original source fact | Original owner | Required result |
| --- | --- | --- | --- |
| 1 | `msdisp.asm` dispatches INT 21h `AH=4Bh` to `$Exec`; `msproc.asm` performs DOS open/read/seek/close and classification. | NTDOS | DOS error/result path remains guest-owned. |
| 2 | `$Exec` allocates arena space, invokes `$Dup_PDB`, copies the command tail/FCBs/environment relation, sets `PDB_Parent_PID`, and saves `PDB_Exit` plus `ADDR_INT_TERMINATE`. | NTDOS | Child PSP/PDB, JFN/FCB/arena, return address and initial stack remain guest-owned. |
| 3 | `$Exec:Xfer_To_User` issues `SVC_DEMENTRYDOSAPP`, then clears InDos, installs `SS:SP` from `AX:DI`, creates the `DS:SI` far-return frame, sets DS/ES from DX, restores AX from BX, and `retf`s. | NTDOS | Child entry begins only through original CPU/guest transfer mechanics. |
| 4 | A terminating child enters `$Exit/$Abort`; `msctrlc.asm:reset_environment` sends `SVC_PDBTERMINATE`, then frees arena state, runs `DOS_ABORT`, restores the parent PDB/user stack, and reaches `DOIRET`. | NTDOS | Child teardown and parent continuation remain guest-owned. |
| 5 | COMMAND.COM returns through `cmdReturnExitCode` / service `54:0B`; `cmdexec.c` preserves DX exit code, current-drive/redirection order, and branches CF for re-entry versus no-next completion. | COMMAND | Guest child return is not top-level VDM exit and is not a nested host process. |

The exact S43 source mapping proves order 3 is emitted with `DS:SI` child
entry, `AX:DI` initial stack, DX PDB, and BX initial AX. The S42 record agrees
with this contract at `DS:SI=0c41:0100`, `AX:DI=0c41:fffe`, `DX=0c41`, and
`BX=0000`; it is still only a copied pre-dispatch observation.

## Current formal composition disposition

| Crossing | Bound current route | Direct / Readonly disposition | Evidence status |
| --- | --- | --- | --- |
| CLI admission and machine start | Formal Ninja target links the CLI, `bx-vdm`, `bx-mantle`, `bx-core`, and admitted `opennt-host` modules; the composition stages NTIO and the original-shaped `50:11` NTDOS guest-RAM action. | Shared input mechanics; no profile fallback. | Retained S7/S10/S32 graph and staging records. |
| NTDOS ordinary EXEC file operations | `bx_ntvdm_dem_package_session_v1_dispatch` owns the full DEM package through the live boot-namespace composition. | Direct is host-integrated through the selected namespace; Readonly is capability-governed; Overlay is ABI-reserved. | S19 full-table Direct/Readonly regression; not EXEC reach proof. |
| `$Exec` entry notification `50:36` | `bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch` validates the original DEM classification and returns `resume(fault_rip + 4)`. Its only retained no-VDD effect is the original void return. | Identical profile-neutral lifecycle result. | Current source plus S42/S43 observation and attribution. |
| Child termination notification `50:3c` | The same lifecycle provider releases only per-PDB search state before NTDOS's later ordinary close/abort sequence. | Direct/Readonly package ownership remains as recorded by S19. | S19 lifecycle-order evidence; no child teardown reach proof. |
| COMMAND `54:0B` parent-return contract | `bx_ntvdm_command_package_session_v1` retains re-entry versus no-next semantics; `copy_ordinary_completion` exposes only an opaque post-return fact. `bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal` accepts that fact without exporting BOP/DOS data. | Same copied route in Direct/Readonly; historical pipes/redirection remain contained unavailable/deferred where unadmitted. | S20 family route and S27 focused/composition evidence. |
| Engine exit classification | CLI consumes only the existing opaque ordinary-terminal query after normal cleanup. | Neither profile creates a host child or substitutes a top-level `54:00` exit for a guest return. | Current composition source and S27 distinction. |

`bx_ntvdm_adapter_runtime.c` is retained historical/diagnostic code and is not
the live formal composition route. The formal path is the bound
`bx_ntvdm_boot_namespace_composition_v1_handle` sequence: typed BOP ingress,
provider registry, DEM package session, COMMAND package session, then other
explicit package facades. No raw selector-recognizer or adapter-owned DOS
loader is selected as the current recovery route.

## Recovery-ladder assessment

| Rung | Result |
| --- | --- |
| 1. Original source | NTDOS owns EXEC, PSP/arena/JFN construction, entry transfer, termination and parent return. COMMAND owns `54:0B` semantics. The historic host translation units themselves remain unavailable as a directly composable x64 CLI product shell because of CCPU/SAS, VDD, broker/thread, GUI and legacy session dependencies. |
| 2. Smallest compatible seam | Existing DEM/COMMAND package sessions retain source-defined selector, copied-register, ordering and failure contracts. `50:36` preserves no-VDD void resume; `54:0B` preserves CF/re-entry/no-next behavior and exports only an opaque completion fact. |
| 3. External-code intrusion | Rejected: no Bochs CPU/device/firmware change is indicated by the current source/current-route facts. |
| 4. New behavior | Rejected: no synthetic loader, host child launch, adapter process/return queue, or top-level-exit alias may stand in for guest EXEC. |

## Conclusion

All currently relevant host BOP crossings for this lifecycle have an admitted
owner and bounded Direct/Readonly route. The first remaining closure is **not
a BOP implementation**: it is a whole guest-owned native execution package
that proves the original source-built COM child reaches its entry, executes its
INT 21h termination path, restores its parent, and reaches the already-bound
COMMAND parent-return contract.

The selected next package must use a source-built guest parent/child/return
fixture. It may add only observation/fixture mechanics needed to prove the
original lifecycle, and must first map each such mechanic to bx-core,
bx-mantle, bx-vdm, or guest owner. It must not infer a repair from a single
later BOP, alter guest executable bytes merely to create a marker, enable an
extra Bochs device, or replace child execution with a host process. Its native
trace is integration validation after its complete fixture/crossing design,
not task selection.

## Limitations

This reconciliation proves routes and their source ownership; it does not prove
that a child instruction after the original `retf` has executed, that an INT
21h termination occurred, or that COMMAND's parent-return path was reached in
a live native run. S42's BOP recorder overflow also prevents any exhaustive
service-sequence claim.
