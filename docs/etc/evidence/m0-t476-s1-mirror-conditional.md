# M0 T476 S1 — mirror conditional admission evidence

## Identity checkpoint

This independent worktree starts from main commit
`eac2932d5ea07d2b3528a05f9ddb6f7c44d7f01d`; its admission does not change
main's separate T474 S2 work. The owner selected the former queue item 11.
The former queue item 2 is a different mirror-difference review and remains
queued.

`python tools/audit/freeze-mirror-conditional.py build/t476-s1/identity.json`
recomputed the 1,055 rows in `s13-r1314-file-audit.csv` (SHA-256
`bf4f827fcbfd4646f9fff327d65abda5d58d7bea24b5af5a16b7050b2ed3b699`). All
1,055 recorded original paths exist and match their recorded SHA-256. All
current paths exist: 1,048 match the historical current hash and seven differ.
At admission, 181 current files match their recorded originals and 874 differ.
These are historical-census counts, not proof of current production closure,
complete-file review, or that every original conditional is correctly handled.
Re-enumeration of tracked `.c` and `.h` files under both host mirror roots
matched those 1,055 census paths exactly; there are no missing or extra C/H
paths (`identity-r2.json`). Other mirror file types are not included in this
C/H count, and guest sources remain outside the permitted change scope.

The owner explicitly limited the audit to host `.c` and `.h` mirrors and
excluded Guest mirrors. An earlier overbroad pass included six non-C/H files;
their source changes were reverted to their exact pre-task hashes and their
markers and rows removed from the deliverable. The C/H inventory and ledger
contain only the 1,055 host paths. No Guest inventory is retained as evidence.

The owner's clarified sequence is: enumerate mirror files; match original
and restore missing conditional structure; keep existing `#error` and `#line`
unchanged; mark branches not already excluded; compile the full supported
product and record hits; then replace each unhit branch body with `#error`,
removing nested conditions inside the excluded body. Existing S13 exclusions
and line anchors are inherited, not restored or discarded.

The seven files changed since the checkpoint are `vdmredir/vrnmpipe.c`,
`wow32/precomp.h`, `wow32/walias.h`, `wow32/wkman.c`, `wow32/wow32.c`,
`wow32/wuman.c` under `src/mvdm-host`, and
`src/non-mvdm/base/win32/client/vdm.c`. Their earlier current-source evidence
must be rebound before reuse. Four `rtl/wow.c` restored branches and selected
empty shells also remain open under the prior closure. Prior partial marker
observations cannot substitute for the newly requested complete marker build.

`tools/audit/inventory-mirror-conditions.py` compared the pinned host C/H files.
Its initial exploratory pass also included out-of-scope files; the retained
C/H-only inventory is `condition-inventory-c-h.json`. That earlier pass
enumerated 8,861 original arms and 7,687 current
arms, including 4,007 current arms with a direct `#error`; 1,531 original
arms were under a matched existing excluded parent and skipped, including
repeated expressions that otherwise match unrelated branches. After matching
body `#line` anchors, group siblings and ancestry, the latest pass
(`condition-inventory-r9.json`) has 39 unmatched arms in nine files and zero
remaining mapped ancestry conflicts. These are lexical review leads,
not a proven count of missing structures: changed guards, reordered conditions,
and rejected enclosing source regions need individual inspection. The earlier
739/44 and 107/23 unmatched counts included repeat-condition misalignment and
are superseded. In `dem.c`, the original nested conditions under an already
excluded DBG arm are explicitly skipped, preserving the existing exclusion.
No original body is restored on the basis of a lexical match.

Manual review of the r7 leads identified 34 more original arms within
already-rejected replacement regions, so they are not candidates for
restoration:

- `egawrtm0.c`: six `BACK_M` arms at original lines 522–544 lie in the
  existing non-CPU40 rejection beginning at original line 467, before the
  retained original function at line 607. Twelve more at lines 1099–1292 lie
  in the existing non-CPU40 rejection beginning at 1085, before the retained
  function at 1356.
- `egwrtm12.c`: ten `BACK_M` arms at original lines 1049–1172 lie in the
  existing non-CPU40 string-move rejection beginning at original line 1028.
- `egawrtm0.c` original lines 289 and 306 and `egwrtm12.c` lines 226 and
  244 belong to original non-CPU40 writer tables already represented by
  existing rejections at original anchors 264 and 205 respectively. The
  selected CPU40 path has its own retained table-storage adaptation.
- `sim32.c`: original `#ifndef PROD` at line 247 lies within the original
  descriptor branch replaced by the existing non-CPU40 `#error` at the
  `#line 239` boundary.
- `xt.c`: the original non-MAC legacy register block at line 76 is covered by
  the existing non-CPU40 rejection; the previous CPU40-owned state adaptation
  is retained.

