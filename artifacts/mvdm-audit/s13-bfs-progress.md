# S13 breadth-first conditional review

**Owner correction:** work strictly one depth at a time. The earlier deeper
first-pass figures below are historical exploratory output only, not current
classification or completion evidence. All 8,549 depth-two-and-deeper arm
statuses in the task queue were reset to unreviewed. Depth one remains active;
children can be queued but must not be examined or classified until the
current depth's source and original-boundary work is complete.

Baseline: Git `c97d4ee2c8be6d71f32272a674a3b0d76bb71a6c` plus the
existing uncommitted S13 changes. The task-local baseline and queue record
the current and original SHA-256 for each of 1,636 host/non-MVDM C, C++ and
header files in the existing pinned-original manifest. All 1,636 pinned
original hashes matched. This manifest is an input inventory, not yet proof
that every relevant file or supported test profile has been included.

Initial lexical inventory: current tree 4,066 depth-one groups and 5,039
arms; original mirror 4,219 depth-one groups. These counts are separate and
do not equate the groups by text. The parser reported an unclosed trailing
comment in both copies of `sim32/sim32.c`; the file must be independently
classified before treating either parse as complete. The trailing unmatched
`/*` is present in both the pinned original and current bytes; no reference to
this translation unit was found in the checked generated Ninja trees, build
tools or tests. That search does not establish that every supported context
has been enumerated, and the file has not been modified.

Depth one, initial review: 63 literal `#if 0` arms already contain a single
`#error`; their conditions are false independent of macro state. No source
was modified for this review. The original-correspondence and diagnostic-line
checks for these arms remain separate. Of 5,039 current arms, 63 have that
established exclusion, 0 have been established retained, and 4,976 have not
yet been classified; no next-depth work has been admitted from a retained
arm. This does not reconcile missing original condition structure.

The CMake production presets name x86 and x64; ordinary and ccache variants
share configuration trees. Individual generated rules also set different
per-source macros, including scoped `MONITOR`, so these names are not an
exhaustive profile or header-inclusion proof. Fresh instrumented preprocessing
is being collected from generated Ninja compilation commands. A positive hit
can support reachability in that exact successful context; absence of a hit
cannot establish universal exclusion.

Fresh collection covered 1,658 generated compiler contexts: 1,646 succeeded
and 12 failed because generated test/reference trees could not resolve headers.
One failure came from the older `call-boundaries` Ninja tree: running its
existing generator and retrying that source with the x86 compiler environment
preprocessed successfully. The other eleven were from the x64 configure
tree's `worker-reference-x86` graph, for which CMake names `-Architecture x86`
and passes the graph as a reference to native generators; CMake does not
expose its worker build target in the x64 branch. The initial collector also
incorrectly used an x64 compiler environment for those x86 reference entries.
They were not used as positive or negative product-branch evidence; no claim
is made that this reference graph is an independently supported build.
For a narrower positive witness, the current ten product targets' Ninja
dependency commands were matched to the compilation database: 981 contexts
matched and all 981 preprocessed successfully. Source-hash-matched markers
observed in these contexts establish 1,849 depth-one arms as retained in at
least one x86 or x64 product context. The 63 literal-false arms and four
`CPU_30_STYLE` arms prohibited by `docs/rules/CODING.md` are already excluded
with sole `#error`; 3,123 arms are provisionally marked pending because this
collection does not establish their reachability or universal exclusion.
They have not received individual semantic disposition. These retained
arms contain 1,395 depth-two groups eligible for later review. A group under
an unreviewed arm may also be reachable; including such arms leaves 3,203
potential depth-two groups. These numbers
describe current source only and do not settle original mirror reconciliation,
include-guard classification, diagnostic mapping or the failed test profiles.

The first-depth-only test-source collection subsequently preprocessed 224
generated x86/x64 test contexts successfully. One additional first-depth arm,
`j_c_lang.h`'s `#ifdef LITTLEND`, was reached by the existing x86
`cvidc-vector-binding-fixture.exe` source and current generated worker graph;
that fixture is recorded in the T473 S5 evidence. This positive test witness
does not establish that any unobserved test arm is unreachable. Updated
current-tree first-depth counts are 1,850 retained, 67 existing exclusions
and 3,122 pending; the immediate-child queue counts remain 1,395 / 3,203.
The three further first-depth markers seen in the unfiltered 1,658-context
collection came exclusively from the x86 reference graph incorrectly run
with an x64 environment (`mvdm.h:68`, `suballoc.h:35`, and `cleanup.c:213`).
They remain pending rather than being counted as supported x64 reachability.
The regenerated `call-boundaries` default target's 15 source/test contexts
all preprocessed successfully; they added no first-depth marker beyond the
product witnesses.

Among current first-depth boundaries, 395 were independently checked as
whole-file include guards: the matching define immediately follows the guard,
and its matching endif is the final significant line. Of these, 91 are
already product-witnessed and 304 remain pending inclusion-context review.
The coding rule excludes ordinary include guards from deleted-branch-body
replacement, so none of these 395 is replaced with `#error` merely for lack
of a marker. This boundary classification does not prove header contents or
every original/local guard mapping. The per-file source identities, line
positions and statuses are in `s13-bfs-depth1-include-guards.json`.

The initial inventory used a library line splitter that also treated form-feed
as a line boundary. It was corrected to split on LF only and the queue rebuilt;
group and arm totals were unchanged, while eleven additional positive markers
matched the corrected physical line numbers. The values above are corrected.

An independent depth-one original/current per-file directive-multiset triage
finds 351 unpaired original groups in 59 files. This is a worklist only: it
does not distinguish omitted whole functions, intended changed conditions,
include guards, or misplaced matching directives, and can miss positional
errors. It is not a count of proven missing boundaries. Candidate rows and the
per-arm current queue are under the task scratch directory `build/t473-s13/bfs`.
Of these, 73 original depth-one groups in kernel `ntstubs.c` have been
classified separately: the registered OPENNT-HOST-076 subset retains only
the complete `NtUserSetWindowFNID` body (original lines 6170–6188), which has
no enclosing conditional; all 73 candidate groups lie outside that retained
body. This is an omitted-whole-function subset disposition, not a claim that
the other original functions were restored. One additional candidate in
`userk.h` is the original `_USERK_` include guard; ordinary include guards
are explicitly outside the excluded-branch-body rule, and the retained subset
has the separate `WOW_USER_OBJECT_FLAGS_ONLY` inclusion control. Seven further
original depth-one candidates are ordinary whole-file include guards in
`ntexapi.h`, `ntregapi.h`, `ntrtl.h`, `nturtl.h`, `globals.h`, `user.h`, and
`userrtl.h`; each original opening is followed by its matching define and
the guard closes at the end of the file. This disposition does not certify
the corresponding current inclusion behavior. `_USERCLI_` is not included
in this group because the original file also has content after its guard.
For `ntcftxt.h`, the registered OPENNT-HOST-036/046 subset retains five
original functions: `GetClassInfo` (111–155), `StringDuplicate` (578–588),
`InitClsMenuName` (595–632), `RegisterClassExWOW` (701–844), and
`UnregisterClass` (1051–1087). The original has eleven top-level `#ifdef
UNICODE` groups; the initial selector and the two groups intersecting retained
functions (601–617 and 802–806) correspond to the three current groups. Its
eighteen `#ifndef UNICODE` groups and two `#ifdef FE_SB` groups do not
intersect any of those five function ranges. The 28 excess original groups
are therefore classified as outside this registered function subset, without
asserting that other text APIs were restored or that all local semantics have
been independently reviewed. Original group interval and retained-line
comparisons were checked against the frozen original and current sources.
For `handtabl.c`, OPENNT-HOST-063 retains only the original ownership table
at lines 39–59; of 28 original depth-one groups, only the `FE_IME` group at
line 56 intersects that table. The 27 other groups lie outside the retained
table subset. Their source identities and locations are recorded in
`s13-bfs-depth1-handtabl.json`; this does not decide whether the retained
`FE_IME` arm is reachable. `input.c` retains original `AddAttachment`,
`Recalc2` and `RecalcThreadAttachment` (OPENNT-HOST-057/058); its 25
unmatched top-level groups lie outside those functions. `queue.c` retains
original `SetAppCompatFlags`, `xxxInitTask` and `_ShowStartGlass`
(OPENNT-HOST-053); the original top-level `DEBUG` arm in `xxxInitTask`
remains in the local subset and is still pending reachability review, while
the other 23 unmatched groups lie outside those functions. Source identities
and ranges are recorded in `s13-bfs-depth1-input-queue.json`. The remaining
In `copy_fnc.c`, DIV-064 deliberately changes the original top-level `#if 0`
at line 115 to `#if 1` to select the original fill loop; the current `#else`
is universally unreachable and already contains only `#error` (DIV-562).
In `nt_msscs.c`, DIV-039 intentionally replaces original `#ifndef i386` with
the CPU40 machine-owner selector; the local selected CPU40 path was witnessed
in the product configuration and the other arm remains separately pending
all-supported-context proof. Both exact correspondences and frozen source
identities are in `s13-bfs-depth1-selected-condition-adaptations.json`.
In `ios.c`, the original top-level `NTVDM && MONITOR` condition at line 413
corresponds to the product-witnessed local condition at line 398, whose
registered DIV-477 adaptation admits CCPU alongside MONITOR. The other
original condition with the same expression retains its original spelling.
This is a first-depth condition correspondence, not a fresh behavioral proof
of the string-I/O consumers; source hashes and exact expressions are in
`s13-bfs-depth1-ios.json`. The remaining 164 original-group multiplicity
candidates are unreviewed.

One of those candidates, `nt_sec.c`, is now specifically identified rather
than treated as a generic text mismatch: original lines 115–158 form a
top-level `#ifdef X86GFX` absent from the local file. This file was not
witnessed in the collected contexts, while X86GFX is used elsewhere. The
file's supported role and all applicable profiles must therefore be established
before deciding whether an exclusion sentinel belongs at that boundary.
`s13-bfs-depth1-nt-sec-pending.json` pins the exact source identities. The
removed hardware-V86 implementation is not restored.

First-depth implementation: DIV-566 restores the original top-level
`#ifndef CPU_40_STYLE` boundary in `base/support/main.c`. Its unsupported
non-CPU40 arm is `#error`; the original DIV-083 CPU40 declaration remains in
the selected alternative. Explicit `#line` directives preserve the existing
selected declaration and subsequent diagnostic positions. The actual x86
worker compilation succeeded; the existing C4210 warning remained at logical
line 107. Real-context preprocessing before and after differs only by blank
lines, and separate x86/x64 preprocessor controls verified lines 61/62 and
the non-CPU40 error. Source hashes, the removed original arm and proof limits
are in `s13-bfs-depth1-main.json`; the focused test is
`tests/build/test_bfs_depth1_cpu_guards.py`. The rest of depth one remains
open, so the complete build, publication and release gates have not run for
this depth.
After this edit, the current depth-one inventory contains 4,067 groups and
5,041 arms: 1,851 retained, 68 previously excluded, one newly excluded, and
3,121 pending. The original unmatched-candidate worklist has 163 pending
groups. These are progress counts, not proof that the breadth-first layer is
complete.

DIV-567 replaces the first-depth `xt.c` parity-table body, guarded by a
required `!CPU_40_STYLE` conjunct, with `#error` for the unsupported profile.
The existing DIV-155 condition adaptation remains registered because it
keeps the selected CPU40 path from introducing a duplicate provider. The
removed 259-line local body matches the original after CRLF normalization;
`#line 370` preserves the next logical diagnostic line. The selected x86
preprocessor output matches the prior source apart from blank lines, the real
x86 `xt.obj` build succeeds, and x86/x64 first-depth controls check both
conjuncts, the error and the logical line. The full source identity and proof
limits are in `s13-bfs-depth1-xt-parity.json`; no child condition was reviewed.
This also maps the two original `xt.c` top-level CPU selector differences to
their existing DIV-147 and DIV-155 adaptations. Updated first-depth counts are
1,851 retained, 68 prior exclusions, 2 newly excluded and 3,120 pending;
161 original multiplicity candidates remain unreviewed.

The original `nt_timer.c` top-level MONITOR/X86GFX condition at line 241 and
the local condition at line 243 are the unique such groups in their respective
files and differ only in whitespace. This classifies one original
multiplicity mismatch without deciding whether the arm is reachable; 160
original candidates remain unreviewed.

DIV-568 retains the existing first-depth `PROFILE && !CPU_40_STYLE` condition
in `main.c` and replaces its five-line unsupported body with `#error`; the
discarded body matches the original after newline normalization. This did not
restore an additional missing condition framework. `#line 221` preserves the
following prior logical line. The real x86 object build and the selected-path
preprocessor comparison succeeded; x86/x64 controls exercised both sides of
the conjunction, the rejected arm and the line mapping. Evidence and source
hashes: `s13-bfs-depth1-main-profile.json`. Updated current depth-one counts:
1,851 retained, 68 prior exclusions, 3 newly excluded, and 3,119 pending.
Exactly one original first-depth framework has been restored in this pass
(DIV-566); DIV-567 and DIV-568 retain existing frameworks. Full first-depth
build, publication, test, commit and push remain due at layer completion.

The previously added `config.c` MONITOR LIM-backfill sentinel duplicated an
already retained top-level sentinel for the same original group. The duplicate
and its redundant test entry were removed; the original sentinel and its
`#line 886` remain. The true x86 object build and selected preprocessing
passed, x86/x64 first-depth guard controls passed, and actual warning lines
757, 898 and 945 match the earlier build. See
`s13-bfs-depth1-config-dedup.json`. This reduces the first-depth group count
by one and does not remove an original framework.

Restoration-count scope: DIV-566 is the one original first-depth framework
restored since starting this BFS pass. Across the entire uncommitted tree
relative to `c97d4ee2c`, the first-depth Git-diff inventory found 34 added
condition-group openings. All 34 have now been anchored to their original
top-level conditions using unique condition, original function or adjacent
source context; repeated `MONITOR` conditions were checked individually.
The per-group source hashes, original line positions and anchors are in
`s13-bfs-depth1-restoration-candidates.json`. Thus 34 original first-depth
frameworks have been restored in the uncommitted work, including the one in
this BFS pass. This counts groups, not arms or `#error` replacements, and
does not assert that their semantic/profile or line-number audit is complete.

After eliminating the duplicated `config.c` sentinel, the current first-depth
inventory is 4,066 groups and 5,040 arms: 1,851 retained, 68 existing proven
exclusions, 3 newly excluded, and 3,118 pending. These numbers do not imply
that pending original structure, diagnostics or supported contexts are done.

DIV-569 subsequently replaces the non-CPU40 i386 memory-size arm at
`config.c:460` with `#error`. Its removed two-line body matches the pinned
original after newline normalization. DIV-056 remains registered for the
selected CPU40 condition and original `GetVDMSize` calculation; its previous
description of retaining both bodies has been corrected. `#line 464` keeps
the selected sibling's existing logical line. The actual x86 object build,
selected nonblank preprocessing comparison, and x86/x64 guard controls pass;
the previous warning locations remain unchanged. Per-source identities and
tests are in `s13-bfs-depth1-config-memory.json`. This is an existing
condition framework, not an additional restoration. Current first-depth
counts are 1,851 retained, 68 prior exclusions, 4 newly excluded, and 3,117
pending; 159 original multiplicity candidates remain unreviewed. Restored
first-depth framework count remains 34.

The `#if 1` alternative also updates the current-tree first-depth totals to
1,850 retained, 68 existing proven exclusions, and 3,121 pending. No source
was newly rewritten for these two determinations.

Only immediate children of depth-one arms remain queued. A pending parent is
potentially reachable and cannot suppress its children. Their contents and
dispositions will be reviewed only after depth one has passed its independent
build, publication, test, commit and push gate.

Of the 978 current mirror files containing conditional directives, 563 had a
file marker in successful product dependency contexts. The same 563 were
witnessed across all 1,646 successful generated contexts; 415 were not. A
missing marker does not establish that a file is wholly excluded from every
supported fixture or include path. The failed contexts and source/generator
inventory must be resolved before a whole-file exclusion is decided.


DIV-570 replaces six first-depth non-NTVDM serial/timer arms in existing
condition groups, not six restorations. Their original/local body hashes and
actual x86 include evidence are recorded in `s13-bfs-depth1-ntvdm-arms.json`
and `s13-bfs-depth1-ntvdm-invariant.json`. Actual selected x86 preprocessing
is equal ignoring blank lines, but raw output is not identical; the two x86
objects compile and x86/x64 focused guard and logical-line controls pass.
Full product build and runtime verification remain pending. First-depth
counts: 4,066 groups, 5,040 arms; 1,851 observed retained, 68 earlier
excluded, 10 newly excluded, 3,111 pending. 34 original first-depth groups
restored relative to HEAD; no restoration in DIV-570. Of these 34, one was
restored after this BFS pass began. 159 original matching candidates remain
unreviewed. Structural pruning leaves 8,537 deeper current arms unreviewed;
this does not constitute a classification of those arms.

