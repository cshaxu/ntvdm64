# T200 S13 — native engine lifecycle skeleton

## Question

Can a native-engine entry validate its copied request, invoke the composition
owner without restoring the environment protocol, always reset that owner, and
return a non-CLI setup category—without initializing a machine?

## Inputs

- S12 native-engine contract;
- `src/bx-mantle/bx_ntvdm_engine_run_v1.c`;
- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.[ch]`;
- `src/bx-vdm/bx_ntvdm_host_drive_policy.[ch]`;
- `tests/bx-mantle/bx_ntvdm_engine_run_v1_test.c`.

## Procedure

1. Refactor the host-drive snapshot owner to expose a direct copied-mask
   capture function; retain the environment parser as a compatibility wrapper.
2. Add a `bx-vdm` composition installation seam accepting borrowed copied
   UTF-16 descriptors and masks for the duration of its call.  It copies and
   validates these values before resource installation; it retains no caller
   pointer.
3. Implement `bx_ntvdm_engine_run_v1`: invalid contract → rejected-request;
   attempted composition installation → unconditional reset; success → no
   terminal category; any installation failure → rejected-composition with a
   diagnostic detail.
4. Build/run the engine entry with a test substitute for the composition seam,
   then separately compile the real composition-runtime translation unit.

## Observations

`tools/Invoke-T200S13NativeEngineLifecycleProbe.ps1` passed at
`artifacts/build/t200-s13-native-engine-lifecycle-r4/` using MSVC x64 `/MT`.
Its test proves three cases: malformed request does not call installation or
reset; both negative and absent composition outcomes call reset exactly once;
successful setup also calls reset exactly once and returns terminal kind
`NONE`.

The real `bx_ntvdm_composition_runtime_v1.c` also compiled cleanly as an MSVC
x64 `/MT` object at
`artifacts/build/t200-s13-composition-object-r5/composition-runtime.obj`.
During that check, the public composition header's missing `<stdint.h>`
self-include was found and repaired.

No test closure linked `bx-core`, `bochs.h`, a machine lifecycle object, or
guest bytes.  The test substitute is intentional: it verifies the engine's
reset and category contract without claiming a full host-capability or guest
run.

## Interpretation and confidence

High confidence: the future native engine now has a concrete lifecycle entry
whose dependencies run in the required direction—mantle calls a `bx-vdm`
input seam; `bx-vdm` receives only borrowed copied scalars/descriptors and
owns its own installed resources.  The legacy environment installer remains
available only for the retained product shell; the new entry does not write
environment variables.

This is still setup/teardown only.  `NONE` means “composition setup completed
and was cleaned up by this skeletal call”; it is explicitly not guest success
or a CLI process exit.

## Follow-up

T200 S14 should construct one valid, source-built profile/resource set and
link the direct composition seam through its real owner, proving direct
install/reset without a machine.  Only after that closure may a later S admit
minimal-machine initialization around the same entry.
