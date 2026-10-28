# Current CCPU386 pending C/H physical-role review

Read-only snapshot: the current four shard decision fields intersected with `host-ch-scope/inventory.csv` host-candidate paths gives 21 pending `.c`/`.h` entries in `softpc.new/base/ccpu386`. This is a classification recommendation, not a ledger edit, build, test or change to guest. A physical-role exclusion below is limited to the observed CPU40/SPC386/non-PIG product; it neither removes the source nor excludes any corresponding capability or alternate supported profile. If ledger exclusion is global, retain those rows pending.

| Physical file | Current role and disposition | Separate capability / acceptance |
| --- | --- | --- |
| `bswap.c` | **Pending**: compiled original object has no 486 body under SPC386. | S11 establish 486 provider and width/fault semantics. |
| `bswap.h` | **Pending**: included by otherwise gated own object; decoder include requires CPU_486. | Same S11 group; include is not instruction execution. |
| `cmpxchg.c` | **Pending**: own object lacks SPC486 body and selected decoder returns Int6. | S11 test 8/16/32 widths, writes, flags, lock and faults. |
| `cmpxchg.h` | **Pending**: typed widths in own object context; decoder gated by CPU_486. | Same S11 group; actual call ABI not evidenced. |
| `invd.c` | **Pending**: gated 486 provider, current object no implementation. | S11 cache model, privilege/fault order; original no-cache body is no modern hardware equivalence. |
| `invd.h` | **Pending**: own object includes declaration, body unavailable. | Same S11 group. |
| `invlpg.c` | **Pending**: SPC486 implementation not enabled in current object. | S11 paging/TLB invalidation, privilege and fault cases. |
| `invlpg.h` | **Pending**: own object includes declaration but decoder branch disabled. | Same S11 group. |
| `wbinvd.c` | **Pending**: own object has no SPC486 body; original empty body depends on its cache model. | S11 cache and privilege model. |
| `wbinvd.h` | **Pending**: declaration included in gated object, not an active function. | Same S11 group. |
| `xadd.c` | **Pending**: object lacks gated 486 provider; current decoder Int6. | S11 arithmetic, aliasing, flags, commit/fault ordering. |
| `xadd.h` | **Pending**: own object includes declaration; no active 486 call. | Same S11 group. |
| `c_getset.c` | **Excluded candidate for physical PIG-only branch in observed non-PIG product**: original state comparison guarded by PIG. | S17 determine whether independent original PIG diagnostics are applicable; S11 CPU state remains in scope. |
| `ccpupig.c` | **Excluded candidate for physical PIG-only branch in observed non-PIG product**: original comparison/history implementation guarded by PIG. | S17 owns historical differential comparison profile; do not conflate with CPU execution. |
| `ccpupig.h` | **Excluded candidate for physical PIG-only declarations in observed non-PIG product**: six valid includes but declarations are PIG-gated and emit no tokens in recorded profile. | Same PIG obligation; dependency itself is not execution. |
| `evid_c.h` | **Pending**: CCPU generated video schema is not selected by recorded dependencies; differs from active C-VID same-name variant. | S11/S13 reconcile vector ABI, generated profile and video state. |
| `evidfunc.h` | **Pending**: CCPU generated video names differ from selected C-VID variant (e.g. SimpleByteWrite S_2451 versus S_2139). | S13 verify generated provider mapping; same basename is not ABI equivalence. |
| `gdpvar.h` | **Pending**: selected `sasCdef.obj` includes this exact variant; offsets differ from other generated schemas, actual macro expansion unknown. | S11/S13 prove SAS/GDP state and ownership; do not infer an offset defect from text alone. |
| `pigger_c.h` | **Pending**: selected `c_reg.obj` includes a single `Pig=1` macro; no evidence of its expansion, uppercase PIG switch is distinct. | S11/S17 establish actual generated-state use and PIG profile. |
| `univer_c.h` | **Pending**: selected `sasCdef.obj` includes sentinel-only variant; actual expansion unproved, C-VID schema differs. | S11/S13 trace SAS/generated contract, do not merge variants. |
| `ntstubs.c` | **Pending**: alternate CPU/FPU/SAS provider omitted by selected generator due to conflicts with real providers; exhaustive symbol-by-symbol equivalence not established. | S11 establish caller/provider coverage and failure semantics, preserve related CPU/FPU/SAS functionality. |

Sources: [CPU 486 profile](../shard-3/cpu486-profile-boundary.md), [remaining CCPU declarations](../ccpu-remaining-header-source-review.md), [remaining CCPU source](../ccpu-remaining-c-source-review.md), [PIG scope](../cross-369/shard3-exclusion-scope-recheck.md), [PIG header](ccpupig-header-reopen-review.md), and [actual dependency snapshot](ccpu-cvidc-pending-header-review.md). The 486 source rows remain pending rather than being declared fully or partially restored: present object edges and original identity alone prove neither decoder execution nor tested behavior. No `full` or behavior-backed `partial` recommendation is supported by this evidence. The original 486 macro provenance and PIG applicability are still bounded, explicit owner-group decisions.
