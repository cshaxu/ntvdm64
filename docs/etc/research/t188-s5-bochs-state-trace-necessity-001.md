# T188 S5 Bochs state-trace necessity 001

Date: 2026-08-12  
Packet: M0 T188 S5  
Disposition: admitted as BX-TRACE-060; implementation is a separate S6.

## Decision

The trace is warranted. T187's sole runtime observation proves generic adapter ingress returns pass-through; T188 S2 proves the same v5 `54:0C` bytes and register endpoint normally queue a multi-write; T188 S3 proves no existing non-consuming runtime observer exists; and T188 S4 adds the smallest copied lifecycle record. The record can distinguish the only remaining high-value states: not installed, no boot provider, or nonempty pending operation.

## Exact proposed surface

One local default-off macro in `src/bochs/cpu/exception.cc` is permitted only inside the existing `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` adapter interceptor. Immediately after the unchanged v4/v3/v2 chain has produced a valid `PASS_THROUGH` result, it calls `bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state` once and appends its four scalar fields to the already existing pass-through log. It does not read the instruction window, selector/service identity, guest memory, payload or private session. It has no conditional outcome and returns the same pass-through value.

This is an adapter-state observation, not BOP decoding. Bochs owns only the macro and formatting; the adapter owns record content and validity. The exception is registered as BX-TRACE-060 before any source patch.

## Fresh-run requirement

If S6 source/static checks close, it may rebuild only `cpu/exception.o` plus the final engine against the S4 adapter object closure and perform exactly one fresh watchdog-bounded v5 run. The controller must create a terminal outcome record even if tree cleanup fails. The old T187 run is immutable and will not be reused or repeated.