These decisions are recorded in
`artifacts/mvdm-audit/t476-s1-condition-dispositions.json`, retain the
pre-existing guards and anchors verbatim, and do not resolve the other leads,
certify full files or authorize new exclusions.

The `dpmiint.c:859`, `emm_mngr.c:1883` and `video.c:1511` original groups
map to existing groups with explicitly changed CPU40, LIM-writeback or
standalone-video guard expressions. Their changed expressions and prior
adaptations are recorded as such, not described as original-expression
matches; the video group's existing `#error` is retained. Two unresolved
structure leads remain: original `usercli.h:15` and `user.h:15` monolithic
include guards have been replaced by current subset guards. The actual WOW32
`wow_control_extent_bindings.c` compiler command preprocesses these headers
three and two times respectively (`control-extent-pre.process`, exit 0).
Wrapping either current file naively in its original single-use guard would
prevent later opt-in declaration passes. Determine a safe reconciliation
before marker preparation; do not damage those selected declarations.
Disposable shadow-only probes confirmed the consequence: adding the original
single-use `usercli.h` guard caused the actual control-extent compilation to
fail on missing `CBOX` and `LBWND`; adding the single-use `user.h` guard
separately caused a missing `BUTNWND` error (`guard-probe.process` and
`user-guard-probe.process`, both exit 2 as expected). Neither probe modified
the mirror source or its existing `#error`/`#line` directives.
The 34 preserved exclusions and these three matched adapted groups are
recorded against current/original hashes. The marker preflight with those 37
records stops at `usercli.h:15` and creates no shadow
(`marker-unreviewed-r10.txt`); at that checkpoint no marker build had started.

Fresh worktree-owned x86 and x64 graphs and ten product target builds passed
(`graph.process`, `native-graphs.process`, `baseline-build.process`, exit 0).
A read-only scan of the ten targets' Ninja command closures found 536 distinct
mirror C source paths, 501 present in the historical file census; this is a
direct-source lead, not a complete header/include or generated-input census.
The baseline build produced no replacement release package at this checkpoint.
`tools/audit/plan-mirror-marker-build.py` selected commands from the ten real
Ninja target closures and reconciled them with their compile databases:
1,018 unique object commands (748 x86 and 270 x64). The resulting
`product-compile-plan.json` records every command and graph hash; it is a plan,
not a marker run or a hit ledger. It must be regenerated if graphs change.
The host-only shadow preparation and isolated compile runner are drafted in
`tools/audit/prepare-mirror-marker-shadow.py` and
`tools/audit/run-mirror-marker-build.py`. At that checkpoint they had not yet
performed the marker build: preparation refused unresolved structure leads, and the user
requires structure reconciliation before marking. Their output, once run,
requires full-context success and preprocessing-equivalence validation
before using no-hit decisions.

The two unresolved include guards are used by actual product contexts:
`collect-mirror-header-consumers.py` found 46 object contexts for `usercli.h`
and 104 for `user.h`. A naive whole-file guard breaks the actual
`wow_control_extent_bindings.c` compilation. Temporary probes which preserve
repeated inclusion compile that translation unit successfully and match its
three link-relevant COFF sections and relocations. The `usercli.h` probe also
places the guard boundary before the original post-guard declarations rather
than around the whole file. The combined shadow compiled all 106 unique
consumer contexts; their link-relevant sections and relocations match the
baseline after preserving the original `__FILE__` for one source and fixing
the comparator's handling of uninitialized COFF sections. The guards and
explicit multipass include adapters have now been applied to the worktree.
Every pre-existing `#error` and `#line` directive was retained. A new
full product build and the global branch-marker build were still required at
that checkpoint.

The independent product build subsequently passed for the x86 and x64
targets (`build/t476-s1/reconciled-build/combined.log`). Re-inventory after
the guard changes found 37 previously recorded dispositions and no additional
unmatched original arms. The initial marker shadow had 3,682 marked arms,
including six subsequently excluded non-C/H files; the corrected C/H-only
manifest has 3,597 marked arms and skips 4,007 previously excluded arms.
The first 1,018-context compile
passed at the compiler level but exposed three backslash-path shadow leaks;
the runner was corrected and the complete compile was restarted. Separate
preprocessor comparison was in progress; until it and the rerun finished, no
unhit marker could be an exclusion decision.

