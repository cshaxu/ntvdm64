# app

`ntvdm64` product entry: CLI, guest-image selection/loading, session startup
and final composition of the selected guest, BOP, host, adapter and Bochs
components.

It may compose `opennt-guest`, `opennt-bop`, `opennt-host`, `adapter-win32` and
`adapter-softpc`, but does not implement their semantics. Its final executable
target is `ntdos64-native`; no component may import `app`.
