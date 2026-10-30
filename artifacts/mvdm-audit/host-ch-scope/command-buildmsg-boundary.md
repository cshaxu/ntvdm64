# COMMAND message-builder source boundary

The provisional `role-pending` entries `dos/v86/cmd/command/buildmsg.c` and
`dos/v86/cmd/command/usa-ms/bldmsg.h` form one original message-generation
tool, rather than two independent NTVDM host runtime owners. `buildmsg.c`
includes `bldmsg.h`; `makefil0` compiles it with `cl16` and links it with
`forcedos`, while the COMMAND makefile invokes `buildmsg.exe` to generate
`command.ctl` from a country message and `command.skl` for the guest build.
The header supplies the generator's message strings.

Both current files have the exact hashes recorded for the selected identical
OpenNT and OpenNT 4.5 originals. In the current selected x86 worker and WOW32
Ninja graphs, the C file is not a direct build input and the header does not
appear among recorded compiler dependencies. [Input hashes and graph checks](command-buildmsg-boundary.json)
bind this observation. The narrow host-runtime disposition is therefore
**excluded as a host implementation** for both: they are original guest-build
tool inputs, not disconnected runtime implementations to port. This does not
exclude COMMAND's guest behavior or message handling; S12 retains that runtime
and immutable-guest evidence obligation. Other configurations and actual
generated guest contents are not established by these selected-graph checks.
No source or guest image was changed and there is no mirror difference in
either file. Preserve the existing frozen ledger until all of its related
scope/status and source-provenance records can be reconciled coherently.
