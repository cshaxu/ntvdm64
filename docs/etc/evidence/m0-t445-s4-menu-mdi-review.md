# T445 S4 standalone menu and MDI review

## Expanded S4 delivery review snapshot, 2026-10-13

The r052 review below is retained historical evidence. It does not describe
the present dirty worktree or the later admitted standard controls. Current
candidate is r255, with full chronology in the
[control and scrollbar audit](m0-t445-s4-scrollbar-fidelity-audit.md).
The expanded-controls checkpoint is committed and pushed as f8e0d5878.
This snapshot is not S4 approval or owner acceptance.

| Required surface | Current direct evidence | Remaining boundary |
| --- | --- | --- |
| Original visual resources | r252 independently verifies all23 embedded resources against pinned owner VGA.DRV | Pixel resources do not prove layout or interaction fidelity; redistribution remains separately restricted |
| Frames and title/system buttons | r251 gallery and menu-dispatch gates retain geometry, suppression and command/lifetime checks | Owner's latest physical flicker acceptance is pending |
| Menus and MDI | gallery native queued loops, test_mdi_keyboard and menu-dispatch; both widths pass | Outer system-menu actual pointer route is not proved by private-desktop keyboard tests; native-menu session issue remains S7 |
| Buttons, checks and radios | test_showcase_keyboard plus control-present and gallery state checks | Message-path verification is not physical pointer acceptance |
| Static labels, groups and images | test_static_text/icon/bitmap, idle-damage and focused typography checks | Application-owned custom drawing is not restyled |
| EDIT | test_edit_scroll, native replacement/undo/selection and shared-font tests | Clipboard fixture compiles but cannot create isolated window station; no copy/cut/paste assertions ran |
| Lists and combos | test_combo_scroll_release, native-paint boundary, integrated/multicolumn scroll and list-tabs fixture | Unknown pre-existing native tab state requires explicit caller input |
| Scrollbars | scroll-geometry, scroll-api, scroll-test and frame callback tests | Native accessibility geometry and documented wide native fallbacks remain non-pass limitations |
| Shared24px System font | font-test, test_menu_font, control detach font restoration | CP1252 test coverage is not universal Unicode certification |
| Attachment lifetime | control-lifetime removal/destruction/nested-installation tests, with old-library negatives in r248/r250; rerun in r255 | Message-path coverage does not certify every arbitrary application callback |
| Delivery | r255 full x86/x64 pass;0 source-manifest mismatches; f8e0d5878 pushed to origin/main | Intermediate checkpoint, not expanded-S4 acceptance; unrelated Queue/WOW changes remain untouched |

r255 x64 demo SHA256:
2102F71F134AF961908C5A434D8FB5EAD674A07D8F28F965B0EB36FC3973DC15.
No production package or O:/winnt deployment is part of this standalone packet.

### Expanded-controls intermediate checkpoint

The checkpoint retains r255's tested standalone implementation and fixtures;
it is not final S4 review or acceptance. r255 passes both architectures and
its source manifest has zero mismatches. The renderer review continued through
static/button/list/combo drawing and scrollbar drawing/tracking helpers. It
found and repaired exceptional scrollbar composition publishing an unpainted
bitmap after SaveDC or checker-brush allocation failure. The real-painter
fixture now proves both failures preserve target pixels and clean up resources,
then proves a normal draw presents once. This is not a new diagnosis of the
owner's ordinary click flicker. See the detailed audit for exact evidence.

The clipboard fixture's isolated-window-station creation is denied before any
clipboard operation. Do not rerun it against the interactive clipboard without
explicit owner consent; a consent question is pending. The fixture remains
outside the default runner. Physical click/menu/scroll acceptance remains open.
The retained table above names earlier tests;
r255 reruns them plus the size-box and exceptional-scroll coverage.

### Resource revalidation procedure

r252 background runner14676 starts2026-10-13 00:39:20 local, deadline60s,
exit0. Before decoding it verifies the original O:/w31/SYSTEM/VGA.DRV hash
53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A.
The formal inspect-win31-frame-resources.ps1 runs with Driver, a fresh
build-owned OutputRoot and VerifyHeader for each family: frame_buttons.h
(default), menu_glyphs.h (-MenuGlyphs), control_glyphs.h (-ControlGlyphs),
combo_glyph.h (-ComboGlyphs), scroll_glyphs.h (-ScrollGlyphs and separately
-DisabledScrollGlyphs). Results verify7/2/1/1/8/4 resources respectively.
Each family retains resources.json and decoded images below r252. No new
resource import, guest write, font installation or broader rights claim occurs.

