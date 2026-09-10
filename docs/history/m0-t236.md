# m0-t236 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t236-closure-20260821.md

# M0 T236 Closure — DEM/COMMAND Minimum-Modification Source Recovery

## Outcome

T236 is closed as the source-first, minimum-modification recovery package for
the currently imported OpenNT DEM and COMMAND paths. It closes its own
DEM/COMMAND route, local-worker, failure-disposition and formal-regression
scope. It does not claim whole-guest continuity, a complete historical NT4
host product, or cross-owner lifecycle completion.

## Reconciled S ledger

| S | Closed result | Current disposition |
| --- | --- | --- |
| S1 | Local OpenNT COMMAND child execution, streams and completion | Direct one-session public-Win32 child profile; remote/named-pipe and WOW transfer. |
| S2 | Imported worker topology and bounded pending continuation | One session, opaque handles, controlled resume and local re-entry; no private BaseSrv/CSR broker. |
| S3 | OpenNT DEM hard-error/retry | Imported dispatcher and retry register restoration are active through the typed result boundary. |
| S4 | OpenNT DEM DASD/IOCTL | Fixed-volume direct/failure paths are active; removable and INT13 mechanics transfer to machine ownership. |
| S5 | COMMAND WOW disposition | Ordinary imported path is active; VDMForWOW is an explicit controlled unavailable stop. |
| S6 | DEM VdmQueryDir reset disposition | Private fast path is unavailable; imported slow reset/search continuation remains active. |
| S7 | Reconciliation and v1 retirement | Superseded DEM/COMMAND v1 source, tests, probes and manifests are removed; no executable fallback remains. |

## Formal regression

The source-current MSVC x64 /MT Ninja root
build/M0-T236-S2/007 rebuilt and executed the frozen matrix:

- t231-s7-command-get-next-direct-fixture;
- t234-s2-command-pif-direct-fixture;
- t236-s1-command-local-child-fixture;
- t236-s2-machine-pending-fixture;
- t236-s2-native-cmdexec32-fixture;
- t230-s8-demerror-lock-direct-import-fixture;
- t230-s7-demdasd-ioctl-direct-import-fixture; and
- t230-s5-demsrch-fcb-direct-import-fixture.

All returned zero. The immediate Ninja dry-run reported no work to do. The
T236 S7 v1-retirement gate and the COMMAND v2 route-precedence gate pass.

## Explicit owner transfers

| Unfinished historical/product behavior | Receiving owner |
| --- | --- |
| Remote/named-pipe protocol, VDMREDIR and 57:xx | OpenNT Redirector owner package. |
| WOW root, shortcut, WOWEXEC/WOW32 and NE lifecycle | OpenNT WOW16 owner package. |
| Console input injection, VDD/debugger/top-level notification | Debugger/VDD/console/input/top-level owner package. |
| FDC/DMA/CMOS/timer, INT13 and host_simulate machine mechanics | bx machine/BIOS owner package. |
| DOS EXEC, PSP/arena/JFN/environment and ordinary 54:0B parent return | NTDOS/COMMAND guest EXEC-parent-return package. |
| Cross-owner cmdExec32 broker/lifecycle integration | Post-code-complete dependency and lifecycle closure package. |

XMS and DPMI remain independent owner packages; T236 neither implements nor
claims them.

## Verification and scope limit

Documentation governance, the S7 retirement gate, the COMMAND v2 route gate
and git diff --check passed. Git history retains prior v1 experiments, but the
formal runtime has no DEM/COMMAND v1 product or test fallback. Native guest
trace remains a later integration activity and was not used to select a leaf
implementation here.


## m0-t236-s1-closure-20260821.md

# M0 T236 S1 Closure — COMMAND Local Child Execution

## Closed Scope

S1 closes the locally composable OpenNT COMMAND child-execution profile:
`54:06`, `54:08`, `54:0A`, `54:0B`, and their local `50:47/48` standard-stream
boundary. It does not close Redirector protocol, remote/named pipes, WOW,
concurrent-session scheduling, or Bochs execution mechanics.

## Result

- Imported `cmdExec32` is now an executing OpenNT body, rather than a macro
  alias to the old synchronous shim. Its original event, `GetNextVDMCommand`,
  carry/AL and cleanup order remain in `cmdexec.c`.
