# T95 S6 empty-port source-object separation

## Result

`BX-IO-029` and the follow-on `BX-IO-030` isolate the exact original
empty-port initialization and default-handler registration members into
`src/bochs/iodev/minimal_port_space.cc`:

- `bx_devices_c::init_empty_port_space(void)`;
- `bx_devices_c::default_read_handler(...)`; and
- `bx_devices_c::default_write_handler(...)`;
- `bx_devices_c::register_default_io_read_handler(...)`; and
- `bx_devices_c::register_default_io_write_handler(...)`.

Those five definitions were removed from
`src/bochs/iodev/devices.cc`. Their signatures and bodies, including the
all-ones unmapped read default and ignored unmapped writes, were moved without
alteration. `cleanup_empty_port_space` remains in `devices.cc` exactly as
before.

## Scope and evidence

The new translation unit includes only the original `iodev.h` input and has
no adapter or OpenNT reference. No header, ABI, caller, configuration or
feature flag changed. In particular, this package does not alter full device
initialization, plugins, timers, firmware, lifecycle, or port-space cleanup.

`tests/bx-ntvdm-adapter/Test-BochsEmptyPortSpaceObjectBoundary.ps1` verifies
that every moved definition occurs exactly once in the isolated source and
zero times in `devices.cc`, that cleanup remains in `devices.cc`, and that the
isolated source has no adapter/OpenNT term. It is a static source check only.

## Non-claims and next gate

No build recipe was changed and no compiler, linker, executable, reset or
runtime action was invoked for this source separation. Therefore it is not
evidence that a future object model includes the new source file, dead-strips
`devices.cc`, or closes any unresolved symbol. The pre-existing compile-only
observation is evidence only for the earlier three-member source version; a
separate admission is required to compile or otherwise observe this five-member
version.
