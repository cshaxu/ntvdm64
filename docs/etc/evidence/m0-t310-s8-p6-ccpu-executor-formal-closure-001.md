# M0 T310 S8 P6 — CCPU executor formal closure

## Result

`SPC-CCPU-EXECUTOR-DISPATCH` is closed.  This record is limited to the
original CCPU executor/FPU dispatch contract; it does not claim the later
CCPU access-table, SAS, EMS, device, BOP or whole-machine contracts.

## Selected profile and source disposition

- The selected CPU_40/FPU profile composes the original `fpu.c` body.
- `ntstubs.c` remains an unmodified mirror source, but is an alternate
  host-profile carrier and is not selected beside `fpu.c`.  Selecting both
  created duplicate `initialise_npx`/`npx_reset` and SAS fallback definitions.
- `New-T310OriginalSoftpcNinja.ps1` now selects 130 CCPU sources rather than
  131.  The only removal is the alternate `ntstubs.c` translation unit.
- The imported NTVDMx64 CCPU-vector-default archive remains limited to its
  registered debug-break default symbols.

## Formal evidence

Both fresh MSVC `/MT` graphs completed their 362-node candidate targets and
their forced-link audits after the selection correction:

- `build/M0-T310/S8/p6-formal-refresh-20260829/x64`
- `build/M0-T310/S8/p6-formal-refresh-20260829/x86`

Neither forced-link log has an `fpu.obj` duplicate-definition record.  The
only CCPU-attributed unresolved symbols are deliberately outbound edges:

- `c_xcptn.obj -> trace_file`: debug trace provider, later debug owner;
- `ccpusas4.obj -> LIM_b_write` and `LIM_w_write`: original EMS/LIM memory
  provider, later `MVDM-DOS-OWNER-PACKAGE`/mapping-owner work.

They retain the original declarations and failure direction under the S8
external-boundary rule; S8 does not fabricate implementations for them.

## Focused behavior

Fresh bounded executables built from the current sources both returned zero
and emitted the same sequence:

`sas-init -> cpu-init -> access-init -> seed -> start -> returned-start ->
reenter -> returned-recursive`.

Outputs:

- `build/M0-T310/S8/p6-execution-refresh-20260829/x64/ccpu-bounded-execution.exe`
- `build/M0-T310/S8/p6-execution-refresh-20260829/x86/ccpu-bounded-execution.exe`

The initial typed-null exception-frame diagnostic is intentional and proves
the retained invalid-TLS failure path.  This bounded observation selected no
new work.
