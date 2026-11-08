# Project Status

## Current Work

**Active: M0 T481 S3 — Win3.1 VM I/O and nested-chain completion.**
Owner expands T481 to own the complete queue-first guest multitask completion
package. S1 integration delivery is committed and pushed at ceea7f1f8; its
package remains the comparison baseline, not proof of complete VM I/O.
The [working plan](../etc/operations/m0-t481-multitask-completion-plan.md)
retains every T479 S6 obligation and the remaining sequential stages.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | Queue-first guest multitask I/O completion, now admitted into T481. |
| Identifier Mode | M0 T481 S3, Ordinary Mode. |
| Admission And Approval | Owner directs this T to finish the queue-first task and sets active goal: complete multitask payload functionality. |
| Objective | Complete remaining reached Win3.1 input, text, geometry, fonts, codepages, display/metadata and nested chains without suspending unrelated VMs. |
| Non-goals | No original guest modification/rebuild; no Win95-first work, hardware V86 monitor, speculative protocol completion or new lifecycle authority. |
| Reference Baseline | f25220d98; S2 published package8b216ec7-75d1-4979-a21e-c588ca4d02d6. |
| Files And ABI Surface | Authored ntvdm64-386, NTVDM VM text/I/O/frontend, NTVWM presentation and existing RPC/I/O callers; exact changes follow caller/provider audit. |
| Applicable Rules | Execution, architecture, coding, source policy and the approved BIOS/INT16 input-return boundary. |
| Verification | Reached caller/provider matrix; actual Win3.1/native I/O acknowledgements, nested chains in both completion orders, input/output/redirection and cancellation; cached builds and applicable Full gates. |
| Expected Markers | No lost returned keys, no cross-VM delivery, no stranded worker or blocked retirement; actual guest receipts rather than dispatch-only coverage. |
| Asset Needs | Existing immutable Win3.1 media and owned probe runners; independently authored addon build. |
| Reporting Requirements | Selected code versus candidate; caller/provider evidence; exact open failures and coherent package hashes. |
| Stop Conditions | New semantic concession or original guest patch required; retain failing evidence and continue independent work. |
| Exit Criteria | Required Win3.1 operation and nested-chain matrix proved; coherent host/addon publication, review, commit and push. No known foundation defect transferred to force closure. |
| Original Owner Request | This T must also own complete queue-first multitask payload completion. |
| Similar-Issue Sweep | VM generation reuse, all four cancellation owners, input ownership, nested handback and NTSRV coordination. |
| Temporary Evidence | build/t481-s3, task-owned runs; 24-hour/4-GiB review budget, inspect no-progress runs at their contract deadlines; archive required evidence before disposal. |

## Current Technical Baseline

S2 P1 package8b216ec7-75d1-4979-a21e-c588ca4d02d6 is published at both
release destinations: all11 host images, Full04116/16, final production VM
retirement/reuse, diagnostic unread-input handback and normal Win3.1 exit.
Win3.1 ntvdm64.386 is published only in assets/release. This is a bounded
verified baseline, not complete VM I/O. S2 is closed with owner-approved historical failure disposition in the working plan.


## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure and qualification limits.
- [T479](../history/m0-t479-closure.md): owner-directed closure; unfinished guest multitasking is preserved in the queued successor.

## Recent Governance

S2 P1-P5 are committed/pushed through f25220d98. Cached builds, Full04116/16,
real input/lifecycle probes and input0573126 assertions per width qualify the
bounded S2 closure. Owner directs that unreproduced fill002/119 remain history,
not active defects or TODO; investigate only on a current reproducer. S3 is
admitted under the existing sequential plan and continuous-execution approval.
No whole-T completion or Win95 qualification is claimed.

## S3 Current Delivery And Evidence

P4 production repair is committed/pushed at6f0c60bfa. Complete11 package
c7f7cdca-4214-4452-a01d-d2f1c3858d25 is hash-verified at assets/release and
O:/winnt/system32; independently authored addons remain assets-only. Full196
passes16/16, keyboard/PIC89/89, editor198/199 and nested200 pass. Evidence:
artifacts/t481-s3/final201.json and publication203.json. No original guest
binary is modified. Reset clears device queues before cancelling IRQ because
cancellation synchronously invokes EOI/refill; DIV-321 registers the repair.
DIV-317 separately excludes consumed port data from unused-input history.

P5 test-only delivery is committed/pushed at e6313ac62. Managed worker-close205,
service-loss206 and console-close207 pass real ordered VM task retirement.
Redirect209 proves separate MEM files, A/B append markers and pipe output;
original nested210 also passes320 callback checks and normal Windows exit.
Delivery212 verifies both destinations retain all11 exact hashes and manifest;
governance passes. Source records: artifacts/t481-s3/worker-close205.json,
lifecycle206.json, redirect209.json and qualification210.json. These provider
stimuli do not claim physical guest-keyboard proof or every cancellation order.