### Remaining closure decisions

Do not mark S4 complete from the retained27,261 headline count. Resolve or
obtain an explicit disposition for the isolated clipboard test, and obtain owner physical review
of sample01 plus outer system menus and combo/list scrolling. The existing
S7 deferral covers native-menu fresh-session blankness only, not all these
expanded-S4 requirements. Tests cannot silently narrow the owner request.

### Post-checkpoint source and coverage review

Review against f8e0d5878 covers the changed frame/layout/API code, controls
renderer and dispatch/installation/teardown, font lifetime, creation scope,
scroll geometry/tracker, runner changes and focused fixture assertions. The
last discovered defects are retained with regression evidence: installation
reentrancy (r249/r251), omitted original size-box variant (r254) and failed
scroll composition (r255). No further required source repair is identified
in this review. This is a bounded engineering review, not a claim of absence
of bugs or universal compatibility with all modern native controls.

The current headers consistently expose the standalone integration limits:
same GUI thread, explicit original EDIT border and known list-tab state,
caller-owned list/edit theme configuration and frame scrollbar API binding.
The demo supplies those inputs for its owned windows. Hook propagation,
arbitrary application attachment and production integration remain S5/S6,
not evidence supplied by this experiment. Modern accessibility geometry and
wide native fallback limits remain recorded, not silently called passing.

The r255 x64 all-controls-message-popup capture was visually inspected again:
the six families, three Combo forms, tabbed/multicolumn lists, both scroll
orientations and size box are present; the shared typography and layout have
no newly identified overlap. This is explicit WM_PRINT output, not physical
composition. It cannot settle the owner's clicking/flicker report. The r255
input manifest and pushed checkpoint identity agree; the unmodified executable
remains the candidate, with no redundant rebuild for this documentation review.

Closure is now waiting on two concrete acceptance decisions: actual owner
interaction of sample01 and outer menus1/12/18 (including dropdown thumb release
and moving focus caret), and isolated clipboard execution or an explicit owner
disposition. A question requesting permission for interactive clipboard use is
pending; no consent is inferred from automatic goal continuation. The deferred
native-menu fresh-session issue remains S7 separately. Do not close S4 or the
S3+S4 objective while these acceptance requirements remain unresolved.

### Sequential source-review findings after r252

Reviewed the current frame/layout/header diff, font allocation/reference
cleanup, creation-scope callback pins, control presentation/message dispatch,
scroll geometry and tracking termination. Frame feedback uses bounded menu/
button damage; borrowed DC state is restored. Bitmap-static image ownership
remains native. Control WM_PAINT buffers only its dirty region; native EDIT
editing is not replaced. Scroll termination commits state before application
notifications, and geometry uses widened arithmetic before narrowing. The
review does not identify another repair in these inspected paths. It does
not certify the unreviewed entire control renderer or physical composition.

Coverage finding: controls.c supported() rejects SBS_SIZEBOX together with
SBS_SIZEGRIP. OpenNT public/sdk/inc/winuser.h distinguishes them: SIZEBOX is
unconditional, while SIZEGRIP is gated by WINVER>=0x0400. Hash:
62384DCC83A298A9EE68ACEC85AAEFDD1CECC3F6880D1104E7FBCC4F3ABF636E.
OpenNT windows/core/ntuser/kernel/sbctl.c (hash
E60DD4AD4F71631CC312107BD4A3AC13FDD2CE577608E7F555AD68E2F78F44DE)
has separate fSize painting through DrawSize and size/zoom command handling.
Its private PWND/SBCalc/USER drawing and tracking boundary is not composable
as a standalone module; it is comparison evidence, not imported implementation
or proof of original Win3.1 artwork. The next required action is to identify
the supplied Win3.1 size-box visual/behavior contract and cover that legacy
variant in the gallery, preserving native command behavior. Do not substitute
a Win95 diagonal grip or declare the whole scrollbar family complete.

