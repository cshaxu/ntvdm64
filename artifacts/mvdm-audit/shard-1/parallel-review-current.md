# S10 shard 1: selected C-VID source and headers

Scope: 29 pending shard-1 paths in the original frozen partition, as observed at audit time. These are recommendations for the main reviewer, not changes to the shared ledger. All 29 current files match both original editions byte-for-byte and have one DOS16 and two WOW32 frozen catalog rows. Original C-VID build recipe is unchanged. No source, guest, product or installed image was changed.

The [retained evidence](parallel-cvidc-evidence.json) records source hashes, catalog IDs, current VALID dependency blocks, five actual production preprocessing contexts and linked table targets. Dependency presence or source identity alone is not behavior proof. The preprocessor comparison verified the selected cached objects and worker image were unchanged.

Original vglfunc.c defines the 81-entry C_Video table. In the released image, all 78 nonzero object relocations resolve to original SINIT011/012/013 owners and slots 78–80 are zero. The original setup calls the project vector binder; evidence of initial table identity does not establish selected runtime bindings after republish, concurrent access, all backing state or guest rendering. Existing fixture evidence under ../cvidc-vector-fixture-r74-evidence.json is a bounded host test, not complete guest-mode proof.

| File | Recommendation | Source retention | Current dependency / preprocessed output | Remaining boundary |
| --- | --- | --- | --- | --- |
| access_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| bodge_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| btest_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| buffer_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| c2cput_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| contxt_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| cpu_c.h | partial candidate | 100% text; behavior unmeasurable | nt_cpu.obj; host_nt_cpu.i: 83 emitted lines | Audit declaration ABI, actual selected nt_cpu consumers, state transitions and teardown. |
| cpumod_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| cpureg_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| flags_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| frag_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| inst_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| io_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| lxs_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| mark_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| misc_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| nanoc_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| pigger_c.h | pending | 100% text; behavior unmeasurable | obj/cvidc/accessfn.obj; no emitted lines in sampled preprocessor | Audit conditional macro expansion and host debug/profile ownership before assigning role. |
| ppchst_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| sas_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| segreg_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| sf_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| strint_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| strsub_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| sub_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| task_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| test_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |
| vglfunc.c | partial candidate | 100% text; behavior unmeasurable | original C-VID archive; linked C_Video, 78 targets and 3 null slots | Verify runtime republishing, per-field state ownership, thread lifetime and all host/guest behavior. |
| xchg_c.h | pending | 100% text; behavior unmeasurable | no current direct dependency observed | Inspect generated-build provenance, macro/alternate consumers and selected capabilities; absence is not exclusion. |

There are two evidence-backed partial candidates, not completed capability acceptance. The remaining 27 remain pending, including pigger_c.h which is an actual dependency with no emitted declaration lines in the sampled context. No row is excluded merely because it was absent from current Ninja dependencies. No runtime, guest, desktop or physical mouse test was run.
