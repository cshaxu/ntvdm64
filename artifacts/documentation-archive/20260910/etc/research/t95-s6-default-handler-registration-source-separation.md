# T95 S6 default-handler registration source-object separation

## Question

Can the two Bochs-local registration members imported by the isolated empty
port initializer be detached from the full `devices.cc` product object without
changing I/O behavior or moving machine semantics into the adapter?

## Inputs and procedure

The prior compile-only evidence
`t95-s6-minimal-port-compile-observation-001.md` records exactly five
undefined symbols for `minimal_port_space.obj`. Three are normal MSVC CRT
allocation/security support. The remaining two are
`bx_devices_c::register_default_io_read_handler` and
`bx_devices_c::register_default_io_write_handler`, whose only pre-change
definitions were at `refs/bochs/iodev/devices.cc:717-743`.

Source review shows both functions only populate the existing
`io_read_handlers` or `io_write_handlers` record: function pointer, object
pointer, allocated handler name and mask. They contain no device construction,
plugin dispatch, timer, CMOS, firmware, configuration, adapter or OpenNT term.
`BX-IO-030` was recorded in the external-intrusion register and the active S6
packet before the source edit.

The two complete definitions were then moved unchanged into the already
Bochs-internal `refs/bochs/iodev/minimal_port_space.cc`; they were removed from
`refs/bochs/iodev/devices.cc`. The focused source-boundary test was expanded
from three to five required definitions.

## Observations

- `Test-BochsEmptyPortSpaceObjectBoundary.ps1` passes and proves each of the
  five isolated member definitions occurs exactly once in
  `minimal_port_space.cc` and zero times in `devices.cc`.
- The same test proves `cleanup_empty_port_space` remains exactly once in
  `devices.cc`, and rejects adapter/OpenNT terms in the isolated source.
- The FMT lexical boundary test, default-off #UD boundary test, retained #UD
  seam test and documentation-governance verifier pass.
- No compiler, linker, executable, configuration/project command, or runtime
  action was performed after this source separation.

## Interpretation

The empty-port source object no longer has an ordinary Bochs undefined edge to
the rejected full-device object. Its next eventual object observation should
show only standard MSVC CRT allocation/security references, if the accepted
CPU3 projection and compiler invocation are otherwise unchanged. That is a
prediction, not a result: a new fresh-root compile admission is required.

This does not prove COMDAT removal, a linker closure, reset, device absence,
or a runnable Bochs machine. The remaining `cleanup_empty_port_space` in
`devices.cc` is deliberately not moved: it is outside the initializer's
single-object import set and requires its own source/retention evidence before
any change.

## Follow-up

Before any link attempt, admit at most one fresh CPU3 compile-only observation
of the five-member `minimal_port_space.cc`, pinning the source and projected
configuration hashes and capturing only its object hash and undefined-symbol
dump. Do not rebuild `devices.cc`, link, execute, or add a device/provider.