Original-baseline first-depth reconciliation: two apparent i386 mismatches in
`inc/mvdm.h` and `inc/suballoc.h` correspond to existing first-depth
conditions extended under the registered MVDM-SUPPORT-DIV-006/007 adapters.
They do not add restored groups. Pinned hashes, line anchors and conclusions
are in `s13-bfs-depth1-i386-correspondence.json`; arm reachability remains
a separate question. This reduces unreviewed original multiplicity candidates
from 159 to 157 (77 rows). The first-depth restored-group count remains 34.

The actual NTVDM invariant and unique original first-depth anchors also
confirm 26 previously present sole-`#error` arms in `com.c`, `timer.c` and
`reset.c`; `s13-bfs-depth1-ntvdm-existing.json` records each. No source or
framework was restored in this classification. Current first-depth counts:
1,851 observed retained, 94 prior exclusions, 10 newly excluded, 3,085
pending, across 4,066 groups / 5,040 arms. Restored groups remain 34.

DIV-571 replaces one existing first-depth non-NTVDM timer arm and preserves
its original selected sibling and the previously existing `#line 1816` and
`#line 1959`. No new condition group was restored. Actual x86 preprocessing
matched the preceding candidate ignoring blank lines, but not bytewise; the
timer object rebuilt and x86/x64 focused positive/negative and logical-line
controls passed. Full product and runtime verification remain pending.
First-depth totals are 4,066 groups / 5,040 arms: 1,851 observed retained,
94 previous exclusions, 11 newly excluded, 3,084 pending. The second-depth
queue contains 3,193 candidate groups, including children of unresolved
first-depth arms; their selection and bodies have not been classified.
Original-group restorations remain 34; 157 original multiplicity candidates
remain unreviewed.

An additional actual worker include-chain sweep confirms 37 existing
first-depth NTVDM exclusions in 14 source files, each with an original anchor
and sole `#error`; see `s13-bfs-depth1-ntvdm-batch2-existing.json`. The
keyboard source has an additional directly including observation fixture and
was left pending for separate context verification; `nt_rflop.c` also remains
pending its additional syntax-test context. No source was rewritten and no
condition group was restored. First-depth totals: 4,066 groups / 5,040 arms;
1,851 observed retained, 131 prior exclusions, 11 newly excluded, 3,047
pending. Restored original groups remain 34; unreviewed original multiplicity
candidates remain 157. The 3,193 second-depth candidate groups include
children of unresolved first-depth arms and have not been reviewed.

The separately compiled `keyba_observed.c` fixture was then checked with its
actual x86 compiler command and include trace. It includes both original
`keyba.c` and `host_def.h`, defines NTVDM and contains no NTVDM undef in its
recorded include chain. Together with the product trace and unique pinned
original anchors, this confirms six existing keyboard first-depth exclusions;
see `s13-bfs-depth1-ntvdm-keyba-existing.json`. No new restoration or source
change resulted. First depth: 4,066 groups / 5,040 arms; 1,851 observed
retained, 137 prior exclusions, 11 newly excluded, 3,041 pending. Original
frameworks restored: 34; 157 original multiplicity candidates still open.

The 3,193 currently queued second-depth group candidates were checked only
against their first-depth parent-arm identities: 1,395 are below observed
retained arms and 1,798 below still-pending arms. None is below an already
classified excluded first-depth arm. This checks queue placement only and
does not examine or classify the candidate groups themselves.

Original `client/clmsg.c` reconciliation used the pinned original and the
registered complete-function subsets OPENNT-HOST-089/120. Ten original
first-depth groups lie wholly outside those admitted function ranges; four
intersect `DefWindowProcWorker` and remain pending under its local selector.
Only original group start/end and admitted function boundaries were checked;
the retained groups were not classified as excluded. Evidence is
`s13-bfs-depth1-clmsg-original.json`. The original multiplicity candidates
still unreviewed decrease from 157 to 147; no framework was restored.

The current original first-depth correspondence queue is also retained at
`s13-bfs-depth1-original-candidates.csv`; its multiplicity count is distinct
from both current-tree arms and restored-group count.

DIV-572 replaces the first-depth non-NTVDM BIOS signon body in `reset.c`;
the original boundary remains and `#line 898` preserves the observed following
line. Actual x86 preprocessing matched its previous selected path ignoring
blank lines, but not bytewise. The x86 object and x86/x64 focused condition,
negative and line controls pass; full product/runtime testing remains pending.
Two previously excluded timer arms were separately anchored to the pinned
original and classified; see `s13-bfs-depth1-ntvdm-timer-anchors.json`. Neither
change restored a condition group. First depth: 4,066 groups / 5,040 arms;
1,851 observed retained, 139 prior exclusions, 12 newly excluded and 3,038
pending. Restored original groups remain 34; 147 original multiplicity
candidates remain unreviewed.

DIV-573 replaces five existing first-depth non-NTVDM arms in time service
and PIC, with the pinned original boundaries retained and following logical
lines preserved. Source/body identities are in `s13-bfs-depth1-ntvdm-batch3.json`.
The actual x86 selected preprocessor outputs compare equal ignoring blank lines
(not bytewise); both x86 objects compile and x86/x64 positive/negative guard
and line controls pass. Rebuilt controller fixtures pass PIC reset (24
assertions) and keyboard/PIC (86 assertions); full product and runtime gates
are pending. Results are in `s13-bfs-depth1-batch3-verification.json`.
First depth: 4,066 groups / 5,040 arms, 1,851 observed retained, 139 prior
exclusions, 17 newly excluded, 3,033 pending. Restored original first-depth
groups remain 34; 147 original candidates remain unreviewed.

After DIV-573, the next-depth queue contains 3,186 candidate groups: 1,395
below observed-retained first-depth arms and 1,791 below pending arms. No
second-depth groups below a classified excluded first-depth arm remain. All
8,519 remaining deeper arms retain the unreviewed status. This is a queue
integrity check, not a review of any next-depth body.

First-depth DBG review: 23 pre-existing sole-`#error` branches in 12 DEM
and DPMI source files were matched to unique original anchors. Actual x86
worker and x86-reference compiler commands both succeeded; their commands
and included file chains contain no DBG definition, undefinition or command
flag. Directly source-including test fixtures use two additional files, which
remain pending rather than being inferred from product evidence. Source hashes,
24 compiler-context traces and per-arm anchors are in
`s13-bfs-depth1-dbg-batch1.json`. No code was changed or restored by this
classification. First depth: 4,066 groups / 5,040 arms; 1,851 observed
retained, 162 prior exclusions, 17 newly excluded, 3,010 pending. Restored
original groups remain 34; 147 original multiplicity candidates remain open.

The `demsrch.c` and `int21map.c` DBG review additionally traced their actual
product and reference compile contexts and the three direct source-including
fixtures in both contexts. All ten syntax/include-chain checks succeeded with
no DBG definition, undefinition or flag; 13 existing `#error` arms also match
unique original anchors. Details: `s13-bfs-depth1-dbg-fixtures.json`. No
source or original structure was changed. First depth now has 4,066 groups /
5,040 arms: 1,851 observed retained, 175 existing exclusions confirmed, 17
newly excluded and 2,997 pending. Restored original groups remain 34; 147
original multiplicity candidates remain unreviewed.

The next DBG batch checked 25 actual x86 worker/WOW32, x86-reference and x64
launcher compiler/include contexts for 14 sources. Twenty-eight existing
sole-error arms match unique original anchors. The additional COMMAND
dispatcher source and direct-including fixture were each checked in x86
worker and reference contexts, confirming one more existing arm. The traces
are `s13-bfs-depth1-dbg-batch2.json` and `s13-bfs-depth1-dbg-command.json`.
No source rewrite or original framework restoration resulted. First depth:
4,066 groups / 5,040 arms; 1,851 observed retained, 204 previous exclusions,
17 newly excluded, 2,968 pending. Restored original groups remain 34; 147
original multiplicity candidates remain open.

A subsequent first-depth DBG pass used three uniquely matching following
source lines after the conditional group, pinned original identities and the
already recorded complete compiler/include contexts. It confirmed 113 more
existing sole-error branches across eight files, without modifying source.
Physical line positions are split on LF rather than generic splitlines, so
form-feed bytes do not shift indices. A separate physical-line recheck of all
65 previous DBG classifications found no mismatches. Details:
`s13-bfs-depth1-dbg-following-anchors.json`. First depth: 4,066 groups /
5,040 arms; 1,851 observed retained, 317 earlier exclusions, 17 newly
excluded, 2,855 pending. Restored original groups remain 34; 147 original
multiplicity candidates remain unreviewed.

Original-baseline reconciliation for `client/wow.c` and `rtl/wow.c` accounted
for 19 further first-depth groups in six candidate rows. Their recorded
original line positions fall outside the specific original function bodies
admitted by OPENNT-HOST-078/106/108; the two original `_GetClientRect` WOW
groups remain present locally. Fixed-original and current hashes, retained
function ranges and group positions are recorded in
`s13-bfs-depth1-original-wow-subsets.json`. This did not change source or
restore conditions. Remaining original multiplicity candidates: 128 groups
in 68 rows; restored original first-depth groups remain 34, one during this
BFS pass. These original-source counts are separate from the 5,040 current
source arms.

A broader first-depth cohort classified 178 additional existing sole-error
arms in 83 files. It first excluded names defined or undefined in project
source and names set by any of the 2,836 generated compile commands; the
remaining entries still required matching source hashes, successful actual
preprocessing and uniquely anchored pinned-original outer groups. MONITOR and
X86GFX have real per-target definitions and are not treated as globally absent;
UNICODE and YODA have project definitions and were also excluded from this
cohort. Details and evidence limits are in
`s13-bfs-depth1-macro-broad-cohort.json`. No source or condition structure was
changed. Root: 4,066 groups / 5,040 arms; 1,851 observed retained, 1,165
existing exclusions, 127 newly excluded, 1,897 pending. Restored original
first-depth groups remain 34 (one in the current BFS pass); 147 original
multiplicity candidates remain unreviewed.

The next DBG batch checked 22 x86 worker/reference compile contexts for 11
additional directly compiled host units, with no DBG definition or flag in
those commands or their include chains. Distinct original first-depth anchors
confirm 86 existing sole-error arms; see `s13-bfs-depth1-dbg-batch3.json`.
All checks use physical LF positions; no source changed and no original
structure was restored. First depth: 4,066 groups / 5,040 arms; 1,851 observed
retained, 403 existing exclusions confirmed, 17 newly excluded, 2,769 pending.
Restored original groups remain 34; 147 original multiplicity candidates remain
unreviewed. The 3,186 second-depth candidates have not been reviewed.

Three more DPMI sources and their direct-including observation fixtures were
checked in six product/reference and six fixture syntax/include contexts.
Five existing DBG exclusions have unique original first-depth anchors; see
`s13-bfs-depth1-dbg-dpmi2.json`. The fixture-only trace variants do not occur
in the included provider source and do not define DBG. No source was changed.
First depth: 4,066 groups / 5,040 arms; 1,851 observed retained, 408 existing
exclusions, 17 newly excluded, 2,764 pending. Original first-depth frameworks
restored remain 34; 147 original multiplicity candidates remain open.

The first PROD batch checked 10 actual x86 worker/reference compiler and
include chains for five SoftPC units, all selecting `/DPROD` without a
subsequent include-chain redefinition. Relevant controller/PIC and BOP
fixture generators explicitly select PROD as recorded in
`s13-bfs-depth1-prod-batch1.json`. Sixty-seven existing sole-error arms have
unique original first-depth anchors. No source or framework was changed.
First depth: 4,066 groups / 5,040 arms; 1,851 observed retained, 475 existing
exclusions, 17 newly excluded, 2,697 pending. Restored first-depth original
groups remain 34; 147 original multiplicity candidates remain unreviewed.

The second PROD batch collected 80 actual x86 worker/reference compiler and
include-chain traces for 40 sources, each selecting `/DPROD` with no PROD
redefinition in its recorded include chain. Unique original outer anchors
confirm 120 existing sole-error arms across 39 files. `nt_event.c` and
`nt_com.c` were held for their source-including fixtures; `nt_rflop.c` was
held for a differently flagged older syntax graph; files absent from generated
compilation graphs were not classified by absence. Evidence is
`s13-bfs-depth1-prod-batch2.json`. No source changed or framework was restored.
First depth: 4,066 groups / 5,040 arms; 1,851 observed retained, 595 prior
exclusions, 17 newly excluded, 2,577 pending. Restored original groups remain
34; 147 original multiplicity candidates remain unreviewed.

The NTVDM first-depth sweep now also covers `#else` arms of original
`#ifdef NTVDM` groups: unique original anchors, actual worker/include-chain
evidence and the separately checked keyboard fixture confirm 56 existing
sole-error alternatives in 12 files. Parent groups remain unchanged. See
`s13-bfs-depth1-ntvdm-else.json`. First depth: 4,066 groups / 5,040 arms;
1,851 observed retained, 651 prior exclusions, 17 newly excluded, 2,521
pending. No new framework was restored; cumulative original first-depth
restorations remain 34.

DIV-574 replaces five first-depth non-NTVDM `#else` bodies in keyboard,
time, PIC and timer sources; it retains the original group frameworks and
selected arms, and uses `#line` for the observed following logical lines.
The four actual x86 selected preprocessor outputs match before/after ignoring
blank lines, but not bytewise. All four x86 objects rebuilt; x86/x64 focused
positive, negative and line controls pass. Rebuilt PIC and keyboard/PIC
fixtures passed 24 and 86 assertions respectively. Source identities, removed
body hashes and fixture evidence are in `s13-bfs-depth1-ntvdm-else-new.json`
and `s13-bfs-depth1-else-verification.json`; full product/runtime gates are
still pending. Root: 4,066 groups / 5,040 arms, 1,851 observed retained,
651 existing exclusions, 22 new exclusions, 2,516 pending. Restored original
first-depth groups remain 34; 147 original multiplicity candidates are open.

The PROD review of `nt_com.c` and `nt_event.c` now includes both actual
worker/reference builds and their directly source-including fixtures. The
serial mock lifecycle passed, and the event fixture syntax/include checks
selected PROD in both graphs. Nine original-anchored existing exclusions
were confirmed; see `s13-bfs-depth1-prod-special.json`. The serial mock is not
physical COM acceptance. No source changed in this classification. First
depth: 4,066 groups / 5,040 arms; 1,851 observed retained, 660 existing
exclusions, 22 newly excluded and 2,507 pending. Restored original groups
remain 34; 147 original multiplicity candidates remain open.

The PIG first-depth review inventoried 2,836 generated commands across 15
current graphs, with no PIG definition flag, and found no project host/test
`#define` or `#undef PIG`. The old `cdefine.inc` PIG option is not part of
the generated active graphs. For same-hash sources with successful actual
product/test preprocessing and a uniquely anchored original outer condition,
64 existing sole-error arms in 17 files were confirmed. Per-file compilation
contexts and instrumentation hashes are in `s13-bfs-depth1-pig.json`. Files
without matching source snapshots or witnessed include contexts remain open;
no code or original group was restored by this pass. First depth: 4,066 groups
/ 5,040 arms; 1,851 observed retained, 724 existing exclusions, 22 newly
excluded, 2,443 pending. Original first-depth restorations remain 34; 147
original multiplicity candidates remain unreviewed.

DIV-575 replaces seven unsupported `#ifdef DEBUG` bodies in the actual
WOW32 `wkman.c` translation unit, preserving original top-level groups,
selected alternatives and measured logical lines. The actual x86 WOW
compiler/include chain contains no DEBUG definition or flag. The selected
preprocessor output matches before/after ignoring blank lines, not bytewise;
the x86 WOW object and x86/x64 positive/negative and line controls pass.
No intermediate WOW runtime test was claimed; full product/runtime gates are
pending. See `s13-bfs-depth1-debug-wkman.json` and its `-verification.json`.
Root: 4,066 groups / 5,040 arms; 1,851 observed retained, 724 prior exclusions,
29 newly excluded and 2,436 pending. Restored original first-depth groups
remain 34; 147 original multiplicity candidates remain unreviewed.

The remaining PIG review confirmed 16 existing guards using equal ordered
original/current first-depth group sequences and 33 independently verified
anchors in three same-hash CCPU files; adjacent identical excluded bodies
cannot independently reveal deleted-body ordering, which is recorded as a
provenance limit. Fresh product/reference include traces and unique original
anchors confirmed three more in the changed BIOS/timer files. Evidence:
`s13-bfs-depth1-pig-ordered.json` and `s13-bfs-depth1-pig-changed.json`.
The two PIG groups in unwitnessed DASM source files remain pending; absence
of a build observation alone is not treated as proof. No source changed.
First depth: 4,066 groups / 5,040 arms; 1,851 observed retained, 743 prior
exclusions, 29 newly excluded, 2,417 pending. Restored original first-depth
groups remain 34; 147 original multiplicity candidates remain unreviewed.

