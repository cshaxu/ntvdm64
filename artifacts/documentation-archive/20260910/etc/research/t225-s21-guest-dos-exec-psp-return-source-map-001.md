# T225 S21 P1 Guest DOS EXEC, PSP, and Parent-Return Source Map

## Decision

A multi-program sequence remains a single guest DOS process tree inside one
Bochs VDM. OpenNT NTDOS owns EXEC, program loading, memory arena/PSP
construction, inherited process state, child termination, and parent return.
The CLI, bx-vdm, and bx-mantle may not replace any of those operations with a
host child process, a synthetic DOS loader, or an adapter-owned return queue.

The current composition already contains the correct narrow NTDOS admission
seam: DEM `50:11` writes the identity-checked NTDOS payload into ordinary
guest RAM at the guest-selected `DI:0000` address. This is a mechanical copy,
not a DOS loader. Current execution has not yet demonstrated a continuous
NTIO-to-NTDOS-to-COMMAND path, so no end-to-end EXEC claim is made here.

## Original Guest Control Flow and ABI

| Phase | Original source and ABI | Owner and required result |
| --- | --- | --- |
| EXEC entry | `doskrnl/dos/msdisp.asm` dispatches INT 21h `AH=4Bh` to `msproc.asm:$Exec`; the executable path is `DS:DX`, and the EXEC parameter block is `ES:BX`. `exe.inc` defines `EXEC0`, `EXEC1`, and `EXEC3`; AL permits only `0`, `1`, `3`, or `5`. | Guest NTDOS. Invalid AL follows its original DOS invalid-function failure; bx-vdm does not decode it. |
| Classification and image I/O | `$Exec` performs binary classification, ordinary guest open/read/seek/close, and `ExecRead`. `ExecRead` invokes the normal guest READ path; NTDOS owns SFT position and error translation. | Guest NTDOS plus already-admitted DEM namespace/file I/O (`50:12`, `50:00`, `50:42`/`50:16`, `50:02`). No adapter file-to-entry loader is permitted. |
| Arena and PSP | `$Exec` allocates DOS arena space, sets the initial stack and zero return word, then `Exec_Build_Header` transfers ownership and invokes `$Dup_PDB`. `pdb.inc` fixes the PSP/PDB layout, including `PDB_Parent_PID`, JFN table/pointer, environment, user stack, FCBs, command tail, and `PDB_Exit`. `misc.asm:$CREATE_PROCESS_DATA_BLOCK` initializes the public PSP call/JFN fields. | Guest NTDOS. No host-created PSP, MCB, JFN inheritance, FCB copy, or environment block. |
| Entry transfer | `$Exec` saves the parent return address in `PDB_Exit` and `ADDR_INT_TERMINATE`, then emits `SVC_DEMENTRYDOSAPP` with `DS:SI` entry, `AX:DI` initial stack, and `DX` PDB. It restores guest registers and transfers by `retf`. | Guest NTDOS controls CS:IP, SS:SP, DS/ES and AX. DEM `50:36` is only the void entry notification. |
| Child termination | INT 21h `AH=4Ch` reaches `msproc.asm:$Exit`, then `$Abort`/`reset_environment`. The latter reads `CurrentPDB` and `PDB_Parent_PID`; for a real child it issues `SVC_PDBTERMINATE` with BX=PDB, frees the process arena, performs `DOS_ABORT`, restores the parent PDB/user stack, and returns through the saved terminate address with `DOIRET`. | Guest NTDOS owns order and return. DEM `50:3C` releases only per-PSP search state; it is not file-token or parent-return authority. |
| COMMAND loop | COMMAND receives a guest child return through the restored PSP/terminate path. Its host `54:01`/`54:0B` declared-plan contract remains a distinct host COMMAND package; it is not a substitute for DOS child execution. | Guest COMMAND plus existing COMMAND package only where its original host BOP is reached. No nested host NTVDM launch. |

## Host BOP Boundary

OpenNT historical DEM implementations confirm the boundary:

