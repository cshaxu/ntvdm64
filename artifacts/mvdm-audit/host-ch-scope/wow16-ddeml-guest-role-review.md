# Seven DDEML guest implementation files: host-scope role

The seven paths below are `.c` implementations, not shared ABI headers. The original `wow16/ddeml/makefile:63-64` compiles them into its 16-bit DDEML object set using the 16-bit compiler options at line 61; `ddeml.def:19,28,49` exports DDE initialization, transaction and the internal manager window procedure. This establishes a historical guest build role, not the identity or behavior of any installed DLL.

| Exact file | Original guest role |
| --- | --- |
| `wow16/ddeml/dmgdb.c` | DDE manager application/data lookup. |
| `wow16/ddeml/dmgdde.c` | DDE structure and protocol operations. |
| `wow16/ddeml/dmghsz.c` | DDE string-handle management. |
| `wow16/ddeml/dmgmem.c` | DDE manager memory. |
| `wow16/ddeml/dmgmon.c` | DDE monitoring. |
| `wow16/ddeml/dmgq.c` | DDE manager queue. |
| `wow16/ddeml/dmgwndp.c` | DDE manager window procedures. |

The [read-only observer](wow16-ddeml-guest-role-observe.py) ran successfully on the current snapshot. Its [receipt](wow16-ddeml-guest-role-evidence.json) records each file's SHA-256 matching both retained original source trees, hashes all 34 discovered generated Ninja graphs under `build/.ninja`, and finds no exact source edge for any of the seven. It also finds no literal inclusion of these `.c` basenames in current `src` C/H outside `wow16`. These are bounded source/graph observations, not proof about ungenerated configurations, preprocessor-generated paths or binary consumers.

Recommendation: classify each as **guest-only implementation in the inspected host build**, outside host C/H implementation coverage; retain source and the guest DDEML feature in S14. Do not apply this conclusion to `ddemlp.h`, public argument structures, or other potentially shared headers. The existing [DDEML route report](ddeml-route-review.md) documents historical immutable guest init/reinit/uninit witnesses, with no transaction, callback or arbitrary current-guest identity proof. Real guest DLL identity, cross-task transaction, callback/reentry, failure and retirement remain explicit work. If the ledger cannot express host-build scope without implying guest feature exclusion, keep pending. No guest image was altered or compiled; no product or shared ledger was modified.