DIV-576 replaces 43 unsupported first-depth DEBUG arms in 24 actual WOW32
source units, preserving original group boundaries and three selected else
alternatives. Their actual x86 WOW compiler/include chains contain no DEBUG
definition or flag. Forty-six measured line probes cover selected alternatives
and following lines. The selected preprocessing of all 24 sources matches
before/after ignoring blank lines, not bytewise; all 24 x86 WOW objects and
x86/x64 positive/negative line controls pass. These intermediate objects have
not received WOW application runtime acceptance; full product gates remain
pending. See `s13-bfs-depth1-debug-wow-batch.json` and its
`-verification.json`. Root: 4,066 groups / 5,040 arms; 1,851 observed
retained, 743 prior exclusions, 72 newly excluded, 2,374 pending. Restored
original first-depth groups remain 34; 147 original multiplicity candidates
remain unreviewed.

DIV-577 replaces 20 unsupported DEBUG first-depth bodies in selected
`wow32.c` and `wcall16.c`, retaining original outer conditions and two selected
else alternatives. The actual x86 WOW and direct-including callback fixture
contexts contain no DEBUG flag or definition. Twenty-two pre-edit probes
measured selected and following logical lines. Both selected preprocessors
match before/after ignoring blank lines, not bytewise; x86 objects and x86/x64
guard controls pass. The callback-frame fixture passes four frame phases and
11 abort cases; it is a controlled host fixture, not full guest acceptance.
The first run passed behavioral assertions but its final PowerShell hash
pipeline failed; the harness now uses explicit `-LiteralPath` and a fresh
complete rerun passed. Evidence: `s13-bfs-depth1-debug-wow-special.json` and
its `-verification.json`. Full product/runtime gates remain pending. Root:
4,066 groups / 5,040 arms, 1,851 observed retained, 743 earlier exclusions,
92 newly excluded, 2,354 pending. Restored original first-depth groups remain
34; 147 original multiplicity candidates remain unreviewed.

A first-depth macro-cohort pass audited HUNTER, SPC486, SEGMENTATION, HERC,
NULLSTRUCT and REAL_VGA against 2,836 generated graph commands and project
macro definitions. For same-hash files actually reached by successful
product/test preprocessing, with unique original outer anchors and a sole
existing `#error`, it confirmed 166 prior exclusions: NULLSTRUCT 65,
SEGMENTATION 48, HUNTER 15, HERC 20 and REAL_VGA 18. No SPC486 body was
classified in this pass. The per-file actual contexts, hashes, and limits are
in `s13-bfs-depth1-macro-cohort.json`; unwitnessed or changed files and
non-sole-error bodies remain pending. No source or framework was changed.
Root: 4,066 groups / 5,040 arms; 1,851 observed retained, 909 existing
exclusions, 92 newly excluded, 2,188 pending. Restored original first-depth
groups remain 34; 147 original multiplicity candidates remain unreviewed.

DIV-578 replaces 35 unsupported SPC486 first-depth bodies in 15 SoftPC
units, retaining original outer conditions, 17 selected alternatives and
measured logical lines. Both actual x86 worker/reference compile/include
chains and project macro inventory contain no SPC486 definition. All 15
objects and x86/x64 guard controls pass. Selected preprocessing matches
ignoring blank lines in 14 files; c_reg differed only in the scratch `__FILE__`
path and matches after restoring the real logical filename. The selected
CCPU bit matrix is NOT a semantic pass: 64 of 118 cases fail in both isolated
pre-cleanup and current-source fixtures; all 118 full results match between
them and their exit statuses match the prior vector/direct comparison. This
pre-existing boundary is not concealed or attributed to the branch cleanup.
See `s13-bfs-depth1-spc486.json` and its `-verification.json`; full product
acceptance remains pending. Root: 4,066 groups / 5,040 arms, 1,851 observed
retained, 909 prior exclusions, 127 newly excluded, 2,153 pending. Restored
original first-depth groups remain 34; 147 original candidates remain open.

Further first-depth reconciliation confirmed 25 existing `#error` arms:
12 from original-order correspondence and 13 HUNTER arms from uniquely
anchored original correspondence. The latter use eight successful actual
product/reference include traces and two successful keyboard fixture traces;
their limits are recorded in `s13-bfs-depth1-macro-ordered.json` and
`s13-bfs-depth1-hunter-changed.json`. This classifies existing source only;
it does not add exclusions or restore condition structures. Root: 4,066
groups / 5,040 arms; 1,851 observed retained, 934 previously excluded,
127 newly excluded and 2,128 pending. The number of original first-depth
groups restored relative to the fixed HEAD remains 34, of which one was
restored during the present BFS pass. Another 147 original multiplicity
candidates remain unreviewed, so this count is not a final reconciliation.

The next same-hash cohort independently inventories 2,836 generated compile
commands and project definitions for SUPPORT_LPK, BIGEND, SFELLOW and CPU_286.
Using successful product/test preprocessing and unique pinned-original outer
anchors, it confirms 53 existing sole-error arms: SUPPORT_LPK 38, BIGEND 12,
SFELLOW 3; none of the CPU_286 candidates met these evidence requirements.
Details and unresolved boundaries are in `s13-bfs-depth1-macro-next-cohort.json`.
No source was changed and no original condition was restored in this cohort.
Root: 4,066 groups / 5,040 arms; 1,851 observed retained, 987 existing
exclusions, 127 newly excluded, 2,075 pending. Restored original first-depth
groups remain 34, including one in the current BFS pass; 147 original
multiplicity candidates remain unreviewed.

Latest first-depth snapshot: 4,066 current groups / 5,040 arms; 1,851
observed retained, 1,165 already excluded, 127 newly excluded, 1,897 pending.
Original-baseline reconciliation has 128 unreviewed candidate groups in 68
rows; 34 original first-depth groups were restored relative to fixed HEAD,
including one in the current BFS pass. The next-depth candidate count is
3,174, including children of still-pending parent arms. This snapshot does not
claim completion or alter the per-depth delivery gate.
Original first-depth subset reconciliation also resolved three groups in
`client/handles.c` and `kernel/inctlpan.c`: their pinned-original positions
are outside the respectively registered GetFullUserHandle body and
sysBmpStyles table. See `s13-bfs-depth1-original-small-subsets.json`.
No source or condition was restored. The current count of unreviewed original
multiplicity candidates is 125 groups in 65 rows; first-depth restorations
remain 34 relative to fixed HEAD, including one in the current BFS pass.
Further original first-depth subset reconciliation accounts for three groups
in `probe.c`, `random.c` and `exitwin.c`: each group ends before the sole
registered retained function begins. Source hashes, exact original positions
and registrations are recorded in
`s13-bfs-depth1-original-standalone-subsets.json`. The separate ALLOC_PRAGMA
candidate in probe.c remains open because it mentions the retained function;
no inference is made from its position alone. Original candidates remaining:
122 groups; restored groups remain 34 relative to fixed HEAD (one in this BFS
pass). No source or deeper condition was changed.
Two more original first-depth groups in `client/clmenu.c` lie after the
OPENNT-HOST-043/045 retained menu client/parser bodies; the original positions,
source hashes and boundaries are in `s13-bfs-depth1-original-clmenu.json`.
No condition was restored. Remaining original multiplicity candidates: 120
groups in 60 rows. Restored first-depth groups remain 34 relative to fixed HEAD,
including one in the current BFS pass. Current source arm counts are unchanged.
Original first-depth reconciliation resolved eight groups from `getset.c`,
`profile.c` and `rtl/draw.c` outside the specifically registered retained
function bodies, including the original profile table conditional whose mapping
is provided by the registered compatibility binding. Function ranges, original
positions and hashes are in `s13-bfs-depth1-original-function-subsets.json`.
No current source or retained condition was changed. Remaining original
multiplicity candidates: 112 groups in 56 rows; the count of restored original
first-depth groups is still 34 relative to fixed HEAD, one in the current BFS
pass. Current source arm counts remain 1,851 observed retained, 1,165 existing
exclusions, 127 newly excluded and 1,897 pending.
First-depth original reconciliation additionally resolved 16 groups in
`client/client.c`, `kernel/class.c` and `rtl/drawtext.c`. For each, both
original group boundaries were checked against every retained registered
function range, including the FE_SB group that remains inside GetPrefixCount.
Only groups with no retained-range intersection were classified outside the
subset. See `s13-bfs-depth1-original-selected-subsets.json`; no source changed.
Original candidates remaining: 96 groups in 48 rows. Restored first-depth
groups remain 34 relative to fixed HEAD, including one in the BFS pass.
DIV-579 restores two original first-depth MONITOR shells in `host/src/nt_com.c`
with sole `#error` bodies and `#line` preserving the prior logical positions.
Both actual x86 worker/reference contexts have no MONITOR flag or definition;
the source-linked serial mock passes before and after. Production x86 selected
preprocessor output matches after normalizing its sole scratch __FILE__ path;
raw output differs by that path. Isolated guard positive/negative controls pass;
the full-unit negative build stops at the earlier nt_eoi.h guard. These are
local checks, not a complete product or real-COM acceptance. Details are in
`s13-bfs-depth1-ntcom-monitor.json`.

Current first-depth snapshot: 4,068 groups / 5,042 arms, 1,851 observed
retained, 1,165 existing exclusions, 129 newly excluded, 1,897 pending.
Original multiplicity candidates remaining: 94 groups in 47 rows. Original
first-depth groups restored relative to fixed HEAD total 36, including three
in the current BFS pass. All 8,501 deeper arms remain unreviewed.
Original nt_graph.c line 760 maps to the current first-depth DIV-322 CCPU
condition at line 690; original source and retained-body anchor evidence is in
`s13-bfs-depth1-original-ntgraph.json`. Both generated actual worker contexts
define CCPU and have no CCPU undefinition. DIV-580 replaces the unselected
non-CCPU/non-X86GFX body with `#error`, retaining the adapted outer condition
and logical line numbers. The x86 object builds; all four isolated CCPU/X86GFX
combinations give the expected result; selected preprocessing matches after
normalizing four scratch __FILE__ paths, and injected `__LINE__` probes match
at 782. These do not replace full product tests. Evidence:
`s13-bfs-depth1-ntgraph-ccpu.json`.

First-depth current snapshot: 4,068 groups / 5,042 arms, 1,851 observed
retained, 1,165 existing exclusions, 130 newly excluded, 1,896 pending.
Original multiplicity candidates remaining: 93 groups. Restored original
first-depth groups remain 36 relative to fixed HEAD (three in this BFS pass).
Original first-depth reconciliation mapped seven `mouse_io.c` NTVDM groups to
registered DIV-318 non-MONITOR route conditions, plus `rom.c` group 282 to
DIV-165 and `video.c` groups 1579/2042 to their standalone-SAS adaptations.
The mouse mapping is in `s13-bfs-depth1-original-mouse-route.json`; ROM/video
and their compile-context evidence are in `s13-bfs-depth1-video-sas.json`.
No mouse or ROM source was modified. DIV-581 records the two existing video
selector adaptations and replaces only their unselected direct-write bodies
with `#error`; the x86 video object built, before/after selected preprocessing
is raw byte-identical, and eight isolated guard checks passed. These checks do
not assert complete VGA or product acceptance. A similar second-depth condition
was not assessed.

First-depth current snapshot: 4,068 groups / 5,042 arms; 1,851 observed
retained, 1,165 existing exclusions, 132 newly excluded, 1,894 pending.
Original multiplicity candidates remaining: 83 groups in 43 rows. Restored
first-depth groups remain 36 relative to fixed HEAD, three in this BFS pass.
Four DPMI first-depth original `#ifndef i386` groups map to the existing
CPU40-or-non-i386 conditions in `dpmiint.c` and `dpmiint.h`; the original
software-executor selection is retrospectively registered as DIV-582. Three
original WOW32 FASTBOPPING groups map to CPU40-first groups retaining
FASTBOPPING as a same-depth alternative; the existing selection is registered
as DIV-583. Six existing excluded alternatives were classified using actual
WOW32 CPU40 compile contexts and fixture graph requirements. These are source
selection and structural evidence, not full DPMI or fast-BOP behavior proof.
See `s13-bfs-depth1-original-dpmi-selectors.json` and
`s13-bfs-depth1-wow32-fast-selectors.json`. No executable source changed.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,171 previously excluded, 132 newly excluded, 1,888 pending. Original
multiplicity candidates remaining: 76 groups in 40 rows. Restored original
first-depth groups remain 36 relative to fixed HEAD, three in this BFS pass.
First-depth original reconciliation confirmed that `clglobal.c` debug and Wx86
condition groups lie outside OPENNT-HOST-092's retained scalar/table subset.
The `base/win32/client/vdm.c` OLD_CFG_BASED group encloses an omitted,
old-signature BaseGetVDMKeyword; the retained caller uses the separately owned
modern-signature provider. Source identities, bounds, and limits are in
`s13-bfs-depth1-original-clglobal.json` and
`s13-bfs-depth1-original-vdm-config.json`. No runtime source changed. First-
depth original candidates remaining: 73 groups. Restored groups remain 36
relative to fixed HEAD, including three in this BFS pass. Current-source arm
counts are unchanged.
The original mouse callback CPU30 condition at line 6625 maps to the existing
CPU40 condition at current line 5897 under DIV-222. Both anchors select the
unadjusted callback offset; the selected current arm was product-observed and
its alternative remains pending. See `s13-bfs-depth1-original-mouse-callback.json`.
No source or deeper condition changed. First-depth original candidates now
remaining: 72 groups. Restored groups remain 36 relative to fixed HEAD (three
in this BFS pass); current-source arm counts are unchanged.
A fresh first-depth per-file MONITOR/X86GFX cohort captured 44 successful
actual x86 worker/reference compiler include traces, including the two
compilations of the direct-including `nt_event_observed.c` fixture. Among
current-hash C files with both production contexts and no per-source macro
definition, it matched unique pinned-original outer boundaries and verified
sole existing `#error` bodies. It classified 117 prior exclusions in 21
files: MONITOR 58, X86GFX 59. No global claim is made for those macros; the
video unit is deliberately outside this cohort. No source or inner branch was
changed. Traces and individual correspondence are in
`s13-bfs-depth1-host-macro-contexts.json`.

Current first-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed
retained, 1,288 prior exclusions, 132 newly excluded, 1,771 pending. Original
multiplicity candidates remain 72 groups. Restored original first-depth groups
remain 36 relative to fixed HEAD, three in this BFS pass. All 8,501 deeper
arms remain unreviewed; per-layer full gates remain pending.
Six host headers were checked against 161 historical actual inclusion contexts,
each mapped to fresh successful current-source compilation. All ten generated
TU contexts that explicitly set MONITOR or X86GFX were also freshly compiled
and verified not to include these headers. Ten existing first-depth #error
arms were matched to pinned original outer boundaries and classified (four
MONITOR, six X86GFX). Evidence is in
`s13-bfs-depth1-host-header-contexts.json`.

The remaining `nt_fulsc.h` X86GFX arm was an actual unselected declaration
body. DIV-584 preserves its original condition and replaces the body with
#error; this branch ends at EOF. Cached nt_graph and nt_com object compiles
passed, nt_com selected preprocessing was byte-identical, nt_graph selected
nonblank preprocessing matched, and positive/negative isolated guard controls
passed. These do not establish full runtime or product acceptance. Details are
in `s13-bfs-depth1-ntfulsc-header.json`.

Current first-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed
retained, 1,298 existing exclusions, 133 newly excluded, 1,760 pending.
Original multiplicity candidates remain 72 groups; restored first-depth groups
remain 36 relative to fixed HEAD, including three in this BFS pass.
DIV-585 replaces eleven first-depth WOWPROFILE bodies in `wmdisp32.c` and
`wudlg.c`, preserving original condition shells and all eleven measured
following logical line positions. Current compiler/include traces establish
release WOW32 macro selection, and the removed bodies match the pinned
originals. Cached x86 WOW32 objects build; both selected preprocess outputs
match ignoring blank lines; all 22 isolated guard controls pass. Evidence:
`s13-bfs-depth1-wowprofile-batch.json` and
`s13-bfs-depth1-wowprofile-verification.json`. Full product acceptance remains
open. The historical extract-wow-wndproc-fixture.ps1 byte-exact source hash
check was already inconsistent before this batch; its contract and dependent
fixture require reconciliation before layer closure.

