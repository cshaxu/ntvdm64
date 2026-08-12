# M67 COMMAND Bootstrap, Environment, and Stream ABI Ledger

Status: source-derived COMMAND bootstrap record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/command/cmdmisc.c`
- `src/opennt/base/mvdm/dos/command/cmdenv.c`
- `src/opennt/base/mvdm/dos/command/cmdredir.c`
- `src/opennt/base/mvdm/dos/command/cmddisp.c`

## Command and Directory Services

`cmdGetNextCmd` consumes a guest `CMDINFO` structure at `DS:DX`, including
return code, command/environment buffers, executable path, current drive, and
redirector state. It is the historical BaseSrv-facing broker: it calls
`GetNextVDMCommand`, may alter host current directory and `=X:` environment
state, uses PIF/configuration/event-thread policy, and fills guest command
buffers. It is an oracle for required data categories, not reusable runner
code.

| SVC | Handler | Guest ABI / historical behavior |
| --- | --- | --- |
| `02h` | `cmdComSpec` | On first call only, reads a string at DS:DX, creates `COMSPEC=<value>` in host state, and returns a console/WOW flag in AL. |
| `04h` | `cmdGetCurrentDir` | DS:SI destination, AL drive. Reads/writes the host `=X:` environment variable; returns CF plus AX=0 for nonexistent/oversize state. |
| `05h` | `cmdSetInfo` | DS:DX points to SCS info; DS:BX and DS:CX locate additional DOS-data globals. The host caches raw guest pointers for later command dispatch. |
| `09h` | `cmdInitConsole` | Starts the historical console event thread once. No guest input/output ABI. |
| `10h` | `cmdGetStartInfo` | Returns AL based on historical `DosSessionId` console-session policy. |

The `cmdSetInfo` pointer caching is particularly incompatible with a bounded
software backend: future code must retain typed, validated registrations or
copied state tied to one guest-memory generation, never raw indefinitely-lived
host pointers into guest memory.

## Initial Environment Service

`SVC_GETINITENVIRONMENT` (`0Fh`) accepts ES:0 as output and BX as output
capacity in paragraphs. It returns BX=0 outside first initialization. On first
initialization it builds a double-NUL terminated OEM environment and reports
the required/resulting paragraph count in BX:

1. takes the ambient host environment;
2. removes variables beginning with `=` and the original `COMSPEC`;
3. removes `windir` for ordinary DOS launches, with a separate-WOW exception;
4. OEM-converts values and uppercases variable names; and
5. prepends the DS:DX-provided 16-bit `COMSPEC=<...>` record from `cmdComSpec`.

The historical behavior also inserts `PROMPT=$P$G` into the host environment
when absent. Those ambient mutations and host-environment enumeration are
product policy, not a compatibility necessity. The evidence does establish
the guest buffer form: a bounded OEM multi-string plus a distinct COMSPEC
record, with an explicit capacity negotiation.

## Standard-Handle Service

`SVC_GETSTDHANDLE` (`06h`) takes CX as stdin/stdout/stderr selector and AX:BX
as a `REDIRCOMPLETE_INFO` address. It returns a raw 32-bit host handle in
BX:CX and a zero file-size pair in DX:AX. Pipe cases create temporary files,
threads, and event objects to emulate blocking/redirected stream behavior.

This is not a viable external ABI. A contained runner must use opaque
session-owned stream tokens, an explicit host stream inheritance policy, and
bounded pipe support. It must not hand guest code raw Win32 handles, create
temporary redirection files as an implicit policy, or mutate process-wide
standard handles merely to satisfy a COMMAND service.

## Minimal Contained CLI Contract

The source supports a deliberately smaller first CLI request:

- caller-supplied command line, allowed working directory, and explicit
  environment map rather than ambient `GetEnvironmentStrings`;
- profile-selected COMSPEC and bounded OEM environment serialization;
- three explicit stream capabilities or no stream redirection; and
- a single command-result/re-entry decision, with no BaseSrv, PIF, event
  thread, temporary configuration files, or host `=X:` mutation.

Exact `CMDINFO` layout and the guest-side COMMAND initialization caller remain
the next evidence slice. No command, environment, console, or stream handler
was implemented.
