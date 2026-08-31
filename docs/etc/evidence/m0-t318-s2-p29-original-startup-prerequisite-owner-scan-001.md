# M0 T318 S2 P29 — Original startup prerequisite owner scan

## Scope and method

This is the required static predecessor scan for the selected CPU40 product.
It follows the original entry path, rather than inferring a missing service
from a single runtime symptom:

`obj.vdm/ntvdm.c::main` → `base/support/main.c::host_main` →
`host/src/nt_reset.c::host_applInit` → original SoftPC setup →
`host/src/nt_msscs.c::InitialiseDosEmulation` →
`host/src/nt_cpu.c::host_start_cpu`.

It records a direct startup predecessor if it can terminate/report an error,
depends on a historical console/CSR product facility, or controls the point at
which guest execution becomes possible.  It is not a claim that every listed
path has executed in the current product.

## Owner clusters

- **Entry and early host state — original MVDM host.** `main` performs
  `TimerInit`, `CpuEnvInit`, `nls_init`, then `host_main`.  `CpuEnvInit` treats
  an absent historical `...\\Wow\\CpuEnv` key as an empty set; it is not an
  unconditional startup failure.  `nls_init` remains the original resource
  dependency and was already restored by S2 P4.  Neither routine dispatches a
  BOP.

- **Console/display and event ownership — original MVDM host plus public
  Win32 bindings.** `host_applInit` requires `-f`, initializes ICA state, then
  calls `init_host_uis` and `nt_start_event_thread`. `InitScreenDesc` and
  `SetupConsoleMode` can call `DisplayErrorTerm` on invalid standard handles,
  console modes or buffer information. They use public console APIs; the P22
  container deliberately supplies `CONIN$`/`CONOUT$`. The later
  `initTextSection` `RegisterConsoleVDM` boundary is separately source-shaped
  by S2 P19. This whole cluster is therefore an owner review unit, not a
  reason to add a BOP shim.

- **Core SoftPC initialization — original machine host.** `host_main` keeps
  the original ordering: `gfi_init`, virtual-driver initialization, `config`,
  CMOS pickup, `cpu_init`, CCPU/VGA global setup and `host_init_screen`. Any
  failure here precedes DOS media and BOP ingress. The selected CPU40 C-VID
  vector assembly is the P18/P23 unit; an unavailable C-VID slot remains an
  explicit machine-owner disposition, not an adapter implementation.

- **DOS media and first-VDM control — original MVDM host with existing
  adapters.** `InitialiseDosEmulation` sets the fixed VDM state, runs
  `io_init`, `reset`, `SetupInstallableVDD`, optional LIM setup, and
  `scs_init`; it then resolves and loads original `ntio.sys` before setting
  CS:IP.  P2 provides only the immutable session media root and P5 provides
  the absent-retired-VDD-key-as-empty-configuration distinction. `scs_init`'s
  `GetNextVDMCommand(NULL)` is first-VDM state only. The ordinary child is not
  consumed until guest COMMAND `54:01`, as proven by P26.

- **CPU handoff — original CPU40 SoftPC.** `host_start_cpu` immediately calls
  `cpu_simulate`; there is no direct `DisplayErrorTerm`, BaseSrv command
  acquire, NTIO file operation or BOP operation between this call and the CPU
  engine.  Therefore a normal product that has reached this call may be
  executing before it can possibly emit a `50h` or `54h` record.

- **Error termination — original host/UI owner.** `DisplayErrorTerm` routes
  to the original error-dialog machinery. The current composition's registered
  session termination divergence keeps an original guest/VDM error from
  killing the app process, but it must not convert errors into successful
  startup. No static startup caller in this scan has a source-defined return
  value of `120`; the historical `120` dialog was a console-product symptom,
  not a stable product exit code.

## Consequence for runtime work

The P28 instrumented derivative is rejected, so it supplies no BOP reach
evidence. The next recovery action must choose one complete unresolved cluster
above and recover it from its original source and already-declared adapter
boundary. It must not add selector-specific BOP logic or change the fixed
runtime container. Only after an unchanged normal product reaches a stable
NTIO marker is a NTDOS/EXEC-specific observation admitted.
