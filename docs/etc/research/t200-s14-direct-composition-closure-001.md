# T200 S14 — direct composition installation closure

## Question

Can the native-engine contract install and release the real current whole
composition from source-built BYOB resources, without restoring the retained
environment handoff or initializing a machine?

## Inputs

- S13 native-engine contract/entry and copied-input composition seam;
- `tools/build/t198-s50-bx-vdm-composition-manifest.json`;
- T198 S94's source-built v7 fixture preparer and its pinned v7 resource root;
- `tests/bx-mantle/bx_ntvdm_engine_direct_composition_v1_test.c`.

## Procedure

`tools/probe/Invoke-T200S14DirectCompositionProbe.ps1` creates a fresh build root,
prepares the exact source-built v7 profile, compiles the current whole
composition manifest in one x64 `/MT` environment, then links a fixture with:

- `bx_ntvdm_engine_contract_v1` and `bx_ntvdm_engine_run_v1`;
- the current composition objects;
- the existing generic mechanical exception bridge required by the baseline
  CPU exception object.

The fixture passes the prepared profile and retained source-built root through
the copied native-engine request.  It calls the entry twice.  Each call must
install then reset the real composition successfully; the second call is the
release witness.  The fixture has no machine initialization or CPU-loop call.

## Observations

The successful fresh run is:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T200S14DirectCompositionProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s14-direct-composition-r5
```

It completed with exit `0`.  Its profile is the prepared source-built v7
profile and its root is `artifacts/inputs/t194-v6-normal-return-root-r1`.
The generated record explicitly states `machineInitialized: false`,
`cpuLoopEntered: false`, and `guestExecution: false`.

During closure construction, the baseline exception object required the
already-existing `bx_ntvdm_vdm_generic_ud_bridge_v1.c`.  The manifest labels
that source compile-only for the S94 fixture, but it is an unresolved generic
mechanical bridge dependency when a different `main` is linked.  Adding the
same existing source resolved the link; no selector/service behavior was
added.

## Interpretation and confidence

High confidence: the new direct engine path can now bind and release the real
whole-package BOP composition from source-built inputs without process
environment mutation.  It also gives an exact source/object recipe for the
next machine-lifecycle package.  This remains a composition closure—not a
machine, guest, COMMAND, or CLI-exit success claim.

## Follow-up

T200 S15 must source-map the native engine's machine-lifecycle entry:
request-owned startup image/entry construction, mantle initialization/reset,
composition bind order, and terminal result propagation.  It must reject the
old full-product `main.cc` startup-plan hook as its implementation path.  No
guest run is admitted until the map has established a product-owned bounded
execution contract.
