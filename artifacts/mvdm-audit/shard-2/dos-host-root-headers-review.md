# Shard 2: DOS host BOP, reset and thread declarations

Three pending host headers match both original trees. The observer and JSON record normalized Ninja dependencies and the selected worker graph/map snapshot. All three are **partial** candidates; the single object and sample symbols do not cover their entire contracts.

| Entry | Evidence and bounded limitation |
| --- | --- |
| `softpc.new/host/inc/host_bop.h` | Exact normalized dependency of selected `nt_bop.obj`; it declares host BOP-related constants. The captured map includes `MS_bop_2` and `MS_bop_4`. Actual BOP routing, guest argument validation and error propagation are separate. |
| `softpc.new/host/inc/nt_reset.h` | Exact normalized dependency of `nt_bop.obj`; the header declares host/reset and lifecycle-related state. `nt_reset.obj` is selected and map contains `host_reset`. This does not prove reset ordering, session ownership, or cleanup. |
| `softpc.new/host/inc/nt_thred.h` | Exact normalized dependency of `nt_bop.obj`; it defines host thread data structures. A dependency does not prove thread ownership, stop ordering or failure recovery. |

The current BOP implementation is not asserted identical to original: it has project adaptations whose registered exceptions and behavior require their own audit. Source declarations and map selection do not prove that a particular guest invoked a BOP or that the corresponding worker/server lifecycle is correct. Normalized dependency results apply to the captured `nt_bop.obj` configuration only; additional consumers and configurations can exist.

Replay `python artifacts/mvdm-audit/shard-2/dos-host-root-headers-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare its JSON. Graph/map hashes are snapshots. No guest, product, installation, shared ledger or build tree was modified.
