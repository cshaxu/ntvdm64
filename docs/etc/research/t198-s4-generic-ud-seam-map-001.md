# T198 S4 Generic #UD Seam Map 001

## Result

The current `src/bx-core/cpu/exception.cc` seam is not yet usable with the
finite mantle runner.  It combines one legitimate selector-blind CPU mechanic
with historical runtime composition and adapter-owned transactions.  The next
slice must extract that composition out of `bx-core`; it must not reproduce it
in a new Bochs callback.

## Source Path

For a `#UD`, `BX_CPU_C::exception` currently builds copied CPU state and a
fixed instruction window, then calls `bx_ntvdm_try_intercept_ud`.  On an
accepted typed result it applies the register delta, sets `RIP`, and
`longjmp`s back to the original CPU loop.  The current helper and interface
live in `exception.cc` and `bx_ntvdm_exception_intercept.h` behind
`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT`.

| Current surface | Classification | T198 disposition |
| --- | --- | --- |
| `#UD` vector gate, copied CPU state/window, response validation, typed register delta, resume/`longjmp` | Generic CPU mechanics | Retain in `bx-core`, selector-blind. |
| `SIM->quit_sim(0)` for a typed stop | Product-lifecycle coupling | Replace with a mantle-owned finite-run stop outcome; do not call SIM product shutdown. |
| `bx_ntvdm_exception_interceptor_t` registration function pointer | Cross-component raw function-pointer ABI | Reject.  The revised seam has one fixed C ABI entry, not callback installation. |
| `getenv("NTDOS64_UD_TEST")` and `getenv("NTDOS64_ADAPTER_BRIDGE")` | Product/test runtime selection in CPU core | Remove from the core seam; test selection belongs only in fixture composition. |
| `bx_ntvdm_adapter_interceptor` and all v1/v3/v4 runtime dispatch | `bx-vdm` host-service composition | Move out of `bx-core`; preserve it as the adapter-side provider/transaction path. |
| gather/read/write/bulk/snapshot ordinary-RAM copies | Mixed adapter request semantics and generic RAM mechanics | Mantle owns checked RAM transfer only after a fixed bridge outcome; `bx-vdm` owns deciding the request. |
| startup transaction/deferred plan/machine-composition conditionals | Retained historical composition alternatives | Keep default-off and exclude from the finite T198 composition until individually re-admitted. |

## Required Revised Direction

```text
bx-core exception mechanics
  -> fixed copied #UD event / typed outcome C ABI
  -> bx-mantle event bridge (mechanical RAM transfer and controlled stop)
  -> bx-vdm ingress / global service plane
  -> original OpenNT provider, contained CLI backend, source-derived provider,
     or explicit unavailable/deferred disposition
```

The CPU core may know only an exception vector, copied scalar state, bounded
instruction bytes and a typed outcome.  It cannot decode a selector, name a
provider, access an environment variable, hold an adapter pointer, or copy an
adapter-owned payload.  `bx-mantle` supplies the concrete bridge symbol when
the finite run is composed.  The bridge returns a fixed disposition:
pass-through, checked resume, or controlled stop.  The latter maps to the
finite runner's stop state, not `SIM->quit_sim`.

## Build Consequence

The current retained derivatives rebuild `cpu/exception.o` with `/Iadapter`
and link it with product `main.o`, GUI/device archives and historical adapter
objects.  T198 rejects that closure.  The next implementation must instead
compile one core exception object against a small, fixed bridge declaration;
the finite-run probe then links its mantle bridge and no product `main.cc`,
GUI/device archive, environment selection or raw callback object.

## Follow-Up

T198 S5 may make the selector-blind core/mantle bridge extraction.  Its first
acceptance target is deliberately a typed pass-through or controlled-stop
fixture, not a BOP service.  Only once that bridge source-builds with the
finite runner can `bx-vdm`'s existing holistic ingress/catalog/plane be
attached on the adapter side.
