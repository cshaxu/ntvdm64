# T95 S7 COMMAND COMSPEC Bootstrap Closure 001

## Reached boundary

The r21 source-built trace reaches `C4 C4 54 02` immediately after the
adapter has resumed `50:36` into the permanent COMMAND.COM path.  The generic
listener records this at `0bc1:1b0e`; it is currently passed through, then
causes the generic `BOP 06` path to repeat.

`cmdsvc.h:18` names the service `SVC_CMDCOMSPEC`.  `command/init.asm:1121-28`
calls it only for permanent COMMAND, with `DS:DX` pointing to the resident
`ComSpec` string.  It consumes only the returned `AL`, storing it into
`SCS_PAUSE`; its original owner is `cmdComSpec` in `cmdmisc.c:635-650`.

## Original data flow

The host owner copies the guest string into `lpszComSpec` prefixed with
`COMSPEC=`, records `cbComSpec`, and returns `AL = (!fConOutput || VDMForWOW)`.
That retained value is not merely diagnostic: `cmdGetInitEnvironment` in
`cmdenv.c:179-330` later copies it into the permanent COMMAND environment,
and `cmdexec.c:249-257` uses it for the historical 32-bit COMSPEC/SCS path.
Therefore a `54:02` endpoint that only returns AL would be an incomplete
semantic migration: it would turn the next `54:15 SVC_GETINITENVIRONMENT`
call into a separate state-invention problem.

## Current contained-profile inputs

The current v2 profile parser requires canonical guest placement
`\\COMMAND.COM` and the adapter already constructs an immutable three-file
namespace (COMMAND, minimal CONFIG.SYS, empty AUTOEXEC.BAT) in
`bx_ntvdm_readonly_namespace_v1`.  This supersedes the older statement that
guest placement was only a design prerequisite.  It does **not** authorize
using the BYOB root as a DOS drive or importing a host environment.

## Required cohesive admission

The next capability is a single COMMAND-bootstrap pair, not independent BOP
patches:

1. `54:02` may issue one bounded adapter/Bochs gather read for the guest
   `DS:DX` COMSPEC string, validate that it is the profile's canonical initial
   DOS path, retain only the bounded `COMSPEC=<path>\0` adapter value, and
   return the CLI's explicit non-console pause value in AL.
2. `54:15` may use only that retained value plus a separately declared
   CLI environment policy to materialize the initial DOS environment at the
   guest-owned ES:0 buffer, with source-compatible BX sizing/retry behavior.
3. No VDD/SCS, Win32 process environment, host path, Bochs device, or NTDOS
   source mutation is admitted.  A noncanonical or unterminated input remains
   listener pass-through until a deliberate rejection contract is defined.

## Implementation and unit evidence

`bx_ntvdm_cmd_comspec_bootstrap_service_v1` now implements the pair using the
existing generic gather-read and atomic multi-write transports.  It asks only
for the exact canonical path length (including NUL), rejects a different byte
sequence, retains no host pathname, and emits only `COMSPEC=<canonical path>`
plus the final environment-list NUL.  Its DOS drive letter is derived as
`'A' + drive_index` (therefore profile index 2 is `C:`).

The isolated C11 target `bx-ntvdm-cmd-comspec-bootstrap-service-test` verifies
the exact gather range, `AL=1`, the BX shortage retry, and a single successful
ES:0 write.  It passed in
`artifacts/build/t95-s7-comspec-bootstrap-c11-001`.

Same-version r22 MSVC/x86 runtime evidence closes the first real path: it
observes `54:02` at `0bc1:1b0e`, copies exactly 15 guest bytes and accepts the
resume to `1b12`; it later observes `54:0f` (the hexadecimal form of
`SVC_GETINITENVIRONMENT` service 15), commits one 23-byte environment write,
and accepts the resume to `0bc1:03aa`.  The binary SHA-256 is
`62304E5847F6020C9A7DCDADEC74AC802A31FC71E6212240010A7D04405DDB92`; evidence
is `artifacts/analysis/t95-s7-runtime-trace-021-20260811-001`.
