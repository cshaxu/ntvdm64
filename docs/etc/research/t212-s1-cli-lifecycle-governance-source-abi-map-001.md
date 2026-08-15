# T212 S1: CLI Lifecycle Governance Source/ABI/Failure Map

Packet: M0 T212 S1, Ordinary Mode  
Date: 2026-08-15  
Status: closed source/ABI mapping; no runtime behavior changed

## Question

What coherent CLI-owned lifecycle boundary can govern the existing one-shot
engine without turning a COMMAND-local record, a host process exit, or a
legacy child-runner handle into a guest result?

## Inputs and procedure

- The lifecycle decision in
  [the CLI capability roadmap](proposal-cli-noninvasive-capability-roadmap-001.md#modern-cli-lifecycle-governance).
- Current copied engine contract and execution path:
  `src/bx-mantle/bx_ntvdm_engine_contract_v1.[hc]` and
  `src/bx-mantle/bx_ntvdm_engine_run_v1.c`.
- Current machine-stage execution lifetime:
  `src/bx-mantle/bx_ntvdm_machine_stage_v1.[hcc]`.
- The native CLI entry and its current presentation mapping:
  `src/cli/ntdos64_native_cli.c`.
- The retained external-child runner cancellation path:
  `src/cli/ntdos64_run.c`; and the separate native-Bochs child helper,
  `src/cli/ntdos64_bochs_engine.c`.
- The private fixture-only finite runner:
  `src/bx-mantle/bx_ntvdm_finite_run.[hcc]`.
- T203, T206 and T207 closure records.

The source sweep searched for every engine terminal enum producer, every
instruction-budget consumer, and every console-cancellation/child-wait API.
No native trace, Bochs build or BOP implementation was run for this mapping.

## Current ownership and disposition

| Concern | Current source owner and copied boundary | Observed disposition | S2 implication |
| --- | --- | --- | --- |
| Request validation and instruction budget | `bx_ntvdm_engine_request_v1` is fixed-width; the mantle validates nonzero `instruction_tick_budget`. `ntdos64-native` hard-codes `1,000,000`. | Available, but policy is implicit in the CLI source rather than a named lifecycle record. | Define CLI-owned copied policy input and resolve its instruction budget before forming the unchanged engine request. |
| Machine execution budget | Machine-stage `execute` installs a Bochs timer and returns `EXECUTION_BUDGET`; engine maps it to `BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET`. | Available engine producer; mantle owns timer and reset. | CLI may select/present the result, but must not own the timer or Bochs stop primitive. |
| Controlled guest terminal | Machine-stage generic typed STOP maps to `CONTROLLED_GUEST_TERMINAL`; the native CLI maps it to process success. | Available engine producer. Its meaning is a controlled engine terminal, not a DOS exit code. | Preserve separately from normal guest completion in the lifecycle result/presentation table. |
| Ordinary guest completion | The enum exists, and the native CLI currently maps it to process success. T203/T206/T207 prove no engine producer and that `54:0B` is only a package-local COMMAND record on the current profile. | Explicitly unproduced/deferred. | S2 must retain a distinct state but cannot expose it as a reachable success result. |
| Host cancellation | The engine enum exists but `engine_run_v1` has no cancellation input or producer. The old runner owns an event and terminates only a child it created. | Explicitly unproduced for the in-process engine. | Do not pass raw events/handles into mantle or guest code. First define a CLI-owned cancellation request/result record; a later admitted S may add a cooperative engine seam. |
| Composition failure and cleanup | `engine_run_v1` resets composition after install failure, machine-stage preparation failure and after every execute return; it always resets stage before reporting a terminal result. | Available; mantle/composition owns cleanup order. | CLI records the returned result only after the call. It must not independently reset or retain mantle objects. |
| Machine failure | The engine maps begin/entry/reset failure and unrecognised execute outcomes to `MACHINE_FAILURE`. | Available engine producer. | Keep distinct from composition failure and user cancellation. |
| Result presentation | `ntdos64-native` prints two numeric fields and maps controlled terminal/ordinary completion to `0`, request/composition rejection to `3`, budget to `4`, and all else to `1`. | Current host presentation; ordinary-completion branch is dormant. | Establish one named presentation mapping with distinct ordinary, controlled, budget, cancellation, composition, machine and invalid-result categories. Host exit remains host-only. |
| Audit | No lifecycle audit record or durable sink exists in the engine/CLI path; only diagnostic `wprintf`/`fwprintf` output exists. | Absent. | Define an in-memory copied audit/result record first. A later file sink must be explicit CLI policy and ordinary user-mode I/O. |
| Wall-time and resource limits | No current request field, producer, clock or process/resource limiter is in the engine path. | Absent/deferred. | Do not represent a wall-time or resource guarantee until a separately admitted cooperative or process-topology mechanism exists. |

## Host API availability audit

The relevant normal user-mode Win32 APIs are not missing: the retained runner
already uses `CreateEventW`, `SetConsoleCtrlHandler`, `WaitForMultipleObjects`,
`TerminateProcess`, `CloseHandle`, and `CreateProcessW`; an ordinary process
can also use clocks and ordinary file APIs for later CLI-owned accounting.
No deleted or unavailable Win32 API blocks S2.

The limitation is **composition**, not API availability. `TerminateProcess`
is correct only for the separately created child in `ntdos64_run.c`. The
current engine executes Bochs in the invoking process, so using that API would
kill the CLI instead of producing a bounded engine terminal. Likewise, the
private finite-run timer and `kill_bochs_request` are mantle fixture mechanics,
not a public cancellation ABI.

## Required boundary and failure model

```text
CLI option/policy
  -> copied CLI lifecycle policy + in-memory audit/result
  -> unchanged copied engine request/result
  -> mantle composition -> machine stage -> Bochs execution
  <- engine terminal and completed cleanup
  <- CLI-only presentation/audit
```

The first lifecycle record is owned by `src/cli`. It contains only fixed-width
policy and result values: selected instruction budget, cancellation request
state, terminal classification and host presentation classification. It carries
no guest pointer, DOS register value, Bochs object, timer identifier, callback,
Windows `HANDLE`, or engine-owned lifetime.

Failure rules are therefore:

1. An invalid CLI policy is rejected before the engine request is formed.
2. A composition, machine, budget or controlled terminal remains the engine's
   typed result and is only classified by the CLI after cleanup returns.
3. `54:0B` and a host process exit do not create ordinary guest completion.
4. A Ctrl+C request may be recorded by the CLI, but until a cooperative engine
   seam is separately implemented it cannot be reported as an in-flight
   `HOST_CANCELLATION` result.
5. Audit write failure, output-stream failure and future wall/resource limits
   must receive their own host lifecycle classifications; none may overwrite a
   guest or engine result.

## Follow-up and confidence

High confidence for the current producer/consumer map: all results follow
from the current runtime sources and the T203/T206/T207 native conclusions.
The coherent next subtask is **T212 S2: fixed-width CLI lifecycle
policy/result-disposition ABI**. It may implement and test policy validation,
classification and in-memory audit records in `src/cli` only. It must not add
a cancellation route to the engine, alter Bochs/mantle, claim normal guest
completion, run a native trace, or implement BOP behavior. A future S3 must
be separately admitted before cooperative in-process cancellation or any
wall-time/resource enforcement is attempted.
