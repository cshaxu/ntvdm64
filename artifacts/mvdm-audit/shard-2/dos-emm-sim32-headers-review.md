# Shard 2: expanded-memory and Sim32 host headers

Both pending files match both original trees. The companion observer captures their exact normalized Ninja dependencies, selected worker compile edges and representative map entries. These are **partial** file-role candidates, not complete memory-subsystem verification.

| Entry | Evidence and bounded limitation |
| --- | --- |
| `softpc.new/host/inc/host_emm.h` | Exact dependency of selected `emm_mngr.obj`; source includes the host expanded-memory interface and the captured worker map names `host_allocate_storage` in the separate `nt_emm.obj`. This does not establish individual allocation, mapping, host-pointer reuse or cleanup calls at runtime. |
| `softpc.new/host/inc/sim32.h` | Exact dependency of selected `sim32.obj`; the captured map names `Sim32FlushVDMPointer`. The original header distinguishes `MONITOR` and non-MONITOR branches; the existing `../shard-3/monitor-legacy-interfaces-role-review.md` discusses that profile boundary. Selection alone does not prove guest-pointer validation, flush ordering or lifecycle. |

The observer resolves dependencies from the worker build directory rather than comparing relative strings. Source/header identity does not imply every implementation is unmodified. The [EMS owner audit](../shard-0/ems-owner-audit.md) records a real isolated guest map/alias/move/exchange witness against an earlier package, not the latest installed image. The [EMS lifecycle review](ems-alias-memory-lifecycle-review.md) identifies additional failure and alternate-profile boundaries. Before claiming complete recovery, verify current-image mapping, callback/lease validity, reset and worker/task teardown, including failed allocations and stale handles. The existing XMS/suballocator review must not be conflated with EMS ownership.

Replay `python artifacts/mvdm-audit/shard-2/dos-emm-sim32-headers-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare its JSON. Graph/map hashes are snapshots. No guest, product, installation, shared ledger or build tree was modified.
