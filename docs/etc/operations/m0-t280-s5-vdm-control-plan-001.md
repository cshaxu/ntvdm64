# M0 T280 S5 — reached VDM directory-control adapter

The selected original DEM package reaches only one historical private VDM
control: `NtVdmControl(VdmQueryDir, VDMQUERYDIRINFO)`. S5 restores the
original declaration and the synchronous session dispatch contract, not the
full NT4 kernel VDM service.

`VDMQUERYDIRINFO` contains host-local handle, buffer, Unicode-string and index
values created by DEM. It must never enter the guest-memory mapping manager.
The later DEM namespace capability owns the actual directory-query backend and
registers the session control handler. Before that registration, and for every
other VDM service class, the adapter returns `STATUS_NOT_IMPLEMENTED`.

`winbasep.h` remains an empty behavior-free same-name include carrier because
the selected DEM sources reach none of its declarations.
