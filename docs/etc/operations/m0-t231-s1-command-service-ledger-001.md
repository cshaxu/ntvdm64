# M0 T231 S1 COMMAND service ledger

## Source authority

The identity list is the original `apfnSVCCmd[]` order in
`src/opennt/base/mvdm/dos/command/cmddisp.c`, with constants from
`src/opennt/base/mvdm/inc/cmdsvc.h`.  `nt_bop.c` reads the service byte and
calls `CmdDispatch`; it does not replace the owner dispatcher.

## Service map

| BOP | Original function and file | Contract class | T231 cluster | Historical composition seam |
| --- | --- | --- | --- | --- |
| `54:00` | `cmdExitVDM`, `cmdexit.c` | terminal lifecycle | S9 | historical VDM exit/event ownership → controlled CLI stop shim |
| `54:01` | `cmdGetNextCmd`, `cmdmisc.c` | command queue and `CMDINFO` delivery | S7 | `GetNextVDMCommand`, guest memory, re-entry state |
| `54:02` | `cmdComSpec`, `cmdmisc.c` | host environment query → guest buffer | S2 | `GetEnvironmentVariable`, OEM conversion, checked guest write |
| `54:03` | `cmdSaveWorld`, `cmdmisc.c` | saved CPU/guest context | S3 | CCPU registers and fixed `SAVEWORLD` layout |
| `54:04` | `cmdGetCurrentDir`, `cmdmisc.c` | host directory → DOS path | S2 | drive/current-directory and checked guest write |
| `54:05` | `cmdSetInfo`, `cmdmisc.c` | `CMDINFO` registration | S3 | guest pointer/address validation and lifetime |
| `54:06` | `cmdGetStdHandle`, `cmdredir.c` | standard handle projection | S4 | x86/x64 handle-token mapping and inheritance |
| `54:07` | `cmdCheckBinary`, `cmdexec.c` | executable type/path admission | S6 | host file search, DOS/WOW/Win32 disposition |
| `54:08` | `cmdExec`, `cmdexec.c` | host-child launch | S8 | guest env/handles, scheduler pause/resume |
| `54:09` | `cmdInitConsole`, `cmdmisc.c` | console initialization | S4 | host console and standard-handle capability |
| `54:0A` | `cmdExecComspec32`, `cmdexec.c` | host `%COMSPEC%` launch | S8 | same `cmdExec32` lifecycle as `54:08` |
| `54:0B` | `cmdReturnExitCode`, `cmdexec.c` | child completion/re-entry | S8 | redirection completion, queue/re-entry, resume |
| `54:0C` | `cmdGetConfigSys`, `cmdconf.c` | CONFIG.SYS preprocessing | S5 | PIF input, transformed temporary file, OEM guest write, terminal failure |
| `54:0D` | `cmdGetAutoexecBat`, `cmdconf.c` | AUTOEXEC preprocessing/environment extraction | S5 | PIF input, transformed temporary file, OEM guest write, terminal failure |
| `54:0E` | `cmdGetKbdLayout`, `cmdkeyb.c` | keyboard layout | S4 | host layout/code-page capability and guest write |
| `54:0F` | `cmdGetInitEnvironment`, `cmdenv.c` | initial environment block | S5 | OEM/ANSI, DOS-size bounds, checked guest write |
| `54:10` | `cmdGetStartInfo`, `cmdmisc.c` plus `cmdpif.*` | startup/session information | S5 | historical PIF/session state → admitted CLI startup contract |

## v1 retirement map

The retained COMMAND v1 code is comparison-only and has no formal product
membership.  T231 deletion scope includes every `bop-v1/bx_ntvdm_cmd_*`,
`bop-v1/bx_ntvdm_command_*`, and the COMMAND portions of
`bop-v1/bx_ntvdm_adapter_runtime.*` and
`bop-v1/bx_ntvdm_boot_namespace_composition_v1.*`.  The latter two mixed
historical files cannot be deleted until their non-COMMAND owners are moved or
their dependency is removed; their COMMAND references are nevertheless
product-forbidden from S1 onward.

## Classification

The simple query/bootstrap group is only `54:02` and `54:04`.  Original
`cmdconf.c` proves that `54:0C/0D` create and transform temporary boot files,
so they belong with environment/startup configuration in S5 rather than a
synthetic path-query implementation.
All remaining services either own a persistent guest/host state boundary or
belong to a coherent lifecycle cluster.  This classification is intentionally
conservative: a service is not made simple merely because a prior v1 fixture
resumed it.

## Required shim boundary

The future `command_*` shims may provide only: checked guest-memory and
register access; 32-bit guest-token representation of host handles; host
environment/current-directory/console/process capabilities; and a session
scheduler equivalent to the original event/re-entry contract.  They may not
recognize BOP bytes or implement DOS/Bochs mechanics.
