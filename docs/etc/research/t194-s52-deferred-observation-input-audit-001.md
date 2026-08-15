# M0 T194 S52: Deferred Observation Input Audit

## Question

Is the S51 deferred native-POST bundle eligible for one watchdog-bounded
observation using the retained v6 controller?

## Inputs and Procedure

The audit read `tools/probe/Invoke-T194S10V6BoundedObservation.ps1`, its static
boundary test, S51's bundle manifest, and the current process table. The
controller retains its one-slot properties: a caller-selected bounded
watchdog, `retryPolicy = 'none'`, a refusal to overwrite an evidence root, a
terminal record in `finally`, and a PID-specific tree-kill attempt. Its source
SHA-256 is `002B217C5525CE9D72BC27A3FFF2EA091A63232369A1A55B0DAFF775D5AD364A`.
The S51 engine hash is
`79D404A9451F218009408D75AF2F2A36CFB8F984FFF64C0328DC34A8EBB0857E`.

The proposed fresh root
`artifacts/analysis/t194-s53-deferred-metadata-observation-001` did not exist.
The audit did not invoke the controller or create that root.

## Observation

The ordinary process query found three matching processes. Elevated read-only
process metadata established one exact old process tree:

| PID | Parent | Executable / role |
| --- | --- | --- |
| 11396 | 1192 | retained S15 `ntdos64-run.exe` wrapper |
| 23532 | 11396 | retained S15 `ntdos64-bochs-engine.exe` child |
| 12560 | 23532 | S46 direct bundle `ntdos64-bochs.exe` child |

The wrapper command line selects the old S46 direct metadata bundle, not the
S51 deferred bundle. Therefore the tree is neither an S51 result nor an
ambiguous same-name process. Its presence triggers the packet stop condition.

## Disposition

S52 rejects an S51 observation while this known old tree remains. A new,
separately admitted cleanup S may terminate only wrapper PID 11396 with its
confirmed descendants; no name-based cleanup and no new engine invocation are
authorized by this audit. After exact cleanup and a no-residual check, a later
packet may re-audit and admit one S51 observation.
