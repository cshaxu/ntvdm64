# R3 Direct-CLI Command Transport Contract

Status: source-derived design gate, 2026-08-07. This is not an implementation
and does not authorize a default runner target.

## Historical Boundary

Original `host/src/config.c` asks `GetNextVDMCommand(&GetPIF)` with
`ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY`. The public internal `VDMINFO` ABI
and the OpenNT Base client show that the historical implementation transports
this request through CSR/BaseSrv, carries command/PIF/current-directory/
environment buffers, and can block waiting for another DOS application.

That CSR transport is unavailable and out of scope for a non-invasive direct
CLI process. It is a host-context seam, not DOS, BIOS, BOP, DEM, CCPU, or
device behavior.

The same `config.c` then calls the original `GetPIFData`. The fixed source
contains the original parser in `host/src/nt_pif.c`; it establishes defaults
before reading a PIF. Therefore the modern seam must not implement or replace
the PIF parser.

## First-Cut Scope

The future direct-CLI provider is a one-command, one-session source of
`VDMINFO` data. It has exactly three supported calls:

| Historical call shape | Allowed result |
| --- | --- |
| `GetNextVDMCommand(NULL)` | Return a fixed per-session first-VDM result; do not contact a system service. |
| `GetNextVDMCommand(info)` with `ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY` | Fill only caller-provided PIF metadata needed to keep `config()` on its original path. No command launch, no environment creation, no directory mutation, and no blocking. |
| Any subsequent command request or WOW/re-entrancy state | Return `FALSE` with a redacted, explicit unsupported-session result. |

The provider owns no long-lived pointers. Every written buffer must be
caller-provided, bounded by its matching `VDMINFO` length field, and
NUL-terminated only when there is capacity. It returns no PEB, CSR, console
server handle, PIF object, or host path outside the session's approved root.

## Explicit Non-Behavior

The first provider must not:

- block for another application or implement command queuing;
- launch DOS, NE, PE, WOW, or any host process;
- synthesize a PIF, read `%WINDIR%`, set a host current directory, alter the
  environment, or use `NtVdmControl`;
- implement PIF hotkeys, fullscreen mode, console shortcuts, fast paste,
  VDD setup, or a DOS service;
- return success for an unsupported state merely so initialization continues.

## PIF Defaulting Gate

`GetPIFData` calls `SetPifDefaults` before it attempts to resolve a PIF. If
the PIF name is empty, however, it probes `%WINDIR%\\_default.pif`, which is
forbidden ambient discovery. The direct-CLI provider therefore cannot simply
return an empty PIF name.

The selected R3.C option is an explicitly user-selected,
manifest-validated `_default.pif` file under the session root. It is declared
as the `pif-default` role when the BYOB `startup-profile` feature is active.
It is an external configuration input, not a guest DOS image and not a file
the project generates or distributes. The provider passes only that validated
contained path to the original code; it does not read, parse, or manufacture
the PIF.

The provider may not use a nonexistent path merely to induce parser defaults:
that would falsely mark `AppHasPIFFile` and leave a hidden file-access policy.
No source change is admitted until the selected role's failure status and
redacted trace record are specified.

The default parser path itself is now source-level tested by the separate
`ntdos64-opennt-pif-default-parser-fixture`: it generates and deletes an owned
minimal structural PIF while executing original `nt_pif.c`. The parser returns
its original default XMS/EMS/idle/key settings and does not reach the fixture's
fail-closed environment-expansion or message-box callbacks. This is evidence
for parser behavior only, not permission to create or distribute a PIF in the
runner or repository.

## Admission Tests

An implementation may enter a default-disabled configuration trace only when
it proves all of the following:

1. valid `VDMINFO` buffer lengths are respected and invalid capacities fail
   before any partial write;
2. every supported request returns the same recorded result without a CSR,
   PEB, registry, process launch, global current-directory change, or ambient
   drive/file discovery;
3. the original `GetPIFData` parser receives the chosen contained input;
4. unsupported later-command/WOW/re-entrant requests fail closed; and
5. original `config()` reaches its first machine/profile dependency without a
   replacement PIF parser, BIOS, BOP, DEM, or DOS handler.

This contract is deliberately narrower than a general `GetNextVDMCommand`
replacement. COMMAND's later request loop is a Phase R4 concern and must
receive its own contract after original NTDOS and COMMAND initialization are
reached.

## Fixture Evidence

The default-disabled
`ntdos64-opennt-direct-cli-command-transport-fixture` compiles the local
transport against the fixed historical `VDMINFO` definition in
`public/internal/base/inc/vdmapi.h`; it has no `config.c`, CCPU, BIOS, BOP,
DEM, DOS, or runner dependency. It was rebuilt and executed on 2026-08-07
with exit code `0`.

The fixture proves the selected first-query contract only: unconfigured
rejection, first-VDM result, exact-capacity PIF-path copy, undersized-buffer
rejection with no writes, repeated-query rejection with no writes, and
unsupported WOW-state rejection with no writes. It does not prove that
original `config()` or the PIF parser can yet run.
