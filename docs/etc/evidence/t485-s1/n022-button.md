# S15 P30 N022 button migration

Status: final cached build, runtime31 dual publication and targeted verification passed. Commit/push follow governance review.

## Scope and original identity

First DIR-ordered N022 file: `src/non-mvdm/windows/core/ntuser/client/btnctl.c`.
Pinned XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
`Source/XPSP1/NT/windows/core/ntuser/client/btnctl.c`, blob
8e51fc19006730db78acf2de4635ebe31eacbbd6. Before source SHA256
7c385d5ce44a84090a1e9e8c68cb68d5bc8c98efd01ca1e93c54fd4a244e0113.
The authoritative complete hash is recorded in build/t485-s15/n022-button/inputs.json.
Original Git bytes, existing local source, normalized OpenNT adaptation diff,
XP upstream diff and conflict-free three-way merge are retained in that directory.
Original guest files and winnt.zip are unchanged.

Paired existing owners `ntuser/inc/user.h` and `ntuser/kernel/server.c`
remain registered against OpenNT until their whole-file migration. Only the
required XP BUTN keyboard-cue field, ISBSTEXTOROD macro and Button message-table
entries lead their directory. This is not a claim that those entire files migrated.
The new field is included automatically by original INITCONTROLLOOKASIDE's
sizeof(BUTN) allocation; no hard-coded allocator size or guest layout changes.

## Behavior and ownership

Preserve XP RTL text alignment, horizontal/vertical alignment correction,
keyboard-cue/focus painting, owner-draw state, pointer-width spelling and
accessibility notifications. Obsolete OpenNT LATER guards disappear because
XP activates their original bodies; no artificial exclusion replaces them.

C retains the original button algorithm and private data/resource projection.
B retains the existing scoped atlas, exact WND/BUTN borrow lifetime and native
USER callout boundary. XP gpsi atlas spelling references that existing atlas;
its metadata is borrowed, not copied or independently allocated. Display-depth
spellings alias the same capture. No added HWND registry, queue or state machine.

Native USER owns actual UI-state and input-modality policy. The retained NT4
WND view cannot supply XP's private flags merely by defining their constants.
The compile binding queries native WM_QUERYUISTATE through the established
synchronous native-call wrapper. Default UI-state dispatch uses the existing
native default-window boundary, retaining borrow validation and exception cleanup.
Correction to preliminary research: XP routes UPDATE through its server-message
array, but CHANGE/QUERY through client special cases. The latter depend on XP
private UI flags and last-input policy; the project uses native USER for that
policy rather than recreating input state. Only the original Button table's two
new entries are imported in this batch; no wholesale default-table replacement.

## Residual adaptations and evidence

- Existing immutable atlas and private BUTN projection: original btnctl caller,
  wow_button_resources/lifetime providers, and production wow-controls selection.
- Existing callout-destruction checks and nullable DC release: original button
  DC and lifetime fixtures deliberately destroy the HWND during callbacks.
- XP newly-active RTL release checks PWND before reading its style, because
  the same admitted destruction path leaves pbutn->spwnd null.
- XP WM_UPDATEUISTATE checks survival after default-window callback before
  repaint. It uses the existing retirement semantics, not a new state flag.
- Existing original line mapping is recalculated against XP physical LF lines.
  Matched original nonblank lines have zero mapping discrepancies.

All other XP button code remains original. The paired header/table retain
existing #error branches; btnctl has none in XP. No guest rebuilding.

## Final verified census

| File | Before points / lines | After points / lines | Baseline |
| --- | ---: | ---: | --- |
| btnctl.c | 1 / 3774 | 15 / 73 | OpenNT to XP |
| user.h | 1 / 5144 | 1 / 5149 | OpenNT, paired prerequisite |
| server.c | 1 / 2964 | 1 / 2967 | OpenNT, paired prerequisite |

Global1325files,777byte-identical,788nonerror-identical/537different,
7798points/131529lines. Delta0files/0exact/0same/0different,+14points/-3693lines.
1263XP/62OpenNT. Restoring XP LF unfolds the old whole-file block into actual
adaptations; line-ending restoration has standing approval. Paired changes add
8 lines against their still-OpenNT baselines. Normalized diagnostics do not
replace this byte-sensitive census.

## Verification state

First compile exposed absent MAXUSHORT in the selected public SDK binding;
use equal-valued MAXWORD. Second link exposed the missing original ISBSTEXTOROD
macro; restore it in its existing C header owner. Third cached build passed;
publication was stopped before staging because review identified the RTL
retired-window dereference. The corrected final cached build is in progress.

Targeted plan: original WOW control/lifetime cohort; explicit native UI-state
routing, state transitions and destruction cleanup; real published-package
SOL Game/Options child-window acceptance. This is three changed mirror files,
bringing the checkpoint count from6 to9 after successful delivery. Full is not
yet due. N022 directory remains incomplete and overall57/159 is unchanged.

Final cached build and dual13-image26hash publication passed. Native controls
38/38 passed, including native UI flags, native default dispatch and callback
retirement without DC leaks. Real SOL Game > Options exposed its child dialog,
verified owner/OK/Cancel, closed it and recovered the parent. No new product
change followed publication. Test-only stale callback/type consumers from prior
XP header migrations were corrected; failed compile attempts remain evidence.
The selected CCPU callback ABI uses WINAPI, not SDK FASTCALL.

[Receipt](n022-button-result.json); [global census and delta](n022-button-census.json).
The normalized explanatory values are button17/72 ->15/73, user58/160 ->60/165,
server33/1574 ->34/1577; they do not replace the byte-sensitive global values.
N022 first file complete1/39; next cldib.c. Three changed mirror files bring
Full checkpoint count to9; next completed file requires Full.
