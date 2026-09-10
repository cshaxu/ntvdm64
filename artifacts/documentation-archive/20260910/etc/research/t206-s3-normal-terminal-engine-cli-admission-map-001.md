# M0 T206 S3 — normal terminal engine/CLI admission map

## Question

May the T206 S2 COMMAND terminal record be published as an engine ordinary
completion and a CLI exit result?

## Current data/control-flow map

| Stage | Current owner and fact | Publication status |
| --- | --- | --- |
| Original COMMAND return | `cmdexec.c:602` identifies `cmdReturnExitCode`; `DX` is the DOS exit code and `CF` distinguishes re-entry from a shelled-out session ending. Its no-command behavior calls the historical `GetNextVDMCommand` broker. | Original owner is COMMAND plus unavailable BaseSrv/CSR composition. |
| BOP dispatch | `bx_ntvdm_cmd_return_exit_code_v1_dispatch` accepts only `C4 C4 54 0B`, resumes, and writes a record only on declared-plan exhaustion. | bx-vdm owns the record. |
| Record lifetime | `bx_ntvdm_boot_namespace_composition_v1` owns the COMMAND package session, whose `get_next` state owns the record until the next committed `54:01` or composition reset. | No composition-runtime copy API exists. |
| CPU loop return | `bx_ntvdm_machine_stage_v1_execute` returns only for an accepted generic STOP, finite budget, or unexpected loop return. A `54:0B` resume cannot itself return control to the engine. | bx-mantle is selector-blind. |
| Current declared-plan terminal | After all slots are delivered, `bx_ntvdm_command_package_session_v1.c` makes a later `54:01` a typed STOP. | Current profile composition behavior, not proof of the original no-command completion chain. |
| Engine result | `bx_ntvdm_engine_run_v1` can produce rejection, machine failure, budget, or controlled guest terminal. It has no ordinary-completion producer and resets composition before returning. | mantle/engine owns public result but has no terminal-record input. |
| CLI | `ntdos64_native_cli.c` maps the enum name `ORDINARY_GUEST_COMPLETION` to process exit zero but receives no DOS exit-code field. | CLI mapping is prospective only. |

## Required distinction

The current declared-plan STOP is neither the original BaseSrv/CSR
`RETURN_ON_NO_COMMAND` branch nor independent evidence that a guest executed
the source-level normal terminal sequence. The terminal record, conversely,
is a truthful `54:0B` fact but is not a CPU-loop termination mechanism.

Therefore these implications are invalid:

```text
controlled STOP             -> ordinary DOS completion
execution budget            -> ordinary DOS completion
54:0B record by itself      -> engine can return now
CLI process exit zero       -> guest DOS exit code
```

The only future candidate chain is stricter:

```text
source-derived 54:0B final return (bx-vdm record)
  -> subsequently source-proven normal guest terminal boundary
  -> selector-blind mantle returns that already accepted terminal boundary
  -> engine copies the record before composition reset
  -> fixed-width engine result -> CLI presentation
```

It must prove that the terminal boundary occurs after the final `54:0B` in the
same installed composition. It must also prove that a prior record followed by
budget/failure, or an unrelated controlled STOP, remains non-ordinary.

## Historical dependency disposition

`cmdReturnExitCode` normally asks `GetNextVDMCommand(RETURN_ON_NO_COMMAND)`
for a new DOS record or the host broker's terminal result. That branch depends
on `VDMINFO`, BaseSrv/CSR state, event-thread scheduling, handles, Redirector
completion and process-global stdio restoration. T202 S7 correctly retains it
as unavailable; a local CLI queue would recreate the missing broker rather
than preserve the source contract.

Normal user-mode Win32 APIs are not the blocker here. The missing ownership is
the historical command-broker lifecycle, while the current profile must also
prove its guest-side post-return terminal sequence.

## S3 decision

**No engine or CLI publication implementation is admitted by S3.** A record
copy API, a new engine enum field, or a CLI exit mapping would be premature:
without the required post-return terminal witness, each could turn either a
budget or an unrelated STOP into fabricated success.

T206 has nevertheless closed the reusable COMMAND-side prerequisite: exact
`54:0B` route and a fixed-width record. The next work may return to complete
owner-package BOP recovery. A later, separately admitted normal-terminal
package may reopen publication only with an integrated source-derived witness
and whole-chain positive/negative regression; its test must not use a trace to
invent the endpoint.

## Inputs

| File | SHA-256 |
| --- | --- |
| `src/opennt/base/mvdm/dos/command/cmdexec.c` | `98A941095D47AE73EF9731D389A433203C7184B0A016B62E24B202BB89F829D0` |
| `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c` | `65869373D6865815CDF81AB130CDE98DB360897441ACCE179B1C9CFB375D4221` |
| `src/bx-vdm/bx_ntvdm_command_package_session_v1.c` | Current worktree source, read-only S3 inspection. |
| `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc` | Current worktree source, read-only S3 inspection. |
| `src/bx-mantle/bx_ntvdm_engine_run_v1.c` | `D043B1859CF57A89F4D1F1498108969FDD339E3D51909CF1D15B757B4A2A25DD` |
| `src/bx-mantle/bx_ntvdm_engine_contract_v1.h` | `02BFB65978A75EF40D813C0FCA83755FF421EFE41B2D0F728E6ACC6D21C92BC5` |
| `src/cli/ntdos64_native_cli.c` | `875C08E84502EB9E48034C85C448BCE2C7D6F022B986A2E2D8170A99A070402E` |

