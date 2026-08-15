# T215 S3: Native Controlled-Terminal Causal Map

## Question

What causes the full current-source composition fixture to report
`CONTROLLED_GUEST_TERMINAL` with detail `1`, and does that observation admit a
new BOP implementation?

## Inputs

- `src/bx-mantle/bx_ntvdm_engine_run_v1.c` and
  `src/bx-mantle/bx_ntvdm_machine_stage_v1.{h,cc}`;
- the existing default-off copied STOP observer in
  `src/bx-vdm/bx_ntvdm_terminal_observation_v1.{h,c}`;
- full-source fixture `tests/bx-mantle/t214_s3_composition_owner_fixture.c`;
- OpenNT `base/mvdm/inc/dossvc.h`, `dos/dem/demdisp.c`, and
  `dos/dem/demmisc.c`; and
- retained source classification
  `t198-s91-dem-exit-vdm-classification-001.md`.

## Procedure

The fixture enabled the already-present test observer for one run, executed
the existing full current-source x64 `/MT` composition, copied the accepted
fixed-width generic-#UD STOP record after engine return, and disabled the
observer again.  It did not alter a provider, BOP route, profile, Bochs
mechanics, guest memory, or CPU state.

The exact command was:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T214S3CompositionOwnerFixture.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t215-s3-controlled-terminal-r2
```

## Observations

The source-built run passed and emitted:

```text
t214-s3 engine call=1 valid=1 terminal=5 detail=1
t215-s3 stop cs=8dc8 eip=00000984 window=c4c4503d disposition=2
```

`BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP` is enum value `1`.
`bx_ntvdm_machine_stage_v1_execute` returns that value only when the generic
STOP observation was marked; a watchdog instead returns execution status `0`,
and cancellation uses its separate status.  Therefore the fixture's
one-instruction watchdog budget did not produce this terminal.

The copied instruction window is `C4 C4 50 3D`: DEM selector `50h`, service
`3Dh`.  OpenNT `dossvc.h` names it `SVC_DEMEXITVDM`; the authoritative
`demdisp.c` table maps the slot to `demExitVDM`; and
`demmisc.c::demExitVDM` displays its fatal `config.nt` diagnostic then calls
`TerminateVDM`, documenting that the VDM is killed rather than resumed.

The current selected route is
`bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch`: after the DEM misc plane
declines the historical product-bound terminal owner, service `3D` emits a
typed STOP with no guest resume.  The engine deliberately maps that generic
mechanical outcome to `CONTROLLED_GUEST_TERMINAL`; it does not parse the BOP.

## Interpretation

The bounded run has reached the original fatal startup/error path, not an
ordinary COMMAND return, not a missing `50:11` load-DOS operation, and not a
Bochs watchdog or lifecycle failure.  The current STOP is source-shaped and
honest for the unavailable historical `RcErrorDialogBox`/`TerminateVDM` host
product shell.  It is evidence of reachability only; it is not a successful
boot, a normal CLI completion, or evidence that any BOP package is closed.

No implementation follows from this trace.  Any later change must be a whole
one-shot startup/terminal package that proves the configuration-error cause
and gives `demExitVDM` an explicit engine-owned result contract.  It may not
make `50:3D` resume, invoke the historical process teardown, or turn the
observer into product behavior.

## Confidence And Follow-up

Confidence is high: the terminal status branch, copied bytes, provider STOP,
original selector/dispatcher/implementation, and source-built result agree.
The unproven fact is why this declared fixture takes its original fatal
startup path; that is a separate guest/startup-package question, not an
unimplemented service selected by the trace.
