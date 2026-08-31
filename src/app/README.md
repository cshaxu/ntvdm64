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

`launch_declaration.{c,h}` is the corresponding thin assembly owner: it
creates no command protocol and no guest state. It initializes and binds the
adapter-owned record before original `scs_init` asks `GetNextVDMCommand(NULL)`
whether this is the first DOS VDM. A declared command payload remains owned by
the Base VDM adapter.

For the admitted one-child integration profile, `--ordinary-child` declares
the selected session's immutable `dos/COMMAND.COM /C VER` image through that
same copied record. App removes only this composition option before entering
the original SoftPC argument parser. It derives the command, application,
environment and current directory from the session-selected DOS media root;
it does not load, execute, emulate or return the child itself.

## M0 T310 S3 selected backend composition

`machine_shell` composes only a session which made the selected SoftPC choice
while inactive. CCPU40's `host_cpu_init` is an original empty compatibility
hook, so app does not mistake it for product initialization. `machine_shell`
enters the exact original `obj.vdm/ntvdm.c` body through the registered
same-shaped SoftPC binding; that body retains timer, environment, NLS,
configuration, DOS-emulation and CPU-start ordering. There is no alternate
machine fallback.

`entry.c` is the sole product process entry. It configures only the immutable
package roots, selects the already fixed SoftPC backend and invokes the shell.
It does not load guest bytes or reproduce any original `ntvdm.c` startup step.

## M0 T310 S8 installed package layout

`package_layout.{c,h}` is the app-owned installation contract for immutable
machine inputs. Before activating a session, app resolves the executable
directory and selects `<exe-directory>\\softpc\\roms` as that session's
firmware root. The lookup itself remains in the original SoftPC
`host_find_file` call shape through the named SoftPC adapter. `dos/` and
`win16/` are sibling guest-media roots, not host libraries and not firmware
linked into the executable.