- The smallest source-derived seam,
  `bx_ntvdm_command_local_child_execute`, owns only modern public-Win32 child
  mechanics: copied command/environment, explicit inherited stream endpoints,
  Job/wait, exit result and fixed-width session state.
- `cmdCreateProcess` remains excluded after direct dependency analysis: its
  CCPU worker cannot carry the active checked guest-copy context safely, and
  its CSR/BaseSrv and process-global standard-handle operations are not a
  standalone CLI composition. This is a recorded minimal-composition
  substitution, not an unimplemented BOP route.
- Historical event and console helpers no longer silently no-op: their
  supported local state is visible in the session; console injection itself
  remains explicitly unavailable.
- The new T236 fixture verifies Direct child exit, COMSPEC launch, invalid
  opaque standard-token rejection and anonymous stdout pipe transport. The
  retained lifecycle fixture and the independent current-directory fixture
  also pass against the source-current archive.

## Verification

- Changed `cmdexec.c`, `cmdmisc.c` and `command_misc_shim.c` compiled with the
  formal MSVC x64 `/MT`, `/W4 /WX` command shape.
- A fresh graph at `D:\tmp\ntdos64-M0-T236-S1-formal-r3` dry-runs the exact
  82-edge source closure, including the new fixture, static archive and link.
- The local-NTFS Ninja executor still stalls after it creates `cmd.exe`; this
  known executor limitation is retained as a limitation, not a passing full
  Ninja claim. Exact formal commands and the source-current archive performed
  the focused link/run checks successfully.
- Documentation governance and diff checks pass at closure.

## Follow-up Ownership

- T236 later S items own DEM retry, DASD/IOCTL, WOW disposition and
  `NtVdmControl` disposition.
- The Redirector owner package owns remote/named-pipe protocol and any
  extension of the session record required by `57:xx`.
- The WOW16 owner package owns WOWEXEC/WOW32 composition; the Direct child
  record must not be treated as WOW support.

See [S1 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s1-command-local-child-source-recovery-result-001.md)
and [the T236 plan](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md).


## m0-t236-s2-closure-20260821.md

# M0 T236 S2 Closure — COMMAND Historical Worker And Host-Broker Recovery

## Closed Scope

S2 closes the one-session, source-first recovery of the imported OpenNT
`cmdCreateProcess` worker and its `54:08`/`54:0A`/`54:01` composition boundary.
It retains original COMMAND ordering while replacing only unavailable CCPU and
private-broker transport with a fixed-width pending continuation.

## Result

- Imported `cmdCreateProcess`, `cmdExec32`, `cmdExec`, `cmdExecComspec32` and
  `cmdReturnExitCode` are in the formal closure; the worker is no longer a
  synchronous replacement algorithm.
- A session-owned detached worker holds copied inputs and opaque host-handle
  IDs, returns `PENDING`, then resumes the exact BOP without exposing a guest
  pointer or raw host handle across the asynchronous boundary.
- The native `C4 C4 54:08` witness now resumes through the imported completion
  path and returns `AL=37` (`AX=0025` for its direct `AH=0` fixture input).
- `nt_std_handle_notification` now retains OpenNT's non-graphical
  `stdoutRedirected` transition as session state. The optional fullscreen
  display work remains bx-mantle ownership.
- `54:01` verifies local command handoff, environment retry/re-entry and
  terminal behavior. PIF parsing is independently proven; first-VDM PIF
  metadata remains a console-host input rather than fabricated adapter state.

## Verification

- Formal Ninja root `build/M0-T236-S2/007` builds and runs the `54:01`, PIF,
  Direct/COMSPEC/stream/cancellation, selector-blind pending-machine and
  native `54:08` fixtures successfully.
- Immediate Ninja dry-run reports `no work to do`.
- Documentation governance and `git diff --check` are required at commit.

## Transfers

- Redirector remote/named-pipe protocol transfers to the queued Redirector
  owner package; WOW composition transfers to the queued WOW16 package.
- Machine/BIOS mechanics transfer to bx machine/BIOS ownership.
- Guest DOS `EXEC`/PSP/parent restoration needs a separately admitted NTDOS
  guest-exec package. Console input injection remains unassigned and cannot be
  introduced into this session worker.
- CSRSS/BaseSrv multi-session brokering remains explicitly unavailable.