Current first-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed
retained, 1,298 previous exclusions, 144 newly excluded, 1,749 pending.
Original multiplicity candidates remain 72; restored first-depth groups remain
36 relative to fixed HEAD, including three in this BFS pass.
DIV-586 replaces fourteen first-depth WOWPROFILE/DEBUG_OR_WOWPROFILE bodies
in `wow32.c` and `wowtbl.c`, retaining original condition boundaries and
fourteen measured following logical lines. Each body matches the fixed original;
actual WOW32 compiler/include contexts select the non-profile path. Cached x86
objects built, selected preprocessing matched ignoring blank lines, and all 28
isolated guard controls passed. Evidence: `s13-bfs-depth1-wowprofile2-batch.json`
and `s13-bfs-depth1-wowprofile2-verification.json`. Full runtime/product tests
remain pending, including the previously identified WndProc fixture mismatch.

Current first-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed
retained, 1,298 previous exclusions, 158 newly excluded, 1,735 pending.
Original multiplicity candidates remain 72; restored first-depth groups remain
36 relative to fixed HEAD, including three in this BFS pass. All 8,501 deeper
arms remain unreviewed.
The stale WndProc extraction gate is reconciled: the extractor verifies the
fixed original source identity, extracts the current selected mirror function
byte-for-byte, records both source/body hashes, accepts validated absolute or
relative build paths, and does not assert that the full adapted source equals
NT4. Its bounded dispatch fixture now links the extracted function only once;
its explicitly unreachable conversion sentinels remain fail-closed and distinct
from the map-gated unrelated-original-service sentinel. The hash collection
pipeline now uses literal file arguments. A clean full fixture rerun passed
eight exception phases, five hard-error modes, the debugger-first-chance child,
and the unrelated-service map gate. See `s13-bfs-depth1-wndproc-fixture.json`.
This is controlled host evidence, not real Win16 acceptance. First-depth source
counts and 36 restored original groups (three in the BFS pass) are unchanged.
DIV-587 replaces eleven unselected first-depth DEBUG_OR_WOWPROFILE bodies in
WOW32 thunk-table headers, preserving their original outer conditions. Ten
bodies were replaced line-for-line and the remaining group ends at EOF. The
pinned-original bodies match; actual include contexts were collected and 2,836
generated commands had no DEBUG/WOWPROFILE profile flags. The two unobserved
headers remain pending. Before/after selected preprocessing of wow32.c and
wowtbl.c matches after scratch path normalization; 22 isolated guard controls
pass; the cached WOW32 DLL graph completed 91 steps; the controlled dispatch
exception fixture passed again. Evidence:
`s13-bfs-depth1-wowprofile-headers.json`. This is not a full product or guest
acceptance.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,298 prior exclusions, 169 newly excluded, 1,724 pending. Original candidates
remain 72 groups. Restored original first-depth groups remain 36 relative to
fixed HEAD, including three in this BFS pass. Deeper arms remain unreviewed.
First-depth `#if DBG` reconciliation used fresh successful worker and
worker-reference compiler/include traces for six selected redirector, DPMI and
DEM source units. No trace defines or undefines DBG or supplies a DBG flag;
direct-source fixtures inherit the production profile. Matching complete
original/current group-kind sequences and non-conflicting unique anchors
classified 44 existing #error arms: 35 in vrnetapi/vrnmpipe and nine in
int21map/xmem/demsrch/vrmslot. Identical unanchored blocks are assigned by
order, an explicit evidence limitation. See
`s13-bfs-depth1-vdmredir-dbg.json` and `s13-bfs-depth1-dpmi-dem-dbg.json`.
No source or nested condition changed.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,342 existing exclusions, 169 newly excluded, 1,680 pending. Original
multiplicity candidates remain 72; restored first-depth groups remain 36
relative to fixed HEAD, three in the current BFS pass.
DIV-588 replaces 35 first-depth historical i386 debugger bodies with
#error, preserving original condition shells, 21 selected alternatives and
56 measured diagnostic positions. All removed bodies match their pinned
original counterparts; both generated worker contexts lack the historic i386
macro. Cached x86 dbg.obj built, selected preprocessing matched ignoring blank
lines, and all 70 isolated guard controls passed. Two independent Win32
debugger transport/module layout smoke tests also passed; those smokes do not
execute the modified dbg.c body. Evidence: `s13-bfs-depth1-debugger-i386-batch.json`
and `s13-bfs-depth1-debugger-i386-verification.json`. Full product and real
guest acceptance remain pending.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,342 existing exclusions, 204 newly excluded, 1,645 pending. Original
multiplicity candidates remain 72; restored first-depth groups remain 36
relative to fixed HEAD, three in this BFS pass. All 8,501 deeper arms remain
unreviewed.
DIV-589 replaces six original first-depth historical i386 branches across
`wkgthunk.c`, `wgprnset.c`, `wkmem.c` and `wow32.c`, preserving all outer
conditions, the selected alternative and seven measured logical positions.
Their removed bodies match the pinned originals; the actual WOW32 source
contexts have no historic i386 flag or source/header definition. Four cached
x86 objects built; selected preprocessing matched ignoring blanks; 12 isolated
guard checks and the controlled dispatch fixture passed. This is not full
printing, floating-point, guest or release acceptance. See
`s13-bfs-depth1-wow-i386-batch.json` and its `-verification.json`. The DPMI
conditional with an explicitly supported CPU40/i386 test remains separate.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,342 existing exclusions, 210 newly excluded, 1,639 pending. Original
multiplicity candidates remain 72; restored original first-depth groups remain
36 relative to fixed HEAD, including three in this BFS pass.
First-depth PROD reconciliation classified 45 existing sole-error guards in
nine selected host units after fresh x86 worker/reference compiler traces
proved `/DPROD` without overrides and original outer group sequences matched
with non-conflicting anchors. The 28 `nt_rflop.c` guards have 28 independent
ordinal anchors; the other 17 are in com.c, trace.c, main.c, ica.c, timer.c,
nt_bop.c and nt_wcom.c. The old T280 PDB syntax script supplies a non-PROD
profile but references removed include directories; a direct same-flags
probe fails on missing ntpsapi.h, so it is recorded as a stale non-working
historical graph, not silently counted as a supported passing build. This does
not prove non-PROD runtime behavior. Details:
`s13-bfs-depth1-rflop-prod.json`, `s13-bfs-depth1-prod-extra.json`. Non-sole
bodies, uncompiled units and nested conditions remain pending. No source changed.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,387 existing exclusions, 210 newly excluded, 1,594 pending. Original
multiplicity candidates remain 72; restored original first-depth groups remain
36 relative to fixed HEAD, including three in this BFS pass.
DIV-590 replaces three first-depth non-PROD bodies in BIOS reset and timer
units with #error, retaining outer conditions and three measured following
logical positions. The 20 nested unreviewed groups are subsumed and removed
from the next-depth inventory, without auditing their contents. Existing
DIV-498/520/522/525/530/535 registrations remain applicable elsewhere;
no whole-project registration was removed. Both selected preprocess outputs
match ignoring blank lines, x86 objects build, six isolated guard controls pass,
and source-linked PIC reset (24 assertions) and keyboard/PIC (86 assertions)
fixtures pass. Details: `s13-bfs-depth1-nonprod-outer-batch.json` and
`s13-bfs-depth1-nonprod-outer-verification.json`. Full real-guest and product
acceptance remain pending.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,387 existing exclusions, 213 newly excluded, 1,591 pending. Original
multiplicity candidates remain 72; restored first-depth groups remain 36
relative to fixed HEAD, including three in this BFS pass. Remaining deeper
unreviewed arm count is 8,480 after removing the subsumed nested inventory.

First-depth NTVDM reconciliation classified nine existing sole-error guards in
six source units, based on actual selected x86 worker/reference and relevant
fixture include contexts. DIV-591 replaces four additional proven unselected
outer bodies in get_env, keyba, ica and illegalp; ten nested unreviewed groups
are subsumed without auditing them. Six logical diagnostic positions are
preserved. Selected preprocessed output matches ignoring blank lines; four
x86 objects, eight isolated guard controls, PIC (24/24), keyboard/PIC (86/86)
and the mirror guard suite (235 guards plus six profile checks) pass. See
`s13-bfs-depth1-ntvdm-extra.json`, `s13-bfs-depth1-ntvdm-outer-batch.json`
and `s13-bfs-depth1-ntvdm-outer-verification.json`. Traces refer to pre-edit
snapshots. These results do not establish guest or complete release coverage.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,396 existing exclusions, 217 newly excluded, 1,578 pending. Original
multiplicity candidates remain 72. Relative to fixed HEAD, 36 previously
deleted first-depth groups were restored, including three in this BFS pass;
this count is separate from exclusions. Remaining deeper arms: 8,468, all
unreviewed. First depth remains open; no level-wide build or release claimed.

First-depth compound NTVDM && MONITOR reconciliation classified 13 existing
sole-error arms: seven in mouse_io.c and six in six other source files.
Each classified arm has a matching pinned-original outer condition and unique
preceding anchor; current-hash actual x86 worker and worker-reference compiler
include traces prove MONITOR was not defined in those contexts. Sixteen new
traces succeeded for the other eight files. Six unmatched source arms and the
CPU header require separate evidence and remain pending. No source or nested
condition changed. See `s13-bfs-depth1-mouse-compound.json` and
`s13-bfs-depth1-compound-monitor.json`; these are not runtime proof.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,409 existing exclusions, 217 newly excluded, 1,565 pending. Fixed-HEAD
restored original first-depth groups remain 36, including three in this BFS
pass. Original multiplicity candidates remain 72; deeper arms 8,468 remain
unreviewed. The first-depth gate remains open.

First-depth follow-up matched eight previously pending MONITOR compound
sole-error arms using unique following anchors and the pinned original outer
conditions. Worker/reference same-hash compiler traces show MONITOR undefined.
The eight arms comprise five across four files and three further mouse arms.
Only classification changed, with no edits or inspection of nested semantics.
Evidence: `s13-bfs-depth1-compound-following.json` and
`s13-bfs-depth1-mouse-compound-extra.json`. Other contexts and the remaining
unmatched guards still require separate proof.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,417 existing exclusions, 217 newly excluded, 1,557 pending. Restored
original first-depth groups remain 36 relative to fixed HEAD, including three
in this BFS pass. Original multiplicity candidates remain 72; 8,468 deeper
arms remain unreviewed. First-depth gate is still open.

An additional tape_io.c NTVDM/MONITOR guard matched a unique original
preceding anchor after ignoring diagnostic #line directives; the current
following content contains a registered local instrumentation patch, so it
was not treated as an exact following anchor. Both actual same-hash x86
worker/reference include traces exclude MONITOR. Evidence is in
`s13-bfs-depth1-tape-compound.json`. No code changed.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,418 existing exclusions, 217 newly excluded, 1,556 pending. Original
first-depth groups restored remain 36 relative to fixed HEAD, including three
in the BFS pass; 72 original multiplicity candidates remain. First depth has
not passed its build, release, test and submission gate.

First-depth SHORT_TRACE reconciliation classified eleven existing sole-error
branches in com.c using unique pinned-original preceding anchors and fresh
same-hash worker/reference compiler traces without the SHORT_TRACE macro.
Three other SHORT_TRACE arms remain pending because these anchors do not match.
No source code or deeper branch was changed; see
`s13-bfs-depth1-short-trace.json`.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,429 existing exclusions, 217 newly excluded, 1,545 pending. Original
first-depth groups restored remain 36 relative to fixed HEAD (three in this
BFS pass). The 72 original multiplicity candidates and all 8,468 deeper arms
remain unresolved and unreviewed respectively. First-depth release gate open.

First-depth SEGMENTATION reconciliation classified nine existing sole-error
guards in six host C files using unique original outer anchors and fresh
same-hash x86 worker/reference compiler include traces. The direct-including
keyba observation fixture was also traced in both graphs. Units without a
working build context and guards without a unique anchor remain pending. No
source code or deeper branch changed. Evidence:
`s13-bfs-depth1-segmentation.json`. No runtime or guest validation implied.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,438 existing exclusions, 217 newly excluded, 1,536 pending. Restored
original first-depth groups remain 36 relative to fixed HEAD, including three
in this BFS pass. Original multiplicity candidates remain 72; deeper arms
remain unreviewed. First-depth release gate still open.

First-depth FIFO_ON reconciliation classified nine existing sole-error
NTVDM && FIFO_ON arms in com.c. Their original outer structures have unique
preceding or following anchors, and fresh same-hash actual x86 worker and
worker-reference traces contain no FIFO_ON definition or compiler flag.
Evidence: `s13-bfs-depth1-fifo-on.json`. No source or nested branch changed.
The separate msvc-*-formal-release and pkg-uuid-* Ninja files found for
com.c still reference the old, nonexistent src/mvdm path; those historical
files are not evidence of an active current-source configuration. The
complete supported-context audit remains open.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,447 existing exclusions, 217 newly excluded, 1,527 pending. Original
first-depth groups restored remain 36 relative to fixed HEAD (three in BFS).
The 72 original multiplicity candidates and 8,468 unreviewed deeper arms
remain. The level-wide build, release, test and submission gate is open.

First-depth legacy-source reconciliation classified 23 existing sole-error
arms using unique original outer anchors and 32 fresh same-hash compiler
include traces across worker/reference and direct-including fixtures. Counts:
GISP_SVGA 5, SYNCH_TIMERS 5, SCCSID 5, IRET_HOOKS 5, SECURE 3. Macro
definitions were evaluated per macro and source; YODA remains pending because
an included header contains a conditional definition. No source or deeper
branch changed. Evidence: `s13-bfs-depth1-legacy-sources.json`. These traces
alone do not prove every supported context or runtime behavior.

First-depth snapshot: 4,068 groups / 5,042 arms; 1,851 observed retained,
1,470 existing exclusions, 217 newly excluded, 1,504 pending. Restored
original first-depth groups remain 36 relative to fixed HEAD, three in this
BFS pass. Original multiplicity candidates remain 72, deeper arms remain
unreviewed, and the first-depth release gate remains open.

First-depth original-boundary investigation found cpu_vid.h original line 90
NO_STRING_OPERATIONS guard duplicated at current lines 94/101 inside the
DIV-089 CPU40/C_VID type adaptation. Twenty-two fresh real include contexts
compiled with CPU40/C_VID set and NO_STRING_OPERATIONS unset; the source,
original hashes and exact contexts are recorded in
`s13-bfs-depth1-original-cpu-vid.json`. The original first-depth guard has
not yet been restored. Reordering must retain both declaration variants and
logical diagnostics, and requires context-complete validation and queue
remapping. No inner disposition has been decided. This candidate remains in
the original-boundary unresolved inventory; no restoration counted.

DIV-592 restores the original first-depth NO_STRING_OPERATIONS guard in
cpu_vid.h, while retaining the DIV-089 typed CPU40/C_VID and alternate
declarations. The new inner selector is queued unreviewed. All 22 actual
worker/reference inclusion-context preprocessor outputs match before/after
ignoring blank lines; three representative x86 objects compile (with existing
warnings). An extracted declaration matrix covers eight macro combinations:
outputs and measured logical line positions match. One initial object build
command used a wrong target path and was corrected; the corrected build
passed. Evidence: `s13-bfs-depth1-original-cpu-vid.json` and task build logs.
These checks do not replace the full layer release and guest acceptance.

First-depth snapshot: 4,069 groups / 5,043 arms; 1,852 observed retained,
1,470 existing exclusions, 217 newly excluded, 1,504 pending. Restored
first-depth groups total 37 relative to fixed HEAD, four in this BFS pass.
One original multiplicity candidate is resolved; 71 remain. The depth-2
queue was remapped without deciding the newly nested selector. First-depth
release gate remains open.

DIV-593 restores the original first-depth X86GFX EOF guard in nt_sec.c and
replaces its body with #error. Two actual same-hash worker/reference x86
include contexts exclude X86GFX; the x86 object compiled with warnings, and
normal/unsupported isolated guard controls passed. Before/after product
preprocessing matches after normalizing the scratch input pathname and ignoring
blank lines. The unnormalized first attempt exposed only expected __FILE__
path differences; __LINE__ values matched. Evidence:
`s13-bfs-depth1-original-nt-sec.json`. No guest or full-layer acceptance.

First-depth snapshot: 4,070 groups / 5,044 arms; 1,852 observed retained,
1,470 existing exclusions, 218 newly excluded, 1,504 pending. Restored
first-depth groups total 38 relative to fixed HEAD, five in this BFS pass.
Seventy original multiplicity candidates remain. First-depth gate open.

DIV-594 restores the original first-depth i386 selector around the DIV-077
XMS copy adaptation. The selected non-i386 adapter remains intact; the
historical i386 branch is rejected. Two real worker/reference x86 source
contexts exclude i386. Before/after product preprocessing matches after
normalizing the scratch filename and ignoring blank lines; xmsblock.obj
compiled with existing warnings, and selected/rejected guard controls passed.
Logical line positions are preserved. Evidence:
`s13-bfs-depth1-original-xms.json`. No guest runtime or full-layer release
acceptance is claimed.

