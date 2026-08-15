# Bochs 2.6 Machine-Assembly Boundary: r11 Finding

## Question

Can the existing Bochs 2.6 `nogui` configuration be treated as a bare CPU/RAM
machine merely by disabling optional features, or must machine assembly move to
the adapter boundary?

## Evidence

The r11 `make -n bochs.exe` closure shows that Bochs requests an `iodev`
library containing `devices.cc`, timers, PIC, PIT, serial/parallel, floppy,
keyboard, VGA, CMOS, DMA, disk and other device objects. This is not a claim
that every device is active in the chosen guest profile; it is a compile/link
closure fact.

`refs/bochs/iodev/devices.cc:154-206` is the decisive runtime fact. Its
`bx_devices_c::init` unconditionally initializes timer infrastructure and
loads core `cmos`, `dma`, `pic`, `pit`, `floppy`, and `vga`, followed by the
standard keyboard path. `--disable-pci`, `--disable-usb`, `--disable-smp` and
similar configure switches reduce feature families, but do not make that
method a bare-machine assembler.

On 2026-08-10, r11 compiled `iodev/devices.o` directly with the declared UCRT
toolchain. The previously observed r10 default-target failure is therefore
not evidence that this particular translation unit is intrinsically
uncompilable; it remains a full-target evidence issue until reproduced under a
bounded closure record.

## Result

The source supports the owner's earlier expectation: the first viable
architecture is **adapter-owned assembly over preserved Bochs mechanics**, not
“Bochs has assembled a PC and adapter merely configures it.” The latter would
retain the entire hard-wired `devices.cc` policy and obscure the true NTVDM64
reuse boundary.

This does **not** authorize reimplementing CPU, memory, timing, PIC, CMOS or
devices in the adapter. The adapter may select and connect existing Bochs
mechanisms only through an explicit machine-composition interface. The first
next design task is to derive the smallest reset profile's required endpoints
from the reset ROM trace, then determine the minimal generic Bochs lifecycle
hook that permits adapter selection without exposing Bochs C++ objects in the
versioned host/guest ABI.

## Explicitly Rejected Shortcuts

- Calling the current `bx_devices_c::init` and describing its full default
  device suite as a “minimal” machine.
- Inferring runtime necessity from static link presence.
- Replacing any Bochs device in adapter code.
- Adding a BOP/DEM/DOS-specific branch to Bochs device initialization.
- Opening a full `make` loop before this composition interface and its exact
  object closure are defined.