The first exclusion candidate failed the product rebuild: an `ega_vide.c`
directive's comment spans the following physical lines and swallowed the
instrumentation, falsely classifying a selected branch as unhit. All initially
unhit marker locations were checked; that was the sole marker inside a block
comment. The candidate files were restored to their recorded pre-exclusion
hashes, retaining the earlier guard recovery. The shadow generator now places
probes after a directive's multiline comment. Only `ega_vide.c` differs between
the two marker shadows; the ten graph hashes and all 1,018 compile commands
are unchanged. The complete 1,018-context marker build passed again; the
affected preprocessing context and the three backslash-path retries passed.
After removal of non-C/H rows, all 35 unhit C/H markers were checked to be
outside block comments. The C/H ledger records 3,562 hit arms, 35 unhit arms,
and 4,007 previously excluded arms. Five unhit arms are nested under another
unhit arm, leaving 30 outermost arms in 15 host C/H files. Those 30 exclusions
were applied after preserving all prior `#line` and `#error` directives;
`tests/audit/verify-t476-s1-exclusions.py` passed for all 30. The subsequent
independent x86/x64 product build passed (`final-build/result-r3.json`, exit
0), and a coherent build-owned candidate package was staged successfully.
All 1,018 final-source preprocessing contexts matched the pre-exclusion
instrumented shadow after removing probe messages and normalizing the shadow
root (`post-change-preprocess/result.json`, exit 0). No final release result
is claimed yet.

The first Control run passed its RPC and GUI checks but failed its version
negative gate because admission had left the product at `0.0.474`. Per the
shared version header's T-level rule, `APP_VERSION` is now `0.0.476`; the RPC
protocol remains 65. The x86/x64 product rebuild passed and a new coherent
candidate was staged. Ninja dependency records identify 56 product compile
contexts consuming that version header; all 56 matched the updated shadow in
`post-version-preprocess.process` (exit 0). The Control rerun passed
(`control-gate-r2.process`, exit 0), covering RPC, GUI, version rejection,
command interaction, session retirement and nested-window handoff. DOS4GW and
NESticle were not requested and were not run. The same candidate package,
`23c63a1e-af45-43fc-92ad-e5b9a8dd9988`, was published to this worktree's
`assets/release`; the ten binary hashes match its manifest
(`publish-local-release.process`, exit 0). No configured system32 target or
other worktree was published.

## Initial `modesw.c` evidence — partial, not complete-file closure

`src/mvdm-host/dpmi32/modesw.c` matches historical current SHA-256
`7dd7d5fe43e2ba7ced3855e36df2bdbebd26603cf637439d7a6fc59c4ecee280`;
its pinned original matches SHA-256
`aa7157e2a39fead41e42e87f87e332e9464bf69a668671de976c69637ce06a67`.
The fresh x86 product graph selects `obj/dpmi/modesw.obj`; building this
target passed (`modesw-build.process`, exit 0). MSVC preprocessing using its
actual graph command passed (`pre.process`, exit 0).
`tools/audit/verify-modesw-line-evidence.py` checked twelve selected statement,
function and closing-brace logical line anchors against the pinned original.

The actual compile context with forced `i386` fails first at the earlier
`non-mvdm/public/internal/base/inc/vdm.h` exclusion, so it does not reach the
`modesw.c` guards. As separate negative controls, each guard was extracted
with its original `#line` and compiled in isolation; each rejected `i386`
with `MVDM-HOST-DIV-619` at original line 50 or 98 (`guard-1.process` and
`guard-2.process`, nonzero expected). This demonstrates guard diagnostics, not
a complete supported-context proof. Selected adaptations, every conditional,
other consumers and all relevant line mappings remain to be reviewed before
this file can be counted complete. Reproducible isolated controls are in
`tools/audit/verify-modesw-guards.py` (`guard-repro.process`, exit 0).

## `rtl/wow.c` restored-branch follow-up — partial

The four historical `restored-unverified` arms are selected in the actual
WOW32 `control-view-wow.obj` x86 compiler context. A build-owned shadow copy
inserted one `#pragma message` into each arm, restoring its logical line after
each marker; the otherwise identical graph command compiled it successfully
(`rtlwow-shadow.process`, exit 0) and emitted all four markers for original
lines 109, 151, 245 and 345. Their source ranges match the recorded original
after newline normalization. The resulting five `.text$mn` section contents
and their relocation records match the original product object; `.debug$S`
and checksum sections differ, so the objects are not byte-identical.
`tools/audit/compare-marker-objects.py` also matched all six non-debug,
link-relevant sections and their resolved relocations for this pair; an
unrelated object was rejected as a negative control (`coff-negative.txt`).

This is evidence of selection in that specific build context, not a complete
per-file review or proof of other consumers. The shadow object is not linked or
published. Full original/current identities are retained in the identity
checkpoint and `s13-r1437-rtlwow-line-edits.json`.

`tools/audit/verify-rtlwow-selection.py` reproduces the shadow compilation,
the four marker checks and the text/relocation comparison
(`rtlwow-repro-r2.process`, exit 0); the first script run failed due to a
Windows-path regular-expression replacement escape and is not passing evidence.
The retained `_IsChild`, `_ClientToScreen`, `_ScreenToClient` and
`_IsWindowVisible` function bodies also compare exactly to the original after
newline normalization. `_GetClientRect` includes rejected WOW arms and line
markers and requires its own full contextual audit. Unselected function
shells and DEBUG consumers remain open.
