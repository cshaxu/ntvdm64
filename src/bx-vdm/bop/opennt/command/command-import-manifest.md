# OpenNT COMMAND source-mirror manifest

This is the T231 S1 import manifest.  It records exact in-tree OpenNT source
inputs and the destination under this directory.  A destination is not a
product input until its neutral shim is present and the formal Ninja manifest
admits it.

| OpenNT source | Mirror destination | Role | S admission |
| --- | --- | --- | --- |
| `base/mvdm/dos/command/cmddisp.c` | `cmddisp.c` | 17-slot original dispatcher | S10 dispatcher admission |
| `base/mvdm/dos/command/cmd.h` | `cmd.h` | common types, layouts, prototypes | S2 foundation |
| `base/mvdm/dos/command/cmd.c` | `cmd.c` | command source/bootstrap | S7 |
| `base/mvdm/dos/command/cmdconf.c` | `cmdconf.c` | CONFIG.SYS/AUTOEXEC paths | S2 |
| `base/mvdm/dos/command/cmddata.c` | `cmddata.c` | shared COMMAND data | S3/S8 |
| `base/mvdm/dos/command/cmdenv.c` | `cmdenv.c` | COMSPEC/environment/current directory | S2/S5 |
| `base/mvdm/dos/command/cmdexec.c` | `cmdexec.c` | binary admission and child lifecycle | S6/S8 |
| `base/mvdm/dos/command/cmdexit.c` | `cmdexit.c` | VDM exit | S9 |
| `base/mvdm/dos/command/cmdkeyb.c` | `cmdkeyb.c` | console/keyboard layout | S4 |
| `base/mvdm/dos/command/cmdkeyb.h` | `cmdkeyb.h` | keyboard layouts | S4 |
| `base/mvdm/dos/command/cmdmisc.c` | `cmdmisc.c` | CMDINFO, context and handles | S3/S4 |
| `base/mvdm/dos/command/cmdpif.c` | `cmdpif.c` | startup/PIF information | S5 |
| `base/mvdm/dos/command/cmdpif.h` | `cmdpif.h` | startup/PIF layouts | S5 |
| `base/mvdm/dos/command/cmdredir.c` | `cmdredir.c` | standard-handle redirection completion | S4/S8 |
| `base/mvdm/inc/cmdsvc.h` | `cmdsvc.h` | BOP service constants and fixed guest layouts | S2 foundation |

`tools/import/Sync-OpenNtCommandMirror.ps1` is the admitted exact-copy helper:
it replaces each mirror input and refuses a source/destination SHA-256
mismatch.  Its successful T231 S1 run is the import identity witness.  Once a
translation unit is admitted, the retained OpenNT body remains the baseline
but its documented shim/include divergence is applied after that exact copy.

## Admitted translation-unit deltas

| Mirror | Admission | Delta from exact copy | Reason |
| --- | --- | --- | --- |
| `cmdmisc.c` | T231 S2/S3 (`54:02`--`54:05`) | replaces only the unavailable historical include closure; selectively admits original bodies with translation-unit guards; adds one bounded `size_t`→`USHORT` cast | preserves `cmdComSpec`, `cmdGetCurrentDir`, `cmdSaveWorld` and `cmdSetInfo` while preventing legacy CCPU/PIF/console composition from becoming accidental product input. |
| `cmdkeyb.c` | T231 S4 partial (`54:0E`) | replaces only the old include closure and maps the unavailable console-layout export to a public Win32 capability | preserves the original keyboard registry/file checks and `NoInstallkb16` failure path. |

The source files remain governed by the source-policy recovery ladder.  Their
historical NT4 CCPU/SAS, scheduler and process-host dependencies must be
replaced only by documented `bop/shim/command_*` seams; they may not be
rewritten into a new adapter dispatcher.
