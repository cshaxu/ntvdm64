# T212 S4: Cooperative Cancellation and Resource-Limit Admission Map

Packet: M0 T212 S4, Ordinary Mode  
Date: 2026-08-15  
Status: source/ABI/failure mapping; no engine or Bochs change

## Question

Can a CLI cancellation request stop the in-process minimal Bochs engine with
a bounded, selector-blind mechanical operation while preserving the existing
engine cleanup and typed-result ownership? Which time/resource limits are
admissible on that same boundary?

## Source facts

| Fact | Evidence | Consequence |
| --- | --- | --- |
| `bx_ntvdm_engine_run_v1` owns composition install, stage begin/entry/execute, stage reset and composition reset in one synchronous call. | `src/bx-mantle/bx_ntvdm_engine_run_v1.c` | A CLI must not reset a stage or composition during cancellation. The engine remains the sole cleanup owner. |
| Machine-stage execution already registers a one-shot instruction timer; its callback runs in the CPU execution path and sets `bx_pc_system.kill_bochs_request`. | `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc` | A cancellation poll can reuse this native timer mechanism without a CPU decoder or event-loop rewrite. |
| Bochs returns from `cpu_loop` when the existing `kill_bochs_request` latch is observed in asynchronous event handling. | `src/bx-core/cpu/event.cc` | The required mechanical stop primitive already exists and is core-owned. Mantle need only request it at an admitted safe point. |
| `kill_bochs_request` is a Bochs `volatile` field, not a cross-thread synchronized lifecycle API. | `src/bx-mantle/pc_system.h` | A Win32 console handler, CLI worker or adapter must never write it directly. |
| The retained external-child runner signals a Win32 event from its console handler and lets its owning runner terminate/reap a child process. | `src/cli/ntdos64_run.c` | This pattern is usable only for CLI event ownership. `TerminateProcess` is not valid for the in-process engine. |
| The engine result already has `HOST_CANCELLATION`, but no producer. | `src/bx-mantle/bx_ntvdm_engine_contract_v1.h` and `engine_run_v1.c` | A source-backed producer can map a new machine-stage cancellation status to the existing copied engine result; the V1 result layout need not change. |

## Rejected alternatives

| Alternative | Rejection reason |
| --- | --- |
| Console handler writes `bx_pc_system.kill_bochs_request` directly | Cross-thread access to a non-synchronized Bochs field; leaks CLI/Win32 control semantics into the machine. |
| Pass a Win32 event/HANDLE, callback, pointer or `std::atomic*` in an engine/mantle request | Violates the fixed-width, no-handle/no-pointer bridge rule and makes lifetime ambiguous. |
| Have the console handler invoke arbitrary mantle or composition cleanup | Console control handlers are not the engine cleanup owner; a direct cleanup race can destroy active machine state. |
| Use `TerminateProcess` on the native engine | The engine is in the CLI process; this kills the host instead of yielding a typed terminal and cleanup. |
| Poll cancellation from BOP/adapter ingress or a DOS service | Cancellation is host lifecycle policy, not BOP/DOS/OpenNT semantics; this would make it selector-dependent. |
| Impose a Job Object, self working-set change or host CPU quota as a current resource-limit guarantee | These act on the CLI process or require a separate worker process topology, cannot publish a typed in-process terminal, and are not a safe current profile capability. |

## Admitted candidate boundary

The sole candidate is a **selector-blind mantle lifecycle controller** with a
small engine-facing C API. It is not a Bochs-core modification and it carries
no host handle, callback, guest state, BOP selector or OpenNT identity.

```text
console handler -> CLI-owned Win32 event
                       |
CLI main thread waits for engine-worker/event
                       |
                       +--> engine request-cancel(reason) [fixed enum only]
                                      |
                                      v
                       mantle-private atomic cancel state
                                      |
                  existing CPU-thread timer callback polls state
                                      |
                                      v
                existing kill_bochs_request -> cpu_loop return
                                      |
           machine-stage cancellation status -> engine HOST_CANCELLATION
                                      |
              existing stage reset + composition reset -> copied result
```

The public request needs only a fixed `uint32_t` reason (`USER_CANCEL` for the
first slice); it needs no pointer or object identity because the engine permits
one active process-local stage today. Mantle owns the atomics, stage activity,
timer registration and CPU-thread latch write. The engine checks an already
requested cancellation at safe phase boundaries (before composition entry and
before stage execution), and maps the post-execute cancellation status to the
already-defined engine terminal. CLI owns thread/event handles, output and
the decision to call the typed request API; none cross into mantle.

The CPU-thread timer polls at one named bounded interval. A request observed
there wins over watchdog expiry at that same callback; otherwise the existing
instruction-budget result wins. Generic typed `#UD` STOP remains its existing
distinct result and never becomes host cancellation.

## Time and resource disposition

| Concern | Current disposition | Reason/follow-up |
| --- | --- | --- |
| Instruction budget | Available | Existing mantle timer and engine `EXECUTION_BUDGET` result remain authoritative. |
| Explicit user cancellation | Candidate, S5-admissible | Requires the selector-blind atomic controller and CLI worker/event orchestration above. |
| Wall-time budget | Deferred after S5 | CLI can own a normal user-mode wait timeout and request the same typed cancel reason, but must first have a tested cooperative stop and a distinct presentation reason. |
| Guest/engine memory limit | Deferred | The minimal machine has fixed admitted RAM today; no host-process memory guarantee is honest without an independently designed process topology. |
| Host CPU quota/Job Object | Deferred/not admitted | A self-applied limit cannot safely yield an in-process typed terminal; a child-process design would be a separate CLI architecture task. |
| Durable audit sink | Deferred | S2 audit is in-memory; file/stream failure needs its own CLI-only contract and must not overwrite the engine terminal. |

The normal user-mode Win32 event/thread/wait APIs needed for future CLI
orchestration are available; no missing Win32 API blocks the candidate. The
missing part is the project-owned cooperative engine boundary, not a host
system dependency.

## S5 admission

T212 S5 may implement and test only the candidate selector-blind controller:

1. register its mantle lifecycle change in the Bochs-intrusion exception
   ledger before modification;
2. use mantle-private atomic state and an existing CPU-thread timer callback;
3. expose one fixed reason enum and no handles/pointers/callbacks;
4. add a distinct machine-stage cancellation status and map it to existing
   engine `HOST_CANCELLATION` only after normal reset/cleanup;
5. prove pre-entry cancellation, in-flight cancellation, budget precedence,
   generic typed stop distinction, malformed reason rejection and two-run
   cleanup/reuse with a source-built MSVC x64 `/MT` fixture.

S5 may not add console handling, worker threads, wall-time, resource limits,
BOP behavior, guest inspection, adapter routing or Bochs-core changes. CLI
orchestration is a later task after the mechanical cancellation fixture closes.
