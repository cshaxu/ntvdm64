# T225 S27 P1 — COMMAND Parent-Return Source, ABI And Failure Map

## Question

What is the smallest source-faithful COMMAND-owned completion seam for a guest
DOS child that reaches `54:0B`, and how must it distinguish declared guest
re-entry, ordinary parent completion, and top-level VDM exit?

## Inputs

- OpenNT COMMAND: `dos/command/cmddisp.c`, `cmdexec.c:cmdReturnExitCode`,
  `cmdmisc.c:cmdGetNextCmd` and `cmdUpdateCurrentDirectories`, `cmdredir.c`,
  `cmd.h`, and `dos/v86/cmd/command/tcode.asm`.
- Current bound composition: `bx_ntvdm_cmd_get_next_service.*`,
  `bx_ntvdm_command_lifecycle_provider_v1.*`,
  `bx_ntvdm_command_package_session_v1.*`,
  `bx_ntvdm_boot_namespace_composition_v1.*`, and engine run control.
- Prior owner evidence: T225 S20, S21, S25 and S26.

## Original COMMAND contract

`cmddisp.c` assigns service 11 to `cmdReturnExitCode`. COMMAND.COM's two
return paths load `DX=retcode`, obtain `AL=current drive` through INT 21h AH
19h, copy its redirection completion pointer into `BX:CX`, issue the service,
and branch on CF:

| Original branch | Required source effect | Current product disposition |
| --- | --- | --- |
| Enter | `VDMInfo.ErrorCode = DX`; enter `RETURN_ON_NO_COMMAND`; increment shell count and block the historical event thread. | Preserve copied low-16-bit input only until the source-defined result is formed; old broker/thread state is not a CLI ABI. |
| Directory handoff | `cmdUpdateCurrentDirectories(AL)` publishes the DOS program's drive context. | Use the existing selected host/session directory capability only when the completed COMMAND package admits it; do not retain an unbounded guest pointer. |
| Redirection completion | `cmdCheckCopyForRedirection(BX:CX)` retires pipes/files and converts allocation failure to `ERROR_NOT_ENOUGH_MEMORY`. | Historical pipe/thread/handle ownership is unavailable for guest-DOS plan execution. It is a contained unavailable/deferred subcapability, never a reason to change `54:0B` CF or fabricate success. |
| Declared next command | `GetNextVDMCommand` reports a command: set CF and `IsRepeatCall`, causing COMMAND.COM to re-enter its command-get-next path. | Preserve through the existing immutable launch-plan delivery state: CF=1, no terminal result, exactly one return per delivered slot. |
| No next command | `GetNextVDMCommand` reports no command: clear CF; set AL from final exit value; resume event thread/standard handles; clear block. | Preserve CF=0 and AL=`DX & 0xff`; publish one opaque ordinary-completion fact after the engine's ordinary cleanup boundary. |

The OpenNT translation unit cannot directly compose: it calls the historical
VDM command broker, event-thread functions, redirection-handle helpers,
SoftPC-facing standard-handle notification, and legacy global session state.
That rules out rung 1 direct reuse for the current x64 CLI, but it does not
change COMMAND ownership.

## Current composition and exact gap

| Surface | Existing behavior | Status |
| --- | --- | --- |
| `54:01` | Copies one immutable plan slot into guest state and increments `delivered`; it has no ambient host queue. | Retain. |
| `54:0B`, declared next slot | `bx_ntvdm_cmd_return_exit_code_v1_dispatch` sets CF, marks `returned`, and carries no terminal record. | Retain; it is the source-shaped re-entry path. |
| `54:0B`, exhausted plan | The same helper clears CF, writes AX low byte from DX, and creates `BX_NTVDM_CMD_TERMINAL_REASON_V1_DECLARED_PLAN_EXHAUSTED` with the low DOS exit byte. | Source-derived CMD fact exists but is orphaned from engine completion. |
| `54:00` | Lifecycle provider creates a separate `TOP_LEVEL_EXIT` record and typed stop. | Retain distinct. It cannot mean a child returned to its parent. |
| Engine handoff | Engine resets machine/runtime, then queries only `copy_ordinary_terminal`, which currently accepts only `TOP_LEVEL_EXIT` without a DOS code. | Gap: cannot consume the existing parent-return record. |

## Required seam and ABI rules

The smallest compatible seam is **not** a second dispatcher and not an engine
understanding of COMMAND. It is a COMMAND-package method that, after the
existing `54:0B` result is formed, exposes one opaque ordinary-completion fact
only when the current CMD terminal record is valid and present. The outer
composition may combine that fact with the existing top-level terminal fact
for engine lifecycle classification only after normal machine cleanup.

It must satisfy all of the following:

1. only a valid, exhausted, already-returned `54:0B` state may produce the
   parent-completion fact;
2. no-next returns CF clear and AL is exactly `DX & 0xff`; a declared next
   command returns CF set and cannot create that fact;
3. invalid, duplicate, cross-service, or pre-delivery `54:0B` returns fail
   closed without changing plan state;
4. `54:00` remains the only top-level terminal origin and carries no DOS exit
   code; it is not an alias for a parent return;
5. the engine receives no BOP, selector, guest address, host handle, or DOS
   code. It sees only its existing opaque ordinary-completion boolean after
   reset/cleanup; and
6. no guest-memory read is needed at return: DX, AL and BX:CX are already in
   the copied CPU state. Unsupported directory/redirection subcapabilities
   retain explicit contained failure/deferred disposition.

## Profile and failure matrix

| Case | Direct | Readonly | Overlay |
| --- | --- | --- | --- |
| Guest DOS plan return/re-entry | Same copied COMMAND contract; no host-child dependency. | Same copied COMMAND contract. | ABI reserved; no implementation admitted. |
| Host current-directory capability | Existing Direct session capability may be selected only through its separate owner. | Existing readonly failure/disposition remains. | Deferred. |
| Historical pipes/redirection cleanup | Not silently approximated by host child handling. Explicit unavailable/deferred unless its complete owner is admitted. | Same. | Deferred. |
| Parent completion publication | One opaque post-cleanup fact, once. | Identical. | ABI-compatible extension boundary only. |

## Procedure and observations

The source and current code above were inspected directly. The existing
`tests/bx-vdm/bx_ntvdm_cmd_get_next_service_test.c` already proves the narrow
return register/CF and copied terminal record cases, but it does not exercise
the composition-to-engine consumer. `copy_ordinary_terminal` demonstrably
reads only `lifecycle_provider.terminal`, whose validator disallows a DOS exit
code. This is direct source evidence of the missing handoff, not a trace
inference.

## Interpretation and confidence

Confidence is high. The original dispatcher and two COMMAND.COM callers agree
on service identity and CF meaning; the current code has the exact low-byte
terminal fact but does not publish it to the opaque engine boundary. The
correct recovery rung is (2), a smallest source-derived rehost of the original
broker outcome. No Bochs intrusion, device, firmware, DOS-kernel behavior,
host child process, or synthetic DOS return queue is justified.

## Follow-up

Implement this one seam in the existing COMMAND package and composition
consumer, then add one focused regression covering no-next, declared-next,
invalid/repeated return, Direct/Readonly parity and the distinct `54:00`
terminal. Only after the family regression passes may a bounded native
parent-return observation be admitted. A source-built COM parent/child fixture
remains a later S; MZ remains later still.