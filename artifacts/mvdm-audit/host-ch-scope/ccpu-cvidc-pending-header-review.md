# CCPU and C-VID pending header dependency audit

This is a read-only snapshot of the four shard **actual decision** fields, not the historical `reporting_class` or a ledger mutation. On this run, 55 `.h` paths under `softpc.new/base/ccpu386` and `softpc.new/base/cvidc` are pending. The [observer](ccpu-cvidc-pending-header-observer.py) records each path and shard, all input SHA-256 hashes, selected x86 worker graph and Ninja dependency output hashes, and all exact valid dependency objects in [JSON](ccpu-cvidc-pending-header-evidence.json). No guest, product, or cached object is built or modified.

The current dependency query found no stale blocks and ten matching CCPU paths; it found no pending C-VID exact-path match. Hits:

| Header | Valid selected object(s) | Decision boundary |
| --- | --- | --- |
| `bswap.h` | `obj/ccpu/bswap.obj` | 486 function body and selected decoder are disabled. |
| `cmpxchg.h` | `obj/ccpu/cmpxchg.obj` | Same 486 gate; widths, locking, fault order unproved. |
| `invd.h` | `obj/ccpu/invd.obj` | 486 gate; original cache-model boundary remains. |
| `invlpg.h` | `obj/ccpu/invlpg.obj` | 486 gate; paging, privilege and TLB behavior remain. |
| `wbinvd.h` | `obj/ccpu/wbinvd.obj` | 486 gate; no full hardware-cache claim. |
| `xadd.h` | `obj/ccpu/xadd.obj` | 486 gate; two-result/flags/fault behavior remains. |
| `ccpupig.h` | `c_addr`, `c_intr`, `c_tlb`, `c_stack`, `c_xcptn`, `c_reg` CCPU objects | Declarations are under inactive PIG; inclusion does not prove debug behavior. |
| `gdpvar.h` | `obj/ccpu/sascdef.obj` | Generated layout differs from other variants; macro expansion and ownership are unproved. |
| `pigger_c.h` | `obj/ccpu/c_reg.obj` | `Pig=1` is not the uppercase PIG build switch; actual expansion is unproved. |
| `univer_c.h` | `obj/ccpu/sascdef.obj` | The sentinel alone does not establish an active generated-schema consumer. |

The six instruction headers' corresponding objects have no active 486 implementation in the observed SPC386 profile; original opcode paths return Int6. `CPU_486` guards decoder includes while `SPC486` guards bodies and calls. See [486 profile boundary](../shard-3/cpu486-profile-boundary.md) and [CCPU header review](../ccpu-remaining-header-source-review.md). A dependency on the physical header therefore does not establish these instructions' working behavior. The PIG and generated variants similarly require actual emitted/preprocessed consumers and semantics. C-VID generated/vector object fixtures do not show that these specific pending headers were consumed; see [C-VID header family](../shard-1/cvidc-header-family-review.md), [batch](../shard-2/cvidc-header-batch-review.md), and [generated carriers](../shard-3/cvidc-generated-carriers-role-review.md). On existing evidence none of these 55 paths is recommended as a behavioral partial solely from its include or related subsystem evidence. A coordinator can separately assess a narrowly labelled declaration-only status if its ledger supports that meaning.

The earlier `shard-3/header-inclusions.csv` is a **bounded historical snapshot, not a complete current dependency log**: for these current pending paths it reports only `invlpg.h`, whereas this query finds the ten above. Do not use it as a complete list or infer that either snapshot proves runtime use. Other profiles, generated includes, stale/unbuilt objects and alternate build trees remain outside this query.

An additional source check of pending C-VID `gdebug.h`, `trace_c.h` and
`prof_c.h` confirms why the absence of an exact dependency is not sufficient
for a global disposition. The first two describe a GDP structure and its
trace ring; the distinct MIPS production variant puts `TraceRingRec` at a
different documented offset. The third defines nine profiling EOI flags also
found in `base/inc/profile.h`, which contains additional definitions. Matching
flag values do not prove the generated declarations, debug profile or tracing
behavior equivalent. Each remains pending until generator and consumer/profile
ownership is established. These are source observations, not execution tests.

Replay from repository root: `python artifacts/mvdm-audit/host-ch-scope/ccpu-cvidc-pending-header-observer.py`. It reads live shard inputs and the populated `build/.ninja/msvc-x86-release/worker` graph and runs `ninja -t deps`, which does not compile. The output is a new snapshot if any inputs change. Two consecutive runs at the recorded inputs produced JSON SHA-256 `ECF19384951D005E232CFC957A4E2F938D6EB39DC405430DD5BB164D495AEFDE`.
