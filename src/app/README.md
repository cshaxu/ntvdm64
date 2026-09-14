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
exists, app's BaseVDM-facing adapter resolves one executable token in DOS
`.COM`, `.EXE`, `.BAT` order, first beside the product and then through the
ordinary current-directory/PATH search. A resolved DOS image uses the original
Base VDM split (`AppName` image plus `CmdLine` argument tail), NE is held at
the explicit WOW-bootstrap gate, and a resolved native image is handed to
public Windows process creation. An unresolved token is forwarded unchanged to
`COMSPEC /c`, preserving host built-ins and shell syntax without an app parser.
The first resident `COMMAND.COM`
receives a DOS record through its original BOP and performs the original DOS
`EXEC`; app does not wrap it in `COMMAND.COM /C`.

## M0 T387 positional command declaration

The ordinary product spelling is
`ntvdm32.exe <command> [argument ...]`.
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

Every DOS positional target, including `ntvdm32.exe command.com`, is therefore an
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
| `APP-DIV-017` | NT4's ordinary bare VDM can wait for a CSRSS/BaseSrv command producer after startup and ordinarily selects configuration through the original BaseVDM PIF carrier. | The unpack-and-run one-session CLI has no CSRSS producer and must not silently fall back to the DOSX/WOW default when its declared product scope is pure DOS. | App keeps the first `COMMAND.COM` as PermCom. It supplies a declared target through the original `AppName`/`CmdLine` split, enables the existing exhaustion disposition only after that record is consumed, and supplies the same packaged `pure-dos.pif` path through `PifFile`. Before copying a declared image it asks the adapter to resolve `.COM`/`.EXE`/`.BAT` beside the product then through current-directory/PATH; an unresolved token remains a public `COMSPEC /c` request. A bare launch uses only a bounded child `/C` tail; explicit targets, including `COMMAND.COM`, are never wrapped in `/C`. | `launch_declaration.c`; `../adapter-mvdm-host-out/basesrv/{source/base_vdm_local.c,source/mvdm_image_classification.c}` |

## T412 launcher composition

`run16_entry.c` is the in-progress public launcher, not the worker entry.
It calls the original `OpenNtBaseGetBinaryTypeW` from `opennt-host`; BaseSrv
consumes that classification and must not parse the executable again. Worker
startup will retain the historical ntvdm argument and command-consumer contract.
The three suffix declarations/initializers retain `basedll.h` and `baseinit.c`
values without importing the Base DLL initialization shell. Standard CRT tail
delivery and public CreateProcess provide the approved CLI composition boundary;
no replacement image parser or shell/redirection grammar is introduced.

The explicit x86 `run16.exe` build target currently proves only native child
launch, untouched command tail, inherited directory/environment/streams and
exit status. DOS/WOW dispatch deliberately fails while S3 IPC integration is
unfinished. This intermediate target is not publishable and does not replace
the deployed ntvdm32.exe. See the [entry evidence](../../docs/etc/evidence/m0-t412-s3-entry-integration.md).

The build-only run16 also has the reserved `--internal-console-probe` role
implemented in `console_probe.c`. It is not a user launch option or an
authentication credential. An initially detached helper accepts a versioned
fixed-width header and up to 4096 candidate PIDs through an inherited input
pipe and emits status/membership through its output pipe. It caches these
handles before Console attachment, rejects attached/non-pipe invocations,
handles partial transfers, and exits after one observation. Oversized/invalid
headers fail without allocating candidate storage. This finite private channel
is newly authored because the NT4 Console identity transport cannot be reused;
original BaseSrv policy is unchanged. Verify-BrokerConsoleMembership launches
the formal run16 with an explicit inherited-handle allowlist and verifies a
real query and invalid-version response. The product broker parent, timeout,
authenticated transaction and process-generation revalidation are still open;
the helper alone grants no reuse or record-selection authority.

