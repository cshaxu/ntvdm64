# M0 T271 S3 P1 — COMMAND original entry and globals recovery

## Recovered source ownership

- `src/opennt-bop/command/nt_bop_command.c` is the admitted true subset of
  OpenNT `base/mvdm/softpc.new/host/src/nt_bop.c:MS_bop_4`. Its source order
  loads the service byte from staged CS:IP, calls `CmdDispatch`, then advances
  the staged IP.
- `src/opennt-bop/command/cmddata.c` is the admitted true subset of
  `base/mvdm/dos/command/cmddata.c`; it again owns the original COMMAND
  globals and initializers.
- `src/opennt-bop/command/cmd.c` is the admitted true subset of
  `base/mvdm/dos/command/cmd.c:CMDInit`.

## Minimal divergences

`BOP-DIV-099` supplies only call-scoped CCPU/SAS `sas_load` and staged-IP
facades. The outer CPU resume remains the existing fixed-width typed result.
The retail `cmddisp.c` table had no non-DBG range guard, so the facade rejects
an unreadable or out-of-range staged byte before that original table can be
indexed. `BOP-DIV-100` replaces only `cmddata.c`'s historical include closure;
the globals and initializers remain source-shaped. `BOP-DIV-101` maps
`CMDInit`'s original system-directory call through the same-shaped public
Win32 facade.

## Result and verification

`command_native_session` no longer directly invokes the project-owned
`runtime_command_misc_invoke` service entry. It stages the copied real-mode
CPU IP after `C4 C4 54`, invokes `MS_bop_4`, and lets that source body enter
the imported 17-slot `CmdDispatch` table. The copied event service remains
only the bounded callback/post-body descriptor.

On 2026-08-25, outside the sandbox, a refreshed formal Ninja graph at
`build/M0-T271-S3/r001` built the changed `opennt-bop` library and
`t231-s10-command-native-session-fixture.exe`; the fixture exited zero. It
creates a minimal machine stage, writes the actual `C4 C4 54 00` bytes to
checked RAM, and proves that the original `MS_bop_4` route reaches the
original `cmdExitVDM` terminal result. The low-level COMMAND direct-import
fixtures for services `00`, registration and console/key layout also exit
zero. The broader historical host-child/configuration fixtures remain under
their separate source/lifecycle closure and are not claimed by this P1.
