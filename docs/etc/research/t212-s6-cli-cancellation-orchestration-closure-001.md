# T212 S6: CLI Cancellation Orchestration Closure

Packet: M0 T212 S6, Ordinary Mode  
Date: 2026-08-15  
Status: source-built CLI orchestration closure

## Question

Can the native CLI turn a console control event into the already-admitted
fixed engine cancellation reason, join the in-process engine worker, and
classify only the copied completed result—without transferring a host handle,
pointer, callback, BOP identity, or guest state into mantle or Bochs?

## Delivered ownership boundary

`src/cli/ntdos64_console_cancellation_v1.[hc]` owns a single invocation's
manual-reset event and `SetConsoleCtrlHandler` registration.  Its control
handler accepts only Ctrl+C/Ctrl+Break and only signals that CLI-owned event.
It invokes neither engine nor cleanup API.

`src/cli/ntdos64_engine_worker_v1.[hc]` copies the complete engine request
before it starts a CLI-owned worker.  The outer CLI waits on that worker and
the CLI event.  On event observation, the outer thread calls only
`bx_ntvdm_engine_request_cancellation_v1(USER_REQUEST)`.  After the worker is
joined, it validates and copies the engine result, then releases the worker
handle.  No `HANDLE`, pointer, callback, or console object is present in the
engine contract.

The native entry starts this lifetime after BYOB admission and ends it before
lifecycle presentation.  It records whether the typed request was accepted;
that fact is supplied to the existing CLI-only lifecycle classifier, not to
the guest or BOP plane.

## Procedure and observations

1. A fresh focused MSVC x64 `/MT` fixture compiled the worker, copied engine
   contract, and a deterministic engine substitute.  Its uncancelled branch
   returned the original execution-budget result without an engine request.
   Its signalled-event branch accepted exactly one fixed cancellation reason,
   joined the worker, and observed the copied `HOST_CANCELLATION` result.  A
   null event was rejected before worker creation.
2. A fresh native CLI closure copied the pre-S5 current composition object
   cache into a new artifact root and rebuilt its one newly required S5
   controller object from current source.  It then compiled the native CLI,
   lifecycle classifier, console-event owner, and worker under MSVC x64
   `/MT`.  The declared current profile retained its expected pre-execution
   composition rejection (`terminal=2 detail=1 lifecycle=3 presentation=3
   cancellation=0`).  This is integration evidence for the no-cancel path,
   not guest execution or normal-completion evidence.

Commands:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T212S6CliCancellationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s6-cli-cancellation-r1

powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T212S6NativeCliCancellationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s6-native-cli-cancellation-r2 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s3-current-composition-r3
```

## Negative boundary and remaining work

The implementation contains no `TerminateProcess`, no direct Bochs stop-latch
write, and no BOP or OpenNT route.  It does not prove that a real console
Ctrl+C has reached a continuously executing guest; present native composition
still stops before guest CPU entry.  It also does not add wall-time or resource
limits, durable audit output, normal guest completion, or DOS-visible
cancellation semantics.  Those concerns remain separately governed.
