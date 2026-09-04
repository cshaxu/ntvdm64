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

The current declaration first copies its selected DOS command into that
adapter's transport-neutral broker record and delivers it into the local
BaseVDM record only after the copy succeeds.  This is not an app-owned broker
protocol and does not claim named-pipe or cross-process operation.

`launch_declaration.{c,h}` is the corresponding thin assembly owner: it
creates no command protocol and no guest state. It initializes and binds the
adapter-owned record before original `scs_init` asks `GetNextVDMCommand(NULL)`
whether this is the first DOS VDM. A declared command payload remains owned by
the Base VDM adapter.

`--command <text>` declares one concrete product-entry target. Before a VDM
exists, app's BaseVDM-facing adapter classifies it without shell parsing:
native PE starts directly through public Windows process creation, DOS uses the
original Base VDM split (`AppName` image plus `CmdLine` argument tail), and NE
is held at the explicit WOW-bootstrap gate. The first resident `COMMAND.COM`
receives a DOS record through its original BOP and performs the original DOS
`EXEC`; app does not wrap it in `COMMAND.COM /C`.

## M0 T387 positional command declaration

The ordinary product spelling is either
`ntvdm32.exe <command> [argument ...]` or
`ntvdm64.exe <command> [argument ...]`, matching the published host
architecture. The command contract is otherwise identical.
The first non-SoftPC option and the remainder form one app-declared DOS command
that travels through the existing Base VDM record. Earlier original SoftPC
options remain untouched. A one-token command preserves that token verbatim;
multiple host argv tokens are reconstructed only with the necessary DOS
whitespace quoting. `--command <text>` remains an explicit equivalent form for
diagnostic use. App rejects a mixed explicit/positional declaration or an
embedded quote/newline rather than parsing DOS syntax.

Original `nt_reset.c::host_applInit` expects NT4's launcher-provided `-f`
marker and otherwise exits. App supplies it once in a private forwarded argv
vector before calling the original entry, which preserves the original parser
and lets ordinary direct CLI launch use the historical foreground path.

No declared command is represented as an app-specific command loop. A bare
launch supplies the selected `COMMAND.COM` as a bounded `/C` child of the
resident PermCom; an explicit positional target is copied into the original
`AppName` carrier and is never wrapped in `/C`. The local Base VDM exhaustion
boundary ends the session only after its copied record has been consumed and
PermCom makes its next original request.

Every DOS positional target, including `ntvdm32.exe command.com` or
`ntvdm64.exe command.com`, is therefore an
ordinary guest EXEC target. A second `COMMAND.COM` receives no app/session/BOP
specific handling:
it retains the same `DoReEnter -> Do16BitPrompt` route, banner, prompt and
DOS-CON behavior as any ordinary guest shell. App does not print a prompt,
read a Console line, write guest input or resubmit a record after child exit.

For a guest COMMAND `EXEC`, the guest DOS kernel remains the format owner.
Recognized DOS COM/MZ images run in the same VDM and return through the
original parent PSP; only an unknown format reaches `cmdCheckBinary`. `DOSONLY`
is an original guest policy and is not an app image-classification switch.

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
directory and selects it unchanged as that session's MVDM system root, with
`<exe-directory>\\softpc` as its firmware root. This matches the original
SystemRoot-relative `system32`, configuration and DOS media shape. The lookup
itself remains in the original SoftPC `host_find_file` call shape through the
named SoftPC adapter. DOS and Win16 remain separate source mirrors but share
this installed MVDM media root; none of their bytes is linked into the
executable.

## M0 T354 original COMMAND path admission

`package_layout` preserves the original `cmdconf.c` generated `shell=` form
and the NTDOS `sysconf.asm::commnd` 64-byte storage contract. It rejects a
package root unless its public Windows short spelling makes the generated
COMMAND value at most 63 visible bytes. `entry.c` then displays an app-owned
user-visible installation-path explanation before original host startup. It
neither creates a DOS-device alias nor changes guest/firmware bytes.

| Exception | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| `APP-DIV-014` | Original NT installations supplied a short `%SystemRoot%` to `cmdconf.c`; NTDOS stored the generated shell value in `commnd`. | Modern portable package roots can exceed the original 63-visible-byte shell-value capacity and otherwise overflow the unchanged guest contract. | App computes the unchanged original generated-value length, rejects an invalid package before startup, and shows an app-owned explanatory dialog. | `package_layout.c`, `package_layout.h`, `entry.c` |
| `APP-DIV-015` | Original COMMAND environment initialization enumerates its inherited host environment. | A host-only continuation-observer path must not enter the original guest environment/allocation input. | App captures the explicitly optional path before original startup, deletes that one inherited variable, and the adapter uses only its private bounded copy. | `entry.c`; `../adapter-mvdm-host-out/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| `APP-DIV-017` | NT4's ordinary bare VDM can wait for a CSRSS/BaseSrv command producer after startup and ordinarily selects configuration through the original BaseVDM PIF carrier. | The unpack-and-run one-session CLI has no CSRSS producer and must not silently fall back to the DOSX/WOW default when its declared product scope is pure DOS. | App keeps the first `COMMAND.COM` as PermCom. It supplies a declared target through the original `AppName`/`CmdLine` split, enables the existing exhaustion disposition only after that record is consumed, and supplies the same packaged `pure-dos.pif` path through `PifFile`. A bare launch uses only a bounded child `/C` tail; explicit targets, including `COMMAND.COM`, are never wrapped in `/C`. | `launch_declaration.c`; `../adapter-mvdm-host-out/basesrv/source/base_vdm_local.c` |

## M0 T346 S3 presentation window

`presentation_window.{c,h}` is an app-owned public Win32 presentation
component. It renders copied text or graphics snapshots and forwards ordinary
keyboard records through the original `CONIN$` input endpoint when its owning
display-arbitration path has prepared it. It is deliberately **not** opened by
normal product startup: original SoftPC acquires the process Console for the
default character route. T388 S5 opens it only after the original
`graphicsResize` boundary or a Console-consumed Alt+Enter request. Alt+Enter
from this window returns to Console; it does not select `X86GFX`, fabricate a
Console Server fullscreen mapping, or change original SoftPC fullscreen state.

The app never receives a source DIB pointer, `HPALETTE`, or source mutex.
The source-facing adapter resolves the original DIB mutex through the
session's existing host-resource mapping manager, waits and copies the
graphics/palette snapshot before app paints.  User close requests a typed
session cancellation; normal app teardown posts a distinct shutdown message
and waits for the UI thread before session disposal.

## M0 T388 S5 display arbitration divergence

| Exception | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| `APP-DIV-016` | NT4 Console Server selected graphics/fullscreen surfaces and consumed Alt+Enter before the original SoftPC keyboard worker. | Modern public Console has no private VDM graphics controller or hardware-fullscreen transfer. | App binds a passive sink before activation, opens only on the adapter's original-graphics or consumed-Alt+Enter event, and returns focus to Console when the presentation window handles Alt+Enter. It never parses DOS, emits BOP, or writes guest memory. | `entry.c`, `presentation_window.[ch]`; adapter peer `../adapter-mvdm-host-out/win32/source/console_compat.c` |
