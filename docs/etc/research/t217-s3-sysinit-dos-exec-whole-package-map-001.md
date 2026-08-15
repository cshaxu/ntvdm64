# T217 S3 — SYSINIT to DOS EXEC Whole-Package Map

## Question

What complete owner package lies between the current successful startup
configuration sequence and the source-backed `SVC_DEMEXITVDM` terminal, and
which recovery-ladder action is justified without selecting a leaf BOP from a
trace?

## Inputs

- OpenNT SYSINIT source:
  `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1542-1630`.
- OpenNT DOS EXEC source:
  `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm`, especially
  `EXEC001E`, `Exec_Check_Environ`, and `Exec_Com_File`.
- OpenNT DOS file/handle owner sources:
  `dos/file.asm`, `dos/handle.asm`, and `inc/dossvc.h`.
- OpenNT DEM owner sources: `dos/dem/demdisp.c`, `demfile.c`, `demhndl.c`.
- Current source-built image and composition evidence: T216 closure and
  `t215-s6-startup-bop-sequence-causal-map-001.md`.
- Current initial-environment source-reuse audit:
  `t217-s1-command-environment-source-reuse-audit-001.md`.

## Original Control Flow

For the normal (non-`noexec`) path, SYSINIT first obtains and releases the
largest DOS allocation, deducts the SYSINIT footprint, then opens the command
interpreter and seeks to its end to determine whether the COM image will fit.
It closes the sizing handle, constructs `comexe`, and calls DOS INT 21h EXEC
(`AH=4Bh`, `AL=0`). Every error in the open/seek/size/EXEC path falls through
`comerr`, calls `badfil`, then issues `SVC_DEMEXITVDM`.

The current retained command image is a 0xc4d0-byte COM image beginning with
`E9`, not an MZ image. That is compatible with SYSINIT's documented
COM-sizing assumption and does not itself explain the terminal.

## Observed Integration Boundary

The bounded T215 observer recorded the accepted sequence
`50:12 → 50:00 → 50:21 → 50:16 → 50:02` while CONFIG is consumed, then a
second `50:12` at `0034:6BDF`, followed directly by SYSINIT's
`8DC8:0984 50:3D`; it recorded no overflow and no intervening `50:00`,
`50:16`, or `50:02` for that second open.

`dossvc.h` maps those identities to DEM Open, ChangeFilePtr, IOCTL, Read, and
Close. `file.asm:209-212` is the normal DOS-file route to `SVC_DEMOPEN`.
Combined with the exact SYSINIT source order, the evidence strongly indicates
that the command-interpreter `DEMOPEN` result carries the failure branch.
It does **not** prove the pathname, registers, host result, or provider
selection: those values were intentionally outside the old selector-only
observer. This is integration-localization evidence, not an implementation
request.

## Owner, ABI, And Failure Table

| Requirement | Original owner/source | ABI and failure behavior | Current disposition |
| --- | --- | --- | --- |
| Largest DOS block and SYSINIT reservation | Guest DOS `sysinit1.asm:1551-1579`, DOS MCB allocator | INT 21h allocation/deallocation; failure branches to `mem_err` before command open | Guest-owned. Second `50:12` proves this earlier branch was not taken in this run; no adapter or Bochs implementation is admitted. |
| Command pathname open | Guest DOS `file.asm:209-212` → DEM `demfile.c` | DOS path in guest memory; DEM Open returns a DOS-compatible handle/error/carry result | Existing whole DEM direct provider is the intended owner. Its actual SYSINIT request/result boundary is not yet observed. |
| Size query and handle close | Guest DOS `handle.asm:698-724`, DEM `demhndl.c` | 32-bit host handle split through DOS SFT; lseek returns `DX:AX`; close propagates error | Existing direct handle provider is intended owner. The second open did not demonstrably reach these calls. |
| COM image load | Guest DOS `msproc.asm:925-1104` | DOS allocates PSP/load block, seeks to zero, reads image, checks short read, sets `CS=DS=ES=SS`, `IP=100h`, stack and PDB | Guest DOS/NTDOS artifact owns it. No BOP or adapter replacement is admitted. |
| EXEC environment copy | Guest DOS `msproc.asm:450-520` | Double-NUL scan limited to `CX=8000h` (32 KiB), allocates/copies env plus argv[0]; bad env or allocation failure returns DOS error | Initial environment's 65,535-byte publication bound is not a claim that all EXEC paths accept 65 KiB. This is an explicit later capability/admission constraint. |
| Initial COMMAND environment | OpenNT COMMAND `cmdenv.c` | ANSI snapshot/filter/OEM/name case/PROMPT; `BX` paragraph retry | Recovered source-derived seam in T217 S2. Its distinct AUTOEXEC merge is still deferred. |
| Machine execution | Bochs core CPU/RAM; mantle lifecycle | Ordinary real-mode execution and checked mechanical memory action | Existing machine runs through the second DEM Open. No extra device, firmware, IRQ, or Bochs intrusion is evidenced by this path. |
| Fatal terminal | SYSINIT `comerr` → DEM `demmisc.c:626-630` | Historical UI/error then VDM termination | Current adapter provides an honest controlled stop; it must not turn this terminal into success. |

## Recovery-Ladder Decision

| Rung | Decision |
| --- | --- |
| Direct original OpenNT source | Retain guest SYSINIT, DOS file/handle/EXEC, and DEM owner sources as normative. They cannot be linked wholesale into the x64 process because DOS is guest machine code and DEM source depends on CCPU/SAS/NT4 host composition. |
| Smallest seam | Reuse the installed source-derived DEM direct file/handle provider. Before changing it, make one bounded integration observation of the already-defined Open pathname/result ABI at the second Open and compare it with `demfile.c`/`file.asm` contracts. |
| External intrusion | Rejected. This path reaches ordinary CPU/RAM execution and no missing Bochs mechanical feature is evidenced. |
| New behavior | Rejected. No new DOS EXEC, BOP, filesystem, or terminal behavior may be authored until the existing package's actual ABI result is classified. |

## Selected Next Work

The next subtask is an **integration verification of the existing DEM normal
file/handle package**, not a `50:12` implementation. It may capture only the
already copied fixed-width BOP event, typed provider decision/result, and
checked transaction outcome for the second Open. It must not read arbitrary
guest memory, parse DOS semantics in the adapter, alter routing, or change
Bochs. It then has exactly two permissible outcomes:

1. the existing provider returns an error due to a source/ABI/profile mismatch:
   admit the whole DEM normal-file integration repair; or
2. the provider succeeds and the guest nevertheless branches: continue the
   guest DOS EXEC package map with the observed typed result.

Neither outcome permits a standalone `50:3D` or EXEC handler.