First-depth snapshot: 4,071 groups / 5,046 arms; 1,853 observed retained,
1,470 existing exclusions, 219 newly excluded, 1,504 pending. Restored
first-depth groups total 39 relative to fixed HEAD, six in this BFS pass.
Sixty-nine original multiplicity candidates remain. First-depth gate open.

OPENNT-HOST-144 restores the original first-depth ALLOC_PRAGMA guard in
probe.c, rejecting the unsupported kernel allocation profile while retaining
the user-mode binding and #line diagnostics. Three current worker/reference
and WOW compiler contexts exclude that macro. Worker and WOW preprocessing
outputs match before/after following scratch filename normalization and blank
line filtering; the two x86 objects compile; normal/rejected guard controls
pass. Evidence: `s13-bfs-depth1-original-probe.json`. No whole-layer or
runtime acceptance is claimed.

First-depth snapshot: 4,072 groups / 5,047 arms; 1,853 observed retained,
1,470 existing exclusions, 220 newly excluded, 1,504 pending. Restored
first-depth groups total 40 relative to fixed HEAD, seven in this BFS pass.
Sixty-eight original multiplicity candidates remain. First-depth gate open.

OPENNT-HOST-145 restores the original first-depth _USERK_ codepage selector
in chartran.c, rejecting the kernel USER arm while retaining the existing
OPENNT-HOST-119 user-mode adapter and logical diagnostics. Both actual WOW
and call-boundaries contexts exclude _USERK_. Their before/after preprocessing
outputs match after scratch filename normalization and blank line filtering;
both x86 objects compile; normal/rejected guard controls pass. Evidence:
`s13-bfs-depth1-original-chartran.json`. A second unmatched original _USERK_
condition remains open. No whole-layer or runtime acceptance is claimed.

First-depth snapshot: 4,073 groups / 5,049 arms; 1,854 observed retained,
1,470 existing exclusions, 221 newly excluded, 1,504 pending. Restored
first-depth groups total 41 relative to fixed HEAD, eight in this BFS pass.
Sixty-seven original multiplicity candidates remain. First-depth gate open.

DIV-595 replaces three unselected local DEBUG initialization bodies in
wow32.c with #error, retaining their first-depth conditions, the non-debug
registry facade and measured diagnostic line numbers. The current WOW build
and its dispatch fixture use the release macro profile. Selected preprocessing
matches after scratch filename normalization and ignoring blank lines; the
WOW object compiles with existing warnings, six isolated guard controls pass,
and the controlled W32Dispatch/W32Exception fixture exits zero. These local
branches include initialization adaptation, not three original-structure
restorations. Evidence: `s13-bfs-depth1-wow-local-debug.json`. No guest or
whole-layer release acceptance claimed.

First-depth snapshot: 4,073 groups / 5,049 arms; 1,854 observed retained,
1,470 existing exclusions, 224 newly excluded, 1,501 pending. Restored
first-depth groups total 41 relative to fixed HEAD, eight in this BFS pass.
Sixty-seven original multiplicity candidates remain. First-depth gate open.

OPENNT-HOST-146 replaces eight first-depth DEBUG USER bodies in six source
files; each removed body matched its pinned-original counterpart. Both actual
WOW and call-boundaries release compiler contexts exclude DEBUG, including
queue fixture compile flags derived from the product command. Selected WOW
preprocessing matches before/after in all six files after scratch-path
normalization and blank filtering; six x86 objects and eight isolated
normal/rejected guard controls pass, and 30 remapped queue directives match
source. All 29 controlled call-boundary fixture programs pass. The first
broad fixture build hit its script-owned deadline after 98/143 steps; after
all ninja/cl processes had exited, a new evidence directory reused the
intermediates and completed. Initial source-probe and invocation mistakes
were corrected and are recorded in `s13-bfs-depth1-user-debug.json`. This
does not establish guest runtime behavior or whole-layer acceptance.

First-depth snapshot: 4,073 groups / 5,049 arms; 1,854 observed retained,
1,470 existing exclusions, 232 newly excluded, 1,493 pending. Restored
first-depth groups remain 41 relative to fixed HEAD, eight in this BFS pass.
Sixty-seven original multiplicity candidates remain. First-depth gate open.

Owner resumed S13 with a two-phase gate at each depth: prioritize proven
excluded arms, then build/publish/test/commit/push that exclusion phase, and
only afterward audit selected arms and enqueue the next depth. Unknown
contexts cannot be classified as excluded; earlier mixed-pass records remain
as historical evidence.

DIV-596 replaces seven first-depth DEBUG bodies in six WOW host headers
with #error; all removed bodies match pinned originals. Eighty-eight actual
WOW source contexts exclude DEBUG; original VDMEXTS is outside the admitted
product closure. A representative actual selected WOW preprocessor output
matched before/after; x86 wow32.obj compiled, seven normal/rejected controls
and their #line checks passed, and the existing 235-guard suite passed. The
direct wparam fixture passed 54 current scenarios and its baseline negative
controls, with identical selected original cache preprocessing. Its stale
baseline path mapping, pre-existing reverse-record size expectation (DIV-363
adds eight bytes), and raw DEBUG-body comparison were corrected, with
unchanged-checkpoint comparisons documented in
`s13-bfs-depth1-wow-header-debug.json`. Guest-associated wowfax.h remains
pending; no guest runtime or full-layer release acceptance is claimed.

First-depth snapshot: 4,073 groups / 5,049 arms; 1,854 observed retained,
1,470 existing exclusions, 239 newly excluded, 1,486 pending. Restored
first-depth groups remain 41 relative to fixed HEAD, eight in the BFS pass.
The working inventory contains 3,143 depth-2 groups (5,304 arms); their
dispositions remain unreviewed and their final eligibility depends on the
first-depth retained-arm audit. Sixty-seven original multiplicity candidates
remain. The exclusion-phase delivery gate is still open.

The owner requested a single complete first-depth exclusion audit and one
batch before compilation. The structural inventory covers 5,049 current
arms and checks all 1,636 pinned mirror files without hash mismatch. Current
preprocessing completed for 829 header-consumer contexts without failure;
no textual include additions were found since the queue baseline, while 175
include removals require conservative treatment. Candidate source scans and
original multiplicity hints are recorded in the depth1 audit JSON files.
The CPU decode diagnostics test selects a branch omitted by default product
builds, proving default compilation alone is insufficient. A source-and-flag
cross-check also found six further candidate profile intersections; their
actual support and selection remain to be verified. The 67 original
multiplicity mismatches remain candidates, not proven missing structures.
No post-inventory source batch or full build has been performed.
The complete first-depth source candidate inventory now deduplicates 180
branch arms; 60 have literal test/build references. Of the 34 simple header
candidates, 31 match the pinned original by normalized lines; 24 are included
by an instrumented test context and 23 have additional possible direct test
or guest inclusions. Six original groups are byte-equivalent modulo line
endings at already-present nested positions. The current video controller
also explicitly enables X86GFX, so its value cannot be treated as global.
These observations are audit evidence only, not final exclusion decisions.
Compiler commands and the 829-context header trace have been archived and the
archive integrity checked. No additional product source batch or build has
been performed since the owner requested a single combined batch.
Correction: physical line verification must split on LF, not text splitlines,
because some pinned files contain form-feed. All 5,049 current first-depth
arm directives and 4,219 original first-depth group directives were checked
against LF line numbers with zero mismatches. Recomputing original adjacent
contexts changed one candidate from unknown to matching; 106 of 124 uniquely
mapped candidates now match. The scratch compiler probes use byte LF lines
and were not affected. A server.c FE_IME table framework absent beneath its
file-wide selector was identified for the next depth only; the current depth
does not mutate or classify that nested arm. Parallel QUEUE.md and proposal
changes appeared during this audit and remain untouched.
First-depth original branch work found a concrete missing FE_IME framework in
server.c inside the existing file-wide WOW selector. It is recorded as a
next-depth structural candidate and was not edited at this depth. Product
preprocessor boundary probes succeeded for 141 source boundaries and four
FAST header boundaries. The earlier text-splitting line issue has been
corrected in the source-context artifact; one context classification changed.
The 121 observed-file unguarded arms not covered by initial candidate scans
are inventoried explicitly; their 503 product contexts are being captured with
68 exact macro names and exact compiler-flag tokens. No source batch/build,
release, commit or push has occurred in this resumed audit phase.

The later complete graph include collection covered 2,070 actual host compiler
contexts and 29 generated-source contexts, all successfully preprocessed. Ten
generated-source contexts include mirror headers. All 1,636 tracked mirror
file hashes still matched the captured working snapshot immediately before the
batch. Evidence: `s13-bfs-depth1-all-graph-context-summary.json` and its ZIP.

The consolidated batch replaced 104 first-depth excluded arms in 52 files,
preserving their original outer conditions and selected-path logical lines;
17 unreviewed inner groups were subsumed. One previously removed original
first-depth `DEBUG` group in clmsg.c was restored and its body rejected.
All 104 resulting bodies and line-restoration placements passed structural
checks. See `s13-bfs-depth1-consolidated-batch.json` and the clmsg preflight.
The current queue has 4,074 first-depth groups / 5,050 arms: 1,854 observed
retained, 1,470 previously excluded, 344 newly excluded and 1,382 pending.
Those pending arms and missing-original-structure candidates are not claimed
as resolved. The cached x86 worker/WOW32 and x64 native component builds
completed successfully; build success is not proof for untested profiles.
The staged ten-image candidate passed all 16 Full product gates, four
Windows 3.1 Standard/Enhanced and DOSONLY combinations, and the isolated
DOS4GW/NESticle graphics-startup witness. Six checked guest system files
matched their original hashes after the matrix. The ten candidate hashes were
published and verified at both release destinations. See
`s13-bfs-depth1-consolidated-full-test.json` and
`s13-bfs-depth1-consolidated-win31.json`; the DOS4GW witness does not assert
gameplay. This is a progress baseline, not completion of the first layer.
After that delivery, the retained-profile suite also passed 235 guard cases
plus DPMI, ANSI/Unicode, CPU40 and USER profile controls on x86.
Another first-depth marker pass succeeded in all 2,070 current build-graph
contexts. Four getset.c alternatives are selected by the call-boundaries
tests and must be retained; their witnesses are in
`s13-bfs-depth1-pending-positive.json`. The remaining unobserved arms are
not excluded merely because this pass did not select them.
The batch queue initially attached the restored clmsg DEBUG selector to
chartran.c due to loop-variable reuse. The source was correct. The queue is
corrected in `s13-bfs-depth1-corrected-queue.zip`; its 5,050 first-depth arms
now match the physical directives, with zero mismatches. Earlier post-batch
queue snapshots are superseded; see `s13-bfs-depth1-queue-correction.json`.
The separately supported CCPU decode diagnostic profile selects another arm:
the normal object omits its five observers, while an explicitly diagnostic
object contains all five. Its historical fixture's Git lookup was corrected
for the old pre-rename path and the fixture passed. See
`s13-bfs-depth1-diagnostic-profile-current.json`.
Current status: 1,859 observed retained, 1,470 previously excluded, 344
newly excluded and 1,377 pending; of the pending arms, 252 already contain
only `#error` and 1,125 still require a disposition. The corrected latest
queue is `s13-bfs-depth1-current-queue.zip`.

An independent DPMI fixture requires an additional `/UCCPU` object profile.
Its uninstrumented preprocessing currently fails at an already-existing
DIV-515 guard (present before the consolidated batch) nested at depth 2 under
the retained `#ifndef i386` arm. It is an explicit next-depth review item,
not a demonstrated first-depth exclusion, and has not been changed here.
The test-only compile profile is not evidence that hardware V86 monitor or a
non-CCPU product is supported. See `s13-bfs-depth1-dpmi-nonccpu-profile.json`.

Eight standalone SoftPC object-contract fixtures were also run: six passed;
GFI's non-PROD fixture fails at an existing depth-2 DIV-504 header guard,
and NIDDB reproduces the separately recorded inactive-child retirement
counterexample. Both are recorded in
`s13-bfs-depth1-independent-contracts.json`; neither is counted as a passing
fixture or silently repaired outside the current depth.

The next uncommitted depth-1 group has replaced 188 further arms (46 source,
20 header, four NTVDM alternatives, two BaseSrv DEVL arms, six x87
alternate-endian arms, eight host-header alternatives, one ICA header arm,
two Video7 header arms, and two non-WIN32 memalloc alternatives, five PM-disabled alternatives, and four unselected DOUBLE_CPU, DOCUMENTATION
and NOTUSEDNOTUSED arms, nine further header arms, four compound-condition header arms, seven legacy BIOS/driver arms, two HFX header arms, two BIOS ANSI alternatives, three WOW callback/parameter arms, three platform alternatives, three timing-header alternatives, two additional non-WIN32 allocator arms, three WOW header arms, thirteen host video source alternatives, one RTL alternative, the excluded SoftPC PROFILE body, six legacy host-source arms, six WOW redirect alternatives, two C_VID source alternatives, three non-NTVDM source alternatives, one x87 header alternative, four unsupported DPMI CPU40 alternatives, one unsupported boot-profile arm, one RTL time-unit alternative, three packstr alternatives, two host-profile alternatives, two NTVDM header alternatives, two dialog-owner alternatives, one WOW table profiling arm, and one BaseSrv header alternative). Selected preprocessing was
compared in 48 source and 316 header graph contexts, plus eight standalone
header fixtures, two DPMI direct-include fixtures, the enabled CCPU decode
diagnostic profile, and all three BaseSrv x86/x64 contexts. The two
pre-existing standalone failures remain failures.
The current physical inventory has 5,050 arms, with 1,859 observed retained,
1,470 previously excluded, 532 newly excluded and 1,189 pending. This group
has not yet been built, tested, published or committed; the published
ten-image package remains the earlier 104-arm baseline.
The first header comparison initially missed ten contexts because include
paths used mixed slash styles. Normalizing both forms and checking only the
ten missing contexts completed the 316-context comparison without a mismatch.
An initial two-line marker displaced 57 calculated `#line` values by one;
the values were corrected and all 57 independently checked with single-line
markers. See `s13-bfs-depth1-single-line-controls.json`.

The later ICA, Video7 and memalloc alternatives also have selected-context
preprocessing comparisons in their respective batch evidence. The two
memalloc alternatives share one original condition group and one #line.
Pending includes 875 arms in files without a current build-context witness,
62 unguarded arms in witnessed files, and 252 existing #error arms whose
supported-profile exclusion is not yet established. Absence of a witness
is not treated as proof of exclusion.

Five PM-disabled alternatives in tape I/O, keyboard and DMA were checked
against the original boundaries and the unconditional selected host definition.
All six actual product-graph preprocessing contexts matched before/after;
controller, DMA and runtime behavior have not yet been revalidated on this
unbuilt batch. See s13-bfs-depth1-pm-batch.json and the scratch comparison.

Four first-depth DOUBLE_CPU, DOCUMENTATION and NOTUSEDNOTUSED arms have no
definition in project sources, tests or build scripts; six actual graph
contexts produced the same selected preprocessing output after replacement.
This does not substitute for the pending runtime tests. See
s13-bfs-depth1-inactive-batch.json and s13-bfs-depth1-inactive-selected-compare.json.

The current 100-arm working batch passed the standalone first-depth CPU
guard script and the retained-guard script (235 guards plus DPMI, ANSI,
CPU40 and USER controls) in the x86 build environment. These source guard
checks are not a substitute for compiling and running the candidate.

Nine additional header alternatives match the fixed original branch bytes
(after normalizing line endings for comparison), have no project definition
of their selected-off macro, and produced identical selected preprocessing in
355 actual contexts and ten standalone fixtures; GFI fails before and after.
See s13-bfs-depth1-inactive-header-batch.json and its comparison evidence.

Four more original BIOS/ICA compound conditions were checked against all
known definitions and fixed-original boundaries. Selected preprocessing
matched in 130 actual graph contexts and ten standalone fixtures; GFI
remains the same pre-existing failure. See s13-bfs-depth1-compound-header-batch.json.

Seven further BIOS/driver arms have unique exact branch matches in the
fixed original, no corresponding project definitions, and selected output
unchanged in 84 graph contexts and ten standalone fixtures. See
s13-bfs-depth1-legacy-header-batch.json and its comparison evidence.

Two HFX outer branches subsume 11 unselected nested groups and prior
DIV-504/518/528 occurrences; those shared registrations remain because
they still have other uses. HFX has no project or test definition, and
selected preprocessing matched in ten graph contexts and ten standalone
fixtures (the GFI failure remains pre-existing). See
s13-bfs-depth1-hfx-header-batch.json and its comparison evidence.

Two BIOS non-ANSI alternatives are excluded only for this header: all 84
actual consumers specify ANSI; the separate stubs.c undefines ANSI but is
not a BIOS-header consumer. The non-ANSI WOW control compiles a different
translation unit. Selected output matched those 84 graph contexts and ten
standalone fixtures. See s13-bfs-depth1-bios-ansi-batch.json.

