# S10 Ordered Input and Video Repair Progress

Admitted after [S9 closure](s9-progress.md#s9-closure) under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Thirty assigned files,
1060 frozen snapshot points. Counts below are the frozen register population,
not current byte-sensitive diff counts. Measure each complete file at entry;
do not substitute normalized/token counts for original-relative byte differences.
P1 delivered the first file; 29 S10 files remain. Order is canonical relative path.
Whole-file handling and targeted published-package acceptance precede delivery.
Full remains S13; original guest remains immutable.

| Order | File below src/mvdm-host | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | softpc.new/base/cvidc/ev_glue.c | 4 | Delivered f3dda8496 |
| 2 | softpc.new/base/cvidc/evidgen.h | 3 | Delivered d406cb610 |
| 3 | softpc.new/base/cvidc/sevid000.c | 64 | Delivered d406cb610 |
| 4 | softpc.new/base/cvidc/sevid001.c | 25 | Delivered d406cb610 |
| 5 | softpc.new/base/cvidc/sevid004.c | 3 | Delivered d406cb610 |
| 6 | softpc.new/base/cvidc/sevid016.c | 4 | Delivered d406cb610 |
| 7 | softpc.new/base/cvidc/sevid017.c | 11 | Delivered d406cb610 |
| 8 | softpc.new/base/cvidc/sevid018.c | 9 | Delivered d406cb610 |
| 9 | softpc.new/base/cvidc/sevid019.c | 3 | Delivered d406cb610 |
| 10 | softpc.new/base/inc/gvi.h | 5 | Delivered d406cb610 |
| 11 | softpc.new/base/keymouse/keyba.c | 44 | Delivered 99c8fc6b1 |
| 12 | softpc.new/base/keymouse/keybd_io.c | 3 | Delivered 3a3ac1248 |
| 13 | softpc.new/base/video/cga.c | 157 | Delivered 3a3ac1248 |
| 14 | softpc.new/base/video/ega_prts.c | 30 | Delivered d4c87dd1d |
| 15 | softpc.new/base/video/ega_read.c | 7 | Delivered d4c87dd1d |
| 16 | softpc.new/base/video/ega_vide.c | 308 | Delivered d4c87dd1d |
| 17 | softpc.new/base/video/ega_writ.c | 5 | Delivered a6ba6aa72 |
| 18 | softpc.new/base/video/egawrtm0.c | 20 | Delivered a6ba6aa72 |
| 19 | softpc.new/base/video/egwrtm12.c | 30 | Source reviewed; P7 delivery pending |
| 20 | softpc.new/base/video/gfx_updt.c | 94 | Not started |
| 21 | softpc.new/base/video/v7_ports.c | 17 | Not started |
| 22 | softpc.new/base/video/v7_video.c | 2 | Not started |
| 23 | softpc.new/base/video/vga_mode.c | 9 | Not started |
| 24 | softpc.new/base/video/vga_prts.c | 24 | Not started |
| 25 | softpc.new/host/src/nt_cga.c | 10 | Not started |
| 26 | softpc.new/host/src/nt_event.c | 50 | Not started |
| 27 | softpc.new/host/src/nt_graph.c | 91 | Not started |
| 28 | softpc.new/host/src/nt_hosts.c | 1 | Not started |
| 29 | softpc.new/host/src/nt_mouse.c | 23 | Not started |
| 30 | softpc.new/host/src/nt_vga.c | 4 | Not started |

## P1: ev_glue.c

[Whole-file review](s10-p1-ev-glue.md): 21/1663 ->5/102 (-16/-1561).
All four frozen points reviewed and residuals registered; zero complete frozen
point restorations. One of 30 files reviewed; 4/1060 points reviewed, 1056 remain.
No delivered S10 file yet. Entire 30-file baseline 201/102863 ->185/101302;
newline/EOF and unpaired added conditional guards are counted. The three initial
mapping failures were resolved explicitly, not dropped from the denominator.
Cached build/dual publication complete; final-package targeted tests pending.

P1 final verification: runtime1 cached build, dual publication (26 hashes),
actual C-VID fixture with 30 added dirty-span cases, live MEM/NTVDMD and
NESticle/Escape passed. All census hashes match. One file final-verified;
P1 commit/push pending. Four residuals remain owner-review items, not restored.

P1 delivery receipt: f3dda8496 committed and pushed to origin/main.
One of 30 files delivered; 4/1060 frozen points reviewed, 1056 remain.
Non-error total=185/101302, cumulative reduction=16 points/1561 lines.
Four evidence-backed residuals remain pending; restored frozen points=0.
Documentation governance and links passed. Next ordered file: evidgen.h.

## P2 source-review checkpoint: evidgen.h

[Whole-file review](s10-p2-evidgen.md): 3/8 ->3/8; two CRLF endings restored
on already-added adaptation lines. Original table preserved; actual necessity
is avoiding generated-accessor shared-scratch writes, not incompatible table
layout. All three pending entries and the master register now carry that evidence.
Source reviewed 2/30 files and 7/1060 points; 1053 points remain unreviewed.
Delivered remains 1/30 files, with four final-verified residual points in P1.
Seven reviewed residuals, zero whole-point restorations or new retain approvals.
Total remains 185/101302, net -16/-1561. Current hash is in the
[live census](s10-current-census.csv); P1 census remains the delivered snapshot.
No P2 build/test/publication/commit/push yet: format-only work accumulates
until the next whole-file batch. Next sevid000.c.

## P2 source-review checkpoint: sevid000.c

[Whole-file review](s10-p2-sevid000.md) and its 64-row record verify all residual
reader adaptations against actual plane layout, original field getters and
nested local-frame lifetime. Restore LF only: 10/6270 ->64/132 (+54/-6138),
covered by owner approval for exposing existing residuals through original
format restoration. No new functional differences. Source reviewed 3/30 files,
71/1060 points; 989 unreviewed, zero fully restored frozen points, 71 residuals.
Delivered remains 1/30. Current total 239/95164 against baseline201/102863:
38 more exposed points, 7699 fewer differing lines. Two format-only files
await cumulative P2 build/publication/tests/commit/push. Next sevid001.c.

## P2 source-review checkpoint: sevid001.c

[Backward-reader review](s10-p2-sevid001.md): all25 points reviewed, original
LF restored; 8/6974 ->25/50 (+17/-6924), no functional change. Four of30 files
and96/1060 points reviewed;964 unreviewed,96 residuals, zero whole-point
restorations. Three format-only files await P2 delivery; one file is delivered.
Thirty-file total256/88240, baseline201/102863, delta+55/-14623.
Next sevid004.c. No P2 build/publication/test/commit/push yet.

## P2 source-review checkpoint: sevid004.c

[Generic-rotation review](s10-p2-sevid004.md): all three offsets justified by
the original DWORD field and x86 low byte, with actual1..7 rotation cases in
the existing fixture. Restore LF only:1/10358 ->3/6 (+2/-10352).
Five of30 files,99/1060 points reviewed;961 unreviewed,99 residuals,zero
whole-point restorations. Total258/77888 against201/102863:delta+57/-24975.
Four format-only files await P2 delivery; one file delivered. Next sevid016.c.

## P2 source-review checkpoint: sevid016.c

[ModeX byte ownership](s10-p2-sevid016.md): four points reviewed, with concrete
S_2628 to S_2629 parent-frame cleanup evidence. LF-only recovery1/9060 ->4/12
(+3/-9048). Six of30 files,103/1060 points reviewed;957 unreviewed,103 residuals,
zero whole-point restorations. Total261/68840, baseline201/102863,delta+60/-34023.
Five format-only files await P2; one file delivered. Next sevid017.c.

## P2 source-review checkpoint: sevid017.c

[Whole-file review](s10-p2-sevid017.md): 1/6722 ->11/36 (+10/-6686),
original LF restored without functional edits. Eleven residuals have explicit
plane-layout or actual parent-frame evidence. Reviewed7/30 files,114/1060
points;946 remain. Total271/62154, cumulative+70/-40709. One delivered file;
six format-only files await P2. No new P2 runtime verification or delivery.

## P2 source-review checkpoint: sevid018.c

[Whole-file review](s10-p2-sevid018.md):9/32 unchanged, no format or source
edit. Nine residuals evidenced;8/30 files and123/1060 points reviewed,937
remain. Total271/62154, cumulative+70/-40709. Delivery remains1/30; P2 pending.

## P2 source-review checkpoint: sevid019.c

[Whole-file review](s10-p2-sevid019.md):3/6 unchanged. Three scalar-layout
residuals evidenced; historical generated block-copy limitation distinguished
from the current CCPU scalar SAS route. Reviewed9/30 files,126/1060 points;
934 remain. Total271/62154, cumulative+70/-40709. P2 delivery pending.

## P2 source-review checkpoint: gvi.h

[Whole-file review](s10-p2-gvi.md):36/41 ->32/33, four whitespace points
restored and XP491 anchor corrected. Thirteen exclusions unchanged;114
matched original content lines checked. Reviewed10/30 files,131/1060 points,
929 remain. Four locally restored points,127 evidenced residuals. Total267/62146,
cumulative+66/-40717. P2 cached build and delivery now being performed;
prior passing evidence is not a P2 acceptance claim.

P2 delivery checkpoint: build/publication owner PID15628 completed; cached
full build and runtime2 dual publication both report PASS. Documentation and
relative-link gates passed after repairing accidental encoding drift in eight
pre-existing documentation lines. Targeted runner PID2192 is active; final
C-VID/live MEM/NTVDMD/NESticle results and commit/push remain pending.

## P2 final verification

[Delivery verification](s10-p2-delivery.md): runtime2 cached build, dual13-image
publication,26 rechecked hashes and C-VID/live MEM/NTVDMD/NESticle passed.
Nine P2 files final-verified;10/30 including P1. Four restored frozen points,
127 residuals,929 unreviewed. Total267/62146, cumulative+66/-40717.
Commit/push pending. Historical checkpoint paragraphs above retain their
then-current state; this receipt supersedes their pending verification state.

P2 delivery receipt: d406cb610 committed and pushed to origin/main. Ten of30
files delivered; four frozen points restored and delivered,127 residuals
await decisions,929 unreviewed. Next keymouse/keyba.c.

## P3 keyba source review

[Whole-file review](s10-p3-keyba.md):1/5452 ->220/308. Twenty format points
restored;24 functional residuals evidenced.11/30 files,175/1060 points reviewed,
885 remain. Total486/57002, cumulative+285/-45861. P3 delivery pending.

P3 checkpoint: owned PID1156 completed cached full build and dual publication
of runtime3. Targeted PID12580 is running controller/PIC, keyboard contracts
and final-package guest tests. Documentation governance/links passed. No
P3 test pass or commit/push is claimed yet.

P3 final verification: runtime3 cached build, dual13-image publication and26
hashes passed; production controller/PIC90 assertions, four keyboard contracts,
C-VID, real MEM/NTVDMD and NESticle passed. Eleven files final-verified;
24 restored frozen points verified (20 in P3),151 evidenced residuals,885
unreviewed. Commit/push pending. No Full claim.

P3 receipt:99c8fc6b1 committed and pushed to origin/main. Eleven files
delivered;24 fully restored points delivered,151 residuals await decisions,
885 unreviewed. Ordered table and master point delivery states synchronized.

## Keybd IO source review

Twelfth file, HXP-R-01201--01203. Baseline hash
5d0f2e8f0de6c909fc21573fc9a1bb2497d4681d22f8f1e7fc465cc30001f68a6.
Fixed XP input is the existing hash-checked keybd_io.c.xp snapshot.
Reviewed BOP 5F -> kb_setup_vectors, CPU40 effective_addr, InitIntelMemory,
and video.c/gfx_updt.c stream consumers. CPU40 returns a numeric guest-linear
address; VirtualAlloc(NULL, ...) creates a separate host allocation. Original
pointer casts omit its base. R01201 must construct the host table pointer;
R01202 must pass the numeric guest address, not that host pointer, to SAS;
R01203 must construct host stream pointers used by direct dereference.
Changing effective_addr globally would break its SAS-address consumers.
Three points remain evidence-backed owner decisions, not approved retention.
This proves address-domain necessity, not arbitrary protected-mode paging
or remapping lifetime safety of the cached stream pointers. No new lifetime
behavior is claimed or introduced.

Removed seven #line anchors whose target already equals the following logical
line, with no intervening conditional boundary. All 32 #error bodies and
conditional frameworks unchanged; stripping #line yields byte-identical code.
Non-error metrics 82/90 ->75/83, reduction7/7. No function restoration.
12/30 files source-reviewed;178/1060 frozen points reviewed,882 remain.
24 restored points already delivered;154 residuals;11 files delivered.
Whole S10 census201/102863 ->479/56995 (+278/-45868). This file is local
format-only work awaiting the next accumulated delivery, not a new runtime pass.

## CGA source review

Thirteenth file,157 frozen points, [per-point record](s10-p4-cga-points.json).
Fixed XP object and source hash checked by inspect-path.py. Restored156 original
format points and LF;19 exclusion bodies unchanged. Exact non-whitespace
line comparison and remaining diff review show no changed executable code.
963 retained original non-directive lines have their exact XP logical lines;
zero mismatches. Frozen physical line positions had drifted, so restoration
used original-content alignment, not blindly applying stale offsets.

R02306 retains only two explicit void prototypes for setWritePointers and
setReadPointers(IUH), actually called by cga_init via egacpu.h macros. Both
providers are in ev_glue.c and return void. Original implicit-int declarations
do not describe these definitions. The prior generic word-width rationale
and nonexistent ev_glue.h reference were removed: neither proves this point.
No wrapper, new algorithm or conditional added. Residual remains pending.

Non-error diff1/2532 ->54/70 (+53/-2462);LF recovery exposes scattered
#line anchors. S10 cumulative201/102863 ->532/54533 (+331/-48330).
13/30 files source-reviewed,335/1060 points reviewed,725 remain.
24 restored/delivered,156 restored locally pending delivery,155 residuals;
11 files delivered. P4 will deliver keybd_io.c and cga.c together. No new
runtime success is claimed before cached build/publication/targeted tests.

P4 owned cached build/publication started 2026-11-26T13:15:51+01:00, PID11304;
command and host recorded in build/t485-s10/p4/run.json, build.log and
publish.log. Governance PID5380. Results pending, no runtime pass claimed.

P4 final verification: cached build and dual publication PASS,13 images/26
hashes checked against runtime4/product-system32-manifest.json. Background
runner PID14100 completed all targeted tests: C-VID setup/CHAIN2, real
controller/PIC90 assertions with0 failures, keyboard contracts, live MEM and
NTVDMD export/debug lifecycle, and actual NESticle menu rendering plus Escape
hide/restore. Evidence in build/t485-s10/p4/{cvidc.log,controller.log,
keyboard-contracts/result.json,debug-live,dos4gw}; all-tests-result.txt=PASS.
This is targeted verification, not Full or a new Windows desktop acceptance.
Thirteen files final-verified; P4 commit/push pending.

P4 delivery receipt:3a3ac1248 pushed to origin/main.13/30 files delivered,
335/1060 points reviewed,725 remain;180 restored/delivered,155 residuals.
No local restorations pending. Census532/54533 unchanged. Next ega_prts.c.

## EGA ports source review

Fourteenth file:30 frozen points;29 restored format points,one evidenced
residual R02461. Fixed XP hash verified by inspect-path.py. Actual provider
is void setMarkPointers(IUH) in ev_glue.c, selected through egacpu.h and
called by ega_init. The prior generic word-width rationale was incorrect
for this file; the one-line declaration supplies the actual void signature.
No provider logic, callback or conditional added.

Restored LF/original whitespace and original blank XP2982. Removed redundant
#line2985/3061/3111 and stale terminal#line2956.2218 matching original
non-directive lines have exact logical positions;zero mismatches.22#error
branches unchanged. Whitespace/#line-stripped file bytes identical.
Metrics1/5944 ->93/93 (+92/-5851),format approval applies. S10 total624/48682
vs201/102863 (+423/-54181).14/30 reviewed,365/1060 points reviewed,695 remain;
180 restored/delivered,29 local format restorations pending,156 residuals.
13 files delivered. Pure format batch may accumulate;next ega_read.c.

## EGA read source review

Fifteenth file:7 frozen points;6 original format restorations,one residual
R02491. cpu_vid.h READ_POINTERS and ev_glue.c actual definitions establish
IU32 byte/word returns,void string return and void(IUH) selector. Explicit
return types/declaration remain; table initializer whitespace restored.
No copied algorithm. Original LF/whitespace restored;25#error bodies unchanged.
379 matching original non-directive lines retain exact logical positions.
Whitespace/#line-stripped bytes unchanged.

Counter correction: repeated CPU_40_STYLE else directives caused the old
SequenceMatcher projection to map three excluded read-mode1 blocks to earlier
read-mode0 blocks. Explicit original#line877/900/918 anchors identify the
correct branches, whose directives match the originals. The diagnostic
metric-code-anchored.py in build/t485-s10 reproduces this mapping and leaves
only the declaration group as non-line source changes. Old before1/1563
and candidate76/191 included18 excluded lines. Corrected1/1545 ->76/173,
actual format reduction1372 lines;18-line counter correction is not a repair.
Only ega_read.c census baseline/current row corrected here.
S10 corrected baseline201/102845,current699/47292 (+498/-55553).
15/30 source-reviewed,372/1060 points reviewed,688 remain;180 restored/delivered,
35 local format restorations pending,157 residuals;13 files delivered.
Next ega_vide.c; pending format batch has not been built or published.

## EGA video source review

Sixteenth file:308 frozen points;307 original format restorations,one residual
R02590. Fixed XP object/hash verified;format candidate preserves executable
non-whitespace bytes and all42 exclusion bodies. RestoreLF/original layout
and add#line1473 after REAL_VGA else to fix the retained JAPAN branch mapping.
1804 matched original non-directive lines now have exact logical positions.
Only remaining non-line source change is twelve added V86 geometry lines.

Necessity is historical real evidence,not a speculative stale-cache warning:
T481 scanline288 records25->60 rows with physical481;scanline292 records
virtualVDE399->479 during the font BIOS call. scanline294 candidate restores
399 before/after with normal Windows exit;P10 c3df64bdf and T483 P69 record
subsequent delivery. These are historical witnesses,not fresh XP acceptance.
ios.c inb/outb uses IOVirtualised before physical adapter dispatch;gvi.h
get_screen_height reads PCDisplay/EGA_GRAPH. Physical CRTC getters likewise
cannot replace the active VM I/O path. Retain existing VGA+V86 scope,three
register reads,overflow/multiplier decode and index restoration. No new
algorithm or helper. Runtime boundary outside the existing scope is not
claimed. R02590 remains pending owner decision despite format reduction.

Metrics1/5005 ->117/134 (+116/-4871). S10 total815/42421 versus corrected
baseline201/102845 (+614/-60424).16/30 source-reviewed,680/1060 points
reviewed,380 remain;180 restored/delivered,342 local format restorations
pending,158 residuals;13 files delivered. P5 will deliver the three complete
EGA files together. Next ega_writ.c after delivery.

P5 owned cached build/publication PID3912 started2026-11-26T13:32:32+01:00;
command/host in build/t485-s10/p5/run.json. Targeted selection:C-VID actual
provider fixture,real MEM/NTVDMD and NESticle rendered-menu/Escape tests.
Keyboard suites omitted:only EGA whitespace/#line changed in P5; their
production inputs and P4 passed contract are unchanged. Full remains S13.

P5 final verification: cached build and dual publication passed;13 images/26
hashes checked against runtime5 manifest. Test PID12120 completed C-VID
selection/CHAIN2/scalar/string/SAS behavior,live MEM/NTVDMD debug lifecycle,
and real NESticle menu pixels with Escape hide/restore. Logs and success
markers in build/t485-s10/p5;source-review.json contains final per-file hashes.
No executable algorithm changed;historical V86 geometry evidence is not
presented as a new Windows desktop test.16 files final-verified;commit/push
pending. Full remains S13.

P5 receipt:d4c87dd1d pushed to origin/main.16/30 reviewed/delivered;680/1060
points reviewed,380 remain.522 restored/delivered,158 residuals,zero local
restorations pending. Census815/42421 unchanged. Next ega_writ.c.

## EGA write source review

Seventeenth file:5 frozen points;4 original format restorations,one residual
R02804. Fixed XP source hash verified. ev_glue.c actual fill_word_ev_glue
usesIU16,not the original declaration IU8;setWritePointers returnsvoid.
Retain these two declaration corrections. MEM_HANDLERS fill slot is
unprototyped,so this review does not claim every actual call truncates its
value. No new executable algorithm.

RestoreLF/original whitespace and remove redundant#line343/727.357 matched
original non-directive lines have exact logical positions;16 exclusion
bodies unchanged. Whitespace/#line-stripped bytes identical. Metrics
1/1200 ->55/122 (+54/-1078). Cumulative869/41343 versus201/102845
(+668/-61502).17/30 reviewed,685/1060 points reviewed,375 remain;522
restored/delivered,4 local format restorations pending,159 residuals;16
files delivered. Next egawrtm0.c.

The earlier sevid019 block-copy concern is not closed by this audit:
Glue_writes still contains move_byte/word_fwd_ev_glue,which dispatch selected
C-VID move providers. No claim that the generated block-copy path is globally
unreachable. Existing CCPU SAS move fixture success has its own scope.
The previously recorded odd-target block-copy boundary remains separate
from these declaration/format changes.

## Mode0 entry investigation

Eighteenth ordered file egawrtm0.c:20 frozen points,current1/2597 non-error
diff. No source edit yet.14 format candidates;four CHAIN2 arithmetic points
and two addedCPU40 table-framework points need final disposition.
Actual caller evidence:ega_writ.c selects ega_copy_b_write/ega_copy_w_write
or ega_mode0_chn_b_write into bios_ch2_* pointers. These scalar helpers are
not globally unused simply because C-VID owns normal CPU writes.
The original mode0_copy_handlers/mode0_gen_handlers initialized tables remain
a separate question:their current zero-storage replacement is not proved
necessary just by dynamic C-VID selection. Next compare selected WRT_POINTERS
ABI and original initializer providers before deciding restoration.
Do not count this file or any of its20 points reviewed/delivered yet.

## Mode0 source review

Eighteenth file20 points:14 format and2 original-table restorations;4
CHAIN2 scalar residuals. WRT_POINTERS fields match original prototypes;all
initializer targets exist in the same original unit. ega_writ.c selects
legacy handler addresses but CPU40 dispatches SetWritePointers instead of
installing their fields. Thus dynamic EVID selection is not evidence that
the original initialized tables must be removed. Restore XP266--318 and
remove the extra CPU40 zero-table/non-CPU40#error framework. This restores
previously replaced original code;all14 original exclusion bodies remain.

Four scalar differences stay:ega_writ.c still sets BIOS copy/generic byte
and word callbacks. Originalcompact addressing sends guest offset2 to
backing4;CPU40 plane layout requires8. Odd word offset1 writes1/8 rather
than1/4. Existing legacy fill/move algorithms are not asserted repaired by
these scalar changes. No new layout or function added.

RestoreLF/original whitespace;remove redundant#line427/472/1090.948
matching original non-directive lines have exact logical positions.
Metrics1/2597 ->39/54 (+38/-2543). S10 total907/38800 against201/102845
(+706/-64045).18/30 reviewed,705/1060 points reviewed,355 remain;522
restored/delivered,20 local restorations pending,163 residuals;16 delivered.
This original-table restoration is substantive and triggers P6 delivery of
the complete egawrtm0.c plus accumulated ega_writ.c changes.

P6 owned cached build/publication PID5564 started2026-11-26T13:48:22+01:00;
command and host recorded in build/t485-s10/p6/run.json. Final diff review
confirms original table declarations/initializers restored without moving
algorithms or adding providers. Targeted selection:actual C-VID fixture,
MEM/NTVDMD lifecycle and real NESticle menu/Escape. Full remains S13.

P6 final verification: cached full build and dual publication passed;13
images/26 hashes match runtime6. Test PID24268 completed actual C-VID
providers/CHAIN2/SAS assertions,real MEM/NTVDMD lifecycle and NESticle
rendered menu plus Escape hide/restore. Governance and links passed.
Final source hashes are in build/t485-s10/p6/source-review.json.18 files
final-verified;20 local restorations await this commit/push. Full remains
S13;no Windows desktop or legacy block-copy boundary closure is claimed.

P6 receipt:a6ba6aa72 pushed to origin/main.18/30 reviewed/delivered;705/1060
points reviewed,355 remain.542 restored/delivered,163 residuals,zero local
restorations pending. Census907/38800 unchanged. Next egwrtm12.c.

## Mode12 source review

Nineteenth file egwrtm12.c:30 points,18 format restorations,2 original-table
restorations,10 CHAIN2 scalar residuals. Restore XP205-255 declarations and
initialized mode1_handlers/mode2_handlers:all providers remain in this unit
and WRT_POINTERS fields match. ega_writ.c chooses their addresses but CPU40
installs dynamic C-VID providers through SetWritePointers; this is not a
reason to delete original tables. No new provider,algorithm or dispatch.

BIOS byte/word callbacks still call mode1/mode2 scalar writers. Current
plane layout requires guest2->backing8 and odd word1->backing1/8; original
compact arithmetic yields4 and1/4. Keep ten expressions across both plane
pairs and logical/simple mode2 paths. Legacy fills/moves are not claimed
repaired by these scalar changes. All11 original exclusion bodies retained;
812 matching original non-directive lines have correct logical positions.
Six redundant straight-section #line directives removed; LF restored.

Non-error diff1/2385 ->51/77 (+50/-2308). Cumulative957/36492 against
201/102845 (+756/-66353).19/30 files and735/1060 points reviewed;325 remain.
542 restored/delivered,20 local restorations pending,173 residuals;18 files
delivered. P7 whole-file table restoration triggers cached build,dual
publication,C-VID provider fixture,real MEM/NTVDMD and NESticle menu/Escape.
Full remains S13. Next gfx_updt.c after delivery.

P7 owned build/publication PID10428 started2026-11-26T13:56:32+01:00;
command/host in build/t485-s10/p7/run.json. Existing C-VID fixture covers
the selected dynamic provider layout;it is not presented as direct runtime
coverage of every legacy BIOS callback. Table ABI/provider availability is
verified by final product compilation and linking. Scalar restoration
rejection is based on actual caller selection and address-layout evidence.

P7 final verification: cached build and dual publication passed;13 images/26
hashes match runtime7. Test PID12316 passed actual C-VID selection and layout
fixture,real MEM/NTVDMD lifecycle and NESticle menu/Escape. Final source hash
in build/t485-s10/p7/final-source.json;line/metric evidence in source-candidate.json.
Governance and relative links passed.19 files final-verified;commit/push
pending. Full remains S13.
