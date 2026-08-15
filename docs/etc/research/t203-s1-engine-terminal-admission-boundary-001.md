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

The direct engine fixture must link the current direct-host namespace.  That
namespace uses ordinary user-mode `ntdll` imports (`NtCreateFile`,
`NtQueryDirectoryFile`, `NtSetInformationFile`, and
`RtlNtStatusToDosError`); these are not deleted Win32 APIs and do not require
host installation or system modification.  Its older response file predates
that normal dependency, so a refreshed engine fixture must list `ntdll.lib`.

Independently, the current composition copies the inherited command
environment into a fixed 4023-byte OEM record.  The current developer
environment measures 6154 OEM bytes including the final NUL and is therefore
rejected before machine entry.  The rejection is an existing bounded-ABI
outcome, not evidence that a normal terminal BOP was reached.  A test may use
an explicitly selected small environment only to exercise engine mechanics;
it must not silently redefine the CLI's host-context projection.

These diagnostics do not authorise a T203 environment-policy rewrite.  That
belongs to a separately admitted CLI host-context/lifecycle package.

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
- `src/bx-vdm/bx_ntvdm_host_namespace.c` ordinary `ntdll` imports.
