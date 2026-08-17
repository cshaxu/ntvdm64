# T225 S20 COMMAND sensitive-subfamily source/ABI map 001

## Purpose

This P2 supplement resolves the six sensitive rows identified by the S20/P1
whole-package ledger before writing the single family fixture. It records
actual current seams and OpenNT ordering; it does not add a BOP handler.

## `54:04` current directory

OpenNT `cmdmisc.c:530..584` first tests drive validity, then obtains the
per-drive `=X:` environment value, supplying `X:\` when absent; invalid or
oversized outcomes set CF and AX. The current COMMAND service
`bx_ntvdm_cmd_current_dir_service_v1_prepare` retains its copied buffer,
CF/AX and checked write contract. It does not invent a virtual root.

The current directory is owned by the shared session host context:
`bx_ntvdm_session_host_context_v1_set` calls `SetCurrentDirectoryW` only for
the Direct profile after the admitted host namespace confirms the path;
Readonly returns its explicit readonly disposition. COMMAND subsequently
projects that session context through
`bx_ntvdm_session_host_context_v1_project_command`. Therefore `54:04` reads
the same real host-session CWD that the original `cmdSetDirectories` /
`cmdUpdateCurrentDirectories` model supplied, while no guest pointer or host
handle crosses the boundary. The family fixture must verify both the admitted
Direct projection and unavailable-drive CF/AX failure; it must not require a
host mutation in Readonly.

## `54:06`, `54:08`, `54:0A`, `54:0B`

Original `cmdredir.c:cmdGetStdHandle` and `cmdexec.c:cmdExec`/`cmdExecComspec32`
use process-global standard handles and `CreateProcess`, then the return path
uses `cmdReturnExitCode` to re-enter the historical VDM broker. Their original
translation units cannot compose because they depend on VDM event threads,
CCPU/SAS pointers, and `GetNextVDMCommand`.

The selected rung-2 seam is one session-owned opaque stream table:

- `54:06` snapshots Direct `GetStdHandle` values, returns opaque tokens only,
  and rejects Readonly without exposing a HANDLE.
- `54:08` and `54:0A` use `DuplicateHandle` and `CreateProcessW` only behind
  the Direct child backend. Readonly cannot select that backend and receives
  the contained `ERROR_NOT_SUPPORTED` (`AL=50`, CF clear) result already
  used by the package lifecycle provider.
- `54:0B` consumes only a completion stored by that child backend. Otherwise
  it retains the source-derived no-reentry response rather than manufacturing
  a host process or raw BOP decline.

The family fixture must use a fake backend through this existing injection
point. It must prove Direct completion ordering and prove no fake-backend call
in Readonly.

## `54:0F` initial environment

OpenNT `cmdenv.c:179..324` builds a dynamically allocated filtered OEM
snapshot and returns a paragraph count in BX when ES:0 is too small. Current
`bx_ntvdm_command_initial_environment_v1_capture` retains that filter and
`BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES=65462` preserves the
maximum 16-bit transaction, not a 4023-byte artificial cap. The existing
focused test already injects a 5000-byte value and verifies the required
paragraph-retry path. The S20 family fixture must consume that package state;
it must not introduce a second environment parser.

## Fixture boundary

The new whole-family fixture will instantiate only the existing
`bx_ntvdm_boot_namespace_composition_v1` package/session, its copied mutation
profile, and a local mechanical read/write stub. It will invoke the common
COMMAND package dispatch for every `54:00..10` slot, then compare Direct and
Readonly results. It will not bind a new adapter runtime, fake BIOS/CPU
behavior, or make a native trace.
