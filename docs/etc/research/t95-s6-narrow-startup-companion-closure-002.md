# T95 S6 Narrow Startup Companion Closure 002

## Question

What is the smallest correction to the frozen startup-transaction companion
after the sole r1 link attempt reported two unresolved fixed-width ABI helper
symbols?

## Inputs

- The r1 terminal build record:
  `t95-s6-narrow-startup-transaction-build-attempt-001.md`.
- The prior frozen candidate:
  `t95-s6-narrow-startup-companion-closure-001.md`.
- `src/bx-ntvdm-adapter/bx_ntvdm_exception_abi.c` and
  `src/bx-ntvdm-adapter/bx_ntvdm_cpu_delta_abi.c`.
- `src/bochs/cpu/exception.cc` and
  `src/bx-ntvdm-adapter/bx_ntvdm_cpu_result_v2.c`.

## Procedure

The two unresolved names in the retained r1 log were traced to their defining
translation units and direct consumers. Their headers and C records were
reviewed for pointer/object crossing and service semantics. No compiler,
generator, archive, link, or runtime command was used for this correction.

## Observations

`bx_ntvdm_exception_abi.c` initializes its pass-through record with
`bx_ntvdm_cpu_delta_v1_initialize`, defined only in
`bx_ntvdm_cpu_delta_abi.c`. `bx_ntvdm_cpu_result_v2_resume`, used by the
narrow generic Bochs result path, is defined only in
`bx_ntvdm_cpu_result_v2.c`; that source in turn calls the same delta helper.

Both sources use only C11 fixed-width record initialization/validation. Their
headers carry no Bochs C++ type, host pointer, guest pointer, BOP selector,
DEM service, DOS operation, filesystem path, device, or configuration policy.

## Corrected object list

Closure 001 is superseded only for the two missing entries below. The corrected
list has 18 adapter/CLI objects:

```text
adapter/bx_ntvdm_startup_session_environment.obj
adapter/bx_ntvdm_startup_session.obj
adapter/bx_ntvdm_startup_transaction_bridge.obj
adapter/bx_ntvdm_cpu_delta_abi.obj
adapter/bx_ntvdm_cpu_result_v2.obj
adapter/bx_ntvdm_exception_abi.obj
adapter/bx_ntvdm_instruction_window_abi.obj
adapter/bx_ntvdm_cpu_state_abi.obj
adapter/bx_ntvdm_guest_range.obj
adapter/bx_ntvdm_guest_write_abi.obj
adapter/bx_ntvdm_session_resource_abi.obj
adapter/bx_ntvdm_host_service_catalog.obj
adapter/bx_ntvdm_host_transaction_abi.obj
adapter/bx_ntvdm_host_service_plane.obj
adapter/bx_ntvdm_startup_plan_abi.obj
cli/byob_profile.obj
cli/byob_identity.obj
cli/byob_image.obj
```

All previously rejected objects remain rejected, including
`adapter_runtime.obj`, `host_session.obj`, read/gather/observation/multi-write
families, and every OpenNT/SoftPC object.

## Interpretation

This is an exact linker-proven closure correction, not a new feature. The two
providers complete the selected result ABI already crossing the narrow
mechanical boundary. They do not widen either the adapter host-service plane or
the Bochs intrusion.

## Follow-up

The generator can now materialize a new r2 root with these 18 objects. That
generation, and any one r2 build invocation, remain separately admitted.