See [S2 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s2-command-worker-host-broker-recovery-001.md)
and [the nonclosure transfer ledger](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s2-nonclosure-transfer-ledger-001.md).


## m0-t236-s3-closure-20260821.md

# M0 T236 S3 Closure — DEM Hard-Error/Retry Source Recovery

## Closed Scope

S3 closes the source-first recovery of DEM `50:32` (`demSetHardErrorInfo`) and
`50:33` (`demRetry`).  The scope is restricted to the imported OpenNT error
owner, the fixed typed bx-vdm boundary, and the existing unified host-handle
manager.

## Result

- `demdisp.c` owns the sole active 73-slot `apfnSVC[]` table.  The shim-owned
  all-unavailable retry table and wrapper were removed.
- Original `demRetry` restores its saved register image and re-enters that
  original table.  The bounded seam preserves GPR plus DS/ES changes for the
  nested original handler, using existing typed result deltas only.
- Original `demClientError` is now the active owner.  The superseded
  source-derived client-error fallback was deleted.
- The retry fixture re-enters original `demChgFilePtr` using a saved 32-bit
  `AX:BP` opaque handle token, which is resolved by the shared handle manager.
  VHE and SYSDEV remain checked guest-memory snapshots rather than handles.

## Verification

- Formal source-current Ninja root `build/M0-T236-S3/001` rebuilt and linked
  the focused source closure with MSVC x64 `/MT /W4 /WX`.
- `t230-s8-demerror-lock-direct-import-fixture.exe` and
  `t230-s10-demdisp-direct-import-fixture.exe` both passed.
- Immediate Ninja dry-run reported `no work to do`; documentation governance
  verification and `git diff --check` passed.

## Transfers

- Hard-error callers must first register the VHE through `50:32`, as required
  by the original owner.  S3 does not fabricate an adapter INT 24/error-dialog
  path for an uninitialized guest.
- DPMI/protected-mode continuation, device/BIOS behavior, and guest DOS
  normal-return continuity retain their respective owners.  S4 owns the next
  DEM DASD/IOCTL source seam.

See [S3 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s3-dem-hard-error-retry-source-recovery-result-001.md)
and [the T236 plan](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md).


## m0-t236-s4-closure-20260821.md

# M0 T236 S4 Closure — DEM DASD/IOCTL Source Seam

## Closed Scope

S4 closes the source-first disposition of the imported OpenNT DEM DASD/IOCTL
owner package: `50:21` (`demIOCTL`), `50:29` (`demAbsRead`), and `50:2A`
(`demAbsWrite`). It does not claim floppy, INT 13, FDC, DMA, CMOS, BIOS or
guest-DOS completion.

## Result

- The original `demioctl.c` and `demdasd.c` bodies remain the active owners.
  Their IOCTL table, BDS data, guest layout, register order, range checks and
  DOS-error conversion were not reimplemented in a BOP provider.
- The original fixed-disk `nt_fdisk` lifecycle is recovered through its
  smallest public-Win32 seam: physical-index table, BPB/geometry, read,
  write, verify, close, idle close and PDB termination. A disposable mounted
  VHDX proved init/read/verify/write/readback/exact restore/close.
- The copied release boundary now directs malformed `50:21 AL>=12` to the
  original `demIoctlInvalid` failure body. This carries the historical
  `ERROR_INVALID_FUNCTION`/`CF=1` contract without allowing the historical
  DBG-only bounds assertion to become a release table overrun.
- Fixed-volume and no-BDS paths have focused formal regression. Every
  removable-media and recursive-INT13 path now has an explicit unavailable
  or machine-owner transfer; it cannot silently succeed.

## Verification

- Fresh formal MSVC x64 `/MT` Ninja root:
  `build/M0-T236-S4/p4-disposition-r1`.
- `t230-s7-demdasd-ioctl-direct-import-fixture.exe` passed with the original
  changeable-media success, malformed-IOCTL failure, and original no-BDS
  read/write failures.
- P3 separately records the authorised VHDX positive fixed-volume lifecycle.
- Documentation-governance and diff checks accompany this closure delivery.

## Transfers

- `nt_rflop`, floppy media, FDC/DMA/CMOS/timer behaviour, hooked INT 13 and
  recursive `host_simulate` transfer to the queued bx machine/BIOS owner
  package. bx-vdm retains only their explicit failure contract.
