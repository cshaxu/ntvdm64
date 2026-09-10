# T126 S1 DEM `50:11` result-bridge audit 001

## Question

Does T125's repeated `C4 C4 50 11` identify a missing DEM/BOP behavior, or a
missing composition edge in the retained native engine?

## Source and trace facts

| Layer | Fact | Evidence |
| --- | --- | --- |
| Original OpenNT | `BIOS[50h]` selects `MS_bop_0`; it reads the service byte, calls `DemDispatch`, then advances IP by one. | `src/opennt/overlay/base/mvdm/softpc.new/host/src/nt_bop.c` lines 145–161. |
| Original DEM | Service `0x11` is `demLoadDos`, whose historical destination is `DI:0000`. | `docs/etc/research/first-owner-transaction-closure-audit.md`; `demmisc.c`. |
| Adapter | `host_service_plane` recognizes only this first transaction, computes physical `DI << 4`, validates immutable NTDOS bytes, and host-session prepares resume at fault RIP plus four bytes. | `src/bx-ntvdm-adapter/bx_ntvdm_host_service_plane.c`; `bx_ntvdm_host_session.c`. |
| T125 trace | Every observed invocation has `50:11`, `CS:EIP=0070:0475`, `DI=08b0`; no `transaction committed` line appears. | `artifacts/analysis/t125-s1-fixed-bundle-preflight-001/stderr.txt`. |
| T121 link | The map contains both `bx_ntvdm_host_service_plane_v1_prepare` and `bx_ntvdm_host_session_v1_take_pending_write`. | `ntdos64-t98-current-adapter.map`. |
| T121 build selection | The retained exception build sets `BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0`, while retaining observation/deferred/machine-composition flags. | `ntdos64-t98-current-adapter.mak`, `cpu\\exception.o` rule. |

## Interpretation

The loop is not evidence for a new per-BOP handler: the complete source-derived
`demLoadDos` transaction already exists in the linked adapter closure. It is
evidence that the retained T121 observation image intentionally omitted the
generic, selector-blind CPU result bridge. That bridge is the only component
that can take an opaque ready transaction, copy it to ordinary RAM, and apply
the prepared RIP+4 result. With it compiled out, the generic BOP listener logs
the `#UD` and original exception processing returns to the same instruction.

## Bounded next action

T127 may make one derivative from the identical T121 build vector with only
`BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=1`. It must rebuild exactly the existing
`main.o` and `cpu/exception.o`, reuse the declared adapter objects, and prove
the map. It may not add a BOP branch, a provider, a device, a source patch, or
a runtime result claim.
