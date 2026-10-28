# C-VID shard 1 evidence correction

This correction supersedes only the preprocessing observations and candidate count in [the frozen first report](parallel-review-current.md); the original files remain intact. MSVC #line paths encode separators with doubled backslashes. The first JSON parser replaced each backslash individually, leaving doubled slashes that failed exact path comparison. Separately collapsing runs of separators rechecks the five output hashes and all 29 original/current source hashes.

Corrected [machine evidence](parallel-cvidc-evidence-correction.json) shows:

- cpu_c.h: 83 emitted nonblank lines in host_nt_cpu.i; partial candidate unchanged.
- pigger_c.h: 113 emitted nonblank lines in cvidc_accessfn.i, not zero; partial declaration candidate, not proof PIG or guest debug behavior. Its actual production accessfn.obj has mapped functions, including getAF/getAH/getAL.
- vglfunc.c: partial candidate unchanged based on the 78 image vector pointers and three zero slots.
- The other 26 headers have no nonblank output from these five sampled contexts. They stay pending; this does not prove absence of macro effects or other consumers.

No product or guest changes. An executable-behavior restoration percentage remains unmeasurable.
