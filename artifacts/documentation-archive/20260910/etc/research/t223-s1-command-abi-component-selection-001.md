# T223 S1 COMMAND ABI and First-Component Selection

## Source-derived ABI facts

| Services | Original ABI/failure facts | Historical host binding |
| --- | --- | --- |
| `01 cmdGetNextCmd` | COMMAND receives a `CMDINFO` record; no-command is a lifecycle decision, not an arbitrary next command. | `GetNextVDMCommand`, redirector completion and VDM session state. |
| `02 cmdComSpec` | reads OEM COMSPEC text at `DS:DX`; first call records `COMSPEC=` and sets `AL` from console/WOW state. | process/session COMSPEC and console state. |
| `04 cmdGetCurrentDir` | `AL` drive, `DS:SI` output; CF failure with `AX=0` oversized / `AX=1` invalid drive. | drive type and `=X:` environment current-directory state. |
| `05 cmdSetInfo` | `DS:DX` SCSINFO plus DS-relative `BX/CX` locators; no result. | raw CCPU/SAS guest pointers retained by the old host. |
| `0C/0D` | `DS:DX` output with at least 64 bytes; old failure terminates VDM. | expanded temporary CONFIG.SYS/AUTOEXEC.BAT files. |
| `0F cmdGetInitEnvironment` | `ES:0` destination, `BX` paragraph capacity; returns zero, consumed size, or required size in BX. | inherited NT environment, COMSPEC conversion and per-drive environment variables. |
| `00/0B` | VDM exit / DOS child return lifecycle. | `TerminateVDM` and `GetNextVDMCommand`. |
| `06/07/08/0A` | standard-handle bridge and non-DOS/COMSPEC host execution. | NT handles, pipe completion, worker thread and event-thread suspension. |
| `09/0E/10` | console/VDD initialization, keyboard registry/KEYB generation, session-start information. | VDD, console events, registry and `DosSessionId`. |
| `03` | compiled as a no-op unless `CHECK_IT_LATER` is enabled. | disabled save-world file path. |

## First implementation component

T223’s first later implementation S is **COMMAND bootstrap, environment and
configuration**: `01/02/04/05/0C/0D/0F`.  It is the smallest original-owner
package that can establish a coherent COMMAND session without inventing a
stream/pipe, host-child process, VDD or interactive terminal.  Existing bx-vdm
helpers are candidates only after ABI reconciliation:

- preserve `CMDINFO` and `SCSINFO` copied layouts and lifecycle;
- use checked guest gather/write actions instead of CCPU/SAS pointers;
- project CLI host environment and current-directory state through the shared
  host capability/session context, never ambient fallback;
- retain CONFIG/AUTOEXEC as declared guest input, not temporary host files;
- make insufficient environment-buffer behavior observable through `BX`.

`06/07/08/0A` remains one later launch/stream component; `09/0E` one console
component; `00/0B` lifecycle; and `10` a separately deferred session-broker
component.  No trace hit selects among them.