Three WOW arms were excluded after tracing WOWFASTEDIT, FASTBOPPING and
MAPPARAM_EXTRA in their own translation units; the retained local callback
patch is unchanged. Product preprocessing and the direct-include abort
fixture matched before/after. Runtime callback and parameter tests remain
required. See s13-bfs-depth1-wow-extra-batch.json and callback comparison.

Three original platform alternatives in COM, ROM and host CPU were checked
against their actual translation-unit flags and original branch bodies.
All six product-graph preprocessing contexts matched; runtime verification
remains pending. See s13-bfs-depth1-platform-batch.json.

Three timing header arms match fixed-original branches and retain both
selected-sibling and end logical line mappings. Their selected preprocessing
matched 62 graph contexts and ten standalone fixtures. See
s13-bfs-depth1-timing-header-batch.json.

Current 188-arm working queue is snapshotted in
s13-bfs-depth1-working-queue-188.zip and checked against all recorded
working-file hashes. Physical inventory: 5,050 first-depth arms, zero
queue/directive mismatches; all 532 newly excluded arms are guard-only.
This working state is not a built or published candidate.

Two additional allocator non-WIN32 branches belong to the same verified
translation-unit profile as the previous allocator batch. Their original
outer selectors are present; selected output matched both graph contexts.
See s13-bfs-depth1-memalloc-extra-batch.json.

Three non-WIN31 alternatives in wow.h are excluded by that same header's
unconditional WIN31 definition. Their fixed-original branches matched and
selected preprocessing was unchanged in 90 graph contexts plus the direct
callback fixture. See s13-bfs-depth1-wow-header-batch.json.

Thirteen X86GFX/non-CCPU alternatives were rejected only in the three
verified host video/event translation units; X86GFX is not globally
forbidden and its cvidc test profile remains intact. These arms include
local DIV-322 adaptations, not exact original branch bodies. Selected
preprocessing matched six graph contexts and both direct-include event
fixtures; the video source-contract check passed. Graphics runtime and
the complete build remain pending. See s13-bfs-depth1-x86gfx-source-batch.json.

The RTL NTOS_KERNEL_RUNTIME alternative was checked in all six actual
x86/x64 compilation contexts: each selects the kernel-runtime form and
produced matching selected preprocessing and logical-line markers. See
s13-bfs-depth1-rtl-batch.json and s13-bfs-depth1-rtl-profile-probes.json.

The original SoftPC debug/profile.c PROFILE arm is not selected by its
two supported graph contexts; its 3,255-line body and eight inner groups
were replaced at the original first-depth boundary. The selected else
preprocessing and line mapping matched both contexts; a forced PROFILE
compile reached the expected #error. PROFILE is not globally forbidden:
the independent CPU guard test uses it for a different file. See
s13-bfs-depth1-profile-batch.json and profile-negative-control.json.

Six original or locally adapted host-source conditions were removed only
from five validated translation units; ten actual graph contexts retained
identical selected preprocessing. HOOKED_IRETS is defined separately in
zfrsrvd.c and is not globally forbidden. Fourteen inner groups are
subsumed; shared older diff registrations remain used elsewhere. See
s13-bfs-depth1-legacy-source-batch.json.

Six WOW thunk-only private-redirect alternatives were rejected only in
wuclass.c/wuwind.c; both actual source compile contexts kept identical
selected output. The private redirect remains selected, and the independent
precompiled-header selection test is not treated as a source-file profile.
See s13-bfs-depth1-wowredirect-batch.json. Runtime tests remain pending.

Two original video.c branches requiring neither EGG, C_VID nor A_VID
were rejected in the verified C_VID source profile. Both actual contexts
retain their X86GFX configuration and identical selected preprocessing.
See s13-bfs-depth1-video-source-batch.json.

Three additional non-NTVDM source alternatives are excluded in the
verified NTVDM translation units. Six actual contexts and two explicitly
enabled CCPU decode-diagnostic contexts preserved selected preprocessing.
See s13-bfs-depth1-ntvdm-extra-source-batch.json and its diagnostic comparison.

The x87 header non-NTVDM arm matched the fixed original, and its selected
output matched all six consumers, including both real x87 layout fixtures.
This is a header-selection check; the compiled x87 tests remain pending.
See s13-bfs-depth1-cfpu-ntvdm-batch.json.

Four DPMI alternatives require the unsupported non-CPU40 path; the
selected CPU40 source/header preprocessing matched, including both x86
contexts. i386 with CPU40 is accepted and i386 without CPU40 hits #error.
The independent /UCCPU fixture still fails at the pre-existing depth-2
DIV-515 guard both before and after; it is not counted as a pass. See
s13-bfs-depth1-dpmi40-source-batch.json and dpmi40-header-batch.json.

Six remaining common-header alternatives were instrumented in eight
standalone fixture contexts; no successful fixture positively selected
them, while GFI remains a pre-existing failed context. This observation
alone does not establish exclusion, so those alternatives remain pending.
See s13-bfs-depth1-standalone-positive-marker-audit.json.

The unselected host boot-file mutation arm is now #error; the selected
no-host-mutation adapter and its original call ordering are unchanged.
Both actual x86 graph contexts matched, and DIV-134 registration now states
the removed original alternate body. Runtime startup remains to be tested.
See s13-bfs-depth1-boot-profile-batch.json.

The standalone RTL time unit retains its selected original 1970-time
conversion and rejects the unselected full-unit alternative at its existing
outer selector (1,285 lines). Both actual x86 contexts have matching
selected preprocessing; the redirector runtime remains to be tested.
See s13-bfs-depth1-rtl-time-batch.json.

The three original packstr bodies are excluded only by the existing
COPY_ONLY build profile; each body matched the fixed original and the two
actual graph contexts retained identical selected preprocessing. The
selected copy path stays intact. See s13-bfs-depth1-packstr-batch.json.

Two additional source-local alternatives were excluded: non-NTVDM
Unix setup in main.c, and the non-V7 build fallback in nt_graph.c. The
selected V7-enabled build can still run ordinary VGA modes; no video-mode
branch was removed. Four actual source contexts matched before/after.
Runtime graphics validation remains pending. See
s13-bfs-depth1-host-profile-batch.json.

Two non-NTVDM header alternatives matched the fixed original and retained
identical selected output in 124 graph contexts, eight standalone fixture
contexts, two painter fixtures, and the direct PIF-caller fixture. GFI
remains the pre-existing failure. See s13-bfs-depth1-ntvdm-header-batch.json.

The two dialog source alternatives without the native dialog owner are
excluded only in dlgbegin.c. Both supported compile contexts preserve the
selected owner behavior and logical lines; dialog runtime tests are still
required. See s13-bfs-depth1-dialog-profile-batch.json.

The WOW message-table profiling arm was matched to the original and
rejected in the one selected release translation unit; profiling macros
are not globally forbidden in unrelated components. See
s13-bfs-depth1-wowtable-profile-batch.json.

The BaseSrv header alternative without OPENNT_BASE_VDM_SERVER is
rejected only in this header. All 63 actual x86/x64 include contexts use
the selected server profile and produced identical preprocessing; older
broker audit drivers also specify that profile. See
s13-bfs-depth1-basesrv-header-batch.json. Runtime tests remain pending.

Batch 188 cached x86/x64 builds completed successfully and was staged as
a coherent ten-image candidate. The first Full attempt failed at the RPC
fixture because it was mistakenly launched with Windows PowerShell 5.1,
contrary to the existing modern-pwsh execution rule. A fresh Full run under
PowerShell 7 is in progress; the first attempt is not a product pass. See
s13-bfs-depth1-batch188-build-results.json and batch188-test-runner-correction.json.

Batch 188 x87 boundary fixture does not pass the aspirational corrected
contract: its eight actual results exactly match the previously recorded
original-source baseline (four failing cases), while the earlier passing
fix was expressly held unpublished and outside this scope. All 48 current
paging-fixture results also match the historical source-derived baseline
after removing its later-added first_supervisor=0 output field. These are
non-regression comparisons, not x87 correctness passes; guest-level
incidence remains unproved and the existing recovery worklist retains them.
See s13-bfs-depth1-batch188-x87-historical-control.json and
s13-bfs-depth1-batch188-x87-pages-control.json.

Batch 188 checkpoint: x86/x64 cached builds and the 17/17 Full gates passed under modern PowerShell. Win3.1 Standard/386 x DOSONLY 0/1, Winmine and SOL interaction probes, keyboard/PIC 86/86, PIC reset 24/24, x87 layout, the 235-condition guard suite, and 54 WOW parameter-map scenarios passed. The ten candidate images were installed and hash-verified in both release destinations. See s13-bfs-depth1-batch188-runtime-results.json and batch188-publication.json. The first WOW map attempt used an invalid relative compiler path; the absolute-path rerun passed. This is an intermediate release, not S13 closure: 1,189 current first-depth arms and original-baseline boundaries remain to be established. Known x87 baseline failures and independent fixture limitations remain as documented above.

After the published 188-arm checkpoint, six more first-depth DOS/WIN_16/SIGNALS header arms were independently matched to the pinned original, checked against 46 actual product preprocessing contexts and an additional WIN_32 comparison, and guarded with #error plus six tested logical line controls (DIV-600). Negative controls deliberately select each rejection. They are not yet rebuilt or released. Current physical first-depth inventory remains 5,050 arms: 1,183 pending. The first selection-probe attempt lacked a header-entry control and is explicitly superseded by r2. Queue snapshot: s13-bfs-depth1-working-queue-194.zip.

The first x64-reference clean RC build exposed a previously incorrect DIV-504 exclusion: the supported resource compiler selects the non-PROD HostDebugBreak declaration. Restored the exact original declaration and original selector; the reference build and 433-check GFI contract now pass. Independently rerunning the x86 resource compiler produced the same SHA-256 as both previous cached and newly built reference resources. See s13-bfs-rc-prod-restoration.json. The WIN_32 marker probe r2 confirms the three cmd.h WIN_32/FLAT_32/WIN arms can be selected in the tested additional profile; the first marker attempt did not actually pass the flag and is invalid.

Batch 194 validation: all cached x86/x64 product build steps succeeded; the independent reference build and GFI fixture succeeded after the RC declaration restoration. All six actual worker/WOW RC targets were independently recompiled and exactly match their cached resources. Both guard suites passed on x86 and x64; the worker increment was a no-op and documentation governance passed. In a same-package-ID comparison to the already published and Full-tested ten-image baseline, six rebuilt images are byte-identical and the other four differ only in COFF and PE debug-directory timestamps. Their raw hashes differ; no new binaries were deployed and prior exact tested hashes remain installed. See batch194-build-results.json, batch194-guard-results.json, batch194-resource-compiler.json and batch194-image-compare.json. This checkpoint does not close S13.

RC context coverage: six actual resource compiler commands were also run in preprocess-only mode; their native #line outputs identify eight repository input paths, including three mirror headers with first-depth groups. All of those first-depth groups were already retained; none is classified excluded. The earlier incorrectly excluded PROD declaration is nested under a retained header guard and was separately restored. RC preprocessing exit 1 is its documented-by-observation preprocess-only result, verified by the output-created marker; see s13-bfs-depth1-resource-inclusion-contexts.json and s13-bfs-depth1-resource-coverage.json. This evidence does not establish the reachability of unvisited branches.

After batch 194, original nt_bop.c hardware-host i386 FPU save/restore was rejected under its original first-depth guard (DIV-601). Both actual CCPU product contexts preserved selected preprocessor output, forced i386 rejection was tested, and the following logical line was checked against the original current profile. DIV-323 was confined to the removed inactive arm and removed from the mirror register; its separately tested adapter remains untouched. The new guard test passed on x86/x64. Both cached worker builds and x86/x64 guards passed; the previously Full-tested exact binary hashes remain published after the new build was verified to differ only in PE timestamps. Current first-depth pending count: 1,182. The active queue is archived as s13-bfs-depth1-working-queue-195.zip.

Current first-depth status (current-source inventory only): 4,074 groups and 5,050 arms; 1,859 retained-observed, 2,009 previously or newly excluded, and 1,182 pending. The latter include 875 unguarded arms whose source file was not witnessed in the product graph, 252 existing guarded arms with unresolved profile coverage, and 55 unguarded arms in previously witnessed files. The 1,395 child groups under retained-observed arms are counted but not audited or admitted; a further 1,677 groups have pending parents. Missing original structures are tracked separately. A refreshed product marker audit covered 863 real compile contexts with zero positive markers and no failures, but absence is not exclusion proof; every stored command matched the current graph. T324 x86/x64 syntax checks and exact x86 marker controls demonstrate that WIN_32, FLAT_32 and WIN in cmd.h are selected by a supported independent profile. See s13-bfs-depth1-current-layer-counts.json, pending-positive-observation.json, pending-graph-freshness.json and command-t324-marker-control.json.

The exact T324 standalone compiler profile was syntax-compiled on x86 and x64, and x86 shadow markers positively witnessed cmd.h WIN_32, FLAT_32 and WIN. These three first-depth arms are now classified retained rather than pending; no inner content was examined. First-depth pending: 1,179. Current queue: s13-bfs-depth1-working-queue-195r1.zip. See s13-bfs-depth1-command-t324-retain.json and current-layer-counts.json.

The original-condition, build-command and macro-definition audit established that 12 existing positive-SFELLOW guards, five Macintosh-only guards and five Unix-only guards correspond to unsupported platform profiles. All 22 were already #error, so only their first-depth dispositions changed; x86/x64 forced-selector controls and fixed-original condition matching passed. AND expressions were controlled with PIG or A2CPU alone, and OR expressions were not classified by these scans. First-depth pending: 1,157. Queue: s13-bfs-depth1-working-queue-195r2.zip. Evidence: sfellow-existing-guard-audit.json, macintosh-existing-guard-audit.json, unix-existing-guard-audit.json.

Eight existing first-depth guards requiring A3CPU or GISP_CPU were matched to fixed original conditions and classified as excluded alternate CPU backends. No supported graph command or repository/generated header defines either; x86/x64 forced-selector controls and IRET_HOOKS-only controls passed. The condition containing an OR with GISP_CPU remains pending. This changes no source. First-depth pending: 1,149. Queue: s13-bfs-depth1-working-queue-195r3.zip; see s13-bfs-depth1-alternate-cpu-existing-guard-audit.json.

Four existing GISP_SVGA hardware-backend guards and two BIGEND/A2CPU guards were matched to fixed original first-depth conditions and to the supported x86/x64 CCPU build profiles, and their forced-selector controls passed. The V7VGA software path remains selected; no code changed. First-depth pending: 1,143. Queue: s13-bfs-depth1-working-queue-195r4.zip; see gisp-svga-existing-guard-audit.json and legacy-cpu-endian-existing-guard-audit.json.

Eight existing SCCSID/DOCUMENTATION first-depth guards were matched to original conditions and unselected supported compiler configurations, with 16 x86/x64 forced-selector controls. One mvdm.h non-x86 else arm was positively selected by the supported T324 x64 profile and retained. No further source changed. Pending first-depth arms: 1,134; queue s13-bfs-depth1-working-queue-195r5.zip. The recorded fixed-HEAD restoration total is 42 first-depth original groups, including nine during BFS, derived from the 41-group checkpoint plus the clmsg DEBUG restoration; original reconciliation remains open. See s13-bfs-depth1-restoration-count.json.

Fixed-original structural triage: seven chartran.c FE_SB/_USERK_ occurrences previously labelled structural hints were rechecked against pinned original group locations, current first-depth guard directives and the existing function-owner reconciliation. All seven were already present; no source was restored and the 42-group restoration count is unchanged. The other nested or owner-absent cases remain separate pending/deferred boundaries. See s13-bfs-depth1-original-occurrence-triage.json and s13-bfs-depth1-original-chartran-reconciliation.json.

A full macro-origin upper bound across 2,070 current commands, 1,656 observed includes, x86/x64 compiler built-ins, repository/generated sources, build scripts, and eight full SDK include directories established 33 existing simple positive-macro guards as inactive in supported profiles; each maps uniquely or through the existing original ledger and passed 66 forced controls. Twenty-two repeated-condition mappings were held; two were subsequently established by unique surrounding anchor pairs and four controls. Eight #if DBG guards were independently matched and checked against undefined/zero/nonzero controls; historical guest and unselected tool manifests with DBG=1 are explicitly outside that conclusion. Four repeated DBG mappings remain pending. No source changed. Pending first-depth arms: 1,091. Queue: s13-bfs-depth1-working-queue-195r6.zip. See simple-macro-existing-guard-audit.json, pending-anchor-confirmed.json and dbg-existing-guard-audit.json.

