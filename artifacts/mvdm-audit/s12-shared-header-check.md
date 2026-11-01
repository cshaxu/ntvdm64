# S12 previously changed header consumer check

The owner excluded code shared by multiple compilation units from the bounded
cleanup. The earlier checkpoint recorded 102 changed headers but did not
classify their consumers. This check joins the 102 changed `.h` paths in
`git diff --name-only b90a0d900^ 39847fc3e -- src/mvdm src/non-mvdm` with
the existing [per-header consumer inventory](s12-header-compound-contexts.csv).
Paths are the pre-split paths used by both historical inputs; the subsequent
physical move is recorded in [the role-split manifest](s12-mvdm-role-split.tsv).

The join found 87 of the 102 headers in the inventory: 83 have more than one
observed consumer, and four have one. Fifteen are absent from the inventory;
absence does not establish that they are unshared or uncompiled. Examples of
multi-consumer headers include `inc/mvdm.h` (122 observed edges),
`inc/vrnmpipe.h` (94), and `dos/dem/dem.h` (15).

These are counts of observed dependency edges in the inventoried x86 worker
and WOW32 graphs, not a complete count of every build profile or an assertion
that the removed branch bodies were compiled. The inventory excludes other
profiles and non-default targets as described in its
[method and limits](s12-header-compound-contexts.md). A consumer count also
does not by itself establish which individual removed bodies were shared or
whether restoring them is correct.

A subsequent cross-check joined each recorded graph/object pair from
`build/t473-s12-flat/compound-header-full-contexts.json` to the matching
current `ninja -t commands` `/Fo` object and C source, normalizing the later
`mvdm-host`/`mvdm-guest` path rename. All edges of the 87 covered headers
resolved, with no conflicting object mappings. Counting distinct source paths
still yields 83 headers with multiple sources and four with one; none of the
multi-edge headers becomes single-source after deduplication. This joins a
historical dependency snapshot to current commands and therefore does not
extend coverage to the 15 missing headers or other configurations.

## Follow-up on the 15 missing paths

The historical inventory filtered paths to `src/mvdm/`, so 12 of the 15
were outside its scope because they belong to `src/non-mvdm/`. A separate
current-build check joined `ninja -t commands` C compiler objects to VALID
`ninja -t deps` records for x86 worker/WOW32 and x64 worker, frontend, monitor,
launcher, service and hook. Every C object in these eight graphs had a valid
dependency record. Counts below deduplicate by C source path across graphs.

| Previously missing path | Distinct observed C sources |
| --- | ---: |
| `non-mvdm/base/win32/inc/base.h` | 13 |
| `non-mvdm/private/inc/dlcio.h` | 5 |
| `non-mvdm/public/internal/base/inc/vdm.h` | 150 |
| `non-mvdm/public/internal/ds/inc/debuglib.h` | 6 |
| `non-mvdm/public/internal/ds/inc/netdebug.h` | 19 |
| `non-mvdm/public/internal/ds/inc/netlib.h` | 13 |
| `non-mvdm/public/internal/ds/inc/rapdebug.h` | 2 |
| `non-mvdm/public/internal/windows/inc/pif.h` | 2 |
| `non-mvdm/public/internal/windows/inc/wingdip.h` | 136 |
| `non-mvdm/public/sdk/inc/ntdbg.h` | 1 |
| `non-mvdm/windows/core/ntuser/client/usercli.h` | 44 |
| `non-mvdm/windows/core/ntuser/inc/user.h` | 96 |
| `mvdm-host/softpc.new/base/inc/hwvga.h` | 0 |
| `mvdm-host/softpc.new/base/inc/j_c_lang.h` | 0 |
| `mvdm-host/softpc.new/base/inc/mouse16b.h` | 0 |

The three zeroes mean only that those **exact paths** were not dependencies in
the observed graphs; they are not a global unreferenced-code claim. In
particular, a different `base/cvidc/j_c_lang.h` exists, so a filename-only
search cannot establish which header a consumer includes. The eight graphs do
not cover guest, alternate configurations, resource compilation, non-default
targets or future build profiles. These source counts do not identify which
conditional branch was selected inside a shared header.

No previously committed source was restored or changed by this check. The
existing [bounded cleanup checkpoint](s12-cleanup-freeze.md) retains the open
disposition of prior shared-header changes; this observation prevents calling
the whole historical header batch single-consumer.
