# M0 T198 S75: BIOS INT 15 Cassette-Handler Audit

## Reached call

S74's copied generic event is a real-mode `C4 C4 15` BOP at `C000:014A` with
`AX=0E43h`. The immediately preceding traced code performs a far transfer to
that address from the guest keyboard/BIOS path. OpenNT `spckbd.asm` defines
`int15h_caller` as `int 15h` followed by its CPU-quit continuation; its
initialization table publishes that caller and the INT 15 vector. The reached
event is therefore a guest BIOS INT 15 request, not a Bochs exception defect
or a DEM service.

## Original module-wide ownership

The SoftPC BIOS BOP table in `base/bios/bios.c` maps selector `15h` to
`cassette_io` in `base/bios/tape_io.c`. Its header explicitly states that the
assembler BIOS invokes it for INT 15 and that unimplemented cassette I/O
returns an appropriate error. The module also owns distinct AT extensions:
device notifications, wait/event timing, extended-memory size, block move,
virtual-mode, configuration, joystick and several device paths.

Those branches require historical ROM/SAS, timer, CMOS, PIC, memory, or device
state. They are not admitted by the contained CPU5 profile and must remain
declined rather than being converted into a generic BIOS implementation.

## Exact reached branch

`AH=0Eh` matches no case in `cassette_io`. Its `default` falls through to the
original invalid-function result: `setCF(1)` and `setAH(INT15_INVALID)`, where
`INT15_INVALID` is `86h`. `setAH` preserves the caller's AL, so the reached
`AX=0E43h` result is exactly `AX=8643h`, CF set, and all other GPR/flag fields
unchanged. The BOP instruction is three bytes, so the host result resumes at
`fault_rip + 3`; the guest ROM/BIOS continuation owns the subsequent IRET.

## Decision

The next S may add one source-derived machine-handler disposition only for
exact real-mode BOP `15h`/AH=`0Eh`: `AX.high=86h`, CF=1, resume `+3`. It must
decline every other BOP 15 function, including AH=88h (already owned by the
separate admitted memory query) and all device/timer/ROM-dependent cases. The
provider belongs in `bx-vdm`; it does not change Bochs, firmware bytes, or
the adapter's generic ABI.
