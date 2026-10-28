# Three excluded host source roles with build-graph hits

`prt_scrn.c`, `cpucstbs.c` and `x86_emm.c` each have an actual selected
compile rule and an archive input; describing them as absent from the graph
would be wrong. Their current x86 COFF objects each have directive, debug and
checksum sections but no code section, and no function owner from those
objects appears in the worker map. The observer binds original/current source
identities, rules, object hashes and section flags.

`prt_scrn.c` is the original deleted Print Screen implementation comment;
the [existing review](../shard-1/native-pending-followup-2.csv) retains BIOS
Print Screen separately. `cpucstbs.c` is guarded by A3CPU and `x86_emm.c`
by LIM plus MONITOR. Their inactive file-role exclusions are consistent with
the current object evidence, but do not prove alternate CPU, EMS, BIOS or
guest behavior. Selected original `nt_emm.c` is a separate owner; its behavior
and memory mapping remain independently in scope. No product or guest changed.