`console_query.h/.c` provide the app-side helper parent, currently an explicit
formal object tested against run16, not a selected BaseSrv RPC operation.
It takes already-pinned authenticated process handles, queries their PIDs
without OpenProcess, creates a detached helper with a two-handle inheritance
allowlist, and sends the bounded request on an owned writer thread while
draining the reply. Deadline/cancellation aborts only the owned helper; its
exit unblocks the writer, which is joined before request memory is freed.
Malformed/failed replies leave caller output unchanged. Live handles are
checked again before publishing membership; the service caller must still
revalidate registration generations. The private pipe orchestration is a
new finite modern-Console dependency, not recovered BaseSrv policy. Tests
cover the actual parent-to-run16 query and a signaled cancellation with
unchanged output. Fixture-only stalled-helper injection also verifies timeout
return, unchanged output and restored parent handle count; malformed non-boolean
membership is rejected. Authenticated service integration remains open.
An impossible failure to terminate a live
owned helper is fail-fast rather than returning with a writer using freed
memory; this exceptional path is not injected or claimed tested.

The generator now provides `worker-shell.lib` containing only machine_shell
and package_layout. It excludes entry and launch_declaration, unlike the old
combined app archive. `worker-command-bindings.lib` separately retains the
three reached image/COMMAND guest-state/native-child support objects, excluding
base_vdm_client/local/broker. These are explicit composition targets, not an
ntvdm executable or runtime acceptance. COMMAND's retained image helper is not
a new public worker launcher; S4 still must route child launches through run16.
Both archives build and their actual librarian member lists were checked.

The generator now provides `worker-shell.lib` containing only machine_shell
and package_layout. It excludes entry and launch_declaration, unlike the old
combined app archive. `worker-command-bindings.lib` separately retains the
three reached image/COMMAND guest-state/native-child support objects, excluding
base_vdm_client/local/broker. These are explicit composition targets, not an
ntvdm executable or runtime acceptance. COMMAND's retained image helper is not
a new public worker launcher; S4 still must route child launches through run16.
Both archives build and their actual librarian member lists were checked.

`worker_entry.c` is the pure-worker main. Its session/media/shell
assembly is extracted from the existing entry, using the same original-entry
execution wrapper and session disposal. It passes argc/argv unchanged and
inherits the launcher's environment. It contains no launch declaration, image
classification, native child launch or local command publication. The x86
`ntvdm.exe` target links it with the authenticated project BaseClient
transport. Its map selects `OpenNtBaseClientCallServer`, not host `ntdll` CSR,
and the first-VDM product RPC test passes. This is not command delivery or a
guest runtime test: only `BasepIsFirstVDM` has an admitted copied endpoint;
other BaseClient operations fail explicitly until their transport bindings are
implemented. No worker registration is claimed implemented by this file.

`worker_entry.c` is the pure-worker main. Its session/media/shell
assembly is extracted from the existing entry, using the same original-entry
execution wrapper and session disposal. It passes argc/argv unchanged and
inherits the launcher's environment. It contains no launch declaration, image
classification, native child launch or local command publication. The x86
`ntvdm.exe` target links it with the authenticated project BaseClient
transport. Its map selects `OpenNtBaseClientCallServer`, not host `ntdll` CSR,
and the first-VDM product RPC test passes. This is not command delivery or a
guest runtime test: only `BasepIsFirstVDM` has an admitted copied endpoint;
other BaseClient operations fail explicitly until their transport bindings are
implemented. No worker registration is claimed implemented by this file.

## T412 standalone service composition

`basesrv_entry.c` is the formal build-only broker entry. It selects original
BaseSrv through adapter-opennt-host's connection binding and native local RPC
generated from broker/service.idl. Endpoint scope is the captured logon LUID
and session; WINNT packet privacy and attached live caller PID are checked
before registration/query/disconnect. Duplicate endpoint ownership fails;
successful authenticated RPC, not a printed line, establishes readiness.
No fixture source enters this link. Current operations include connection
registration, original first-VDM query, typed file/pipe stream attachment,
receipt revocation and disconnect. DOS commands,
worker registration, resource callbacks and empty/idle shutdown are unfinished;
this executable must not be published or substituted for the three-program
delivery. Tests also observe RPC rundown after a registered client exits
without Disconnect and verify subsequent service responsiveness. This covers
registration loss only, not task/resource rollback. Tests terminate only their
own server while shutdown is unfinished.

## M0 T388 S5 display arbitration divergence

| Exception | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
