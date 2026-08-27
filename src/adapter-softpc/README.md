# adapter-softpc

Same-shaped reached SoftPC/CCPU/SAS facade. It may use typed `adapter-bochs`
operations but never a Bochs type or global. Existing project code is recovery
evidence until audited.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-SOFTPC-001 | Define the historical SoftPC host error-table `ERROR_STRUCT`, `EH_*`, and `EV_*` declarations consumed by host bodies. | The byte-exact `softpc.new/host/inc/error.h` documents but does not declare these forms; the matching declarations survive in the byte-exact `softpc.new/base/inc/error.h`. | A build-island-only force-included same-shaped declaration bridge; no original mirror header or body is edited. | `include/error_abi.h` |
| ADAPTER-SOFTPC-002 | Preserve an original x86 MVDM host-pointer identity in a 32-bit DTA/FCB field. | `demsrch.c` directly stores `PFFINDLIST`; that pointer is neither width-safe on x64 nor permitted in guest/MVDM state. No original call-shaped identity API exists. | `mvdm_host_identity_*` resolves only through the thread-bound session's existing `host_resource` mapping instance. It publishes, resolves, reverse-looks-up and releases opaque surrogate32 values, retains `FFindId` as numeric source data, and never acquires guest memory. | `include/mvdm_host_identity.h`, `mvdm_host_identity.c` |
