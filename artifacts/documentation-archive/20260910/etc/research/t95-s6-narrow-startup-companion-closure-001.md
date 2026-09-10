# T95 S6 Narrow Startup Companion Closure 001

## Frozen candidate

The r6 shim is a usable ordering template only; it cannot be reused because
its copied sources predate `BX-ABI-036`. A future fresh root must derive one
new shim from it, preserving its Bochs archive/link order and replacing only
the adapter object list and root-local compile definitions.

## Required adapter objects

The r6 nine-object startup list remains required, except that its environment
endpoint is now used by the bridge rather than directly by `main`:

```text
adapter/bx_ntvdm_startup_session_environment.obj
adapter/bx_ntvdm_startup_session.obj
adapter/bx_ntvdm_startup_transaction_bridge.obj
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
cli/byob_profile.obj cli/byob_identity.obj cli/byob_image.obj
```

Each edge is direct from the bridge/session source. `adapter_runtime.obj`,
`host_session.obj`, all read/gather/observation/multi-write/secondary-service
objects, and every OpenNT/SoftPC object are rejected.

## Required Bochs rebuilds and macros

Only copied-root `main.o` and `cpu/exception.o` must be rebuilt with both:

```text
/DBX_NTVDM_ENABLE_EXECUTION_PLAN=1
/DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=1
```

`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` remains undefined (therefore `0`).
`memory/libmemory.a` is inherited because the registered generic ordinary-RAM
writer remains its owner. All other Bochs archives retain the r6 order; none
is evidence of a newly enabled device or service.

## Build predicate

Before one future invocation, the fresh root must prove: both macros occur
only on the two named copied-root compilation commands; the old intercept
macro is absent; all required source hashes match this worktree; and the link
list contains none of the rejected adapter objects. No build was run here.
