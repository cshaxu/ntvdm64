# app

CLI and final composition component. Existing app code is recovery evidence
until its owner/dependency review; provider algorithms do not belong here.

## M0 T273 S3 minimal machine shell

`machine_shell.{c,h}` is the admitted minimum app assembly: a caller-owned
`session` owns teardown, while the shell invokes the selected original
SoftPC CCPU40 lifecycle names. It does not load a guest, select a BOP,
provide an OpenNT service, expose CPU state, or create a worker/scheduler.

## T302 Base VDM producer boundary

The app no longer owns a Base VDM command protocol. It may declare launch
input, but the copied command record, `VDMINFO` shape, dispatch ordering and
session binding belong to `adapter-mvdm-host-out/basesrv`.

## M0 T310 S3 selected backend composition

`machine_shell` composes only a session which made the selected SoftPC choice
while inactive. The app invokes `host_cpu_init`/`host_cpu_reset`; original
SoftPC configuration and execution retain ownership of memory setup and CPU
execution. There is no alternate machine fallback.

## M0 T310 S8 installed package layout

`package_layout.{c,h}` is the app-owned installation contract for immutable
machine inputs. Before activating a session, app resolves the executable
directory and selects `<exe-directory>\\softpc\\roms` as that session's
firmware root. The lookup itself remains in the original SoftPC
`host_find_file` call shape through the named SoftPC adapter. `dos/` and
`win16/` are sibling guest-media roots, not host libraries and not firmware
linked into the executable.
