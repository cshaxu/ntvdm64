# app

`ntvdm64` product entry: CLI, guest-image selection/loading, session startup
and final composition of the selected guest, BOP, host, adapter and Bochs
components.

The former CLI C sources are this component's implementation.  They own
argument parsing, validated configuration/profile admission, image identity,
child-engine launch environment, cancellation, exit status and diagnostics.
They must not implement guest DOS/BOP semantics or manipulate Bochs CPU/RAM
objects directly.

It may compose `opennt-guest`, `opennt-bop`, `opennt-host`, `adapter-bop`,
`adapter-win32` and `adapter-softpc`, but does not implement their semantics. Its final executable
target is `ntvdm64`; no component may import `app`.
