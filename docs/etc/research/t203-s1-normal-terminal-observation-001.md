# T203 S1 normal DOS terminal observation

## Question

Does the current source-built CPU5 one-shot profile reach the callable normal
DOS return BOP (`54:0B`), and is there already a complete engine-to-CLI result
path that could safely expose its result?

## Inputs and procedure

1. Corrected the selector premise using the
   [T203 selector source map](t203-s1-command-terminal-selector-source-map-001.md):
   `54:0B` is `cmdReturnExitCode`; `54:11` is a dispatcher sentinel.
2. Ran the unmodified current-source fixture:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t203-s1-normal-540b-native-r3
   ```

3. Inspected `bx_ntvdm_cmd_return_exit_code_v1_dispatch`,
   `bx_ntvdm_engine_run_v1`, `bx_ntvdm_engine_contract_v1`, and the native
   CLI result mapping.

## Observation

The retained JSON record is
`artifacts/build/t203-s1-normal-540b-native-r3/t198-s94-source-built-normal-return-native-probe.json`.
It records current-manifest source rebuild, MSVC x64 `/MT`, successful link,
one run, and `runExitCode: 2`.

The run log's first canonical BOP is `50:11`; it then records the established
DEM lifecycle through `50:42`.  It contains no `54:0B` observation.  This is
a source-built, bounded **non-reach** result, not a normal DOS completion or
a BOP-provider failure.

The code map is equally conclusive:

| Layer | Current fact | Consequence |
| --- | --- | --- |
| COMMAND session | `54:0B` can resume and retain a package-local copied low `DX` value when its declared plan is exhausted. | This is not yet a terminal event exported from the session. |
| bx-vdm generic bridge | Only records an accepted generic `STOP` in its test-only terminal observation; product composition leaves that facility disabled. | A resume from `54:0B` cannot be inferred as an engine terminal. |
| engine contract | It defines `ORDINARY_GUEST_COMPLETION`, but `bx_ntvdm_engine_run_v1` never sets that kind. | The enum is a reserved result category, not an implemented result path. |
| native CLI | Prints an engine result and maps terminal kinds to its own process status. | It does not see a copied DOS result, and process status cannot be used as one. |

## Decision

**Do not admit a terminal-result ABI implementation in S1.**  The required
normal-return event was not reached, and the engine has no owner-approved
extraction point between a resumed `54:0B` and an ordinary guest terminal.
Adding a field, observing a generic stop, or translating the fixture's host
exit code would fabricate the missing lifecycle evidence.

The `ORDINARY_GUEST_COMPLETION` enum remains a design placeholder.  It may be
used only after a future owner-package/engine task proves the entire normal
guest path and gives a fixed-width copied terminal payload one owner.

## Follow-up

Keep the present one-shot result categories unchanged.  A future re-admission
needs a normal source-built `54:0B` observation, the post-resume guest
termination path, a fixed-width result payload/ownership design, and positive
and negative lifetime tests.  No trace-selected BOP implementation is
created from this non-reach result.
