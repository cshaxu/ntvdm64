# M0 T318 S2 P24 — Fixed-container startup owner clusters

## Decision

The selected CPU40 x86 runtime observation has one formal container only:
the tracked non-debug, console-owning launcher from P21/P22.  It owns real
`CONIN$` and `CONOUT$`, uses the short formal stage, passes the immutable
`-f -o --ordinary-child` product contract, and waits eight seconds.  A later
observation may replace only the formally linked product executable.  It must
not change console ownership, debugger state, firmware, media, command line,
timeout, or host-process ownership.

The current product baseline remains the reproducible non-debug exit
`0xC0000005`.  This document does not attribute that exit to a BOP, NTDOS, or
a particular CPU-vector slot.

## Static startup boundary

The source-defined pre-NTIO route is bounded as follows:

```text
host_main
  -> host_applInit
     -> init_host_uis -> InitScreenDesc -> SetupConsoleMode
     -> nt_start_event_thread
  -> InitialiseDosEmulation
     -> io_init -> reset -> SetupInstallableVDD
     -> scs_init
        -> GetNextVDMCommand(NULL)
        -> CMDInit -> DemInit -> XMSInit -> DBGInit
     -> original ntio.sys lookup/read/SAS entry setup
  -> host_start_cpu -> c_cpu_simulate
```

The scan separates the following complete owner clusters.  Their order is the
only permitted recovery order before an NTIO-specific observation.

1. **Console/display initialization.** `nt_hosts.c::InitScreenDesc` obtains
   the process standard handles, conditionally opens `CONIN$`/`CONOUT$`, and
   calls `SetupConsoleMode`.  The latter can call original
   `DisplayErrorTerm` after a real Win32 console failure.  Its caller-facing
   private Console Server requirements (`RegisterConsoleVDM`, input wait and
   display operations) are already isolated in the selected
   `adapter-mvdm-host-out/win32` source-shaped compatibility family.  No
   alternate console observer or product-side fallback is admitted.
2. **Error termination.** `nt_error.c::DisplayErrorTerm` translates an NT or
   Win32 error and calls original `host_error(..., ERR_QUIT, ...)`.  It is a
   common terminal surface, not a root cause.  Every new failure attribution
   must name its direct original caller; no generic suppression is allowed.
3. **Base VDM acquisition.** `nt_msscs.c::scs_init` calls
   `GetNextVDMCommand(NULL)` before the COMMAND, DEM, XMS and debugger
   initializers.  The selected `adapter-mvdm-host-out/basesrv` provider is the
   source-shaped local BaseClient/BaseSrv boundary.  Private CSRSS capture,
   remote handle duplication and multi-console records are not silently
   retried or re-created by the observer.
4. **Machine/media preparation.** `io_init`, reset, installable-VDD setup and
   the unchanged original `ntio.sys` lookup/read/SAS/CS:IP sequence are
   separate SoftPC owners.  Media absence is tested only by a declared formal
   counterfactual; normal observations retain the same staged media.
5. **CCPU/C-VID assembly.** `host_start_cpu` calls `cpu_simulate`; the C-VID
   table is therefore a whole owner contract.  P23 restores its generated
   assembly from `c2cpusad.h`.  The 26 explicit unavailable slots are either
   absent from source or excluded by the original retail CPU40 profile
   (`PIG`/`SPC486`); neither fact authorizes a global macro change or a
   per-slot runtime patch.  Under `CCPU`, the C-VID header routes ordinary
   core providers directly to their `c_*` bodies, so an unavailable non-CCPU
   vector member is not by itself evidence for the current crash.

## Verification cadence

1. Ninja builds only changed libraries and the final product link.
2. The immutable container runs one five-to-ten-second startup observation.
3. NTIO/NTDOS/EXEC-specific evidence is enabled only after the fixed result
   proves the product reached the corresponding original boundary.

This prevents a debugger, pipe, inherited console or changed stage from being
mistaken for a product repair.
