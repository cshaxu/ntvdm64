# T223 S1 COMMAND Dispatcher and Current-Route Inventory

## Authoritative entry

OpenNT routes `BOP_CMD` (`54h`) through `softpc.new/host/src/nt_bop.c::MS_bop_4`,
which reads the service byte at `CS:IP`, calls `CmdDispatch`, then advances IP by
one. The authoritative table is `base/mvdm/dos/command/cmddisp.c::apfnSVCCmd`;
it has exactly 17 callable slots (`00..10h`). `cmdsvc.h` supplies the identifiers.

## Inventory and current reconciliation

| Service | Original handler/source | Original owner | Current bx-vdm route | T223 disposition to verify |
| --- | --- | --- | --- | --- |
| `00` | `cmdExitVDM`, `cmdexit.c` | VDM lifecycle | lifecycle provider | source-shaped controlled termination |
| `01` | `cmdGetNextCmd`, `cmdmisc.c` | launch/bootstrap | bootstrap get-next | recover full CMDINFO/queue lifecycle |
| `02` | `cmdComSpec`, `cmdmisc.c` | bootstrap | COMSPEC provider | source contract and guest gather/write audit |
| `03` | `cmdSaveWorld`, `cmdmisc.c` | historical save-world | original-noop plane | retain only after source no-op verification |
| `04` | `cmdGetCurrentDir`, `cmdmisc.c` | session namespace | current-dir provider | reconcile host/session CWD and error ABI |
| `05` | `cmdSetInfo`, `cmdmisc.c` | DOS/COMMAND registration | launch plane | preserve registration layout/lifetime |
| `06` | `cmdGetStdHandle`, `cmdredir.c` | redirection/std-handle table | package route | CLI stream backend or explicit unavailable |
| `07` | `cmdCheckBinary`, `cmdexec.c` | executable classification | launch-execution provider | source map, not an isolated check leaf |
| `08` | `cmdExec`, `cmdexec.c` | host launch/execution | launch-execution provider | bounded CLI child-process capability decision |
| `09` | `cmdInitConsole`, `cmdmisc.c` | console/VDD handoff | console provider | CLI terminal capability or deferred |
| `0A` | `cmdExecComspec32`, `cmdexec.c` | host COMSPEC launch | launch-execution provider | package with `07/08` |
| `0B` | `cmdReturnExitCode`, `cmdexec.c` | execution lifecycle | lifecycle provider | original return/exit transition |
| `0C` | `cmdGetConfigSys`, `cmdconf.c` | bootstrap configuration | boot-file provider | retain declared guest input only |
| `0D` | `cmdGetAutoexecBat`, `cmdconf.c` | bootstrap configuration | boot-file provider | retain declared guest input only |
| `0E` | `cmdGetKbdLayout`, `cmdkeyb.c` | keyboard/console | console provider | source-derived CLI capability disposition |
| `0F` | `cmdGetInitEnvironment`, `cmdenv.c` | environment | COMSPEC/bootstrap provider | separate final ownership |
| `10` | `cmdGetStartInfo`, `cmdmisc.c` | NT session broker | facade typed stop | deferred/unavailable disposition |

## Current-code findings

`bx_ntvdm_command_plane_v1.c` presently groups `01,02,04,05,0C,0D,0F,10` as
bootstrap, `06,07,08,0A` as launch/execution, `09,0E` as console, `0B` as
execution lifecycle, and `03` as original-noop. This is a useful ingress
partition but not an accepted source-owner map: `0F` is environment and `10`
is a historical session broker. `bx_ntvdm_command_package_facade_v1.c` marks
every slot except `10` as an existing provider and translates `10` to a
controlled stop. That must not be mistaken for complete COMMAND composition.

## Next evidence unit

S1 next derives each handler's register/memory ABI, original failure/return path
and unavailable host binding. Only then may T223 select its first whole
implementation component; native trace is not an architecture selector.