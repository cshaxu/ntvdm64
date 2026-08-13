# T188 S6 Bochs copied-state trace closure 001

Date: 2026-08-12  
Packet: M0 T188 S6  
Disposition: BX-TRACE-060 source/static closure complete; no build or guest run.

## Change

`exception.cc` adds `BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC`, defaulting to zero. Only inside the already existing generic adapter interceptor's valid `PASS_THROUGH` branch, the enabled block copies one adapter-owned diagnostic record and logs installed/provider/pending scalar values. It runs before the pre-existing pass-through line and preserves the pre-existing `return 0`.

The block contains no instruction window, bytes, selector, service, guest, payload, queue/take, result, register, flags, RIP or response operation. No adapter production behavior changes; its API was already source/test closed by T188 S4.

## Verification

`Test-BochsAdapterStateTraceBoundary.ps1` passes and enforces the default-off guard, exact post-dispatch branch, required copied fields, prohibited semantic/state-changing terms and retained pass-through ordering. `Test-AdapterRuntimeDiagnosticStateBoundary.ps1` also passes. No Bochs object, engine, CLI or guest run occurred.

## Follow-up

The next S must audit the retained engine's compiler/CRT/object closure. It may build only after identifying whether the S4 adapter runtime object can be replaced compatibly alongside `cpu/exception.o`; an arbitrary adapter rebuild, archive rebuild or full Bochs build is prohibited.
