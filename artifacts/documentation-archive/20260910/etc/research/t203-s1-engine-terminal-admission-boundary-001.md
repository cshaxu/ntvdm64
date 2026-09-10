# T203 S1 engine terminal-result admission boundary

## Question

Can the current engine safely publish the ordinary DOS terminal result that
the CLI already names as `ORDINARY_GUEST_COMPLETION`?

## Static ownership result

The answer is no.  `bx_ntvdm_engine_run_v1` is the only current producer of
the public engine result.  Its result-setting branches produce rejected
request, rejected composition, machine failure, execution budget, or
controlled guest terminal only.  The `ORDINARY_GUEST_COMPLETION` enum value is
valid for contract testing and is recognised by `ntdos64_native_cli.c`, but
has no runtime producer and has no copied DOS-exit-code field.

The COMMAND provider has a deliberately narrower fact:
`bx_ntvdm_cmd_return_exit_code_v1_dispatch` accepts only `C4 C4 54 0B`,
copies the low `DX` exit value into package-local state, resumes at `RIP + 4`,
and records that the plan has returned.  It neither stops the machine nor
publishes state to bx-mantle.  This is correct boundary preservation, not a
missing assignment to the public result.

Therefore no caller may infer ordinary completion from a controlled stop,
execution budget, `54:01` declared-plan exhaustion, process exit code, or a
package-local COMMAND field.

## Current engine-harness admission diagnostic

The direct engine fixture now links the current direct-host namespace.  That
namespace uses ordinary user-mode `ntdll` imports (`NtCreateFile`,
`NtQueryDirectoryFile`, `NtSetInformationFile`, and
`RtlNtStatusToDosError`); these are not deleted Win32 APIs and do not require
host installation or system modification.  Its older response file predated
that normal dependency, so the refreshed fixture explicitly lists `ntdll.lib`.

Independently, the current composition copies the inherited command
environment into a fixed 4023-byte OEM record.  The current developer
environment measures 6154 OEM bytes including the final NUL and is therefore
rejected before machine entry.  The rejection is an existing bounded-ABI
outcome, not evidence that a normal terminal BOP was reached.  A test may use
an explicitly selected small environment only to exercise engine mechanics;
it must not silently redefine the CLI's host-context projection.

These diagnostics do not authorise a T203 environment-policy rewrite.  That
belongs to a separately admitted CLI host-context/lifecycle package.

## Engine-level controlled-terminal witness

The refreshed `Invoke-T200S22EngineMachineStageProbe.ps1` rebuilds the engine
entry, product generic bridge, copied terminal observer, and direct-composition
fixture under MSVC x64 `/MT`.  It starts the fixture with an explicit small
`ComSpec`/`PROMPT` environment, rather than inheriting an oversized developer
shell.  This is fixture input selection only; it does not alter product
host-context policy.

The run at `artifacts/build/t203-s1-engine-observer-r4/` passed twice through
install, real-mode entry, execution and reset.  The copied observer saw at
least one accepted product-bridge outcome and exactly one accepted STOP.  The
captured bytes were:

```text
CS:EIP 8dc8:00000984  C4 C4 50 3D
```

Thus the current engine path reaches a DEM `50:3D` controlled terminal, not
COMMAND `54:01` declared-plan exhaustion and not COMMAND `54:0B` normal
return.  The test therefore fixes a previous stale-object ambiguity in the
engine harness, but supplies no ordinary DOS exit result.

The focused `bx_ntvdm_terminal_observation_v1_test.c` also source-built and
passed under MSVC x64 `/MT`: disabled observation stays at zero, an accepted
resume increments only the opaque call count, and one accepted STOP increments
the STOP count and captures its existing typed outcome.  Neither counter
contains selector, provider, guest-memory, or CPU-state semantics.

Reproduce with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S22EngineMachineStageProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t203-s1-engine-observer-r4 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t203-s1-normal-540b-native-r5
```

## Admission decision

T203 S1 admits **no** engine-to-CLI ordinary-terminal ABI.  A qualifying
future observation must use the product engine lifecycle with an explicitly
admitted host context and show this whole chain without control-flow changes:

```text
original COMMAND 54:0B resume
  -> copied COMMAND terminal record
  -> explicit engine ordinary-completion transition
  -> fixed-width CLI result presentation
```

Until all four links exist, the only truthful CLI outcomes are the existing
engine terminal kinds.  In particular, no BOP provider, Bochs mechanic,
process exit-code convention, raw pointer, or guest-memory handle is added by
this record.

## Evidence anchors

- `src/bx-mantle/bx_ntvdm_engine_run_v1.c` result branches;
- `src/bx-mantle/bx_ntvdm_engine_contract_v1.h` contract enum;
- `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c` normal-return dispatch;
- `src/cli/ntdos64_native_cli.c` current presentation mapping;
- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c` bounded environment capture;
- `src/bx-vdm/bx_ntvdm_command_host_context_v1.h` capacity; and
- `src/bx-vdm/bx_ntvdm_host_namespace.c` ordinary `ntdll` imports;
- `tests/bx-mantle/bx_ntvdm_engine_direct_composition_v1_test.c`; and
- `tests/bx-vdm/bx_ntvdm_terminal_observation_v1_test.c`; and
- `tools/probe/Invoke-T200S22EngineMachineStageProbe.ps1`.
