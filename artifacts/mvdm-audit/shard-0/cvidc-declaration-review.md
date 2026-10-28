# Selected C-VID declaration contexts

Four pending original headers match both pinned originals and are included by
actual worker compiler contexts. The recorded preprocessing uses the exact
Ninja compiler commands with output redirected into an isolated build folder;
the [runner](cvidc-declaration-preprocess.ps1) checked that worker/object hashes did not change. The retained
[receipt](cvidc-declaration-observations.json) includes the selected text,
source/object/output hashes and context. The [observer](cvidc-declaration-observer.py)
can replay while the temporary preprocessed files remain available.

| Header | Selected context | Remaining boundary |
| --- | --- | --- |
| `cpu4gen.h` | `v7_video.c`, production flags | CPU vector ABI, state, nested use, mode selection and reset. |
| `evid_c.h` | `ev_glue.c`, production flags | Video state layout, vector selection, mode transitions and lifetime. |
| `gdpvar.h` | `ev_glue.c`, production flags | Actual global data ownership, update ordering and teardown. |
| `univer_c.h` | `accessfn.c`, its actual non-PROD flags | Selected declarations alone do not prove release call paths or CPU state behavior. |

The worker map has `v7_video.obj` and `accessfn.obj` entries; source/header
selection alone does not prove those particular declarations are exercised.
Original source text is retained, but functional-restoration percentages and
all runtime, failure and lifecycle semantics remain unproved. The other
unobserved C-VID headers remain pending; absence from this preprocessing
group is not proof of exclusion. No guest or product image was modified.
