# Win32 Host API Boundary

## Decision

`ntdos64-run` and the owned `nvtdm` engine use ordinary documented user-mode
Windows APIs as their required host interface. Historical or undocumented APIs
may be examined only to understand why older implementations behaved as they
did. They are never a mandatory installation, bootstrap, process-transfer, or
console dependency for this project.

## Allowed Required Surface

| Need | Documented host family | Constraint |
| --- | --- | --- |
| CLI and target dispatch | `GetCommandLineW`, `CommandLineToArgvW`, `CreateProcessW`, process wait/exit APIs | PE32/PE32+ remains a host process, never a guest image. |
| Runtime discovery | file attributes, file open/read, path APIs | Read-only; runtime root is chosen on the command line. |
| DOS namespace backing | file, directory, find, metadata, share/lock APIs | Must be rooted and policy-checked before every host operation. |
| Guest backing memory | heap or virtual-memory APIs | Owned process memory only; no cross-process memory access. |
| Console and streams | console, standard-handle, pipe APIs | Own process and child processes only. |
| Time, input, display | documented time/input/window APIs | Exposed through capability contracts, never through guest pointers. |
| Diagnostics | normal file output and ETW/debug output if later selected | Redact runtime content and host paths by default. |

## Prohibited Required Surface

The following remain outside the product path even where an older NTVDM-family
implementation used them:

- AppInit/AppCert or comparable global DLL-loading configuration.
- Registry changes used to alter host process creation or compatibility.
- CSRSS, console-server, WOW64, loader, or VDM private interfaces.
- System-directory deployment, file replacement, service creation, drivers,
  injection, remote thread creation, and process-memory patching.
- Any API whose only practical purpose is to re-enter a removed Windows NTVDM
  execution path.

## Research Handling for Undocumented APIs

An undocumented API may appear in a research note only with: the observed
historical caller role, the capability it supplied, a documented substitute or
an explicit "no substitute" result, and an explanation of why it is excluded
from the owned engine. A note must not include copied headers, private ABI
layouts, extracted binaries, or machine-local system data.

## Isolation Rule

If an optional experiment ever needs a private interface, it must be a separate
manual-only lab executable outside the normal dispatcher/engine build. It may
not be invoked by `ntdos64-run`, `nvtdm`, tests, or installation instructions.
