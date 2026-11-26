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
| 12 | softpc.new/base/keymouse/keybd_io.c | 3 | Source reviewed; format delivery pending |
| 13 | softpc.new/base/video/cga.c | 157 | Source reviewed; P4 delivery pending |
| 14 | softpc.new/base/video/ega_prts.c | 30 | Not started |
| 15 | softpc.new/base/video/ega_read.c | 7 | Not started |
| 16 | softpc.new/base/video/ega_vide.c | 308 | Not started |
| 17 | softpc.new/base/video/ega_writ.c | 5 | Not started |
| 18 | softpc.new/base/video/egawrtm0.c | 20 | Not started |
| 19 | softpc.new/base/video/egwrtm12.c | 30 | Not started |
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
