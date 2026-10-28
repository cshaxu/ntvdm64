# Shard 2: XMS dispatch and suballocator roles

Four pending files match both original trees. The observer/JSON capture current source, compile/archive edges, worker map symbols and a normalized `suballoc.obj` dependency snapshot. No runtime test, build or guest change was performed here.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `xms.486/xmsdisp.c` | Current graph compiles `xmsdisp.obj` into the XMS library; worker map attributes `XMSDispatch` to it. Original/current service table selects `xmsAllocBlock` among other entries. Candidate **partial**: linked dispatch is not proof of each guest service. The shown source guards the index check with `#if DBG`; reachability and effective bounds on a release guest request are not established by this review. |
| `suballoc/suballoc.c` | Current graph compiles the object into the host suballocator library; map attributes `SAAllocate` and `SAInitialize` to it. Candidate **partial**: the existing `suballoc-xms-callback-failure-review.md` retains commit/decommit failure and rollback questions, and prior fixture results are not real host fault injection. |
| `suballoc/suballoc.txt` | Original/current text resembles source material but its exact consumer and relationship to the compiled `.c` have not been established. Remains pending. |
| `suballoc/foo.bar` | Original/current content describes a private `suballcp.h`, but its exact role and consumer have not been established and it differs in hash from `suballcp.h`. Remains pending. |

The normalized Ninja dependency query covers only `obj/suballoc/suballoc.obj`; source inputs may be represented in the build edge rather than the deps log. Neither absence from that single dependency list nor a filename suggests whole-graph exclusion. Prior `../shard-3/xms-service-role-review.md` traces guest HIMEM service codes to the host dispatcher; `../shard-0/xms-block-zero-boundary.md` records the zero-size request and unresolved exact semantics. These observations do not establish all guest identity, failed allocation cleanup, bounds, reentry or task teardown.

Replay `python artifacts/mvdm-audit/shard-2/xms-suballoc-roles-observer.py` and compare its JSON. Graph/map hashes are a snapshot and may change after rebuilding. No guest, product, installation, shared ledger or build tree was modified.
