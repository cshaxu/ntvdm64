# M0 T318 S2 P21 — Fixed Console Startup Baseline

## Purpose

Make startup results comparable.  This evidence defines one external
observation container for the selected x86 product.  It is not a product
input, debugger, profiler, breakpoint injector, guest-memory scanner, or BOP
route.

## Fixed container contract

- Source: `tests/observation/console_startup_observer.c`.
- It allocates and owns a real `CONIN$` / `CONOUT$` console, clears only its
  inherited screen buffer, and passes those three standard handles to the
  child.
- It launches exactly `original-softpc-process.exe -f -o --ordinary-child` in
  the supplied immutable stage directory.
- It uses no debugger creation flag, no hardware breakpoint, no process-memory
  read, and no product instrumentation.
- It waits exactly eight seconds; on timeout it records the console screen and
  terminates only the child with `0x53504354`.
- `tools/observation/ObserveSoftpcStartup.mjs` records the SHA-256 of the
  stage manifest beside every result.  Future observations may replace only
  the formally linked product executable; firmware, media, process arguments,
  console ownership, and timeout stay fixed.

## Baseline execution

The x86 formal `original-softpc-process.exe` was staged with
`tools/build/Stage-OriginalSoftpcRuntime.mjs` into
`build/M0-T318/S2/runtime-console-baseline`.  Its stage-manifest digest is
`5a9a8193533a8421fce621ba55ff50ecb3115a24a9473248bc9b92d2a3ba1bcb`.

The fixed container reported:

```text
container=console-owning-nondebug
result=exited
exit=0x00000001
timeout-ms=8000
```

The cleared console capture contains no product text.  Therefore this is a
real product startup failure, but it does not yet assign it to a particular
host call or claim CPU/NTIO reachability.

## Static startup prerequisite scan

The selected source path is:

```text
softpc.new/base/support/main.c::host_main
  -> nt_reset.c::host_applInit
     -> nt_hosts.c::init_host_uis
        -> nt_hosts.c::InitScreenDesc
        -> nt_event.c::nt_start_event_thread
  -> configuration / CMOS / cpu_init / VGA globals
  -> nt_msscs.c::InitialiseDosEmulation
     -> io_init -> reset -> SetupInstallableVDD
     -> scs_init
        -> GetNextVDMCommand(NULL)
        -> CMDInit -> DemInit -> XMSInit -> DBGInit
     -> selected DOS-media lookup -> CreateFile/ReadFile -> SAS -> CS:IP
  -> nt_cpu.c::host_start_cpu -> c_cpu_simulate
```

The resulting owner clusters, in the order they can terminate startup before
NTIO is reached, are:

1. **Console/display/session initialization.** `InitScreenDesc` directly
   calls `GetConsoleMode`, `SetConsoleMode`, cursor and screen-buffer APIs and
   sends failures to `DisplayErrorTerm`.  `RegisterConsoleVDM` is reached by
   `nt_det.c`; its selected same-shaped adapter supplies only the text-buffer
   contract.  This cluster is the first candidate because the new fixed
   container has proved that usable real standard console handles alone do not
   make startup succeed.
2. **Error termination.** `nt_error.c::DisplayErrorTerm` passes `ERR_QUIT` to
   `host_error`; it can turn any earlier prerequisite failure into the observed
   process exit.  The next repair evidence must identify its caller, not
   suppress or reinterpret the error.
3. **Base VDM command acquisition.** `scs_init` calls `GetNextVDMCommand(NULL)`
   before `CMDInit`, `DemInit`, `XMSInit`, or `DBGInit`.  The selected local
   BaseClient/BaseSrv-shaped provider is a separate cluster; it must retain
   original failure and wait ordering.
4. **Machine and media initialization.** `io_init`, `reset`,
   `SetupInstallableVDD`, selected DOS-media lookup, SAS write and entry
   transfer all precede `host_start_cpu`.  They remain original SoftPC/adapter
   owner work; no BOP route is admitted here.
5. **CCPU execution table assembly.** `host_start_cpu` enters
   `c_cpu_simulate`; C-VID and CPU vector-table initialization must be complete
   before this boundary.  This is a whole-table CCPU contract task, not a
   per-null-pointer workaround.

## Next action

Inspect and recover cluster 1 and the exact `DisplayErrorTerm` caller as one
source-shaped startup package.  Do not add a different observer, change the
stage media, or infer NTIO reachability until the fixed container moves past
this terminal result.