Eight additional existing else guards with proven-absent selectors were mapped to fixed originals by unique occurrence, prior ledger or unique anchor pair; sixteen x86/x64 forced controls passed. Three repeated DBG guards were similarly mapped by unique surrounding text and passed six undefined/zero/nonzero controls. No source changed. First-depth pending: 1,079. Queue: s13-bfs-depth1-working-queue-195r7.zip. See inverse-macro-else-confirmed.json and dbg-context-confirmed.json; ambiguous mappings remain pending.

Historical original-body reconciliation established 20 repeated #ifdef guards and eight inverse #else guards by matching the prior removal hashes to the corresponding pinned original branch bodies; x86/x64 forced controls passed. The timer SYNCH_TIMERS comparison initially included the original else arm; correcting the first-arm cutoff shows the original body does match, and the correction is recorded. Four original WOW32 GWL mismatch assertions were preserved; actual x86 SDK controls pass while synthetic x64 SDK controls deliberately trip them and are not a supported WOW32 build. No source changed. Pending first-depth arms: 1,047. Queue: s13-bfs-depth1-working-queue-195r8.zip. See historical-body-guards-confirmed.json, timer-arm-corrected-control.json, inverse-else-historical-confirmed.json and gwl-original-assertions.json.

### First-depth checkpoint: composite and inverse existing guards

Four existing `#error` arms were matched to unique first-depth condition groups in the pinned OpenNT source: SOFTWIN_API, HERC, SWIN_HAW, and the JOKER inverse arm. Project/generated/SDK macro-definition scans and 24 x86/x64 compiler controls support these classifications. The DELTA/A2CPU control explicitly preserves DELTA: DELTA alone remains accepted; only the conjunction is rejected. The two repeated DELTA/A2CPU groups remain pending original-body mapping and were not reclassified. No product source changed. First depth now has 4,074 groups and 5,050 arms: 1,863 observed retained, four original assertions retained, 1,601 already excluded, 539 newly excluded, and 1,043 pending. The snapshot is `s13-bfs-depth1-working-queue-195r9.zip`; original structure reconciliation is still incomplete. Existing released binaries remain the previously verified exact baseline; evidence-only work does not justify another build or partial release.

The two DELTA/A2CPU existing guards now have an exact correspondence: the pre-guard bodies from `7f2bb292^` match the respective pinned OpenNT bodies after line-ending normalization. Together with the x86/x64 DELTA-only and both-defined controls, both guards are classified as already excluded. No source change or fresh release was required. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 539 newly excluded, 1,041 pending. The working queue is archived in `s13-bfs-depth1-working-queue-195r10.zip`; the original-structure audit remains incomplete.

### CCPU carrier context correction

The supported CCPU40 runtime does not imply `/DCCPU` for every translation unit. The actual x86 worker Ninja rule for `accessfn.c` removes both `/DCCPU` and `/DPROD`; its valid Ninja dependency record resolves `sas4gen.h` to `src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h`, ahead of the similarly named original mirror carrier. Eight x86/x64 preprocessor controls demonstrate that each carrier changes its `sas_memory_size()` mapping when `CCPU` is absent or present. See `s13-bfs-depth1-ccpu-carrier-context-control.json`. This rules out a blanket CCPU-macro conclusion for the 55 first-depth groups of the currently unwitnessed mirror carrier; they remain pending, not marked unreachable solely because this particular path was not chosen. No mirror source or runtime behavior was changed.

### First-depth SECURE and EGATEST batch

Five original first-depth unsupported arms were replaced with `#error`: four `SECURE` arms in `bootstra.c` and one `EGATEST` arm in `cpu2.h`; their original selectors, supported else arms, and original diagnostic line positions remain. The removed bodies matched their pinned OpenNT counterparts, and no source, generated or SDK macro definitions or compile-command definitions were found across the reviewed supported configurations. Ten x86/x64 selected-versus-forced preprocessor controls passed. The existing physical-line guard ledger was adjusted and the permanent guard gate now exercises all five new guards. Cached x86/x64 builds succeeded without recompilation; the full 235-guard rejection gate plus the new controls passed on both architectures. Ten already-accepted product images remain byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 544 newly excluded, 1,036 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r11.zip`; details: `s13-bfs-depth1-secure-egatest-batch.json`.

### First-depth API16 WOW32 batch

Ten original `API16` first-depth arms in `wkglobal.c` and `wklocal.c` were replaced with `#error`, retaining the original selectors, supported else arms and original diagnostic line positions. Each removed body matched the pinned OpenNT body after line-ending normalization. Across 2,070 supported Ninja compile commands no `API16` or `WIN_16` definition was found; the sole source definition of `API16` is in `dem.h` under `WIN_16`, whose definition was absent from project, generated and SDK scans. Twenty x86/x64 selected and forced preprocessor controls passed. Cached x86/x64 builds succeeded without recompilation; the 235-guard gate and all additional SECURE, EGATEST and API16 controls passed on both architectures. Ten accepted product images remained byte-identical at both release locations, so no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 554 newly excluded, and 1,026 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r12.zip`; details: `s13-bfs-depth1-api16-batch.json`.

### First-depth STUBBED host configuration batch

Four original `STUBBED` first-depth arms in `stf_conf.c` were replaced with `#error`, retaining their original selectors and diagnostic line positions. Each removed body matched the pinned OpenNT body after line-ending normalization. Across 2,070 supported Ninja compile commands no `STUBBED` definition was found; the sole source definition is local to the separate `nt_unix.c` translation unit and is not included by `stf_conf.c`. Project, generated and SDK scans found no other definitions. Eight x86/x64 selected and forced preprocessor controls passed. Cached x86/x64 builds succeeded without recompilation; the 235-guard gate and all additional SECURE, EGATEST, API16 and STUBBED controls passed on both architectures. Ten accepted product images remained byte-identical at both release locations, so no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 558 newly excluded, and 1,022 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r13.zip`; details: `s13-bfs-depth1-stubbed-batch.json`.

### First-depth legacy platform and diagnostic batch

Fifteen original first-depth platform and diagnostic arms, spanning eleven mirror files, were replaced with `#error`; 1,047 excluded source lines and eight internal queued condition groups were removed without auditing the inner branches. Each removed body matched its pinned OpenNT counterpart after line-ending normalization. Two copyright notices formerly inside removed arms were retained at file scope, with `#line 1` preserving the original numbering. Project, generated, SDK and 2,070 supported compile-command scans found no definitions of the corresponding selectors; 30 x86/x64 normal/forced selector controls passed. Cached x86/x64 builds succeeded without recompilation. The guard gate passed both architectures after correcting its whitespace handling for original tab-separated directives. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 573 newly excluded, and 1,007 pending. For the next depth, 1,396 groups are under observed-retained arms and 1,669 under pending arms; none were audited in this batch. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r14.zip`; details: `s13-bfs-depth1-platform-v2-batch.json`.

### First-depth non-MVDM legacy profiles

Two original `WX86` arms in `srvinit.c` and one `DOS3` arm in `token.h` were replaced with `#error`, preserving their selectors and diagnostic line positions; 307 removed source lines matched pinned OpenNT after line-ending normalization. Project, generated, SDK and 2,070 supported compile-command scans found no selector definitions; six x86/x64 normal/forced selector controls passed. Cached x86/x64 builds succeeded without recompilation, and the guard gate including these selectors passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 576 newly excluded and 1,004 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r15.zip`; details: `s13-bfs-depth1-nonmvdm-legacy-batch.json`.

### First-depth compound legacy profiles

Two original first-depth arms in `cdrom.h` and `host_msw.h` were replaced with `#error`: `GEN_DRVR` and the `MSWDVR && XWINDOW` conjunction. Their 436 removed source lines matched pinned OpenNT after line-ending normalization; 17 inner queued condition groups were removed with the excluded outer body without deeper audit. The selected else and original diagnostic lines remain. Project, generated, SDK and 2,070 supported compile-command scans found no definitions of these selectors. Both architectures passed normal and forced checks, including verifying that either conjunction operand alone does not reject. Cached x86/x64 builds succeeded without recompilation, and the guard gate passed both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 578 newly excluded and 1,002 pending. The next-depth inventory contains 1,396 groups under observed-retained arms and 1,656 under pending arms; it was not audited here. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r16.zip`; details: `s13-bfs-depth1-compound2-batch.json`.

### First-depth CPU_286 disassembler profile

Thirteen original `CPU_286` first-depth arms in the dormant `dasm386/decode.c` source were replaced with `#error`, retaining their alternative arms and original diagnostic line positions. The 112 removed source lines matched the corresponding pinned OpenNT bodies after line-ending normalization; correspondence accounted for earlier line shifts rather than relying on current physical line numbers. Project, generated, SDK and 2,070 supported compile-command scans found no definition of this selector. This is a disassembler compile profile, not the CCPU guest instruction executor, and does not limit guest 286 instruction execution. x86/x64 normal and forced selector controls passed; cached x86/x64 builds succeeded without recompilation, and the guard gate including all 13 selectors passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 591 newly excluded and 989 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r17.zip`; details: `s13-bfs-depth1-cpu286-batch.json`.

### First-depth CMOS test-harness profile

Two original `TEST_HARNESS` first-depth arms in `cmos.c` were replaced with `#error`, retaining the separate normal `#ifndef TEST_HARNESS` branch and original diagnostic line positions. The 26 removed source lines matched pinned OpenNT after line-ending normalization. Project, generated, SDK and 2,070 supported compile-command scans found no selector definition; four x86/x64 normal/forced selector controls passed. Cached x86/x64 builds succeeded without recompilation, and the guard gate including the new selectors passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 593 newly excluded and 987 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r18.zip`; details: `s13-bfs-depth1-cmos-harness-batch.json`.

### First-depth dat2obj host architectures

Three original `dat2obj.c` first-depth arms for `_MIPS_`, `_PPC_` and `ALPHA` were replaced with `#error`, retaining the `_X86_` selector and original diagnostic line positions. The three removed source lines matched pinned OpenNT after line-ending normalization. Project, generated, SDK and 2,070 supported compile-command scans found no definitions of these historical selectors; six x86/x64 normal/forced selector controls passed. Cached x86/x64 builds succeeded without recompilation, and the guard gate including the new selectors passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 596 newly excluded and 984 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r19.zip`; details: `s13-bfs-depth1-dat2obj-arch-batch.json`.

### First-depth legacy interface and debugger profiles

Sixteen original first-depth host-interface and debugger arms were replaced with `#error`; 1,481 excluded source lines and 19 inner queued condition groups were removed with their outer branches, without auditing their inner conditions. Each removed body matched its pinned OpenNT counterpart after line-ending normalization. Project, generated, SDK and 2,070 supported compile-command scans found no selector definitions; 32 x86/x64 normal/forced selector controls passed. Cached x86/x64 builds succeeded without recompilation. The guard gate passed both architectures after correcting its parser to accept original trailing comments. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 612 newly excluded and 968 pending. The next-depth inventory contains 1,396 groups under observed-retained arms and 1,638 under pending arms; it was not audited here. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r20.zip`; details: `s13-bfs-depth1-legacy-interface-batch.json`.

### First-depth ALLOCA and PnP legacy profiles

Original first-depth `ALLOCA` in `sim32.h` and `_PNP_POWER` in `isnkrnl.h` were replaced with `#error`, retaining the original alternate arms and diagnostic lines. The removed bodies matched pinned OpenNT after line-ending normalization, and two inner condition groups were removed with their excluded outer branches without deeper audit. Case-sensitive project, generated, SDK and 2,070 supported compile-command scans found no definitions of these selectors; x86/x64 normal/forced controls passed. A separate control verifies lowercase SDK `alloca` remains accepted and distinct from uppercase `ALLOCA`. Cached x86/x64 builds succeeded without recompilation, and the guard gate passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 614 newly excluded and 966 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r21.zip`; details: `s13-bfs-depth1-sim32-alloca-batch.json` and `s13-bfs-depth1-pnp-power-batch.json`.

### First-depth alternate CMOS FLOPPY_B profile

The original `FLOPPY_B` first-depth arm in the unselected alternate `cmos.c` was replaced with `#error`, retaining its selector and original diagnostic line positions. The removed body matched pinned OpenNT after line-ending normalization, with earlier physical line shifts accounted for. Project, generated, SDK and 2,070 supported compile-command scans found no selector definition; x86/x64 normal/forced selector controls passed. This does not alter the selected runtime disk implementation or establish any new claim about its B-drive behavior. Cached x86/x64 builds succeeded without recompilation, and the guard gate passed on both architectures. Ten accepted product images remained byte-identical at both release locations; no partial replacement or redundant runtime test was performed. First depth: 1,863 observed retained, four original assertions retained, 1,603 already excluded, 615 newly excluded and 965 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r22.zip`; details: `s13-bfs-depth1-floppy-b-batch.json`.

### First-depth original SDK assertions

Preserved and classified five existing original compile-time assertions rather than treating their `#error` as an excluded implementation: the `ERROR_SUCCESS`/`NO_ERROR` zero-value check in `error.c`, and four WOW32 GCL constant checks in `wuclass.c`. Each three-line conditional matches the pinned OpenNT original. The success-code assertion passed normal and separate invalid-value controls on x86 and x64; the four GCL checks passed normal and individually perturbed x86 SDK controls. WOW32 is supported on x86; no x64 WOW32 claim is made. No product source changed. First depth: 1,863 observed retained, nine original assertions retained, 1,603 already excluded, 615 newly excluded and 960 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r23.zip`; details: `s13-bfs-depth1-error-success-assertion.json` and `s13-bfs-depth1-gcl-original-assertions.json`.

### First-depth YODA context-bound existing guards

Five existing first-depth `YODA` guards in `nt_event.h`, `nt_event.c` and `nt_timer.c` were matched to their pinned OpenNT originals via exact pre-guard body identity. Across 32 actual source/include contexts each compile command defines `PROD`; source and SDK scans found `YODA` defined only by the host header under `!PROD && !MONITOR`, and no `PROD` undefinition. The actual six resource compiler contexts do not include these files; the standalone heartbeat build also specifies `PROD`. Eight x86/x64 host-header controls verified that YODA is selected without PROD/MONITOR and suppressed with either, so no global YODA exclusion is asserted. Two additional YODA guards remain pending because historical pre-guard bodies differ from the pinned original. No product source changed. First depth: 1,863 observed retained, nine original assertions retained, 1,608 already excluded, 615 newly excluded and 955 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r24.zip`; details: `s13-bfs-depth1-yoda-prod-context.json`.

### First-depth YODA outer-guard supersession

The remaining two existing YODA guards in `yoda.c` and `nt_yoda.c` each have a unique first-depth selector in both the pinned OpenNT original and the pre-guard historical source; all actual graph contexts for these files define PROD. Their old bodies differ from the original and include 98 and one historical inner divergence markers respectively. Those inner records are superseded by the existing excluded outer guard, and remain historical evidence rather than additional live differences. This establishes outer selector identity and existing supported build disposition; it does not assert that every historical body difference has been individually explained. The source registry records the supersession. No product code changed. First depth: 1,863 observed retained, nine original assertions retained, 1,610 already excluded, 615 newly excluded and 953 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r25.zip`; details: `s13-bfs-depth1-yoda-outer-supersession.json` and `s13-bfs-depth1-yoda-prod-context.json`.

### First-depth PROD header context checks

Seven existing `#ifndef PROD` guards in five original host headers were matched to their fixed OpenNT bodies using the historical removal ledger and body hashes. All 226 actual file/include contexts for these headers define `PROD`; none of the six actual resource compiler contexts includes them. x86/x64 positive and negative selector controls passed. This is a file-specific result: other supported translation units can omit `PROD`, so it is not a global build invariant. No product source changed. First depth: 1,863 observed retained, nine original assertions retained, 1,617 already excluded, 615 newly excluded and 946 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r26.zip`; details: `s13-bfs-depth1-prod-header-context.json`.

### First-depth NTVDM context checks

Eleven existing non-NTVDM guards in six original host files were matched to unique historical removal bodies and fixed OpenNT originals. All 178 actual file/include contexts for these files explicitly define `NTVDM`; none of the six actual resource compiler contexts includes them. The independent keyboard/PIC and PIC-reset tests use the production graph and explicitly assert `NTVDM`. x86/x64 positive and negative selector controls passed. This is a file-specific result, not a claim that every project compilation defines NTVDM. No product source changed. First depth: 1,863 observed retained, nine original assertions retained, 1,628 already excluded, 615 newly excluded and 935 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r27.zip`; details: `s13-bfs-depth1-ntvdm-context-supersession.json`.

### First-depth CPU40 context checks

Five existing non-CPU40 guards in five original host files were matched to unique historical removal bodies and pinned OpenNT originals. All 110 actual file/include contexts for these files explicitly define `CPU_40_STYLE`; none of the six resource compiler contexts includes them. Independent Video7 and planar-painter tests derive the production flags and explicitly require CPU40. x86/x64 positive and negative selector controls passed. The DPMI non-CPU40 guard is held for separate original-structure review because no matching original first-depth selector was found. No CCPU, software V86 or 486 implementation was modified. First depth: 1,863 observed retained, nine original assertions retained, 1,633 already excluded, 615 newly excluded and 930 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r28.zip`; details: `s13-bfs-depth1-cpu40-context-check.json`.

