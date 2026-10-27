# Win16 core source roles and guest-library review

This S10 checkpoint compares against `f11b8823e`. It changes audit evidence
only, not the executable package or any original guest. The disposition is
file-scoped: excluded means an original guest build input is not a modern
host implementation input. It does not exclude the corresponding capability.

## Reviewed source cohort

The [85 dispositions](wow16-core-source-role-dispositions.csv) join exact
paths to the frozen inventory and the three disjoint role reviews:
[shard 1](shard-1/wow16-core-guest-owner-review.md),
[shard 2](shard-2/wow16-core-guest-owner-review.md), and
[shard 3](shard-3/wow16-core-guest-owner-review.md).
Every accepted source/resource has unchanged selected-original identity and
an original source/object/link or direct include/resource edge. The original
recipes use MASM/CL16, link16 and RC16 for guest USER, GDI, SYSTEM,
MMSYSTEM and drivers. Debug/retail, architecture and locale conditions remain
explicit. These recipes were inspected, not executed to rebuild a guest.

The main review checked the original USER OBJ1-OBJ4 lists and actual DAYTONA
link response, GDI's five-object response, SYSTEM's assembler pair,
MMSYSTEM's OBJFIRST/OBJ1-OBJ5 and resource branches, and COMM's actual link
response. A same basename, an unselected source, or a filename extension
alone is insufficient. The recorded host input/dependency snapshots provide
an additional negative observation, not the sole exclusion reason.

No source with a mirror difference is excluded by this cohort. The 52 full
and 228 partial records are unchanged; excluded rises from 1,932 to 2,017,
and pending falls from 2,472 to 2,387. In particular, retained Win16 EDIT and
metafile implementation bodies without an established selected build/owner
chain remain pending. Native equivalents do not automatically settle their
original behavior obligations.

## Library review and corrected inference

All 32 pending `.lib` paths were independently inspected with the retained
[reproducer](guest-library-format-observations.py). The
[per-file observations](guest-library-format-observations.csv) and
[container details](guest-library-format-observations.json) show 31 OMF
archives and one MASM macro source (`dos/v86/cmd/append/sysmac.lib`, included
by `append.asm:118`). The archives contain 5,927 framed modules and 55,858
module records. Nonzero stored record checksums have no mismatches;
module alignment and library-end padding were checked. Observed segment
attributes are USE16. This is not dictionary/trailer semantic validation,
link acceptance, source-to-binary provenance or functional evidence.

An initial shard-2 match confused the already excluded native COFF archive
`softpc.new/base/system/obj/i386/system.lib` with `wow16/lib/system.lib`.
The corrected review intersects exact path, pending status and assigned shard
before selecting evidence. The COFF archive is absent from the 32-file
cohort. This correction is retained in the
[shard-2 record](shard-2/guest-library-role-review.md).

The [shard-1](shard-1/guest-library-role-review.md),
[shard-2](shard-2/guest-library-role-review.md) and
[shard-3](shard-3/guest-library-role-review.md) library records remain
proposals; **none of these 32 library statuses changes in this checkpoint**.
Specific original consumers/producers remain unconfirmed for SLIBCE,
OLDNAMES, CDLLCEW, SNOCRTDW and LIBH. Container recognition does not supply
those missing edges. The other proposed roles still need their final
per-path main review, including the local versus shared OLECLI libraries.

## Verification and remaining work

Run from the repository root in the required owned background context:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-library-format-observations.py
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/wow16-core-roles/checkpoint.json --base f11b8823e --prefix wow16-core-source-role- --expected-count 85
```

The first command is tied to this frozen pending-library cohort; later
classification changes require an explicit cohort update. The second checks
the exact 85 ledger transitions, unchanged unrelated shard rows, original
and frozen hashes, partition, full-leaf identities, live drift and recorded
host snapshot identities. Neither establishes guest execution or T closure.
S10 retains all unresolved file/owner classifications; S12-S16 retain guest
ABI, immutable-image lineage, execution, callback and lifecycle obligations.
Concurrent Win3.1 setup/video candidates are preserved and are not accepted
or published by this audit checkpoint.

Both reproduced checks passed on 2026-10-27: library parser PID 9464,
exit 0, and ledger checkpoint PID 13888, exit 0. The
[retained checkpoint result](wow16-core-source-role-verification.json)
records the exact baseline and counts. These are audit verification results,
not runtime or release acceptance.

Documentation governance (PID 33196) and relative-link verification
(PID 26704) also passed with exit 0; `git diff --check` passed. CURRENT is
15,064 bytes, below its 16-KiB limit. This audit-only delivery requires no
product rebuild or binary publication and makes no new runtime claim.