Follow-up: r254 implements the original size-box branch and adds a labelled
sample01 control. Three native/styled forms pass both-width pixel bounds and
parent-command tests; the newer SIZEGRIP stays excluded. See the original
binary derivation and metric/color adaptation in the scrollbar audit. The
current executable is r254; this resolves the identified missing variant,
not physical sizing acceptance or whole-S4 delivery.

Delivery scope remains src/win16-style, the named standalone tests/observation
scripts and T445 evidence/plan/status. Existing WOW proposal files and QUEUE
changes are unrelated and must not be swept into a blanket commit. No build
output, production image, mirror or guest change is included.

## Historical r052 review

## Question and boundary

Does the standalone component implement the admitted menu/MDI presentation
while preserving native management? S3 is owner-closed; S4 remains active.
This is review evidence, not owner acceptance. The
[working plan](../operations/t445-win31-window-style-plan.md) retains original
source comparisons, failed attempts and run chronology. No product component,
guest/mirror, global setting or production release changes. Native animated-menu
blankness remains the independent S7 question, not a repair claimed here.

## Candidate and procedure

Background tests/observation/verify-win16-style.ps1 run at
build/M0-T445/S4/r052: MSVC x86/x64 /MT /O2 /W4 /WX, Ninja, standalone library
and authored fixtures only. Runner PID35852 started 2026-10-12 13:38:59,
180-second deadline, exit0. Both widths pass 27,261 retained checks plus
menu/MDI and lifetime assertions. All 24 recorded input hashes were rechecked
after execution: zero mismatches.

| Executable under r052 | SHA256 |
| --- | --- |
| x64/win16-style-demo.exe | C76919BBB7071A80633BF946E04B7E9A0A85F5B45F9BE17EC05FD7E91F97E096 |
| x86/win16-style-demo.exe | 35C62AB90A50CF2633EF5F47FA2C5662070D7B64F7986CBCD90E11D84C3E2A9F |

r052-glyphs/resources.json verifies CHECK14x14 and MNARROW7x11 against pinned
VGA.DRV resources. No original executable code is imported.

## Requirement coverage

| Requirement | Evidence and result |
| --- | --- |
| System/ordinary menu painting | menu_items/painter/scope implement string rows, disabled stipple, selection, separators, shortcuts, original check/arrow and border; popup captures reviewed |
| Menu input | Queued native loops pass Alt/F10, arrows, Escape, mnemonics, hover switching and disabled/direct-item navigation; not physical input acceptance |
| Menu ownership | Temporary item data/type restored; application owner-draw retained; nested-loop and callback removal/destruction cases pass |
| MDI | Child geometry, activation, maximize/restore, merged system/restore controls, cross-owner WM_NEXTMENU and child-only close pass both widths; maximized capture reviewed |
| S3 preservation | 27,261 checks per width retain frame/client geometry, titles, palettes, native-button suppression and GDI lifetime; recorded GDI cycles show no net growth |
| Source boundaries | Standalone graph only; no production integration or mirror change |
| Visual/physical acceptance | Owner trial of gallery12/15/18 remains pending; prints and queued messages do not prove DWM/RDP or actual pointer behavior |
| Delivery | Governance/link and diff checks pass; implementation committed and pushed as de5526a16; unrelated queue/proposal work excluded |

## Separate review finding

Reviewed item restoration, nested hook scopes, cross-owner state pins and
reentrant callback exits. STYLE_NCUAHDRAWCAPTION/FRAME incorrectly shared the
mouse-move branch, allowing non-mouse payloads into menu hover selection.
r052 separates these drawing notifications. A focused test sends payloads
inside a real menu cell and verifies no selection change, on both widths.
This local dispatch repair does not diagnose native animated-menu blankness.

## Limits and follow-up

Implementation delivery de5526a16 is on origin/main. This is the standalone
trial delivery, not S4 owner acceptance. All remaining dirty paths belong to
the pre-existing WOW queue/proposal work, not the delivered component.

Arbitrary owner-drawn/bitmap bars, scrollbars, tool/layered/custom-client-edge
forms remain unsupported. Minimized presentation remains native as admitted
in S3. Arbitrary cross-thread/custom WM_NEXTMENU targets and external consumers
of native menu-bar geometry are not general compatibility claims. Final owner
acceptance remains open; keep these boundaries explicit during S5/S6 integration.
