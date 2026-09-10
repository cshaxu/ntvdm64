# OpenNT VDM Command Broker Contract

Status: source-derived research record, 2026-08-07. This document defines no
runtime admission. It records the historical command-broker contract needed
before a modern compatibility shim can be designed without replacing DOS or
PC/AT behavior.

## Evidence and Source Scope

The fixed OpenNT input revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`
now contains these reference-only sources under their original paths:

| Source | SHA-256 | Role |
| --- | --- | --- |
| `src/opennt/base/win32/client/vdm.c` | `3F03D0DBB08E0163F2D9CF415DAAD0981E42E1B1855F6F48A3B59022B7374173` | Win32 client-side `GetNextVDMCommand` and related VDM APIs. |
| `src/opennt/base/win32/server/srvvdm.c` | `C1E2177C6C00679D85CFA475F620841F6736B0E56D8DBF790B71AFE33E1ED80B` | BaseSrv-side DOS/WOW task, console, event, and command-queue ownership. |
| `src/opennt/base/win32/server/srvvdm.h` | `F590EF866F87CE80671C4B6F4E3BBE94347342E6B723BC062ED6F044FBB3820F` | Server task and console-record declarations. |
| `src/opennt/public/internal/base/inc/vdmapi.h` | `B942656BA8761DDB9325A7EEA7997394562D2E57797C923FF5688D7BE86426DE` | Private `VDMINFO` layout and state bits. |

`refs/opennt-45/windows/inc/pmvdm.h` is the cross-version provenance source for
a narrow compatibility-header admission at
`src/opennt/public/internal/windows/inc/pmvdm.h`: fixed OpenNT and OpenNT 4.5
`config.c` are byte-identical, and its three consumed property values persist
in the local XP source. It identifies the Program Manager `Reserved` string form
`dde.%d,hotkey.%d,ntvdm.%d,` and property bits `0x01`, `0x02`, and `0x04`.
This is an explicitly recorded header-level admission, not a recovered NT4
file or a general source-profile merge.

None of these sources is a current CMake target. They are excluded from the
default runner and do not authorize a historical binary build or runtime.

## Historical Contract

`GetNextVDMCommand(PVDMINFO)` is a client/server command broker, not a local
argument parser. The client sends a `BASE_GET_NEXT_VDM_COMMAND` request through
`CsrClientCallServer(BasepGetNextVDMCommand)`. It captures all variable-size
buffers and supplies the request state, console identity, current task, exit
code, and allocated capacities.

The historical server owns the corresponding console record, DOS record, WOW
record, task queue, reentrancy count, parent wake event, and wait event. It may
return a wait object; the client waits, sets `ASKING_FOR_SECOND_TIME`, and
reissues the request. It may also return `STATUS_INVALID_PARAMETER` solely to
report the required field sizes. This makes size negotiation a protocol result,
not an arbitrary error convention.

The request/response consists of `VDMINFO` fields with these ownership rules:

| Field family | Historical meaning |
| --- | --- |
| `CmdLine`, `AppName`, `PifFile` | Command identity and PIF lookup inputs/outputs. |
| `Enviornment`, `CurDirectory` | Environment and DOS current-directory state. |
| `StartupInfo`, `Desktop`, `Title`, `Reserved` | Win32 startup policy, including Program Manager PIF-property suppression. |
| `StdIn`, `StdOut`, `StdErr`, `CodePage`, `dwCreationFlags` | Host process/console setup returned to the VDM. |
| `iTask`, `ErrorCode`, `fComingFromBat`, `CurDrive` | DOS/WOW scheduler and batch-return state. |
| `CmdSize`, `AppLen`, `PifLen`, `EnviornmentSize`, `CurDirectoryLen`, `DesktopLen`, `TitleLen`, `ReservedLen` | Caller capacities before the request; required/actual byte counts after it. |

Relevant request bits include `ASKING_FOR_FIRST_COMMAND`,
`ASKING_FOR_DOS_BINARY`, `ASKING_FOR_WOW_BINARY`, `ASKING_FOR_PIF`,
`ASKING_FOR_ENVIRONMENT`, `RETURN_ON_NO_COMMAND`, and the reentry bits. A
single DOS launch cannot be represented honestly by merely returning a command
line while ignoring the remaining fields and states.

## Modern Direct-CLI Research Profile

A future modern-host shim may offer a deliberately smaller **direct-CLI
profile**. It is a new compatibility boundary, not a claim that modern Windows
implements BaseSrv or that the historical broker has been restored.

Supported profile, only after an explicit implementation task:

- One `ntvdm.exe` process receives one caller-provided DOS command request.
- The command line, application path, inherited environment, current
  directory, standard handles, and code page are supplied by the modern host.
- The implementation preserves `VDMINFO` capacity negotiation: before guest
  startup, insufficient caller buffers return the historical size-reporting
  failure shape with no partial field copies.
- It returns no wait object and never blocks for a later queued command.
- `Reserved` is empty. No PIF, Program Manager, desktop, title, shell DDE, or
  task-hotkey policy is synthesized.

The profile must reject before guest startup: WOW or separate-WOW requests,
reentry increments/decrements, second-call/wait-object behavior, parent wake
events, command queues, bat-notification scheduling, nonempty PIF data, and
any request requiring an unrepresented startup/console policy. A rejection is
evidence of an unsupported host feature, not a reason to add a DOS SVC, BIOS
handler, or CCPU exception.

The eventual shim may be exposed through a narrow, fixed-width `extern "C"`
adapter ABI. It must not pass C++ objects, CRT allocation ownership, exceptions,
or a fabricated PEB across the historical/modern boundary. The original
historical client/server sources remain the behavioral reference; a new shim
implementation must name each replaced host dependency and its supported
profile rule.

## Consequences for the Historical Runtime Path

This broker sits outside CCPU, the BIOS BOP table, `MS_bop_0`, `DemDispatch`,
and DEM. Restoring it does not advance guest instructions or define DOS SVC
semantics. Conversely, a CCPU link block does not justify collapsing the
broker into a self-written bootstrap.

The required normal-path ordering remains:

```text
CCPU / SoftPC monitor -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
                                      ^
                 direct-CLI broker supplies host command/process data only
```

## Future Verification Gate

Any direct-CLI implementation must remain default-off until it records all of
the following:

1. A source manifest/diff showing no historical source-profile mixing.
2. A bounded test proving size negotiation is atomic: failure changes only
   returned lengths, never partial strings or startup state.
3. A supported direct DOS request fixture with empty PIF/Reserved policy.
4. Unsupported WOW, reentry, queue/wait, and nonempty-PIF fixtures that fail
   before guest startup.
5. Link evidence that neither the default runner nor a trace instrument target
   receives the shim or these historical broker sources implicitly.
