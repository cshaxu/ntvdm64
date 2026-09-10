# M0 T355 S17 — CCPU SAS physical-memory lifecycle audit

## Question

After S16 removed the reached C-VID null-vector terminal, why did the fixed
CPU40/x86 product fault in original `ccpusas4.c::_phyR` while reading address
`0x03750000`?  This audit determines whether the reached access can come from
the admitted external-page mapping path or from the original SoftPC SAS RAM
backing lifecycle.

## Inputs

- The one immutable S16 exception record at
  `O:\ntvdm\m0-t355-s16-cvidc-vector.txt.exception.txt`.
- The selected original MVDM mirror files
  `softpc.new/base/ccpu386/ccpusas4.c`,
  `softpc.new/host/src/{config.c,sim32.c,nt_mem.c}` and their selected OpenNT
  originals.
- The formal CPU40/x86 map
  `build/M0-T355/S3/formal-x86/original-softpc-process.exe.map`.
- The existing bounded physical-page adapter
  `adapter-mvdm-host-out/softpc/mvdm_softpc_physical_mapping.c`.

## Source walk

1. `config.c` first invokes `sas_term()`, calculates
   `vdmMemorySize = xmsMemorySize + emsMemorySize + 1024`, then calls
   `sas_init(vdmMemorySize * ONEKB)`.
2. Original `ccpusas4.c::sas_init` installs the CPU40 SAS vector, calls
   `host_sas_init(size)`, marks `[0, size)` as `SAS_RAM`, stores
   `Length_of_M_area = size`, then initializes ROM state.
3. Selected CPU40 `sim32.c::host_sas_init` calls original
   `nt_mem.c::InitIntelMemory(size)`.  That routine reserves the full Intel
   span and commits its conventional-memory/A20-wrapped initial region.  The
   original allocator owns later commits for allocated XMS/EMS ranges.
4. Original `phyR` rejects only a physical address outside
   `Length_of_M_area`; otherwise its RAM/default branch dereferences the
   immediate result of `c_GetPhyAdd(addr)`.
5. The sole current adapter additions before the unchanged RAM calculation in
   `c_GetPhyAdd` are an EMS numeric alias probe and a checked external-page
   resolver.  A resolver hit returns early; a miss preserves the original
   `Start_of_M_area + addr` calculation.
6. The only source callers that publish a host-backed physical mapping are
   `VdmAddVirtualMemory` consumers in WOW/DIB paths.  Their own original
   conditional scope is later graphics/WOW work; the selected DOS startup
   path does not reach a publisher, prepare, or active mapping record.  EMS
   aliases also require later EMM page-map activity.  Therefore neither
   adapter route can explain the S16 startup access.

## Observation interpretation

The S16 exception has a faulting runtime IP of `0x007b9651`.  With image base
`0x007b0000` it resolves to link address `0x00409651`; the formal map places
that address inside original `_phyR` (`0x00409520` through `0x00409677`).
The attempted memory target was `0x03750000`.

The source walk proves that this is an original SAS-RAM dereference through
the normal `Start_of_M_area + addr` route, not a BOP, DEM FastRead, C-VID
vector, external mapping-manager, or EMS alias result.  It does **not** prove
from static material alone whether `addr` was erroneous, whether the chosen
physical span had not yet been committed, or whether a prior lifecycle action
had invalidated/replaced the M-area allocation.  Those cases share the same
machine instruction and exception form.

## Disposition

The next and only selected cohort is a default-off CCPU SAS direct-RAM
observation.  It must record bounded scalar state at the existing normal
`c_GetPhyAdd` seam—Intel address, wrap mask, `Length_of_M_area`, M-area base,
and whether an adapter alias/resolver returned—without retaining a native
pointer, changing a CPU result, changing mapping ownership, committing
memory, or changing a BOP/DEM result.  A single fixed-container observation
may then distinguish the three remaining original lifecycle branches before
any repair is admitted.

No production behavior changes in S17.
