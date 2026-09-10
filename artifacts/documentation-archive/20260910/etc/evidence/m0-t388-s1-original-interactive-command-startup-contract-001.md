# M0 T388 S1 — Original interactive COMMAND startup-contract audit

## Question

What exact original record distinguishes a first interactive `COMMAND.COM`
session from the already working `/C <declared command>` route, and what is
the smallest source-shaped S2 seam?

## Inputs

- Original OpenNT Base client: `base/win32/client/vdm.c`, especially
  `BaseCheckVDM` and `GetNextVDMCommand`.
- Selected mirrors: `src/mvdm-host/dos/command/cmdmisc.c`,
  `src/opennt-host/base/win32/server/srvvdm.c`, and
  `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c`.
- Current composition: `src/app/launch_declaration.c` and its T387 evidence.

## Original contract

1. `BaseCheckVDM` is the historical CreateProcess-to-BaseSrv producer.  It
   separates the executable's OEM short `AppName` from its command-tail,
   copies **only that tail** to `CmdLine`, and always appends `CR`, `LF` and
   NUL (`vdm.c`, approximately lines 1460--1590 and 1940--1960).
2. `cmdGetNextCmd` is the first consumer.  On `IsFirstCall`, it asks for
   `ASKING_FOR_FIRST_COMMAND`, has a completed guest initial environment, and
   explicitly does not ask Base for an environment (`cmdmisc.c:43--139`).
   It later builds COMMAND's UCOMBUF from the returned `AppName` basename plus
   the returned `CmdLine` tail (`cmdmisc.c:404--452`).
3. Therefore an interactive first record is not a missing command.  It is
   `AppName = <selected COMMAND.COM path>` and `CmdLine = "\r\n\0"`.
   The existing positional route is the same record family with the tail
   `"/C <declared command>\r\n\0"`.  `/C` is the only product-selected
   difference; COMMAND itself retains the interpretation.
4. The initial DOS record uses the ordinary DOS queue.  `srvvdm.c` chooses
   that record for `ASKING_FOR_FIRST_COMMAND`, preserves the returned path,
   environment, current directory, drive, code page and start-info fields,
   then transfers it to the BaseClient caller.  The original CSRSS capture
   transport and cross-process wait-handle duplication are not composable;
   the selected local adapter already preserves the relevant DOS/WOW choice,
   capacity checks and copied-record lifetime.
5. Console identity is distinct from the command tail.  Original
   `BaseCheckVDM` computes `bNewConsole` from an absent process Console or
   `CREATE_NEW_CONSOLE`, and passes a Console handle separately to BaseSrv
   (`vdm.c:1371--1439, 2075--2080`).  `cmdGetStartInfo` exposes the resulting
   DOS-session/new-console state as AL (`cmdmisc.c:1103--1116`).  No T388 S1
   change may guess a display mode or create a second text surface.
6. On a later no-record DOS request, BaseSrv normally waits; its special
   `RETURN_ON_NO_COMMAND`/second-call outcome belongs to child/re-entry
   lifecycle, not keyboard input (`srvvdm.c:230--276`).  The current local
   adapter has this distinction.  Its terminal-on-exhaustion switch is only
   the T387 declared-command disposition and must remain off for interactive
   launch.

## Current comparison and S2 seam

- `app_launch_declaration_publish` presently requires
  `command_declared != 0` and writes `/C ` before the declared tail.  It is
  why direct no-argument launch currently stops in app.
- `base_vdm_local_publish` already requires a non-empty copied command
  buffer.  The original interactive tail meets that invariant: its three
  bytes are `CR/LF/NUL`; it is not a zero-length sentinel.
- `base_vdm_local_dispatch` already makes a consumed DOS record BUSY and,
  with `terminal_on_command_exhaustion == 0`, returns the source-shaped
  pending wait for a subsequent DOS record.  S2 must retain that behavior.
- The smallest implementation is therefore app composition only: add an
  explicit initial-interactive declaration kind that publishes the same
  source-shaped DOS record with an empty CR/LF-terminated tail, preserves the
  existing BaseCreateVDMEnvironment/current-directory inputs, and does **not**
  set terminal-on-exhaustion.  It must not introduce a command parser,
  prompt-text producer, guest-memory write, or new BaseSrv protocol.

## Recovery ladder

1. **Original source reuse:** `BaseCheckVDM` producer shape,
   `GetNextVDMCommand` consumer layout and `cmdGetNextCmd` UCOMBUF algorithm
   define the selected behavior and remain the primary implementation.
2. **Smallest same-shaped seam:** existing `base_vdm_broker` plus
   `base_vdm_local` replace only unavailable CSR capture/transport with copied
   one-session state; this is already registered by `ADAPTER-BASESRV-001/002`.
3. **External intrusion:** none.
4. **New behavior:** no new DOS behavior.  The app needs only an initial
   record selection bit, because a direct executable launch has no historical
   external CreateProcess caller to invoke `BaseCheckVDM` for it.

## Disposition

S1 is closed: S2 may implement exactly the identified initial-record binding.
Console creation/diagnostic isolation, keyboard/IRQ input, display routing,
PIF and runtime interaction remain later S owners.
