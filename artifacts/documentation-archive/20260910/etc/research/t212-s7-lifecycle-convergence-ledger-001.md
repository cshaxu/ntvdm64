# T212 S7: Lifecycle Convergence and Deferral Ledger

Packet: M0 T212 S7, Ordinary Mode  
Date: 2026-08-15  
Status: read-only convergence closure

## Question

Do the current engine, mantle and CLI provide one unambiguous ownership and
presentation disposition for every V1 terminal, without misreporting host
cancellation, a controlled BOP stop, or a current composition rejection as
ordinary DOS completion?

## Current terminal ledger

| Engine terminal | Current producer | Cleanup owner | CLI lifecycle / presentation | Disposition |
| --- | --- | --- | --- | --- |
| `REJECTED_REQUEST` | engine request validation | no stage installed | rejected engine request | current |
| `REJECTED_COMPOSITION` | copied composition admission/preparation | engine resets composition and controller | rejected composition | current |
| `MACHINE_FAILURE` | controller activation, begin/entry/reset, or unclassified stage result | engine resets stage/composition and controller where active | machine failure | current |
| `EXECUTION_BUDGET` | mantle execution watchdog | engine resets stage/composition and controller | execution budget | current |
| `CONTROLLED_GUEST_TERMINAL` | typed existing machine-stage stop | engine resets stage/composition and controller | controlled guest terminal | current; not ordinary completion |
| `ORDINARY_GUEST_COMPLETION` | none | none | ordinary completion | valid enum/classifier entry only; deferred |
| `HOST_CANCELLATION` | S5 selector-blind controller and engine mapping | engine resets stage/composition and controller | host cancellation | current; not DOS-visible completion |

`ntdos64_lifecycle_v1.c` has exactly one classified branch for every terminal
enumerator.  `ntdos64_native_cli.c` maps a controlled terminal and a future
ordinary completion to host exit `0`; it maps current host cancellation to
the nonzero fallback.  This is a presentation contract, not a fabricated DOS
exit status.

## Handle, cleanup, and mutation ownership

| Concern | Owner | Current rule |
| --- | --- | --- |
| Console callback and event | `src/cli` | callback only signals its event; event is unregistered and closed after joined worker return |
| Worker handle/request/result | `src/cli` | request is copied before start; result is copied only after join; handle never enters engine ABI |
| Cancellation state and CPU-thread stop request | mantle | controller carries a fixed reason; its timer callback is the only new latch writer |
| Stage/composition reset | engine | one synchronous engine owner, never console callback or CLI worker |
| BOP/DOS/OpenNT semantics | not involved | host lifecycle cancellation is selector-blind and has no guest route |

The retained child-runner `TerminateProcess` path is not part of this
in-process lifecycle.  The current source scan and S5/S6 fixtures record no
raw CLI handle, pointer or callback transfer to engine/mantle.

## Explicit deferrals

| Capability | Disposition | Reason |
| --- | --- | --- |
| Real guest normal completion | deferred | no engine producer or end-to-end normal guest execution evidence exists |
| Wall-clock timeout | deferred | an instruction budget exists; an additional host-clock policy was not admitted |
| Process resource limit | deferred | Job/process-limit routes would change process topology or cannot yield the current typed in-process result safely |
| Durable lifecycle audit sink | deferred | current audit is copied in-memory policy/result data only |
| DOS-visible Ctrl+C/BOP behavior | rejected from T212 | it would add guest/service semantics to a host lifecycle boundary |

## Closure interpretation

T212 closes lifecycle governance only.  Its completed boundary is the
selector-blind, cooperative host cancellation path and its copied CLI
presentation.  It does not close ordinary guest execution, BOP recovery,
interactive COMMAND, redirection, resource policy, or any deferred host
capability.  The first queued interactive package remains inadmissible until
its stated one-shot normal-termination and owner-package evidence exists.
