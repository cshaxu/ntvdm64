# T212 S5: Selector-Blind Mechanical Cancellation Closure

Packet: M0 T212 S5, Ordinary Mode  
Date: 2026-08-15  
Status: source-built mechanical closure

## Delivered boundary

The registered `BX-MANTLE-081` exception is implemented by:

- `src/bx-mantle/bx_ntvdm_cancellation_controller_v1.[hcc]`: one
  process-local controller with the sole fixed reason
  `USER_REQUEST` and no handle, pointer, callback, guest data, selector or
  BOP field;
- `bx_ntvdm_machine_stage_v1`: a continuous 1024-instruction cancellation
  poll timer. Its CPU-thread callback alone reads controller state and may set
  the existing Bochs `kill_bochs_request` latch;
- `bx_ntvdm_engine_run_v1`: activation around the existing composition/stage
  lifecycle, safe phase checks, unconditional normal reset sequence and
  mapping of the new stage status to the pre-existing
  `HOST_CANCELLATION` engine terminal;
- `bx_ntvdm_engine_request_cancellation_v1(reason)`: the typed engine-facing
  request. It has no lifecycle object or host resource argument.

The minimal Bochs configuration macroizes `inline`; therefore C++ standard
library atomics cannot be linked into mantle. The controller instead uses
MSVC x64 interlocked intrinsics from `intrin.h`. This is a toolchain-local
atomic primitive, not a Win32 API integration: the production controller has
no Windows header, handle, callback or thread code.

## Verification

### Mechanical stage

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T212S5MechanicalCancellationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s5-mechanical-cancellation-r5
```

Fresh MSVC x64 `/MT` build/run passed. It proves:

1. invalid/inactive/duplicate reason rejection;
2. pre-entry cancellation returns the distinct stage cancellation status;
3. an in-flight request is issued only after a default-off fixture witness
   proves the CPU-thread cancellation poll has run; the next poll stops the
   loop and returns cancellation;
4. generic typed `#UD` STOP remains `CONTROLLED_STOP` while controller is
   active but unrequested;
5. an unrequested HLT run remains `EXECUTION_BUDGET`;
6. stage reset and controller deactivation allow another initialized run.

The poll witness is compiled only under
`BX_NTVDM_CANCELLATION_TESTING`; it is absent from production sources and
contains no host or guest data. The test fixture's Windows thread/event is
strictly evidence orchestration and is not linked into the controller or CLI.

### Engine cleanup/result mapping

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T212S5EngineCancellationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s5-engine-cancellation-r3
```

Fresh MSVC x64 `/MT` build/run passed. Its copied composition substitute
requests cancellation only after engine activation. The engine resets the
composition, deactivates the controller, returns
`HOST_CANCELLATION` with `USER_REQUEST`, rejects a post-cleanup cancellation,
then completes a second run as its ordinary budget result. This is a focused
engine lifecycle proof, not guest execution evidence.

The current source scan finds stop-latch writes only in the existing watchdog
callback and the new CPU-thread cancellation callback. The controller itself
does not contain a latch, a Bochs object, host API, BOP or OpenNT/DOS term.

## Limitations and follow-up

S5 deliberately does not install a console control handler, make a worker
thread, impose wall time/resource limits, or expose cancellation through a
guest/BOP route. The current native CLI composition is still rejected before
guest execution for its present profile; S5 does not reinterpret that result.

T212 S6 may implement only CLI-owned event/worker orchestration around the
closed typed engine request, with a copied result join and explicit no-engine
handle crossing. Wall-time and resource profiles remain separate admission
work after that orchestration is proven.
