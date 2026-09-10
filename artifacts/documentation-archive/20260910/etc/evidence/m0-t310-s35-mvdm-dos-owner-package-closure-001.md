# M0 T310 S35 P1 — MVDM DOS owner-package closure

## Original source selection

The selected x86 CCPU40 graph directly compiles and archives the original
`sources`-selected MVDM DOS providers:

- `dos/dem`: all sixteen original translation units, from `dem.c` through
  `demlabel.c`, in `original-mvdm-dem.lib`;
- `dos/command`: all eleven original translation units, from `cmd.c` through
  `cmdenv.c`, in `original-mvdm-command.lib`.

The original `demdisp.c` `apfnSVC` and `demioctl.c` `apfnSVCIoctl` tables, and
the original `cmddisp.c` `apfnSVCCmd` table, remain the sole selected service
dispatchers. The existing S31 BOP ingress subset reaches the original
`DemDispatch`/`CmdDispatch` names without adding a second dispatcher.

## Boundary audit

Every selected source body is either direct or has an already registered,
source-local minimal boundary:

- DEM's retained DTA, extended-error, current-PDB and SFT locations are
  numeric guest descriptors with fresh bounded leases; Find/FCB and DOS file
  native handles use the session's sole host-resource mapping-manager
  instance. These are the existing `MVDM-HOST-DIV-002` through `-007`,
  `-103` through `-107` boundaries. They preserve original table order,
  register carriers and failure directions instead of treating host pointers
  as guest values.
- COMMAND's SCS, stream/redirection records and pipe/child workers retain the
  original source bodies. The existing `-009`, `-020`, `-091`, `-108` through
  `-120` boundaries constrain only guest locations, mapped host identities,
  x86 thread callback ABI and checked host-length crossings. They do not
  replace COMMAND's command/environment/child lifecycle algorithms.
- The historical `GetNextVDMCommand` client name is bound through the existing
  BaseSrv-shaped session provider. It is a single-session source-compatible
  binding, not a claim to restore CSRSS or a multi-process broker. WOW's
  multi-caller expansion remains its named S39/S49 integration obligation.
- `cmdkeyb.c`'s one reachable private Console Server query is now supplied as
  `GetConsoleKeyboardLayoutNameA(LPSTR) -> BOOL` by the existing Win32 adapter,
  which forwards only to public `GetKeyboardLayoutNameA`. The declaration
  carrier removes the obsolete DLL-import attribute for this adapter-provided
  symbol while leaving the remaining original `winconp.h` declarations intact.

No selected DEM/COMMAND source invokes a Bochs, MONITOR product, kernel-VDM,
`src.old`, virtual-disk, or generic DOS/COMMAND replacement route.

## Formal and focused evidence

The fresh `build/M0-T310/S35/formal-dos-x86-r1` formal graph completed all
**381/381** selected Win32/x86 CCPU40 Ninja edges. Its forced-link audit has
no unresolved selected DEM/COMMAND archive edge except these deliberate later
owner transfers:

- `demsrch.c:FileFindReset -> NtVdmControl`: S36 SIM32/monitor owner;
- `demmisc.c:SignalSegmentNotice -> ModuleLoad/ModuleFree/ModuleSegmentMove`
  and `demInputString -> DbgPrompt`: S42 debugger owner.

The prior `cmdkeyb.c -> GetConsoleKeyboardLayoutNameA` edge now resolves to
`softpc-win32-bindings.lib(console_compat.obj)` without an obsolete DLL-import
link warning.

Two existing source-shaped x86 checks were regenerated with current include
roots and pass:

- `command_initialization_fixture.exe`: original `CMDInit` plus `cmddata`
  initialization succeeds;
- `nt-bop-dem-command-subset-fixture.exe`: source-shaped DEM/COMMAND BOP
  ingress dispatches first, advances IP second, and preserves the original
  idle ordering.

These checks prove the package's selected entry and dispatch contracts. Full
guest BOP execution, multi-caller BaseSrv/WOW routing, debugger/module events,
and full machine lifecycle remain mandatory later-owner/S49 integration work;
they are not represented as completed DOS behavior here.
