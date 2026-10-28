# Shard 2: DOS video host and alternate-profile headers

Three pending declarations match both original trees. The observer/JSON record normalized Ninja dependencies, the selected EGA object and representative map symbols; no video runtime test or build was performed.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `softpc.new/host/inc/nt_ega.h` | Exact normalized dependency of selected `nt_ega.obj`; the captured worker map names EGA-related symbols. Candidate **partial** for selected declaration only. Several sampled map names share an address, and a source path explicitly reports an unsupported operation; mapped symbols must not be counted as evidence of working rendering. |
| `softpc.new/base/inc/gisp_sas.h` | Original `reset.c` includes it only under `GISP_SVGA`; that macro is not directly present in the captured C flags, and this header is absent from the normalized `reset.obj` dependency list. Keep **pending** for alternate-profile applicability and any other consumers. This does not characterize current V7VGA behavior. |
| `softpc.new/host/inc/hostgrph.h` | Declares terminal/host graphics state, but an exact selected consumer was not established. Keep **pending**, not excluded from a bounded source search alone. |

The related `nt_ega.c` and project video pipeline can contain adaptations separate from these unchanged headers. Actual video-mode selection, pixel/dirty updates, mouse integration, callbacks, reset and unsupported mode behavior require independent runtime evidence. A source include or map name cannot prove a particular function is reached. The original GISP branch is not the same claim as current Video7 support.

Replay `python artifacts/mvdm-audit/shard-2/dos-video-profile-headers-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare the JSON. No guest, product, installation, shared ledger or build tree was modified.
