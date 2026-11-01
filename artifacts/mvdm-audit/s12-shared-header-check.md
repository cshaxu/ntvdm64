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

No previously committed source was restored or changed by this check. The
existing [bounded cleanup checkpoint](s12-cleanup-freeze.md) retains the open
disposition of prior shared-header changes; this observation prevents calling
the whole historical header batch single-consumer.
