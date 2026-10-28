# Shard 2: DOS command and DEM dispatch, PIF ownership

This second, read-only group follows the [330-path snapshot](parallel-review-current.md).
The [observer](parallel-review-next-observer.py) resolves original object
relocations against the currently released NTVDM image. Its [receipt](parallel-review-next-evidence.json)
contains the image/map/object hashes and every target. It changes no shared
ledger, guest, runtime image, or installation. Relocation-table identity is
not an executed call, a lifetime test, or a guarantee that all original API
contracts survive the adaptation.

## Exact dispatcher bindings

The 17 `apfnSVCCmd` pointers in the published image resolve to original
`cmdexit`, `cmdmisc`, `cmdredir`, `cmdexec`, `cmdconf`, `cmdkeyb` and `cmdenv`
objects. In particular 0 -> `cmdExitVDM`, 1 -> `cmdGetNextCmd`, 8 -> `cmdExec`,
10 -> `cmdExecComspec32`, and 11 -> `cmdReturnExitCode`. Original
`nt_bop.c::MS_bop_4` reads a command byte at CS:IP, calls `CmdDispatch`,
then advances IP. This establishes the original source edge plus the linked
table only, not end-to-end execution of any particular request.

The 19 `apfnSVCIoctl` slots likewise resolve to original `demioctl.obj`;
8 and 9 -> `demIoctlChangeable`, 13 -> `demIoctlDiskGeneric`, 17 ->
`demIoctlDiskQuery`, other slots -> `demIoctlInvalid`. The final extra slot
18 exists in the table; ordinary DOS `ioctl.asm::$IOCTL` first rejects AL
above 0x11 and then dispatches through its 18-entry guest jump table. Its two
`HRDSVC SVC_DEMIOCTL` sites forward the accepted subfunction. Thus the
DBG-only host check cannot by itself demonstrate a defect in normal INT 21h
IOCTL handling. A direct or malformed BOP is a separate unresolved boundary.

`CmdDispatch` likewise checks its service byte only under `#if DBG`; compare
`DemDispatch`, which rejects `iSvc >= SVC_DEMLASTSVC` in production. The
source chain establishes a potential out-of-range subdispatch if an invalid
command BOP is actually delivered. Neither reachability from ordinary software
nor a runtime failure is established; no repair is proposed on that basis.

## Per-file dispositions for primary review

| File | Proposal | Authority, retained evidence and remaining work |
| --- | --- | --- |
| `dos/command/cmddisp.c` | partial | Original 17-slot SVC dispatcher; object/image binding plus all table entries above. Prove real BOP execution and wrong-index boundary, then completion/return and reentry; 100% source retained, behavioral percentage unknown. |
| `dos/dem/demioctl.c` | partial | Original 19-slot host table and DOS bounds above. Prove actual media/volume return values, supported operations, failure and lifecycle on isolated resources; 100% source retained, behavioral percentage unknown. |
| `dos/dem/demlock.c` | partial | DEM table -> original `demLockOper` -> Win32 `LockFile`/`UnlockFile`, with guest handle and 32-bit offset/length conversion. Previous object/image binding is in [first receipt](parallel-review-evidence.json). An existing `original_demlock_register_fixture.c` exercises one successful one-byte lock/unlock with host seams; it was not rerun or tied to the current package here. Contention, invalid handles, error translation, ownership and task death remain open. |
| `dos/dem/demdasd.c` | partial | DEM table -> `demAbsRead`/`demAbsWrite` -> shared `demAbsReadWrite`. Previous receipt binds the two entry bodies, not the shared implementation's full behavior. Validate supported physical/image media, buffer and geometry limits, failure translation and cleanup on disposable media only. 100% source retained, behavioral percentage unknown. |
| `dos/command/cmdpif.h` | excluded candidate | Historical duplicate declares it was copied from `NT_PIF.H` and adds `fgprio`, absent from the actual selected host `nt_pif.h`. No source directly includes this duplicate; all observed Ninja dependency blocks are valid and exclude it, while the selected header occurs in dependencies. Original `cmdpif.c` and `nt_pif.c` include the selected header; `cmdCheckForPIF` and `GetPIFData` bodies bind to the release image. This excludes only the duplicate declaration role; original PIF parsing, format, UI and lifecycle remain separate partial obligations. Replacing the selected structure with the duplicate would risk ABI drift. |

Both candidate source editions are byte-identical to current for these five
paths, as independently checked in the first receipt. The excluded duplicate
does not have a meaningful behavioral recovery percentage. No underlying PIF
feature is excluded or newly declared working.

## Lifecycle boundaries still requiring concrete evidence

Original `cmdExitVDM` calls `TerminateVDM`; selected `nt_reset.c` starts base
`terminate()` and the host termination path calls `ExitVDM` before process
exit. `cmdReturnExitCode` reports the guest code and calls `GetNextVDMCommand`
before deciding carry/reentry; `cmdGetNextCmd` contains an original
`DosSessionId`/`CloseOnExit` branch including `Sleep(INFINITE)`. Their
presence alone is neither proof of NTSRV coordination nor proof that this
branch is reachable in the current session architecture. Pin a current
package and observe normal/reentry/managed-exit and each failure before
proposing a change. Preserve the active lifecycle authority and immutable
guest rule; do not eliminate original semantics from the source scan alone.

No executable test was run for this batch. Main review must independently
verify the receipt, source identities, table semantics, source inclusion and
the bounded exclusion before touching the shared ledger.