- `demmisc.c:demEntryDosApp` receives the already-selected guest entry and PDB
  and normally performs only a VDD user hook/debug support. The no-VDD CLI
  keeps the original void-return shape by resuming `50:36` without changing
  guest process state.
- `demsrch.c:demTerminatePDB` notifies VDD/host and releases the terminating
  PDB's FindFirst/FindNext list. The current provider deliberately leaves
  opaque file tokens alive because NTDOS subsequently performs ordinary guest
  close operations during `DOS_ABORT`.
- `demLoadDos` is historically host-composed and unavailable as a directly
  linkable modern CLI implementation. The selected source-derived replacement
  in `bx_ntvdm_dem_load_dos_service_v1_prepare` accepts only `C4 C4 50 11`,
  copies the selected NTDOS image at the guest-provided destination, and
  resumes. It does not inspect or create DOS process state.

## Current Route and Missing Closure

| Surface | Current route | Disposition |
| --- | --- | --- |
| NTIO entry | `composition_runtime` writes the identity-checked NTIO image at physical `0x700`, enters real mode `0070:0000`, and supplies the admitted fixed initial-state bytes. | Retain. This is the existing startup input contract. |
| NTDOS load | Native generic BOP bridge routes original `50:11` to the boot-namespace plane and the checked guest-RAM action above. | Retain. It is the source-derived historical composition seam. |
| EXEC file services | Current whole DEM provider supplies Direct/Readonly file and namespace services. | Retain, subject to real guest reach validation. |
| Entry/exit notifications | `bx_ntvdm_dem_session_lifecycle_provider_v1.c` resumes original `50:36`; for `50:3C`, it releases only per-PDB search state before later NTDOS close. | Retain. No host process lifecycle is present. |
| Guest EXEC/PSP/return reach | The native Direct/Readonly witness currently has a 100,000-instruction budget and ends before any demonstrated `NTDOS $Exec`/COMMAND child lifecycle. | Open. This is the first required implementation/validation closure. |
| Host child substitute | The formal native graph uses composition runtime and bound package sessions. The legacy runtime is not a live route. The retained Direct child-terminal path is a bounded host COMMAND capability, not a guest DOS EXEC implementation or re-entry queue. | Explicitly reject as solution for T225. |

## Failure and Profile Disposition

| Case | Required disposition |
| --- | --- |
| Guest EXEC validation, load, allocation, or ordinary file error | Preserve NTDOS DOS failure and carry/error path; do not translate it into a host process result. |
| Direct host namespace/file operation | Existing Direct capability provider is eligible only through the DEM file boundary. |
| Readonly mutation attempt | Shared mutation policy refuses it at the capability boundary; guest DOS observes the selected source-shaped failure, not a virtual process. |
| Overlay | ABI/deferred only. It may not fall through to Direct and is not required for S21 admission. |
| Missing VDD, debugger, CSR/BaseSrv, GUI or nested host process | Original host dependency is unavailable/deferred in this CLI profile. Preserve no-VDD notification behavior or explicit package disposition; do not emulate it in the adapter. |
| Missing machine mechanic | Stop and admit a separately owned bx-core/bx-mantle package from a reached native observation; do not add DOS or BOP semantics to Bochs. |

## Smallest Next Closure

The smallest implementation package is not an EXEC implementation. It must:

1. preserve the existing NTIO and `50:11` NTDOS staging seam;
2. establish, with source-built primary guest inputs, a bounded continuous
   native observation that reaches original NTDOS initialization and the
   original COMMAND handoff without altering guest bytes;
3. prove whether the first missing prerequisite is guest-visible startup state,
   a DEM service already classified in the whole package, or a separately
   owned bx-core/bx-mantle mechanic; and
4. only after such reach is demonstrated, admit a guest-owned COM/EXE EXEC
   fixture that observes original PSP, child entry, INT 21h termination and
   parent return.

This keeps the recovery order intact: original OpenNT guest code first,
existing host provider/seam second, source-derived host composition only where
the historical NT host cannot be linked, and no new adapter DOS semantics.