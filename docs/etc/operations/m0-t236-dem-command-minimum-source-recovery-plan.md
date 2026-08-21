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
| S1 | COMMAND child lifecycle (`54:08`, `54:0A`, `54:0B`) | Recover the minimal Direct non-pipe child/completion state around the imported `cmdExec`, `cmdExecComspec32` and `cmdReturnExitCode` bodies. | Requires existing session environment, opaque handles and current-directory publication. Excludes pipe/Redirector, nested children and WOW. |
| S2 | DEM hard-error/retry (`50:32`, `50:33`) | Determine and, if mechanically admissible, carry the original retry selector/register restoration through the typed result boundary. | Stops if it requires an unbounded CPU/Bochs semantic expansion. |
| S3 | DEM/COMMAND pipe boundary (`50:47`, `50:48`, `54:06`) | Recover only the original source paths that can use the S1 session child record without exposing a host handle. | Redirector protocol itself remains transferred to the Redirector owner package. |
| S4 | DEM DASD/IOCTL source seam | Separate public Win32 raw-volume capability from uncomposable floppy/FDC/DMA/CMOS and `host_simulate` machine ownership; remove any avoidable replacement where original host source composes. | Floppy/machine mechanics transfer to bx machine/BIOS package. |
| S5 | COMMAND WOW source disposition | Reassess `GetWowKernelCmdLine` and `GetWOWShortCutInfo`; reuse only what fits the fixed WOW root/session ABI or record a source-shaped unavailable outcome. | WOW guest loading/execution transfers to the WOW16 owner package. |
| S6 | DEM search `NtVdmControl` disposition | Recheck whether a public modern host capability can preserve the original fallback ordering; otherwise retain the explicit unavailable provider and document it. | No kernel/API reconstruction or host modification. |
| S7 | T236 closure | Reconcile every item in the T236 audit list, remove superseded no-op claims, run package-local regression and transfer remaining external owners. | Native guest trace is integration-only after package-local closure. |

## S1 Source-Recovery Ledger

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Retain `base/mvdm/dos/command/cmdexec.c`: `cmdExec`, `cmdExecComspec32`, `cmdReturnExitCode`, and the source ordering/CF/AL paths around `cmdExec32`; retain `cmdmisc.c` event call sites. The original `cmdCreateProcess` and `cmdExec32` translation-unit bodies are presently excluded by their named CCPU/BaseSrv/CSR/event-thread/process-global-handle dependencies. |
| Smallest seam | A session-owned child record may replace only the unavailable broker/worker boundary: copied command/environment/opaque stream IDs, pending/completed state, exit byte, and cleanup. It must keep the original COMMAND entry and completion bodies as owners. |
| External intrusion | Rejected. No Bochs, adopted OpenNT, host system, kernel or registry modification is required. |
| New behavior | Restricted to the typed session record and its lifecycle mechanics, because no independently composable historical broker exists. It cannot parse COMMAND input, expose a `HANDLE`, inspect arbitrary guest memory, or manufacture an external command queue. |

## S1 Completion Profile

The only positive profile is one Direct session, one non-pipe host child, one
completion, and one source-ordered `54:0B` consumption.  It must cover normal
exit and source-defined malformed command/environment/standard-token failures.
It must clean its record on both paths.  Pipe/Redirector, repeat command
injection, nested launch, TSR console injection and `VDMForWOW` are negative
or explicitly unavailable paths in S1, not silently successful shortcuts.