The owner-required dual EDIT.EXE/EDIT.COM keyboard/menu/mouse exit matrix and
DOSONLY1 guest EDIT/native rejection/zero-host-admission control are verified
in the retained working plan and final201. Saved-file contents and arbitrary
modes are not inferred from editor interaction. The full S3 scope remains open.

## S3 P6 Delivery

The candidate fixes initial fontless physical-VM publication without replacing
its guest-owned glyphs: NTVWM sends the existing mapped VGA cells through the
ordinary text transaction, then uses the text barrier. Known-font VIDEO and
NTCON retain their exact presentation serial. An off-viewport cursor is hidden
in the guest projection while its absolute native position is retained.

Evidence artifacts/t481-s3/final247.json: cached231 and candidate241 passed;
Full24516/16 passed; native240 passed2253 assertions; editor242 proves both
EDIT menus, guest mouse Exit and normal Windows exit; nested243 proves actual
MEM output in both VMs; grid244/246 prove A80x60/font8 and B80x25/font16 in both
completion orders with320 callback checks. Source review238 and cursor230 retain
negative evidence. Editor236 exposed duplicate Unicode conversion (op15/1113);
using the established packed-glyph mapping fixes it, verified by editor242.
The BIOS1130 test now preserves ES, avoiding corruption of its next environment.

Production package build/t481-s3/r245-package contains all11 manifest-verified
images. Its NTVWM input hash equals guest-tested diagnostic package241's input.
Publish248 verified all11 images and manifest at assets/release and
O:/winnt/system32, package14d987d1-7fe7-4a7d-bc6e-af50e08c0bd6. Addons remain
assets-only. Governance247 passed. Evidence artifacts/t481-s3/publication248.json.
P6 includes reviewed source, tests and published assets; S3 remains active.
No original guest or mirror is changed.

## S3 Current Font Evidence

Test-only font252/253 passes both native completion orders: each VM compares
its actual nonempty VGA glyph banks before/after all three continuations.
A1540/B2320 nonzero bytes,320 callbacks and normal Windows exit per run;
original guest unchanged. Evidence: artifacts/t481-s3/font253.json. This is
BIOS-selected font preservation, not arbitrary custom font or palette proof.
P7 palette254/255 additionally proves distinct guest DAC7 red/blue, all16 text
DAC entries and attribute mappings preserved after three commands in each VM,
in both completion orders. Evidence: artifacts/t481-s3/palette255.json.
Test-only delivery retains the unchanged P6 production package; full S3
closure and arbitrary native palette/font changes remain unqualified.

## S3 P8 Candidate

CP850 publication reproduced wrong glyph003f for U+00F8 (expected009b).
The NTVWM candidate queries existing guest CODE_PAGE and reuses the current
glyph converter; no mirror/guest/protocol change. Provider257 passes216 checks;
native259 passes604+605+1084 assertions, including the failing CP850 case.
Evidence: artifacts/t481-s3/codepage259.json. Qualification267 passed cached
builds and guest editor/nested/font/palette cases; Full266 passed15/16, failing
nested-window input delivery despite expected exit23. Focused272 passes three
times with diagnostic observer, not proof of root cause. Full273 passes16/16;
all11 candidate hashes and guest-tested NTVWM input identity are verified.
Evidence: artifacts/t481-s3/handoff272.json and qualification273.json. Bounded
diagnostic274 passes all10 repetitions with per-milestone timing and unchanged
assertions; handoff274.json retains the result. Full266's single failure is
unreproduced, not attributed or claimed fixed. Publication277 deployed all11
verified images and manifest to both destinations; addons remain assets-only.
Evidence: artifacts/t481-s3/final275.json and publication277.json. P8 is committed/pushed at6a7c6ee8e; S3 remains active.

## S3 guest grid transition evidence

Test-only279/280 pass both completion orders after each real VM changes its
BIOS grid/font before all three native continuations (A8/16/8px, B16/8/16px).
Each run verifies three actual configurations, nonempty glyph and palette
preservation,320 callback checks and normal Windows exit. Original guest
installation unchanged. Evidence: artifacts/t481-s3/grid-switch281.json.
This is guest-owned change between commands, not native-requested resize
inside a command. Product publication277 stays unchanged; P9 is committed/pushed at97aa65255.

## Remaining Scope

S3 remains active: reached operations, arbitrary runtime geometry changes,
actual font bitmap/palette preservation, codepage/layout cases, display/metadata
and remaining nested cancellation boundaries stay in the working plan. Grid
height preservation is not proof of arbitrary font contents. Combined editor save/content and mouse Exit pass306/307 in both orders. Guest pointer clipping must
not alter the unrelated modern desktop. S4 Win95 and S5 combined qualification
remain uncompleted. The BIOS/INT16 handback boundary remains approved.
No S3 or whole-T closure is claimed. Historical evidence stays in the working
plan and artifacts rather than an expanding Current chronology.

P10 scanline investigation and failed controls remain in the working plan and
artifacts/t481-s3/scanline288.json, scanline292.json, scanline294.json and
qualification297.json. The completed delivery is recorded below.

