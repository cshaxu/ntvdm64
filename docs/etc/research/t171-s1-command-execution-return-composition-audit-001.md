# T171 S1 COMMAND execution/return composition audit 001

## Original component

`cmddisp.c` is a contiguous 00--16 dispatcher. Its startup side is not a set
of independent conveniences: `54:15` creates the environment, `54:05` gives
host state the `SCSINFO` location, `54:01` exchanges a `CMDINFO`/`VDMINFO`
launch envelope, and `54:02/04/0C/0D/0E/0F` provide the COMSPEC, CWD, boot
files, keyboard and environment facts consumed around that exchange.

The historical execution side is a different composition. `54:07`
`cmdCheckBinary` reads raw guest pointers and classifies host binaries.
`54:08`/`54:10` (`cmdExec`/`cmdExecComspec32`) obtain guest pointers and then
enter `cmdExec32`: it changes host std handles, creates a thread, calls
`CreateProcess`, waits for it, queries its exit code, uses VDM re-entry and
event-thread controls, and exchanges `GetNextVDMCommand` state. `54:06` passes
32-bit Win32 handles and pipe/temp-file threads. `54:11` (`cmdReturnExitCode`)
updates directories/redirection then re-enters the historical VDM command
broker. `54:00` terminates the VDM process. `54:09` and `54:14` require VDD/
console/keyboard facilities. These are inseparable historical host composition,
not candidate calls for the adapter.

## Complete first-profile disposition

| Dispatcher region | Original ownership | CLI first-profile disposition |
| --- | --- | --- |
| 00 | VDM termination | Exact bounded lifecycle stop only where separately source-admitted; no generic command exit provider. |
| 01,02,04,05,0C,0D,0E,0F,15,16 | COMMAND startup/launch envelope | Existing finite immutable profile/session providers. Keep their table order and copied ABI only. |
| 03 | compile-disabled save-world body | Original no-op disposition. |
| 06 | std-handle/redirection transport | Explicitly unavailable: host handle/pointer/thread ownership cannot cross adapter ABI. |
| 07 | non-DOS binary classifier | Explicitly unavailable for CLI DOS profile; no host executable inspection/rewriting. |
| 08,10 | host 32-bit/COMSPEC process execution | Explicitly unavailable. Rehosting would be invasive host process composition, not DOS guest execution. |
| 09,14 | VDD console/keyboard | Deferred to VDD/device owner; not adapter. |
| 11 | post-DOS child exit/re-entry | Deferred with a complete declared guest-target lifecycle; cannot call historical broker/redirection code. |

## Minimum contained execution contract

The next component is **declared DOS guest-target lifecycle**, not `cmdExec`.
It must use a BYOB profile-selected DOS target and preserve the existing
COMMAND envelope contract: one immutable target identity, finite command tail,
guest-side execution/return state, and a typed terminal outcome. The host does
not execute the target; Bochs executes its DOS bytes. A normal target return
must remain distinct from the exact QUIT `FE` terminal mechanism.

This component may source-derive the necessary `54:01`/`54:11` guest ABI only
after proving layouts, return sequencing and failure behavior. It must not
link `cmddisp.c`, retain guest pointers, expose host handles, invoke
`CreateProcess`, use VDM re-entry, or make `54:08/10` appear available.

## Result

The historical host execution composition is not portable into the noninvasive
CLI, but a bounded guest-native execution lifecycle remains compatible with
the project boundary. The next task should audit that lifecycle's COMMAND/DOS
caller, stack/register/guest-memory contract and normal-return continuation as
one component before code is changed.
