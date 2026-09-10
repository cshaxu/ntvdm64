# T225 S26 — Guest EXEC Current-Route Reconciliation

## Question

After T225's completed DEM file/lifecycle packages and S25's startup-machine
closure, which complete owner package still prevents an original guest DOS
EXEC child from returning through its parent and producing the CLI's ordinary
completion result?

## Original lifecycle

| Lifecycle phase | Original owner and source | Mandatory result |
| --- | --- | --- |
| EXEC dispatch and file/image setup | `dos/v86/doskrnl/dos/msdisp.asm` INT 21h 4Bh and `msproc.asm:$Exec` | Guest DOS validates AL, obtains the image through ordinary DOS file operations, allocates arena/PSP state, and transfers to the guest child. |
| PSP/JFN/environment/entry | `msproc.asm:Exec_Build_Header`, `misc.asm:$Dup_PDB`, `misc.asm:$CREATE_PROCESS_DATA_BLOCK`, `pdb.inc` | Guest DOS copies the parent contract, records `PDB_Parent_PID` and `PDB_Exit`, then calls `SVC_DEMENTRYDOSAPP` before its far transfer. |
| Child teardown | `msproc.asm:$Exit/$Abort`, `msctrlc.asm:reset_environment` | Guest DOS sends `SVC_PDBTERMINATE`, tears down its own arena/JFN/SFT/FCB state, restores the parent PDB/stack, and returns by its saved terminate address. |
| COMMAND parent continuation | OpenNT COMMAND `cmdReturnExitCode` / BOP `54:0B` | Historical command broker consumes the low DX exit code, supplies declared next-command/reentry facts if any, or returns the parent result without confusing it with top-level VDM exit. |

No bx-vdm provider may implement the first three guest-owned phases.

## Current package disposition

| Requirement | Current source route | Direct / Readonly status | Disposition |
| --- | --- | --- | --- |
| NTDOS staging | `50:11` checked guest-RAM NTDOS copy | identical profile-neutral mechanics | closed seam |
| EXEC open/seek/fast-read/slow-read/close | T225 S3 whole DEM provider: `50:12/00/42/16/02` | Direct typed host-view transfer; Readonly declared namespace; shared opaque token ABI | closed host capability package, not EXEC proof |
| PDB entry/termination host notification | T225 S2 lifecycle provider: `50:36/3C` | no-VDD void entry; Direct PDB-owned tokens/search state released; Readonly session token retained | closed host lifecycle package, not DOS teardown |
| Startup machine prerequisites | T225 S25 one-surface package | Direct/Readonly native parity | closed machine package |
| Guest EXEC/PSP/arena/parent-stack behavior | source-built NTDOS in guest RAM | original guest owner only | must be exercised by a later parent/child fixture |
| COMMAND parent return | `bx_ntvdm_command_lifecycle_provider_v1`, service `54:0B` | current route invokes `bx_ntvdm_cmd_return_exit_code_v1_dispatch` only after an internal launch slot; otherwise returns a bounded CF-clear answer; no terminal result crosses to the engine | **open complete owner package** |
| Top-level VDM exit | `54:00` lifecycle terminal record | typed stop after engine cleanup | intentionally distinct; cannot substitute for `54:0B` |

## Source/current mismatch at the remaining boundary

Current `54:0B` preserves a limited source-derived register resume and
package-local get-next state. The `bx_ntvdm_command_terminal_v1` contract,
however, deliberately permits only `TOP_LEVEL_EXIT`; its validator requires
`has_dos_exit_code == 0`. Therefore an ordinary guest child exit cannot cross
from original COMMAND parent-return semantics into the CLI lifecycle without
an explicit owner-scoped COMMAND terminal/reentry contract.

This is not a reason to reimplement DOS EXEC or create a host child process:
the missing boundary follows the guest's restored parent continuation. It is
also not a Bochs/mantle concern, and it is not a new BOP selected from trace.

## Recovery ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | `cmdReturnExitCode` and its command-broker contract remain normative. The historical translation unit cannot directly compose in x64 CLI because it depends on the old VDM command broker, event thread, and redirection product composition. |
| Smallest compatible seam | Required next package: a COMMAND-owned, fixed-width parent-return terminal/reentry record. It receives only the existing copied BOP ABI and declared launch-plan result; the engine receives only its opaque terminal outcome after normal cleanup. |
| External-code intrusion | Rejected. No Bochs, CPU, device, firmware, or mantle change is implicated. |
| New behavior | Rejected. No adapter DOS return queue, host child substitute, or top-level-exit alias is permitted. |

## Next package recommendation

Admit one complete **COMMAND parent-return lifecycle package**. Before
implementation it must source-map the original `54:0B` dispatcher/broker
ordering, record every Direct/Readonly/Overlay disposition, separate
"re-enter declared guest command" from "ordinary parent result", and define
the fixed-width lifecycle hand-off. Its regression must cover no-next-command,
declared-next-command, invalid/repeated return, Direct/Readonly parity, and
prove that `54:00` remains a distinct top-level terminal.

Only after that package and its all-family regression may T225 admit one
source-built COM parent/child/return fixture. MZ relocation follows only after
COM parent return is stable.