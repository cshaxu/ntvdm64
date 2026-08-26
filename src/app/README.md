# app

`ntvdm64` product entry: CLI, guest-image selection/loading, session startup
and final composition of the selected guest, BOP, host, adapter and Bochs
components.

The former CLI C sources are this component's implementation.  They own
argument parsing, validated configuration/profile admission, image identity,
child-engine launch environment, cancellation, exit status and diagnostics.
They must not implement guest DOS/BOP semantics or manipulate Bochs CPU/RAM
objects directly.

`observation/` is default-off CLI diagnostics only. `bop_composition` wraps
the BOP route after it returns in order to record an already-produced typed
outcome; it neither recognizes a selector nor changes guest state. Selector
and service interpretation remains in `opennt-bop`.

`startup_composition` owns only app-selected guest images, launch declaration
and machine-stage assembly. It supplies copied launch values to the temporary
OpenNT provider session through declared calls; it does not interpret BOP
selectors or services.

`command_session_binding` is the app-only startup binding for the recovered
COMMAND provider.  It copies the admitted launch declaration and PIF-derived
CONFIG/AUTOEXEC inputs before binding the provider session; OpenNT source
dispatch and guest service semantics remain in `opennt-bop`.

It may compose `opennt-guest`, `opennt-bop`, `opennt-host`, `adapter-bop`,
`adapter-win32` and `adapter-softpc`, but does not implement their semantics. Its final executable
target is `ntvdm64`; no component may import `app`.
