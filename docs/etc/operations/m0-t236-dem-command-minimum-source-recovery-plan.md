# M0 T236 — DEM/COMMAND Minimum-Modification Source Recovery Plan

## Task Boundary

T236 closes only the currently imported OpenNT DEM and COMMAND source paths
that are still disabled or materially replaced because their NT4 host-product
composition cannot link into the standalone CLI.  The governing recovery order
is original source, smallest compatibility seam, external intrusion, then new
behavior.  It does not use a trace hit to select a leaf implementation.

The task does not claim full DOS continuity, Redirector, WOW, BIOS or device
closure.  Those owners remain distinct even where an imported DEM/COMMAND
routine calls them.

## S Sequence

| S | Owner package | Intended result | Dependencies / explicit transfer |
| --- | --- | --- | --- |
| S1 | COMMAND local child execution (`54:06`, `54:08`, `54:0A`, `54:0B`; local `50:47/48`) | Complete: recovered the locally composable child/completion/standard-stream path around imported `cmdExec32`, `cmdExec`, `cmdExecComspec32`, `cmdReturnExitCode` and local pipe call sites; see [S1 closure](../../history/m0-t236-s1-closure-20260821.md). | Uses public Win32 process/event/Job/anonymous-pipe APIs. Redirector protocol, remote/named-pipe semantics, concurrent-session scheduling and WOW remain explicitly excluded. |
| S2 | COMMAND historical worker and host-broker recovery | Complete: P1 is retained as source-admission evidence; P2 recovers the imported worker's detached topology through a fixed `PENDING` continuation, opaque host-handle IDs, controlled selector-blind machine suspension/resume, source-shaped non-graphical standard-handle notification and `54:01` local continuation verification. | One `bx-vdm` session worker is the CCPU-equivalent composition; Bochs remains the SoftPC-equivalent machine engine. Private broker protocol, multi-session and raw host handles remain excluded. See [S2 closure](../../history/m0-t236-s2-closure-20260821.md). |
| S3 | DEM hard-error/retry (`50:32`, `50:33`) | Determine and, if mechanically admissible, carry the original retry selector/register restoration through the typed result boundary. | Stops if it requires an unbounded CPU/Bochs semantic expansion. |
| S4 | DEM DASD/IOCTL source seam | Complete: retain the imported `demdasd.c`/`demioctl.c` bodies and recover the source-shaped public-Win32 `nt_fdisk` lifecycle; classify every `50:21`/`50:29`/`50:2A` branch as direct, source-failure, or machine-owner transfer. | Floppy/FDC/DMA/CMOS, hooked INT 13 and recursive `host_simulate` transfer to bx machine/BIOS. See [S4 closure](../../history/m0-t236-s4-closure-20260821.md). |
| S5 | COMMAND WOW source disposition | Complete: retain direct `cmdGetNextCmd` and its original `VDMForWOW` branch; its unavailable `GetWowKernelCmdLine` terminal outcome is formally verified. `GetWOWShortCutInfo` remains excluded because its sole startup-reserved input has no admitted WOW consumer. | WOW root publication, shortcut consumption, WOWEXEC/WOW32/NE loading and execution transfer to the WOW16 owner package; see [S5 closure](../../history/m0-t236-s5-closure-20260821.md). |
| S6 | DEM search `NtVdmControl` disposition | Complete: `VdmQueryDir` remains explicitly unavailable; the original `FileFindReset` slow path is forced after source-owned handle eviction and its continuation/exhaustion contract passes. | No kernel/API reconstruction, new directory protocol or host modification. See [S6 closure](../../history/m0-t236-s6-closure-20260821.md). |
| S7 | T236 closure | Complete: reconcile every item in the T236 audit list, remove superseded no-op claims, run package-local regression and transfer remaining external owners; see the [T236 closure](../../history/m0-t236-closure-20260821.md). | Native guest trace remains integration-only after package-local closure. |

## S1 Source-Recovery Ledger

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Retain `base/mvdm/dos/command/cmdexec.c`: `cmdCreateProcess`, `cmdExec32`, `cmdExec`, `cmdExecComspec32`, `cmdReturnExitCode`, and their ordering/CF/AL paths; retain `cmdmisc.c` event call sites and local `cmdredir.c`/DEM pipe call sites. S1 admitted the original `cmdExec32` body.  Only the historical `cmdCreateProcess` worker remains excluded by its named CCPU/BaseSrv/CSR/event-thread/process-global-handle dependencies. |
| Smallest seam | A session-owned child record replaces only the unavailable broker/worker boundary: copied command/environment, opaque stream IDs, pending/completed/cancelled state, exit code, event/wait and cleanup. Public Win32 `CreateProcess`, event/wait, Job and anonymous-pipe APIs may back it. It must keep original COMMAND entry, process setup where composable, and completion bodies as owners. |
| External intrusion | Rejected. No Bochs, adopted OpenNT, host system, kernel or registry modification is required. |
| New behavior | Restricted to typed session lifecycle mechanics, because no independently composable historical broker exists. It cannot parse COMMAND input, expose a `HANDLE`, inspect arbitrary guest memory, manufacture an external command queue, or implement Redirector/WOW policy. |

## S1 Completion Profile

The positive profile is one serialized local session and all of its
source-reachable public-API child paths: direct command and COMSPEC launch,
completion/exit return, session-local standard streams and anonymous pipes.
It must cover normal exit plus source-defined malformed
command/environment/standard-token failures, pipe cleanup and host executable
architecture admission. Redirector/remote or named-pipe protocol, repeat
command injection beyond the imported local queue, concurrent sessions, TSR
console injection and `VDMForWOW` are negative or explicitly unavailable S1
paths, not silently successful shortcuts.
