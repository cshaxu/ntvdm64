# T224 S5 COMMAND Shared Child-Redirection Provider Admission

## Question

What is the smallest source-derived COMMAND composition seam that can preserve
the shared ownership of `54:08`, `54:0A` and `54:0B` in the MSVC x64 CLI
runtime, without importing the historical VDM broker or treating a traced
launch as an independent BOP patch?

## Inputs

- T224 S4 source/ABI/failure map.
- OpenNT `base/mvdm/softpc.new/host/src/cmdexec.c:429-635` and
  `cmdredir.c:225-315,602-656`.
- Current `src/bx-vdm` COMMAND stream-session and lifecycle sources.

## Source-Recovery Audit

| rung | result | reason |
| --- | --- | --- |
| Original translation unit | rejected | `cmdexec.c` reaches CCPU/SAS accessors, `GetVDMAddr`, `GetNextVDMCommand`, global event-thread state and SoftPC notification. It is not independently composable into the x64 CLI process. |
| Smallest contract-preserving seam | admitted | one `bx-vdm` COMMAND-private provider copies bounded launch input, consumes S2 opaque streams, calls a private Direct backend and records its completion for the same provider's `54:0B` route. |
| External-code intrusion | not needed | no Bochs or historical-source modification is required. |
| New behavior | limited source-derived rehost | the private backend substitutes only the unavailable broker/thread composition; it retains the original owner, entry ordering and launch/environment failure terminals. |

## Provider Contract

The provider must own all three identities together:

| identity | retained source ABI | S5 terminal |
| --- | --- | --- |
| `54:08` | checked `DS:SI` CR tail, `ES:0` double-NUL environment, `SS:BP` packed opaque standard streams, `AL` drive and `AH` flag | prepares the shared request; malformed tail is `ERROR_BAD_FORMAT`, invalid/missing environment or processor command is `ERROR_BAD_ENVIRONMENT`; Direct may submit it to the private backend |
| `54:0A` | checked `ES:0` environment and `AL` drive | prepares the same request through the host-context command path; Direct may submit it to the same backend |
| `54:0B` | `DX` exit, `AL` drive and historic `BX:CX` redirection input | consumes only the provider-private completion record; no guest-visible pointer or raw handle is accepted or published |

The Direct backend owns any Win32 `HANDLE` duplication, process creation, wait
and cleanup. Its request/result are adapter-private, bounded and copied. The
guest boundary sees only fixed register results. Readonly refuses before any
stream capture or child operation. Overlay and Virtual remain explicit
unavailable dispositions; no fallback is permitted. Pipe creation, `cmdPipeList`
workers and DEM `50:47/48` success remain outside S5.

## Procedure

1. Extract the former synchronous leaf into the typed private Direct backend.
2. Make the child provider prepare both launch forms and retain one completion
   record for `54:0B`.
3. Route the package session to that provider before the legacy `54:0B`
   approximation, retaining the latter only for non-child state.
4. Add a deterministic backend fixture; it must not launch ambient `cmd.exe`.

## Acceptance

The focused x64 `/MT` fixture must prove both entry forms select the one
provider, Readonly refuses, and completion is consumed exactly once. A native
trace is deliberately deferred until the complete owner package has its
provider-family regression.

## Follow-up

The next package after S5 may admit private pipe-session creation and the DEM
continuation only as one producer/consumer lifecycle. It must not reinterpret
this Direct non-pipe completion seam as pipe support.
## P1 Observations

The focused `command-launch-execution` fixture compiled and linked the complete
listed `bx-vdm` closure under MSVC x64 `/MT` and exited `0`. It injects a
COMMAND-private deterministic backend (not a bridge callback) and verifies:

- `54:08` accepts only the admitted stream-session route and records completion
  result `23`;
- `54:0B` consumes that private completion exactly once;
- `54:0A` uses the same provider/backend request shape without a standard-stream
  record and records result `24`; and
- the existing Readonly `54:06` admission refusal remains a CF failure before
  standard-handle capture (the Readonly launch terminal remains S5 work).

The manifest is intentionally disposable under
`build/M0-T224-S5/006-command-child-provider-final/`; it records `linkExitCode: 0`,
`runExitCode: 0` and `passed: true`.

## Interpretation And Confidence

High confidence that the former leaf `CreateProcessW` code is now structurally
behind one typed private backend, while COMMAND retains the BOP ownership and
completion lifecycle. This is P1 boundary evidence, not full S5 closure:
package-session memory-gather routing and the Readonly launch terminal still
need their own whole-provider exercise, and no pipe/DEM success claim follows.

## P2 Package-Session Observation

The focused command-child-redirection-package fixture passed under MSVC x64 /MT (linkExitCode: 0, unExitCode: 0). It exercises the actual COMMAND package-session route: Direct 54:0A reaches the shared private backend and its 54:0B completion; Readonly 54:0A returns the retained explicit ERROR_NOT_SUPPORTED terminal before the backend is called. This remains non-pipe evidence only.