### First-depth CPU40 compound context checks

Nine existing CPU40/NTVDM/CCPU/PROD compound guards in four original host source files were matched to unique historical removal bodies and pinned OpenNT originals. All eight actual source contexts for these files define the four relevant macros; none of the six resource compiler contexts includes them. Eighteen x86/x64 controls verified the actual combination, the negated operand case and the omitted CPU40 case. The `xt.c` compound guard remains pending because no matching original first-depth selector was found. This is a source-specific result, not a global macro claim. No product source changed. First depth: 1,863 observed retained, nine original assertions retained, 1,642 already excluded, 615 newly excluded and 921 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r29.zip`; details: `s13-bfs-depth1-cpu40-compound-context.json`.

### First-depth xt.c original outer selector restoration

Restored the pinned OpenNT outer `#if defined(CCPU) && !defined(CPU_30_STYLE)` in `xt.c`; the existing DIV-147 CPU40 register-ownership adaptation remains intact as a second-depth conditional and has not been audited at this depth. Corrected the following original diagnostic mappings to lines 147 and 340. Both actual x86 worker graphs selected the restored outer branch, and their selected preprocessing matched the prior source apart from blank lines. Compiler diagnostics verified original lines 152 and 340; x86/x64 CPU guard tests and existing rejection gates passed. The same-source-path before/after object compilation was byte-identical. Both actual worker graphs and top-level cached x86/x64 builds passed. Comparing a raw un-staged worker to the published package initially showed 22,395 differing bytes: `.text` and all other resource payloads matched, while the un-staged executable lacked the package identity resource and its resource directory was rearranged. After staging the complete candidate with the same package identity, six of ten images were byte-identical and four differed only in known PE timestamp fields; no image had any other byte difference. The previously accepted exact release was retained without partial replacement or redundant runtime tests. First depth: 1,864 observed retained, nine original assertions retained, 1,642 already excluded, 615 newly excluded and 920 pending. The original structure restored count is 43 relative to fixed S13 HEAD, including ten during the current BFS pass; reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r30.zip`; details: `s13-bfs-depth1-xt-outer-restoration.json` and `s13-bfs-depth1-xt-packaged-image-compare.json`.

### First-depth DPMI local guard origin

The non-CPU40 guard in `DpmiSwIntHandler` has no matching original outer condition because OpenNT updates the flags unconditionally at that point. Commit `f1a641bd72` introduced the local CPU40-specific deferment to preserve flags if frame construction fails; it is covered by existing DIV-481 and controlled selected-object evidence. The later non-CPU40 guard replacement preserves that local adaptation. Both actual DPMI source graphs select CPU40, and the removed old statement matches the registered historical body. The source registry now explicitly identifies the guard as local rather than original. This is a bounded provenance conclusion, not proof of complete DPMI exception semantics; no product code changed. First depth: 1,864 observed retained, nine original assertions retained, 1,643 already excluded, 615 newly excluded and 919 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r31.zip`; details: `s13-bfs-depth1-dpmi-local-guard-origin.json`.

### First-depth PIG guards and original diagnostic line

The fixed original has the same first-depth PIG selectors in `c_getset.c` and `c_stack.c`. Both actual worker source contexts define PROD and omit PIG; the earlier graph and source scan is recorded in `s13-bfs-depth1-pig.json`. Six x86/x64 controls verified the selectors and original post-guard logical line 37. Restored `#line 37` after the existing DIV-515 excluded body. Both worker graphs, top-level cached builds, and x86/x64 rejection guards passed. The ten-image candidate with the published package identity has six byte-identical images and four with timestamp-only differences; the existing accepted release was retained. No runtime change is claimed. First depth: 1,864 observed retained, nine original assertions retained, 1,645 already excluded, 615 newly excluded and 917 pending. Original structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r32.zip`; details: `s13-bfs-depth1-pig-line-context.json` and `s13-bfs-depth1-pig-line-package-compare.json`.

### First-depth original diagnostic-line reconciliation

A conservative sweep found 48 possible original-line mismatches; 19 had byte-identical original prefixes, unique first-depth single-arm selectors and matching immediate post-group continuation. Restored their original diagnostic line mapping without modifying selected statements. The remaining candidates require separate review; the sweep does not cover every excluded branch. Thirty-eight x86/x64 compiler diagnostic controls, original-guard tests and affected worker/WOW32 graphs passed, as did top-level cached builds. The final same-identity ten-image package comparison found five byte-identical images and five differing only in previously verified PE timestamps; the accepted complete release remains unchanged. First depth remains 917 pending, with original-structure reconciliation open. Queue: `s13-bfs-depth1-working-queue-195r33.zip`. Evidence: `s13-bfs-depth1-original-line-sweep.json`, `s13-bfs-depth1-original-line-batch.json`, `s13-bfs-depth1-original-line-controls.json`, and `s13-bfs-depth1-line19-package-compare.json`.

### First-depth original alternative context checks

Eleven existing alternate `#error` arms were matched to their fixed-original selectors by ordinal and surrounding anchors. Their actual worker source contexts explicitly define the selected NTVDM, CPU40 or PROD macro, with no undefinition in their observed include chains. Twenty-two x86/x64 controls exercised selected and forced-alternative paths. This evidence establishes selector position and supported source contexts, not byte identity of the historical removed bodies; seventeen other candidate arms remain deferred on original structure or context. No product source or release changed. First depth: 1,864 observed retained, nine original assertions retained, 1,656 already excluded, 615 newly excluded and 906 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r34.zip`; evidence: `s13-bfs-depth1-alternative-context-audit.json` and `s13-bfs-depth1-alternative-controls.json`.

### First-depth original virtual interrupt selector

Restored the fixed OpenNT `#ifndef i386` outer selector in `nt_msscs.c`: its i386 hardware-monitor alternative is rejected while the existing DIV-039 CPU40 ownership condition is retained intact and queued for the next depth. Both actual worker preprocessing outputs remained equal ignoring blank lines; original following logical line 128 was verified. The x86/x64 guard tests and worker and cached builds passed. The complete same-package-identity candidate matched the accepted ten-image release byte-for-byte in five images and differed only in PE timestamps in five; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,656 already excluded, 616 newly excluded and 905 pending. Original groups restored: 44 since the fixed S13 head, including 11 during the current BFS pass; reconciliation is not complete. Queue: `s13-bfs-depth1-working-queue-195r35.zip`; evidence: `s13-bfs-depth1-msscs-outer-restoration.json` and `s13-bfs-depth1-msscs-package-compare.json`.

### First-depth original mouse callback selector

Restored the fixed OpenNT `#ifdef CPU_30_STYLE` outer selector in `mouse_io.c`: the unsupported CPU30 arm is rejected while the existing DIV-222 CPU40 direct callback adaptation is retained intact and queued for the next depth. Both actual worker preprocessing outputs remained equal ignoring blank lines; original diagnostic line 6636 was verified. The x86/x64 guard tests and worker and cached builds passed. The complete same-package-identity candidate matched the accepted ten-image release byte-for-byte in five images and differed only in PE timestamps in five; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,656 already excluded, 617 newly excluded and 904 pending. Original groups restored: 45 since the fixed S13 head, including 12 during the current BFS pass; reconciliation is not complete. Queue: `s13-bfs-depth1-working-queue-195r36.zip`; evidence: `s13-bfs-depth1-mouse-outer-restoration.json` and `s13-bfs-depth1-mouse-package-compare.json`.

### First-depth original SAS interface exclusion

Verified the existing CPU40 alternatives in `egacpu.h` and `sas.h` against their fixed-original removed bodies and 248 actual including contexts; all define CPU40, with no included undefinition. Then replaced the whole second non-CPU40 SAS outer branch with `#error`, restoring original following logical line 272 and removing ten now-subsumed inner queue groups without independently auditing them. Three prior nested marker registrations are superseded at this location only and retained for other occurrences. The x86/x64 selector and line controls, actual worker builds, cached builds and guard tests passed. The full same-identity ten-image candidate is compared separately in `s13-bfs-depth1-sas-package-compare.json`. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 618 newly excluded and 901 pending. Original structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r37.zip`; evidence: `s13-bfs-depth1-cpu40-else-header-context.json`, `s13-bfs-depth1-cpu40-alternative-header-contexts.json`, `s13-bfs-depth1-sas-outer-exclusion.json`, and `s13-bfs-depth1-sas-line-controls.json`.

### First-depth original host header alternatives

Replaced four original first-depth unsupported alternatives in `debug.h`, `yoda.h`, `egaports.h`, and `error.h` after checking 376 actual header contexts, source include-chain macro undefinitions and resource contexts. Removed 425 body lines and seven inner queue groups; the copyright notice in `error.h` remains outside the excluded arm and inner DIV-528/530/531 markers are superseded at that location only. Six x86/x64 diagnostic-line controls, the worker and cached builds and guard tests passed. The complete same-identity ten-image candidate is compared in `s13-bfs-depth1-four-header-package-compare.json`. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 622 newly excluded and 897 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r38.zip`; evidence: `s13-bfs-depth1-four-header-exclusion.json`, `s13-bfs-depth1-alternative-body-contexts.json`, and `s13-bfs-depth1-four-header-line-controls.json`.

### First-depth local C-VID carrier alternatives

Verified that fixed OpenNT `cpu_vid.h` and `gmi.h` have unconditional generic carrier declarations and that the existing CPU40/C-VID typed selectors are local DIV-088/089 adaptations. Their 106 actual including contexts define both macros, without included undefinitions. Retained the selected typed declarations and replaced both unsupported generic alternatives with `#error`, restoring original following diagnostic lines. Eight selector controls, four x86/x64 line controls, guard tests, worker builds and cached builds passed. The complete same-identity ten-image candidate has five byte-identical images and five differing only in PE timestamps; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 624 newly excluded and 895 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r39.zip`; evidence: `s13-bfs-depth1-cvid-local-alternatives.json`, `s13-bfs-depth1-cvid-line-controls.json`, and `s13-bfs-depth1-cvid-package-compare.json`.

### First-depth DPMI kernel-VDM alternatives

Five original `i386` alternatives in four DPMI files were matched byte-for-byte to the fixed OpenNT bodies and identified as NT kernel-VDM context, native LDT or direct mapping paths. All observed source and include contexts omit `i386`, and the include chains do not define it. Their original selectors remain with the unsupported bodies replaced by `#error`; selected software branches and original continuation lines are preserved. The separate WOW32 x86 calling-convention paths and locally altered DPMI i386 branches remain pending. Selector and x86/x64 line controls, rejection tests, worker and cached builds passed. The complete same-identity ten-image candidate has five byte-identical images and five differing only in PE timestamps; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 629 newly excluded and 890 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r40.zip`; evidence: `s13-bfs-depth1-dpmi-i386-plan.json`, `s13-bfs-depth1-dpmi-hardware-alternatives.json`, `s13-bfs-depth1-dpmi-hardware-line-controls.json` and `s13-bfs-depth1-dpmi-hardware-package-compare.json`.

### Correction: i386 and CPU40 virtual interrupt boundary

The earlier `nt_msscs.c` original outer restoration prematurely rejected `i386` together with CPU40, despite DIV-554's documented coexistence boundary and the previous DIV-039 machine-owner behavior. Restored the previous CPU40 behavior in both original outer alternatives, kept the original selector, and returned the i386 arm to pending until its supported context is established. The nested CPU40 guard is queued for the next depth. Both actual worker preprocessing outputs remain unchanged; x86/x64 controls now verify CPU40 with and without i386 and rejection of the legacy non-CPU40 path. Worker and cached builds and guard tests passed. The complete same-identity ten-image candidate has five byte-identical images and five differing only in PE timestamps; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 628 newly excluded and 891 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r41.zip`; evidence: `s13-bfs-depth1-msscs-combined-correction.json` and `s13-bfs-depth1-msscs-combined-package-compare.json`.

### First-depth local host register byte order

Excluded the unused non-LITTLEND arm of the existing DIV-320 CCPU host-register union adaptation, preserving the selected low/high byte layout and original continuation line 131. All 172 observed include contexts include `host_def.h` first, which defines LITTLEND; no included file undefines it. The actual union byte order and forced non-little-endian rejection were tested on x86 and x64, as were compiler diagnostics, guard tests, worker builds and cached builds. The complete same-identity ten-image candidate has five byte-identical images and five differing only in PE timestamps; the accepted release remains unchanged. First depth: 1,864 observed retained, nine original assertions retained, 1,658 already excluded, 629 newly excluded and 890 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r42.zip`; evidence: `s13-bfs-depth1-hostcpu-endian.json`, `s13-bfs-depth1-hostcpu-include-order.json`, `s13-bfs-depth1-hostcpu-line-controls.json`, and `s13-bfs-depth1-hostcpu-endian-package-compare.json`.

### Exception test tool compatibility with mirror subsets

The existing test-only exception profile generator failed because it attempted to extract an original non-PROD body already replaced with `#error`. It now uses the hash-pinned OpenNT file directly for the negative baseline, verifies the five recovered blocks individually against that original, and exercises the current source in the supported PROD profile. The x86 fixture passed 27 positive cases and the production environment gate; the original negative baseline failed as expected. This is not a whole-file equality proof or a complete exception-lifecycle proof, and no product source or binary changed. Evidence: `s13-bfs-depth1-exception-tool-repair.json`.

### First-depth local DPMI frame guards and non-CCPU test gap

Three existing CPU40 frame guards in `dpmiint.c` were traced to the local DIV-481 adaptation, introduced in `f1a641bd72`; the pinned original uses unconditional frame logic at those positions. Both actual worker source graphs define CPU40 and their include chains do not undefine it. Six x86/x64 selector controls passed and the actual selected-object fixture passed its frame, width, failure and return checks. The fixture as a whole did **not** pass: its explicit non-CCPU negative-object compile hit the existing nested DIV-515 rejection, a previously recorded second-depth boundary. It remains a real unresolved test gap and must not be claimed as passed or removed to manufacture closure. No product source or release changed. First depth: 1,864 observed retained, nine original assertions retained, 1,661 already excluded, 629 newly excluded and 887 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r43.zip`; evidence: `s13-bfs-depth1-dpmi-local-frame-guards.json` and `s13-bfs-depth1-dpmi-nonccpu-profile.json`.

### First-depth EMS hardware mapping alternative

Replaced the original `X86GFX && NTVDM` EMS header branch with `#error` after matching its body to the fixed OpenNT original and checking all 18 actual include contexts. Those contexts omit X86GFX, and their included files do not define it; this is scoped to `emm.h` and does not exclude X86GFX in other video units. The selected alternative and original diagnostic continuation are retained. Eight x86/x64 operand controls, two selected-line controls, guard tests, worker builds and cached builds passed. The complete same-identity ten-image candidate has five byte-identical images and five differing only in PE timestamps; the accepted release remains unchanged. The separately recorded non-CCPU DPMI negative-object test remains unresolved. First depth: 1,864 observed retained, nine original assertions retained, 1,661 already excluded, 630 newly excluded and 886 pending. Original-structure reconciliation remains open. Queue: `s13-bfs-depth1-working-queue-195r44.zip`; evidence: `s13-bfs-depth1-emm-x86gfx-plan.json`, `s13-bfs-depth1-emm-x86gfx-exclusion.json`, `s13-bfs-depth1-emm-line-controls.json`, and `s13-bfs-depth1-emm-package-compare.json`.

### First-depth original selector reconciliation

Restored the fixed OpenNT outer selectors in `xt.c` and `copy_fnc.c`, retaining the DIV-155 and DIV-064 selected behavior. Both actual worker preprocessing outputs match the preceding source ignoring blank lines; diagnostic controls verify original following lines 609 and 125. The selected inner condition is queued without deeper review. x86/x64 cached builds and guard suites passed. The ten-image same-identity candidate matches the accepted release exactly in five images and differs only in previously verified PE timestamps in five, so the existing release remains in place. Fixed-original signature reconciliation covers 1,321 MVDM-host files and leaves 24 unmatched root occurrences in 13 records for investigation; these are structural leads, not established defects. The separate DPMI non-CCPU negative-object fixture remains unresolved. First-depth arm status: {'pending': 886, 'newly-excluded': 630, 'already-excluded': 1660, 'retained-observed': 1865, 'retained-original-assertion': 9}. Restored original first-depth groups: 47 relative to the fixed head, 14 in the current BFS pass. Evidence: `s13-bfs-depth1-two-original-outers.json`, `s13-bfs-depth1-mvdm-original-structure-inventory.json`, and `s13-bfs-depth1-two-outers-package-compare.json`. Queue: `s13-bfs-depth1-working-queue-195r45.zip`.
