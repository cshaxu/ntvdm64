# M0 T350 S2 — CPU40/SAS/BIOS bootstrap recovery

Date: 2026-09-01

## Question

After the original `MS_bop_4 -> CmdDispatch -> cmdSetInfo` return, does the
selected CPU40 product already contain one complete, source-shaped route for
the first NTDOS `msinit.asm` mechanics, or is a new BOP, guest change, memory
mapper, or CPU executor required?

## Inputs

- OpenNT `softpc.new/host/src/nt_bop.c`, `dos/command/cmdmisc.c`,
  `softpc.new/base/support/main.c`, `softpc.new/host/src/nt_cprgs.c`,
  `softpc.new/host/src/nt_cpu.c`, and `softpc.new/base/ccpu386/ccpusas4.c`.
- Immutable NTDOS source at `mvdm-guest/dos/v86/doskrnl/dos/msinit.asm`,
  lines 346--386.
- The current selected adapter/session bindings and the completed T336,
  T340, T345, T347 and T349 records.

## Source and binding walk

1. `MS_bop_4` reads the one service byte, invokes the original COMMAND
   dispatch, and advances IP by one byte.  Its current observation hook
   (`MVDM-HOST-DIV-164`) runs after decoding and has no result, register,
   pointer, or failure effect.
2. `cmdSetInfo` retains the exact `DS:DX`, `DS:BX`, and `DS:CX` locations
   through the existing `mvdm_command_guest_state` synchronous lease.  This
   is the already-closed T347 replacement for historical durable raw aliases;
   it neither executes CPU instructions nor changes the service return.
3. The next NTDOS instructions restore `BX`, `CX`, and `DX`, copy the
   pre-existing `DI:BX` BIOS exchange pointer into guest `BiosDataPtr`, set
   DOSDATA/low-interrupt fields, save the caller stack, and set
   `SS:SP = DS:dskstack`.  They contain no host service ingress.
4. The original startup body in `base/support/main.c` performs `cpu_init()`
   before `InitialiseDosEmulation()` and before `host_start_cpu()`.  Its
   selected CPU40 call to the original `load_sw_cpu_access_functions()` is
   `MVDM-HOST-DIV-182`: it restores the historical access-vector installation
   formerly reached by the retired CPU30 product shell.
5. The selected original CPU40 branch of
   `nt_cprgs.c::load_sw_cpu_access_functions()` installs `c_get*`, `c_set*`,
   flag accessors, and `host_simulate_func = c_cpu_simulate`.  Its CPU40
   predicate (`MVDM-HOST-DIV-151`) selects the original CCPU body; it does
   not add another executor.
6. `nt_cpu.c::host_start_cpu()` still calls the original CPU macro, which is
   `c_cpu_simulate` for CPU40.  The thin original-name recursive wrapper and
   the removal of duplicate non-CCPU SAS globals are the previously registered
   DIV-150 and DIV-030 link accommodations.
7. `ccpusas4.c::sas_init()` owns the sole selected CCPU backing range,
   original SAS vector table, ROM loading and ordinary RAM fetch/store path.
   The session guest-memory lease is exposed only around host-owned pointer
   operations; ordinary CPU instruction fetches and NTDOS stack writes remain
   direct original CCPU/SAS operations.  No second mapper participates.
8. The BIOS exchange pointer is guest state supplied before NTDOS continuation
   by the already selected BIOS/FDC/INT15/ICA initialization cohort.  It is
   not a host BOP result and no source identifies an FDC wait as an immediate
   prerequisite of the first post-`SVC_CMDSETINFO` stack transition.

## Recovery disposition

All first-rung original source bodies are already in the selected CPU40/x86
graph.  The smallest existing same-shaped bindings are T347's synchronous
numeric guest lease and the outer session execution bridge; neither replaces
an NTDOS, CCPU, SAS, BIOS, or BOP algorithm.  Therefore S2 admits **no new
source change**.  Adding a BOP result, guest mutation, alternative executor,
or new mapper would be a second implementation and is rejected.

## Result

The complete source-defined owner cohort is present: original COMMAND return,
CPU40 access vector, CCPU instruction executor, SAS storage, and pre-existing
BIOS exchange state.  S3 may now verify only this declared contract through
the existing focused CPU40/SAS execution fixture and a fresh formal CPU40/x86
product link.  It must not claim that the process has reached COMMAND.COM,
child execution, FDC completion, or a later BOP.

## Follow-up

S4 may run one unchanged fixed-container observation after S3.  Any later
device wait, BOP, guest failure, or different CPU boundary belongs to a new
source-defined cohort and is not repaired inside T350.
