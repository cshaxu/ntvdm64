# T176 S15 — DEM PDB Termination Provider Audit 001

## Original provider

`demdisp.c` maps `50:3C` to `demTerminatePDB` in `demsrch.c`. Its ordered
contract is:

1. copy terminating PSP/PDB from BX;
2. call `VDDTerminateUserHook(PSP)` unless `IsFirstCall`;
3. call `HostTerminatePDB(PSP)`;
4. locate the PSP-owned FindFirst/FindNext list, release each search entry,
   unlink and free the PSP list; an absent list is a successful no-op.

The host implementation of `HostTerminatePDB` calls floppy and fdisk cleanup.
`VDDTerminateUserHook` walks registered NT VDD user callbacks. Both belong to
historical intrusive host composition, not to Bochs CPU/device ownership.

## Current composition disposition

The non-invasive CLI profile has no VDD registration, floppy/fdisk state or
original PSP-keyed `demsrch.c` list. Its finite boot-namespace search state is
adapter-owned and has a different explicit lifetime. Therefore original source
cannot be linked as a standalone provider, and an unconditional `50:3C`
acknowledgement would misrepresent the original cleanup contract.

The only admissible future path is a **source-derived DEM session-lifecycle
provider** under the common ingress/registry/DEM plane: it must be selected
only when the profile declares no VDD/device backend and the terminating PDB
has adapter-owned finite search state. It must perform that state cleanup
before returning the original no-result resume. Any VDD, device or ambient
host-search requirement stays unavailable/deferred rather than being placed in
Bochs, guest DOS or an ad-hoc BOP handler.
