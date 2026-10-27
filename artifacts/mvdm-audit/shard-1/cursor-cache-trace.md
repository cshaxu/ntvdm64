# Original WOW cursor/icon cache — retained owner and unselected predecessor

## Scope and identities

Frozen input `../frozen-file-reconciliation.csv` SHA-256
`F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`.
Shard 1 owns `wow32/wcurcash.c`, `wow32/wcuricon.c`, and
`wow32/wucursor.c`. The current graph and final WOW32 map select
`wcuricon.obj` and `wucursor.obj` but not `wcurcash.obj`. The pinned OpenNT
`wow32/sources` likewise lists wcuricon but not wcurcash. A whole-MVDM text
search finds the old cache's public names only in `wcurcash.c` and
`wcurcash.h`; absence of a static call is not proof of runtime equivalence.

## Owner and source call chain

- The old `wcurcash.c::W32FindCursorIcon` searches `pCurIconFirst` by instance,
  resource type and resource name, using case-insensitive string comparison
  or exact integer ID. It stores hIcon16, hRes16 and owner thread; its own
  `W32DeleteCursorIconCashForTask` removes thread-owned cache nodes.
- The selected original `wuman.c::WU32NotifyWow` calls
  `wcuricon.c::W32CheckIfAlreadyLoaded` for its original LoadCursor/LoadIcon
  request. That routine searches the selected existing `lpCIAlias` owner by
  instance, cursor/icon type and case-insensitive name or numeric identity,
  returns its h16 and calls `ReplaceCursorIcon` when the alias is locked.
- `wucursor.c::WU32LoadCursor` calls the selected public native resource
  loading boundary, then `wcuricon.c::SetupResCursorIconAlias`. The latter
  reuses an existing alias for hRes16, or a native h32 for stock resources.
  `SetupCursorIconAlias` stores task, module, instance, resource ID and name.
  `wkman.c::W32FreeTask` calls `FreeCursorIconAlias` by original htask;
  `FreeCursorIconAlias` and `InvalidateCursorIconAlias` own normal release.
- Both are **original** owners, not an added replacement table. Reintroducing
  the inactive `pCurIconFirst` would create a second state owner unless an
  original reachable caller and its complete lifetime justify it.

## Proven boundary and remaining test work

The existing source demonstrates a selected original owner for equivalent
lookup categories and task cleanup intent. It does not establish equivalence
of every cache detail or guest behavior. In particular, verify separately:

1. repeated loads for strings with changed case and numeric IDs, including
   same resource name under different instances and cursor versus icon;
2. cross-task and cross-module lifetime, resource-handle reuse and repeated
   original `FreeCursorIconAlias`/task destruction, including allocation
   failure and reentry;
3. the selected lookup's `lpszNameT != NULL` guard: the inactive lookup's
   numeric comparison does not impose that guard. Whether numeric resource ID
   zero is valid/reachable through original guest USER and host loading is
   not established. This is a test question, not a demonstrated defect;
4. actual guest load/free and stock-resource behavior with the tested binary,
   not merely selected symbols, extracted-body fixtures or a screenshot.

T458 S2 `cursor-r16` exercises extracted original cursor/icon/name output and
alias order; T458 S4's guest/window observer stopped before the provider and
is an explicit non-pass (`docs/etc/evidence/m0-t458-s2-resource-conversion-acceptance.md`,
`m0-t458-s4-consumer-acceptance.md`). The subsequent S9 real application
probes show bounded gameplay, not the full identity/lifetime matrix above.
T458 S1/S2's statement that `wcurcash.c` is selected and is the task-cache
owner contradicts both original sources selection and the current linked map;
the source/caller evidence above supersedes that statement for present
selection, without erasing the historical record. Both files remain pending
until the remaining behavior and lineage evidence is available.