## S3 P10 delivery

Production301 passes all four Win3.1 standard/enhanced and DOSONLY0/1 cases.
Publication304 deploys all11 tested images and manifest to assets/release and
O:/winnt/system32, package d75521ce-8b98-4319-9ec7-31b445a12af3; all hashes
match. Authored addons remain assets-only and unchanged. Full29716/16,
DOS298, editor300, scanline294/295 and fixture302 qualify the bounded DIV-890
repair. Evidence: artifacts/t481-s3/review303.json and publication304.json.
P10 is committed/pushed at c3df64bdf. S3 operation scope and S4/S5 remain open.

Dual-editor306/307 now combine SaveWitness and guest mouse Exit in both
completion orders; files A.TXT/B.TXT contain aaa/bbb and both VMs/Windows exit
normally. Evidence: artifacts/t481-s3/editor307.json. Earlier save-only
qualification remains valid; this adds the latest combined path. Visible-grid
resize308 is a new baseline witness: native80x50 buffer AND50-row viewport,
separate from the old25-row scrollback test. Runtime geometry work stays open.

Resize308 is a retained failure, now localized: four publications each write25
rows then WRITE_CELLS_A returns87 for the next row. Native readiness verifies
actual80x50 visible geometry; the VM provider still bounds writes to its
existing BIOS grid. Guest apply also rejects different rows/font, so enlarging
only host storage is insufficient. Evidence: artifacts/t481-s3/resize308.json.
Windows exits normally; this does not make the identity assertions pass.
No new production changes or publication after P10.

Region311 candidate adds explicit visible-region staging with size derived from
the header. x86/x64 mvdm-vm-text tests pass grow/shrink, refusal rollback and
disconnect discard. Evidence: artifacts/t481-s3/region311.json. Native publisher
and real BIOS geometry application are not yet connected; resize308 remains
failing. No new publication/commit and no S3 closure.

Native312 connects PREPARE_TEXT_REGION before VM cell/VIDEO writes inside the
existing transaction. Presentation/input-return/lifetime fixtures pass; the peer
checks preparation ordering and visible20x8->20x10, not hidden20x16 scrollback.
Evidence: artifacts/t481-s3/native312.json. This supersedes Region311's publisher
not-connected limitation only. Real guest geometry switching is still pending.

## S3 visible-grid candidate qualification

NTVDM stages grid/font-height proposals; NTVWM prepares the visible region
inside its existing publication; authored ntvdm64.386 calls original BIOS
font-row services and queries dimensions before acknowledging. insignia.386
and original Windows guest binaries are unchanged. Diagnostic313 builds.
Run314 exposed a probe ES-clobber after BIOS1130, corrected before315.
315/316 pass80x50 in both completion orders.317 passes25->50->25 with actual
BIOS rows/font and cell markers in both VMs,320 callback checks and normal
Windows exit. Evidence: artifacts/t481-s3/resize316.json and resize317.json.
Mouse source audit confirms absolute cell events, not NTCON pixel deltas.
Cached319 completes exit0; both shared suites14/14 and keyboard/PIC pass. Custom-font/other-mode changes
and remaining S3 operations remain open; this is not universal resize proof.
Full320/editor321/editor322/production323 are serialized by qualification320
(owner PID29024; script/log/exit records under build/t481-s3). Production
source stays frozen. No new publication or commit until qualification/review.

Qualification320 completes exit0: Full32016/16, editor321/322 save and guest
mouse Exit in both orders, production323 all four mode/DOSONLY startup and
normal-exit cases pass. Evidence: artifacts/t481-s3/full320.json, editor322.json
and review324.json. Complete publication and P11 commit/push remain pending.

P11 publication326 completed: all11 tested host images and manifest match at
assets/release and O:/winnt/system32, package b6c8ac10-d9d0-46d8-81d4-e7f86ee8991a.
Updated authored ntvdm64.386 and unchanged insignia.386 remain assets-only.
Evidence: artifacts/t481-s3/publication326.json. Governance324 passed.
P11 is committed/pushed at367612661; S3 and later S4/S5 remain open.

Custom328 exercises original BIOS1100 loading distinct glyph65 bytes in two
VMs, verifies actual VGA bytes before/after native continuations and changes
guest grids between commands. This test-only addition uses package313 and
driver313; runtime sources and publication326 remain unchanged. Result pending.

P12 test-only custom329/330 pass both completion orders: original BIOS1110 loads
distinct glyph65 bytes in each VM, actual VGA reads prove installation and
three complete glyph/palette comparisons per VM preserve them across commands
and guest grid switching;320 callbacks and normal Windows exit per run.
Custom328's1100 stale-geometry test assumption is retained as a failed control.
Evidence: artifacts/t481-s3/custom328.json and custom330.json. Production
source/addons unchanged; publication326 all11 hashes still match both targets.
Full320 is reused by unchanged-input identity. Native font replacement/DBCS
and remaining S3/S4/S5 are not qualified by this test.
