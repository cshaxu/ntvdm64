# BYOB Engine Contract

## Dispatcher-to-Engine Invocation

The dispatcher invokes an explicitly configured engine as:

```text
engine.exe -- target [target-arguments...]
```

`target` is the fully qualified path resolved by the dispatcher. The engine is
responsible for its own runtime discovery and must not assume that the
dispatcher copied, installed, or validated any guest/runtime file.

The proposed owned `nvtdm.exe` engine will accept `--runtime DIR` and require
these user-owned, regular, non-empty files before it stages a target:
`NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, `HIMEM.SYS`, and `DOSX.EXE`.
`VDMREDIR.DLL` and `EDIT.COM` are optional. Presence is not compatibility
proof; no file is copied, opened for modification, or executed by the manifest
step. This admissibility check is deliberately weaker than version or
compatibility verification. `--inspect` reads each admissible file solely to
report its SHA-256 fingerprint; that digest can be retained by the file owner
in a local runtime manifest without placing the runtime in this project.

No such executable is currently in the default CMake graph. The former
`refs/archive/legacy-adapters/nvtdm.c` implementation is a trace-instrumentation reference under the
explicitly disabled `NTDOS64_ENABLE_TRACE_INSTRUMENTATION` source-only target;
it contains a handwritten staging/interpreter/DOS-service probe and is not the
future engine implementation.

The retired source reference historically exposed
`nvtdm --runtime DIR --record-manifest FILE`; no current executable exposes
that command. It writes a new local text manifest
using only each profile role name, byte count, and SHA-256 fingerprint. It
refuses to overwrite an existing file and deletes a partial output on failure.
The manifest is identity evidence, not authorization to redistribute its
inputs and not a compatibility assertion.

The retired source reference's `--verify-manifest FILE` recomputes the metadata and
fingerprints and refuses a changed, missing, redirected, or unrecorded required
file. A successful verification locks identity only; it still does not prove
that the selected files are mutually compatible or bootable.

Its `--plan-bootstrap FILE` first performs that verification,
then reads `NTIO.SYS` into a private heap copy and prepares the fixed
OpenNT-profile bootstrap rewrite table. It fails when the registered pattern is
absent. It neither writes the source file nor starts guest execution; this is
identity-gated planning evidence, not bootability evidence.

## Historical BOP Observation

The former `nvtdm.exe --probe-bop-unsimulate target.com` command is retained
only as source-only test instrumentation for the OpenNT `QUIT.COM` body
(`C4 C4 FE`). It is not built into the dispatcher or a normal engine. It is
not a generic instruction override, a monitor ABI, or permission for normal
real-mode execution to consume BOP instructions.

## Non-Invasive Requirements

An engine accepted by this research path must not require process injection,
registry mutation, system-directory placement, a kernel driver, AppInit/AppCert
configuration, or a private console/loader interface. It may use ordinary
documented user-mode Windows APIs and user-selected local files.

## Capability Tiers

| Tier | Required behavior |
| --- | --- |
| `host-handoff` | Launch PE32/PE32+ through normal host process creation. |
| `dos-real-mode` | Execute a selected DOS MZ/COM target with a declared guest machine and local runtime policy. This tier is not currently in the normal build graph. |
| `ntdos-byob` | Load only user-selected runtime files, with an explicit manifest and no redistribution. |
| `dpmi` | Support protected-mode/V86 behavior through documented engine semantics. |
| `win16` | Separate optional tier; it is not implied by DOS support. |

## Retired Owned Prototype

`refs/archive/external-probes/whpx_com_run.c` is a retired, source-only WHPX COM/MZ prototype. Its
handwritten INT 21h adapter is frozen instrumentation and is deliberately not
in any normal engine build. It may not be extended into the owned monitor or
used as evidence of `dos-real-mode` or `ntdos-byob` capability.
