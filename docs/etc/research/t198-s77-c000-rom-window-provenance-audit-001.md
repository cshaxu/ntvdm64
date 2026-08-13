# M0 T198 S77: C000 ROM and Window-Provenance Audit

## Trace facts

S74/S76 instruction history records a far transfer from `073B:0939` to
`C000:014A`; the predecessor bytes begin `EA 4A 01 00 C0`. The resulting #UD
is real mode. Its display, `window=15:ffffffff`, means a 15-byte copied window
whose bytes begin `FF FF FF FF`; `15` is a length, not an instruction byte.
It is not a canonical adapter BOP window (`C4 C4 selector [service]`). The
fixture correctly stops before composition when a copied #UD is not canonical
BOP bytes.

## Machine lifecycle facts

`bx_ntvdm_minimal_machine_c::initialize` performs only minimal SIM setup,
ordinary-memory allocation, empty port-space allocation, CPU initialization,
A20 enable and CPU reset. It does not load a system BIOS, expansion ROM, or
historical SoftPC ROM/IVT image. `BX_MEM_C::init_memory_without_sim` allocates
the ROM backing and fills it with `FF`; the adopted core maps `C0000-DFFFF` as
expansion-ROM space.

The reached `C000:014A` address is therefore outside the declared finite
machine input closure. The core exception hook merely copies bytes from its
current fetch window at `prev_rip`; it neither identifies BOPs nor owns ROM
semantics. Nothing in the observed bytes justifies reconstructing a preceding
`C4 C4` sequence or assigning `cassette_io`/INT15 meaning.

## Classification

This is an omitted machine ROM/IVT provisioning condition, not an OpenNT DEM
or host-service BOP and not a defect to hide in `bx-vdm`. The controlled stop
is correct. The required next work is a whole machine-input map: determine
which reached guest vector/ROM stubs are historically required, whether their
source bytes and initialization order are available, and what minimal
mantle-owned provisioning interface can expose them without making adapter or
Bochs core interpret DOS/BOP semantics.
