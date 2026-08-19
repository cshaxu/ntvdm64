# T230 S15 P2 checked-RAM transport

`bx-mantle` now exposes a selector-blind synchronous ordinary-RAM read/write
surface that is usable only while a `machine_stage_v1` is active.  It carries
only physical address and copied bytes; no BOP, DEM, OpenNT, CPU object or
host capability enters the ABI.

The focused active-stage fixture proves:

- read/write reject before `machine_stage_v1_begin` and after reset;
- a one-byte ordinary-RAM write and read succeed while the stage is active;
- an aperture-end write is rejected; and
- the previously written byte remains unchanged after that rejection.

The fixture was compiled and linked using the formal CPU5/MSVC x64 `/MT`
Ninja configuration in `build/M0-T230-S15/checked-ram-r1`; its explicit
process result was `EXIT=0`.

This is mechanical transport evidence only.  It does not bind a BOP or a DEM
provider and therefore cannot constitute native DEM closure.
