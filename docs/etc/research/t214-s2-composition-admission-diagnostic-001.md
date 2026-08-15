# T214 S2 Composition Admission Diagnostic

## Question

Which owned installation stage rejects the declared source-built CLI profile
before guest CPU execution, and does that rejection leave a reusable process
state?

## Inputs

- Current `bx_ntvdm_composition_runtime_v1` and `bx_ntvdm_engine_run_v1`.
- The retained source-built `profile-v7.json` and its declared BYOB root from
  the T212 native composition input.
- MSVC x64 `/MT`, with the pinned no-device native-core configuration.

## Procedure

1. Build the current composition runtime, engine, cancellation controller and
   bound boot-namespace composition into the retained native CLI object
   closure.
2. Run `tools/Invoke-T214S2CompositionAdmissionDiagnostic.ps1` against that
   closure.
3. Run the focused engine fixture with a deliberately rejected copied
   installation result, then immediately run the existing cancellation and
   execution cases in the same process.

## Observations

The native CLI exits with its expected rejected-composition result and prints:

```text
ntdos64-native: terminal=2 detail=40 lifecycle=3 presentation=3 cancellation=0
```

`40` is `BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_NAMESPACE_INITIALIZE`.  Thus
profile parsing, image loading, launch-plan decoding and the selected
host-capability capture completed before the rejected
`bx_ntvdm_boot_namespace_composition_v1_initialize` call.  No CPU execution,
guest BOP handling, guest path or host handle is reached by this result.

The focused engine fixture supplies `-40` from the copied installation seam.
It observes the rejected-composition result, `detail_code == 40`, one reset,
and a deactivated cancellation controller before successfully continuing with
the fixture's later cancellation and execution cases.  This proves the engine
does not retain a rejected installation as active process composition.

## Interpretation and confidence

Confidence is high for the installation-stage attribution and reset contract:
both are source-built MSVC x64 `/MT` evidence.  This diagnostic deliberately
does not identify which sub-owner inside namespace initialization failed.
The next S maps its provider, DEM-session and COMMAND-session initialization
contracts before admitting a package-owned repair.  It must not use a native
trace to select a BOP implementation.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T214S2CompositionAdmissionDiagnostic.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t214-s2-composition-admission-diagnostic-r1 `
  -CompositionBuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s6-native-cli-cancellation-r2\composition

powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T212S5EngineCancellationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t214-s2-engine-reset-diagnostic-r1
```

## Follow-up

Map the namespace initializer's three ordered owners--boot namespace provider,
DEM package session and COMMAND package session--against the same profile.
Any repair remains a composition/namespace package repair; it is not BOP
implementation admission.
