# T225 S20 COMMAND full-package route reconciliation 001

## Question

For every callable original COMMAND BOP slot in `cmddisp.c` (`54:00..10`),
what is its actual current bx-vdm package route, which source-recovery rung is
available, and which Direct/Readonly disposition may be tested without
turning a native trace into a list of leaf patches?

## Method and source boundary

The original ordered table is
`src/opennt/base/mvdm/dos/command/cmddisp.c`; owner implementation units are
`cmdexit.c`, `cmdmisc.c`, `cmdredir.c`, `cmdexec.c`, `cmdconf.c`, `cmdkeyb.c`,
and `cmdenv.c`. The modern route is inspected through
`bx_ntvdm_bop_ingress_v1.c`, `bx_ntvdm_bop_provider_registry_v1.c`,
`bx_ntvdm_command_package_facade_v1.c`,
`bx_ntvdm_command_package_session_v1.c`, and its four owned subproviders.

No original COMMAND translation unit is currently a directly composable x64
object: each includes the historical `cmd.h`/`cmdsvc.h`/`softpc.h` graph and
uses CCPU/SAS guest pointers, VDM event-thread state, or historical process
composition. Thus rung 1 is not selected for any slot. The audit records
rung 2 only where the modern package preserves a source-proven copied ABI,
order, and failure rule. No Bochs intrusion is relevant (rung 3). A new
behavior may be considered only after the corresponding source contract and
unavailable dependency are separately recorded (rung 4).

## 17-slot current-route ledger

| BOP | Original owner / rule | Current package route | Recovery and profile disposition | P1 decision |
| --- | --- | --- | --- | --- |
| `54:00` | `cmdexit.c:cmdExitVDM`; top-level VDM exit | lifecycle provider | rung-2 copied terminal, Direct/Readonly controlled stop | retain; engine publication is separate |
| `54:01` | `cmdmisc.c:cmdGetNextCmd`; CMDINFO / no-command lifecycle | bootstrap `get_next` | rung-2 copied CMDINFO; Direct/Readonly declared-plan response | retain; multi-program queue is T225 work |
| `54:02` | `cmdmisc.c:cmdComSpec`; first-call COMSPEC | bootstrap COMSPEC | rung-2 copied guest gather/write and first-call state | retain |
| `54:03` | `cmdmisc.c:cmdSaveWorld`; body compiled out | lifecycle provider | rung-1 original no-op semantics; both modes resume | retain |
| `54:04` | `cmdmisc.c:cmdGetCurrentDir`; `=X:` environment / drive validity | bootstrap current-dir service | rung-2 copied directory transaction; must audit real admitted host CWD and source error split | review before acceptance; synthetic-root success is prohibited |
| `54:05` | `cmdmisc.c:cmdSetInfo`; SCS pointer registration | bootstrap launch plane | rung-2 copied locator registration | retain |
| `54:06` | `cmdredir.c:cmdGetStdHandle`; host standard handles | stream session | rung-2 opaque Direct handles; Readonly failure is typed (no handle export) | retain pending full negative regression |
| `54:07` | `cmdexec.c:cmdCheckBinary`; DOS/non-DOS classification | execution lifecycle | rung-2 declared DOS-only profile; no ambient pathname probe | retain |
| `54:08` | `cmdexec.c:cmdExec`; host child execution and redirection | child redirection / fallback lifecycle | rung-2 Direct child backend; Readonly returns source-shaped contained failure | audit full launch/return ordering |
| `54:09` | `cmdmisc.c:cmdInitConsole`; event-thread/Video VDD start | console provider | rung-2 no-install console disposition; no VDD or GUI | retain as explicit profile disposition |
| `54:0A` | `cmdexec.c:cmdExecComspec32`; host COMSPEC child | child redirection / fallback lifecycle | rung-2 Direct child backend; Readonly contained failure | audit together with `54:08` |
| `54:0B` | `cmdexec.c:cmdReturnExitCode`; return/reentry/redirection completion | lifecycle / child completion | rung-2 copied terminal and completion state | audit after `54:08/0A` ordering |
| `54:0C` | `cmdconf.c:cmdGetConfigSys` | bootstrap boot-file provider | rung-2 immutable guest boot-input transaction; both modes | retain |
| `54:0D` | `cmdconf.c:cmdGetAutoexecBat` | bootstrap boot-file provider | rung-2 immutable guest boot-input transaction; both modes | retain |
| `54:0E` | `cmdkeyb.c:cmdGetKbdLayout`; no-KB16 path | console/keyboard provider | rung-2 `DX=0` no-install disposition; both modes | retain; not a KEYB/VDD claim |
| `54:0F` | `cmdenv.c:cmdGetInitEnvironment`; copied OEM environment and paragraph retry | bootstrap environment | rung-2 copied host snapshot and checked ES:0 write; both modes | retain; full environment regression required |
| `54:10` | `cmdmisc.c:cmdGetStartInfo`; `DosSessionId` | facade deferred terminal | source-proven unavailable session broker; controlled stop | retain as explicit deferred terminal |

`54:11` remains the non-callable `SVC_CMDLASTSVC` sentinel and must be
rejected before provider selection.

## Environment correction

OpenNT `cmdenv.c:179..324` dynamically allocates the filtered initial
environment, then reports the actual required number of paragraphs in BX when
the ES buffer is too small. It is not limited to 4023 bytes. The current
`BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES` ceiling is 65462 bytes,
which reserves the maximum 16-bit paragraph-addressable transaction. Its
capture code filters `=...`, replaces COMSPEC with the 16-bit value, removes
WINDIR for DOS, converts to OEM, uppercases variable names, adds `PROMPT=$P$G`
when absent, and preserves the paragraph-retry contract. Any 4023-byte
assertion or fallback is therefore stale and must be removed or corrected by
the family regression rather than treated as a host-environment failure.

## Required S20 sequencing

1. Build a 17-slot fixture that proves the current route and exact
   Direct/Readonly terminal for each ledger row, including sentinel rejection.
2. Review the `54:04`, `54:06`, `54:08`, `54:0A`, `54:0B`, and `54:0F`
   subfamilies against their original ABI/order/failure rules. Migrate or
   replace only the documented stale seam; do not add a selector recognizer.
3. Run one whole-family regression. Only after it passes may exactly one
   bounded native run validate the package; new observed BOP values have no
   implementation consequence.

## Boundary

This is a source/current-route admission map, not a claim that COMMAND,
console, host-child execution, or guest multi-program execution is closed.
The next P is the family fixture and source/ABI failure comparison, not a
native trace and not a per-service patch.
