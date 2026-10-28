# Original WOW kernel dispatch table fragment

`wow32/wktbl2.h` is byte-identical to both compared OpenNT sources.
Original `wowtbl.c` includes it first within the unified `aw32WOW` table;
the current x86 WOW32 dependency snapshot records that exact header for
`wowtbl.obj`, and the cached map assigns `_aw32WOW` to that object. The
[observer](wow-kernel-table-observe.py) and [receipt](wow-kernel-table-evidence.json)
bind current identities, dependency output, map and the prior table record.

The [earlier bounded table analysis](../shard-1/wow-table-boundary-review.md)
binds the historical selected object and release image's table boundaries:
310 kernel entries beginning at index zero, followed by a separator. Its
recorded `wktbl2.h` hash matches this current source. It explicitly says the
observation does not prove runtime initialization or guest publication, and
the prior release image is not thereby identified with the current install.

The table includes real thunks alongside `LOCALAPI`, `UNIMPLEMENTEDAPI` and
other sentinel entries. Its selection and presence should therefore support
**partial** status for this table file only, not a claim that all 310 kernel
operations are implemented or pass. Actual guest ordinal mapping, reached
thunks, error behavior, nested calls and object/task teardown remain open.
Source text is retained 100%; behavioral completeness is not measured.

Replay with `python artifacts/mvdm-audit/host-ch-scope/wow-kernel-table-observe.py`.
No product, guest, build or shared ledger changed during this observation.
