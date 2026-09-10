# T129 S1 bridge rejection and runtime handoff 001

## Question

Can the retained CPU-result bridge be activated by the old paired startup
transaction ingress, and what is the next reached boundary if the current
engine runtime is instead opted in through the fixed CLI shim?

## Inputs and procedure

- T127's derivative was copied and only
  `BX_NTVDM_ENABLE_STARTUP_TRANSACTION` was changed from `0` to `1`.
- The resulting link diagnostics were retained below
  `artifacts/analysis/m0/` and were compared with the current v4 adapter
  closure.
- Separately, the fixed T124 shim set `NTDOS64_ADAPTER_BRIDGE=1` only for its
  native Bochs child. Its parser test verifies the fixed value and still
  verifies the fixed `-q -f bochsrc` invocation.
- The unchanged, manifest-verified T121 bundle was run under the existing
  bounded process-tree watchdog. The observation is retained at
  `artifacts/analysis/t130-s1-shim-runtime-optin-001/`.

## Observations

The paired build has seven unresolved imports:
`bx_ntvdm_cpu_result_bridge_v1_dispatch`, its guest-read and completion
variants, and the four `bx_ntvdm_startup_transaction_bridge_v1_*` imports.
They originate in the obsolete parallel bridge sources, not in the current
adapter runtime already linked by T121. Adding their objects would duplicate
session/namespace/register ownership, so no image was linked.

With the fixed child opt-in, the unchanged engine committed the NTDOS write at
`0x8b00` (`0x6cd2` bytes) and three pre-existing multi-write transactions.
It then repeatedly observed `C4 C4 02` at `073b:03ff`; the adapter correctly
passed it through because it is not an adapter host service. The watchdog
stopped the owned process tree. No continuous execution is claimed.

## Interpretation and follow-up

The useful composition is the current runtime already present in T121, not
the old bridge. The new reachable boundary is original SoftPC
`unexpected_int`, whose handler island was already linked by T119/T121. Its
boolean seam result hides whether it declined or failed mechanics. T130 may
make that result observable while retaining its selector-blind Bochs seam and
the unchanged original handler.
