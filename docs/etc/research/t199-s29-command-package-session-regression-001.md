# M0 T199 S29 COMMAND Package Session Regression

## Question

Can every callable OpenNT COMMAND BOP identity be governed by one
adapter-owned package session, rather than by trace-selected detached helpers?

## Inputs

- Original order and handler ownership: `src/opennt/base/mvdm/inc/cmdsvc.h`
  and `src/opennt/base/mvdm/dos/command/cmddisp.c`.
- Existing bounded source-derived contracts for CMDINFO/launch, COMSPEC,
  current directory, paired CONFIG/AUTOEXEC materialization, and KEYB-not-
  installed response.
- `src/bx-vdm/bx_ntvdm_command_package_session_v1.{h,c}` and the common BOP
  ingress/provider registry.

## Implementation

`bx_ntvdm_command_package_session_v1` owns only copied COMMAND state:
COMSPEC bootstrap, SCS registration, launch declaration, and one-shot
CMDGETNEXT state.  It borrows, without taking ownership of, the already
initialized immutable guest namespace and DEM drive snapshot.

The composition has exactly one COMMAND execution call:
`bx_ntvdm_command_package_session_v1_dispatch`.  The former detached COMMAND
helpers and the COMMAND `0C/0D` branch in the generic boot-namespace plane
were removed.  The session first runs common ingress and the package facade,
then handles the bounded admitted providers.

| Services | Package result |
| --- | --- |
| `54:00`, `54:07`, `54:09`, `54:0B`, `54:10` | Explicit package deferred; no RAM, host-resource, or CPU-state mutation. |
| `54:01` | Bounded copied CMDINFO/launch transaction. |
| `54:02`, `54:0F` | One COMSPEC/environment bootstrap transaction. `0F` is only the second half of this already-admitted finite transaction, not a general environment capability. |
| `54:03` | Original inactive no-op: checked four-byte continuation. |
| `54:04` | Immutable admitted-drive root response. |
| `54:05` | Copied SCS registration. |
| `54:06`, `54:08`, `54:0A` | One explicit unavailable stop: historical Win32 handle/process behavior is not exposed. |
| `54:0C`, `54:0D` | Profile-materialized CONFIG/AUTOEXEC guest-path response. |
| `54:0E` | Fixed no-KEYB (`DX=0`) CLI capability response. |

This does not claim that the original COMMAND host component is fully
recreated.  In particular, deferred members do not start a host process,
acquire a host handle, activate VDD/GUI, or infer an ambient environment.
`54:04` and `54:0E` remain bounded provider facts within the package, not
proof that their original subcomponents are closed.

## Procedure and Observation

Ran the source-built family fixture:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t199-s29-command-package-r7 \
  -HostArchitecture x64
```

The MSVC x64 `/MT` closure compiled, linked, and returned zero.  Its COMMAND
family sweep checks all seventeen callable service IDs before the positive
provider cases: `03` resumes at `RIP+4`; `06/08/0A` stop; and
`00/07/09/0B/10` decline through the package deferred disposition.  The same
fixture then verifies the eight admitted provider paths and their required
state transitions.  No Bochs object, OpenNT runtime object, GUI/device
archive, MinGW object, or native execution trace entered this verification.

## Interpretation

Confidence is high for the adapter package boundary and its bounded provider
outcomes.  This is a source-built adapter composition witness, not a claim of
native guest integration or of full historical COMMAND parity.  Native trace
remains an integration verification after package implementation; it did not
select any service or scope in this S task.

## Follow-up

Apply the same package-session consolidation method to DEM, whose current
providers are still distributed across namespace, session, GSET, IOCTL and
error-lock subplanes.
