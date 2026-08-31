# M0 T318 S2 P32 — CPU40 memory and first-dispatch boundary audit

## Purpose

Close the static audit of the original selected CPU40 memory-to-first-dispatch
cluster before changing the fixed r30 observation container, adding a BOP
probe, or attributing the current unhandled access violation to guest code.

## Selected source path

The selected non-V86 CPU40 source preserves this order:

1. `softpc.new/base/support/main.c::host_main` calls original `config()`.
2. `softpc.new/host/src/config.c::config` calls original
   `sas_init(vdmMemorySize * ONEKB)`.
3. `softpc.new/base/ccpu386/ccpusas4.c::sas_init` calls original
   `host_sas_init`, assigns its returned backing pointer to both
   `Start_of_M_area` and `CCPU_M`, marks the requested span RAM, then calls
   `rom_init()`.
4. The selected CPU40 `softpc.new/host/src/sim32.c::host_sas_init` calls
   original `InitIntelMemory(size)` from `softpc.new/host/src/nt_mem.c`.
5. `main.c` subsequently calls `cpu_init()` (the CCPU40 alias of
   `c_cpu_init`), builds GDP/C-VID state, initializes the host screen, and
   calls `InitialiseDosEmulation`.
6. `InitialiseDosEmulation` loads the selected `ntio.sys` through its original
   file-read and SAS write sequence, then sets `CS:IP` to the NTIO entry.
7. `host_start_cpu → c_cpu_simulate → ccpu(FALSE)` begins the first CCPU
   dispatch.

## Memory ownership result

`InitIntelMemory` is still the original contiguous SoftPC Intel-memory
allocator for this path. It reserves the requested Intel range with
`VirtualAlloc`, commits the real-mode range, initializes its original section
table, and returns an `IU8 *` backing pointer. The selected x86 runtime uses
that pointer directly through `CCPU_M`; it is not a 32-bit opaque mapping ID.

The session mapping manager has a different, later responsibility. The only
reached integration in `ccpusas4.c::c_GetPhyAdd` first asks
`mvdm_softpc_physical_mapping_translate/resolve` whether an address is a
registered external physical-page binding; otherwise it retains the original
`Start_of_M_area + addr` fast path. `nt_mem.c::VdmAddVirtualMemory` likewise
uses the mapping manager only for external host-page aliases. It does not
participate in initial SAS allocation, ROM load, NTIO load, or the first
ordinary CCPU fetch.

Therefore an access violation cannot currently be attributed to a mapping-ID
conversion in the baseline RAM/NTIO path. Conversely, replacing the original
contiguous SAS backing with a mapping-manager token would be an unapproved
semantic change.

## First-dispatch prerequisites

- `c_cpu_init` calls the original `ccpu386InitThreadStuff` before the later
  `c_cpu_simulate` call. This establishes the main-thread `jmp_buf` carrier;
  its failure is a normal original diagnostic/failure path, not evidence of a
  current null frame.
- `c_cpu_reset` initially constructs the historical reset state. The later
  original `InitialiseDosEmulation` deliberately overwrites `CS:IP` with the
  loaded NTIO entry before `host_start_cpu`.
- C-VID/GDP setup is separate from the RAM allocator. The selected original
  `cvidc/ev_glue.c::setup_global_data_ptr` uses the registered GDP carrier and
  original `setup_vga_globals` then performs the generated C-VID binding at
  its existing setup point. Its complete slot disposition remains P23's
  evidence.
- `rom_init` is called during SAS initialization. The local `copyROM` stub is
  a selected historical CCPU carrier symbol; it is not evidence that ROM load
  was skipped, because the actual ROM path is `rom_init` and the selected
  firmware configuration was separately frozen by P7/P30.

## Disposition

No source modification is admitted by this audit. The fixed r30 container,
media, firmware, console ownership, arguments and timeout remain unchanged.
The next static recovery unit is the source-defined handoff cluster:

`c_cpu_simulate → ccpu(FALSE) → first CCPU page/dispatch`, together with its
directly reachable CCPU exception, timer-event and C-VID function-table
branches. It must be audited as one CPU40 owner contract before another r30
run. BOP, DEM, COMMAND, NTDOS and external physical-page work remain outside
that unit until execution reachability is proved.