- Fixed-media `SETMEDIA` retains its original destructive sector-zero path;
  no test or extra host-volume policy was admitted. A future destructive test
  needs independent approval and a disposable-medium restoration protocol.
- T236 continues with S5 COMMAND WOW source disposition.

See the complete [S4 disposition evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s4-p4-dasd-ioctl-disposition-closure-001.md), the earlier [P3 VHDX evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s4-p3-authorized-vhdx-raw-volume-success-001.md), and [the T236 plan](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md).


## m0-t236-s5-closure-20260821.md

# M0 T236 S5 Closure — COMMAND WOW Source Disposition

## Closed Scope

S5 closes the classification of the imported COMMAND WOW-specific helper
branches reached from `cmdGetNextCmd`. It does not implement a WOW VDM.

## Result

- The ordinary `54:01` command handoff remains the direct imported OpenNT
  `cmdGetNextCmd` body.
- Its original `VDMForWOW` branch remains present and is exercised by a formal
  fixture. Since no WOW root, WOWEXEC/WOW32 broker or NE loader is admitted,
  `GetWowKernelCmdLine` preserves OpenNT's non-returning terminal shape as a
  typed controlled stop.
- `GetWOWShortCutInfo` remains excluded from the admitted COMMAND translation
  unit. It only copies BaseSrv-produced `VDMInfo.Reserved` startup metadata;
  there is neither a reached product caller nor a valid WOW consumer in this
  package.
- No substitute command line, shortcut data, private protocol, host pointer,
  raw handle or adapter-owned WOW behavior was introduced.

## Verification

The formal `t231-s7-command-get-next-direct-fixture.exe` verifies ordinary
source delivery/environment retry plus the `VDMForWOW -> STOP` disposition.
The S5 evidence records the source/ABI/failure map and the explicit owner
transfer.

## Follow-up

WOW root/bootstrap, shortcut metadata, WOWEXEC/WOW32 and NE/DPMI execution
transfer to the queued WOW16 owner package. Full historical BaseSrv/CSR broker
semantics transfer to the queued COMMAND `cmdExec32` full-capability package.
T236 advances to S7 reconciliation; it does not claim full COMMAND, WOW or
guest execution closure.

See [S5 P1 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s5-command-wow-disposition-closure-001.md)
and [the T236 plan](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md).


## m0-t236-s6-closure-20260821.md

# M0 T236 S6 Closure — DEM `NtVdmControl(VdmQueryDir)` Disposition

## Closed Scope

S6 closes the final disposition of the historical `VdmQueryDir` fast reset
inside imported OpenNT `demsrch.c`. It covers pathname/FCB search reset after
the source-owned idle-handle eviction, not a general filesystem or a new VDM
kernel service.

## Result

- `NtVdmControl(VdmQueryDir)` remains explicitly unavailable through the
  existing smallest shim, returning `STATUS_NOT_IMPLEMENTED`.
- No public Win32 API supplies its exact cursor-by-`FileIndex` semantics; a
  new adapter protocol or any host/kernel modification is rejected.
- The original `FileFindReset` slow path, including its directory scan and
  remembered-name/index comparison, remains active and is now forced by the
  focused fixture.
- The fixture proves continuation to the remaining match and then the
  original `ERROR_NO_MORE_FILES` terminal after `DemHeartBeat` evicts the
  underlying search handle.

## Verification

The formal MSVC x64 `/MT` Ninja graph in
`build/M0-T236-S6/p1-vdmquerydir-fallback-r1` built and ran
`t230-s5-demsrch-fcb-direct-import-fixture.exe` successfully. Immediate
Ninja dry-run reported no work. Documentation governance and diff checks
accompany the closure delivery.

## Follow-up

Directory mutation during reset retains OpenNT's documented slow-path
limitation; it is not rehosted as an adapter feature. T236 returns to the
previously admitted S5 COMMAND WOW source disposition.

See [S6 P1 evidence](../../artifacts/documentation-archive/20260910/etc/evidence/t236-s6-p1-vdmquerydir-fallback-closure-001.md)
and [the T236 plan](../../artifacts/documentation-archive/20260910/strict-topology/etc/operations/m0-t236-dem-command-minimum-source-recovery-plan.md).

