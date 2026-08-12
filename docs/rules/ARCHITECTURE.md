# Architecture Rules

1. Bochs 2.6 owns guest CPU, RAM/ROM, firmware and PC-device mechanics; guest OpenNT owns DOS/WOW/COMMAND behavior; the adapter host-service plane owns the source-derived replacement of unavailable historical BOP/DEM host composition. Neither Bochs nor guest code may absorb the other's responsibility.
2. The Bochs/OpenNT bridge is the only integration point. It uses versioned fixed-width records, validated guest-memory ranges and explicit stop/result dispositions; no host pointer, C++ object or cross-architecture function pointer crosses it.
3. A local OpenNT recovery requires a reached OpenNT caller, owner analysis, data-layout and failure-behavior evidence, and a bounded fixture.
4. An OpenNT overlay or host seam cannot become a CPU, firmware, device, DOS-kernel/filesystem or WOW implementation. The adapter may dispatch its declared source-derived host-service catalogue, but must not become a Bochs CPU/device substitute or an unbounded DOS product.
5. `ntvdm64` and other comparison repositories remain read-only evidence. Bochs 2.6 is the sole approved third-party internal-research runtime backend once its pinned import record passes T95.
6. Default product targets cannot depend on unreviewed artifacts, fixtures, or an unpinned third-party source snapshot.
7. Bochs is deny-by-default. Each enabled feature needs a reached first-profile OpenNT caller, named Bochs owner, boundary rationale, explicit admission record, and focused negative test. Upstream availability is not a reason to enable it.
8. Preserve original OpenNT service ownership, calling conventions, layouts, dispatch order, and failure behavior. Bochs and the adapter may carry a documented contract but may not recreate that behavior.
