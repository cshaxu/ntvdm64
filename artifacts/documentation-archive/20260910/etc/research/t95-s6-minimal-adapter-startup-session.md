# T95 S6 Minimal Adapter Startup Session

## Scope

`src/bx-ntvdm-adapter/bx_ntvdm_startup_session.c/.h` is the extracted narrow
composition owner. It validates the selected BYOB profile/root, loads all
three declared component identities into adapter-owned immutable buffers, and
can produce only the existing NTIO `startup_plan_v1`:

- payload range: physical `0x700`;
- preserved range: `0x714..0x717`;
- entry delta: real-mode `CS:IP = 0070:0000`.

NTDOS and COMMAND are identity-validated/lifetime-owned only. They are not
written to guest memory, parsed or started.

## Boundary

The component has no dependency on `bx_ntvdm_adapter_runtime`, host session,
host service plane, DEM/BOP/CMD dispatcher, namespace/search/drive policy,
Bochs C++ headers or guest-memory primitive. The pointer returned by plan
preparation remains adapter-owned and is intended only for the already
registered same-island generic plan consumer; it is not an ABI crossing.

## Verification

`tests/bx-ntvdm-adapter/Test-StartupSessionBoundary.ps1` passes. It requires
the three verified image loads, destruction cleanup and exact NTIO plan values,
and rejects forbidden broad-runtime/service/backend terms after stripping
comments. Documentation governance and safe diff checks pass.

The one terminal MSVC/x86 object-only audit is retained at
`artifacts/analysis/t95-s6-startup-session-msvc-objects-003-20260811-001`.
Its `object-audit.json` records exit code zero and SHA-256 manifests for the
eight declared copied inputs and their eight objects. `compile.log` identifies
BuildTools 2022 x86 and exactly those translation units. The audit used
`/nologo /c /MT /W3 /DWIN32 /Iadapter /Icli`; it created no archive, link map,
executable, Bochs object, guest write, or runtime trace.

## Non-Claims

No link, Bochs opt-in, payload write, CPU entry, guest execution, CLI
integration, NTDOS/COMMAND guest load or host capability occurs. The next gate
is a separately admitted static symbol/dependency inventory of these exact
objects before any same-island companion-link proposal.
