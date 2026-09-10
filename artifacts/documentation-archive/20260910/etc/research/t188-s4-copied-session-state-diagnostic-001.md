# T188 S4 copied session-state diagnostic 001

Date: 2026-08-12  
Packet: M0 T188 S4  
Disposition: adapter-owned fixed-width snapshot source/test closed; no Bochs or guest action.

## ABI

`bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state` copies a versioned record with exactly four `uint32_t` fields: version, installed, boot-namespace-provider presence and host-session pending kind. It exports no selector, service, guest address, payload, transaction, pointer or session layout. Its implementation only assigns copied scalar values; it does not dispatch, queue, take, clear or reset state.

## Verification

`Test-AdapterRuntimeDiagnosticStateBoundary.ps1` verifies both field shape and the absence of state-changing terms. The existing MinGW `bx-ntvdm-adapter-runtime-test` target was rebuilt and run in its `--t181-v5-install` mode. That focused test proves:

1. immediately after v5 installation: installed/provider are set and pending kind is `NONE`;
2. after the original single `54:0C` v2 dispatch with `DS:DX=8DC8:33D9`: installed/provider remain set and pending kind is `MULTI_WRITE`;
3. the normal matching take remains responsible for consuming the pending transaction.

The command exited zero. No Bochs source/object/engine, CLI executable, firmware bundle or guest process was built or run.

## Follow-up

This is an adapter-local diagnostic ABI, not a BOP provider. A later S may decide whether to register one default-off Bochs trace consumer that emits the copied record after the already-selected generic dispatch returns pass-through. That decision must first prove the record remains generic and that a single fresh observation has enough value to justify a Bochs exception intrusion.
