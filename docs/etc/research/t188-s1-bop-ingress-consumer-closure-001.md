# T188 S1 BOP ingress consumer closure 001

Date: 2026-08-12  
Packet: M0 T188 S1  
Disposition: the generic ingress already exists and is active; its adapter-plane disposition is the next boundary.

## Question

Which current #UD consumer received the T187 `54:0C` event, and does it have a generic, source-shaped path to the current host-service plane?

## Source closure

The listener line is not a terminal dispatcher. It runs first when enabled and only records a copied CPU/window snapshot. The T187 S3 makefile enables `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1`, and the CLI engine unconditionally sets `NTDOS64_ADAPTER_BRIDGE=1` for the fixed engine profile. In `exception.cc`, that selects `bx_ntvdm_adapter_interceptor` through `bx_ntvdm_try_intercept_ud`.

The selected generic flow is:

```text
Bochs #UD -> copied request/window -> adapter_interceptor
  -> runtime_v4 (gather-read) -> runtime_v3 (single-read) -> runtime_v2
  -> runtime_v1 / COMMAND, DEM and provider plane selection
  -> checked copied guest I/O, fixed-width result, checked CPU resume
```

The S4 `adapter boundary passed through` text is emitted inside that selected interceptor only after the v4/v3/v2 chain leaves a valid `PASS_THROUGH` result. It is direct proof that the BOP entered the adapter boundary; it is not proof that the generic ingress was absent.

## Candidate disposition

| Consumer | Disposition | Reason |
| --- | --- | --- |
| passive catalog listener | observation only | It has no response/result operation by design. |
| startup transaction | rejected for current v5 | Disabled; a separate legacy bridge with its own install/session and single-write contract, not the current v4 plane. |
| CPU-result bridge | rejected for current v5 | Nested under disabled startup transaction and covers only an older partial service set. |
| deferred startup plan | rejected | It is a pre-loop entry/payload consumer, not a BOP service dispatcher. |
| generic exception interceptor | selected | Enabled, selector-blind in Bochs, CLI-opted-in, and owns the already observed pass-through result path. |
| machine composition | rejected | Native machine BOP/port mechanics owner; it must not select host-service semantics. |

The selected Bochs seam knows no selector or service value. Selector/service classification remains in adapter runtime and its plane/provider modules. The current physical write and CPU delta application also remain Bochs mechanics, so no new cross-island ABI is required.

## Verification limitation

The existing deferred-plan and passive-listener boundary scripts pass. Two retained tests are stale against the current multi-consumer source layout: `Test-BochsUdDefaultOffBoundary.ps1` wrongly treats the deferred-plan include of `bx_ntvdm_adapter_runtime.h` as an escape from the generic-interceptor guard, and `Test-BochsStartupTransactionBoundary.ps1` requires an obsolete `prepare_from_environment` token in Bochs source/main. Their failures are evidence, not passing checks; S2 must not repair either test because it audits the adapter-plane predicate without changing Bochs source or its test surface.

## Correction and next boundary

T187 correctly records that the disabled startup transaction cannot consume the event, but its statement that the BOP therefore could not route into the host-service plane is too broad. The enabled generic interceptor does route it and returns pass-through. This record supersedes that narrow inference without altering the immutable T187 observation.

The next question is now internal to the existing adapter plane: which unchanged v2 `54:0C/0D` predicate returns false (`provider`, COMMAND gate, boot-file preparation, or pending multi-write queue), and can it be isolated in a no-runtime, no-reordering fixture. No new Bochs seam, macro change, source build or guest run is justified by S1.
