# wow family

This family preserves the historical WOW32/WOWEXEC host-product boundary. No
WOW provider, broker, GUI, WOWEXEC or generic Win16 API is selected in the
current CCPU40 product path.

The prior copied callback transaction relied on retired machine-frame recovery
code. It is preserved solely as evidence under
`docs/etc/legacy_code/retired-bochs/` and is not a production source or build
input.

## Registered bindings

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-WOW-001 | `wow32/wcall16.c` obtains temporary `VDMFRAME` and `CBVDMFRAME` aliases with `GETFRAMEPTR`, flushes a written callback frame, then frees both aliases before calling `host_simulate`. | The NT4 `FlatAddress` form exposes process pointers; a native pointer cannot cross the current session boundary or the recursive CPU call. The original `vpStack` is a packed 16:16 address, not a SAS-linear address. | `wow_callback_frame_lease` first calls the selected CCPU40 `mvdm_softpc_effective_address` contract, then exposes only a bounded read or write view using the existing session guest-memory mapping instance. The original source retains task/frame algorithm, stack transfer, CPU resume, result and failure semantics; a future mirror hook will acquire, flush/commit and release at the original points. | `include/wow_callback_frame_lease.h`; `wow_callback_frame_lease.c`; original target `../../mvdm-host/wow32/wcall16.c` |
