# T445 S4 scrollbar fidelity audit

## Scope and identity

Read-only binary evidence from owner-provided O:/w31/SYSTEM/USER.EXE,
SHA256 98036967F8396B387DA431699A9D3BE95EB56FCD0118F529E1FC2F25105EFAC6.
No guest/module mutation or executable-code import. The approved isolated
style-resource/research boundary applies. r097-user18 contains reproducible
segment disassembly and relocation evidence from inspect-win31-ne-segment.ps1.
Previously retained S3 r029 supplies segment 3 initialization; its source
identity matches the current image. These are static observations, not a guest
execution trace or certification of all scroll behavior.

## Original chain

The NE export table maps SETSCROLLPOS ordinal62 to segment18:1A00 and
SETSCROLLRANGE ordinal64 to segment18:1A44. Those validation entries branch to
18:0ED9 and 18:0F16. The range/position service uses the original minimum,
maximum and current position, not a modern SCROLLINFO page-size parameter.

Segment3:1794--179D copies initialization globals 0680/067E into 0702/087C.
Segment18:0766 and 078C choose working blocks 06FC/0876; the copied values
therefore occupy offset +6 in the vertical/horizontal calculation blocks.
Segment18:0812--0819 adds that +6 length to the computed thumb start to obtain
the end. The inspected calculation does not derive that length from a page
fraction.

The follow-up read-only decoder inspect-win31-scroll-metrics.ps1 pins both
binary identities and resolves USER's automatic data segment45 offset01E2 to
the resource-type string OEMBIN. USER 3:0DE6--0E1C calls KERNEL FindResource,
LoadResource and LockResource for that type/id1 and copies its first two WORDs
to 0680/067E. VGA.DRV's matching resource is at file offset37024, length112;
both WORDs are17. Thus this VGA initialization supplies 17-pixel vertical and
horizontal thumb lengths. r103-metrics.json records the reproducible result,
hidden owner PID7036, 2026-10-12 15:53:36 local, exit0. Other display profiles
and subsequent driver changes are not certified by these pinned values.

Segment18:07F8--080F computes the position using the range/current fields and
calls segment1:39AB. The relocation at 18:080B proves that segment identity;
unrelocated far-call words are not loaded addresses. Segment1:39AB--39CA
implements the rounded multiply/divide operation. Track limits also include
arrow size and the separate adjustment fields; replacing the formula with a
generic guessed ratio would omit those boundaries.

## Current divergence

controls.c intentionally obtains native GetScrollBarInfo geometry, including
the host page-proportional thumb, and paints it using original arrow artwork.
This preserves native input hit geometry but is not complete Win3.1 geometry
fidelity. The long thumbs visible in sample01/16/25 are therefore not certified
by the exact twelve-arrow resource comparison.

OpenNT NT4 kernel/sbctl.c has a distinct `if (pw->page)` branch that derives
thumb length from page/range. It is useful comparative evidence, not authority
to substitute that later behavior for the owner-requested Win3.1 behavior.

## Required next implementation boundary

Do not simply draw a shorter thumb while retaining different native hit areas.
Resolve original metric values, arrow/track limits, rounding and edge cases,
then use one geometry contract for painting and input. Preserve public native
application state, notifications and cancellation; distinguish modern page-size
state from the original range/position contract explicitly.

In particular, test thumb presses, drag outside/back, release/cancel, line/page
repeat and disabled directions, plus the application's observed position and
track-position queries. GetScrollInfo and native tracking cannot be assumed
to see project-only painted geometry. Any solution requiring broader API
interception must be reviewed against the standalone S4 integration boundary,
not silently added as a drawing fix. Meanwhile retain the working native
behavior rather than introducing mismatched visible/hit geometry.

This finding is an open implementation gap, not an owner-approved exception or
S4 closure. Other standard-control variants and real menu/drag verification
also remain open in the working plan.

## Native tracking bypasses ordinary redraw suppression

r118/r119 extend the live WindowDC observer into the native scrollbar modal
loop. Only a newly created fixture list is affected: save/restore that thread's
keyboard-state bytes, send its nonclient press, sample with its own timer, then
cancel/release its capture. r119 samples twice and queues one thumb movement.
No input-desktop switch, global cursor movement or physical-input claim is involved.
The timer observes GetCapture()==the fixture, proving actual loop entry rather
than merely an after-return repaint. The outer build/fixture deadline remains.

r118 fails its former combined-mode palette assertion inside WM_VSCROLL:
627 foreign pixels already exist when the native press issues its nested
scroll notification. The test-only bracket cannot be called a complete fix.
r119 preserves wheel/motion assertions but explicitly logs tracking as an open
defect; nested notifications do not prematurely restore the outer test bracket.
The default arrow mode samples1586/1847 mismatching pixels (foreign0/661);
default thumb samples676/3640 (foreign650/3500). With per-window theme disabled
and redraw suppressed for the tracking call, arrow samples still mismatch1586
pixels, and the moved thumb samples1422 mismatches, including1353 foreign pixels.
Zero foreign pixels alone is not full original-fidelity evidence.

Read-only OpenNT comparison:
`O:/repos.external/OpenNT/windows/core/ntuser/kernel/sbctl.c`, SHA256
`E60DD4AD4F71631CC312107BD4A3AC13FDD2CE577608E7F555AD68E2F78F44DE`.
xxxTrackBox calls DrawInvertScrollArea on hit-state transitions (around1642),
xxxTrackThumb calls xxxMoveThumb (around1730), and xxxEndScroll directly redraws
the thumb (around1491) and sends SB_THUMBPOSITION followed by SB_ENDSCROLL.
xxxSBTrackLoop (around1745) captures and dispatches its modal input independently
of ordinary WM_PAINT. This is structural corroboration, not a claim that modern
USER is byte-identical or that NT4's proportional thumb is Win3.1 geometry.
No source/runtime import is made across the USER server stopping boundary.

Implementation consequence: a wheel-only redraw/theme bracket is insufficient.
Proceed toward a shared original-profile scrollbar presentation and tracking
owner, with common geometry for paint/hit/drag and explicit native application
notification/state adapters. Do not add a timer that continuously overpaints the
native tracker, globally disable themes, or silently use mismatched hit areas.
Theme-association restoration, cancellation, nested notification destruction,
32-bit native track-position queries and original fixed-thumb limits remain
design obligations. No new owner-facing repaired demo is published here.

r118 is a failed combined-mode candidate (x86 assertion, runner PID48880,
start2026-10-12 16:33:55 local, exit1). r119 preserves this as an explicit
diagnostic instead of claiming it fixed: x86 retained suite passes27,261/GDI56,
but x64 later fails sample01's existing1-second menu capture deadline; overall
exit1 (PID45676, start16:35:17). No full-pass claim is made for r119.

r120 separates --scroll-test into its own process/private desktop and30-second
deadline; the full menu/control fixture remains separate with its original
60-second deadline and1-second menu capture gate. Every width executes both.
This prevents the native tracking diagnostic's temporary queue/capture/timer
state from being shared with menu acceptance; it does not waive menu coverage
or establish that probe interference caused the preceding timeout.
r120 completes both widths: separate scrollbar diagnostics reproduce the same
native-tracking leak counts above; the retained full fixture passes27,261 checks
per width with GDI56/56. Runner PID5316, start2026-10-12 16:38:05 local, exit0;
documentation governance and diff checks pass. This validates the isolated
test organization, not the known-defective scrollbar rendering. Runtime
controls.c is still unchanged from r100; no repaired demo is delivered.

## Shared geometry implementation foundation

scroll_layout.c/.h independently express the pinned USER geometry and hit
ordering; no original executable code is copied or linked. The module is built
into the standalone library but deliberately not connected to native tracking
or the existing painter until those consumers can use the same geometry.
Its inputs are caller-relative rectangles, explicit arrow/thumb/border metrics
and effective minimum/maximum/position. Modern page size belongs to the native
state adapter, never to fixed-thumb sizing. Native-width arithmetic is an
explicit extension: 64-bit intermediates, checked coordinate extents and clamped
positions avoid reproducing 16-bit overflow. Inverse pixel mapping is bounded
and rounded; the complete original drag/cancel state machine is not implemented.

Pinned USER18:07AA..07F4 gives arrow=min(length/2-border,arrowMetric),
minimumPixel=start+arrow-border and
travel=end-arrow-minimumPixel+border-thumbMetric. USER18:07F8..0819 adds rounded
position-in-range times travel to that minimum and preserves the fixed length.
USER3:1794..17BB initializes thumb/arrow metrics and axis-specific border
overlaps. The existing VGA OEMBIN resource at37024 has border words1/1 at
offsets14/16, consumed at USER3:0EA6/0EB0 into06A0/06B4. Arrow bitmap dimensions
are retained in the existing pinned resource audit. USER18:03DE..03EE skips
thumb painting when space between arrows is smaller than its fixed size.

USER18:170B..1792 checks first-arrow, last-arrow, page-before, page-after,
then thumb, in that order. Thumb tracking requires space strictly greater than
the fixed thumb, whereas painting admits equality. The new hit function
preserves arrow precedence over shared thumb-border pixels and separate
disabled-arrow bits. It does not start capture, timers or send notifications.

win16_style_scroll_geometry.c uses explicit original-profile vectors (17/17/1),
horizontal/vertical transpose, small extents, monotonic forward/inverse mapping,
disabled hits, native LONG endpoints and invalid ranges. These tests prove the
implemented geometry contract, not a guest execution comparison or finished
scrollbar UI. r121's initial geometry passes710,818 checks on x86; its subsequent
isolated full-menu suite hits the existing sample01 menu capture deadline,
so the run fails and never reaches x64 (PID41940, start2026-10-12 16:42:51,
exit1). r122 adds the source-backed hit-boundary checks and recompiles both
widths; retain its terminal result separately rather than treating r121 as a
full pass. No guest/mirror or production package changes occur.
r122 completes both x86/x64:791,821 geometry assertions per width, the
separate scroll diagnostic,27,261 retained full-fixture checks and GDI56/56.
Runner PID21896, start2026-10-12 16:44:57 local, exit0. The build records the
new geometry fixture in its input manifest. Passing arithmetic/hit tests do
not certify the unimplemented tracking state machine or remove the native
scrollbar leak recorded by the diagnostic.

## Shared tracking model foundation

scroll_track.c/.h adds a per-instance, callback-free interaction model sharing
scroll_layout. Its output is bounded notices plus paint/timer actions; HWND
capture, message dispatch and attachment/gesture revalidation remain adapter
responsibilities. This separation prevents a pure calculation from holding a
dangling HWND-owned state across an application callback. It does not yet
implement the adapter or prove callback destruction/removal safety in a live
window. The model remains unconnected to current control rendering/tracking.

Original USER18 observations:1447..14DC tracks arrow/page inside/outside state;
1492 sets initial delay200ms;110C..1176 changes repeats to50ms. A stable mouse
move must not rearm that timer and starve repeats.14E5..1597 preserves the grab
offset, clamps inside the drag rectangle and returns to initial pixel outside;
moving back resumes the original grab relation.17DD..1823 initializes position
and sends SB_THUMBTRACK, including at initial thumb press. The calculation
block fields are position at+0, minimum at+2, maximum at+4 (the Pascal MulDiv
argument order matters);1074..10AE maps clamped pixels back to positions.

1369..1444 releases capture and ends thumb tracking with SB_THUMBPOSITION then
SB_ENDSCROLL. Its cancellation branch calls11A4 with the initial pixel and a
visual-only flag; that branch bypasses the position/notification update at11EC,
so04FE remains the last notified position. The model preserves that observed
last-position final notice rather than inventing an extra SB_THUMBTRACK reset
to the start. This is static source interpretation, not guest runtime
certification; final live integration still needs callback and cancel probes.

The focused fixture adds26 assertions for begin/duplicate begin, grab offset,
unchanged motion, outside/back, cancel ordering, idempotent end,200/50ms timing
actions, no motion-induced timer starvation, disabled arrows and page repetition
stopping when updated geometry no longer contains the pointer in the same part.
Geometry validation additionally rejects an unrepresentable future maximum
thumb coordinate even when the initial position would have fit. All arithmetic
stays native-width checked; original binaries and mirrors remain unchanged.
r123 passes both widths:791,847 geometry/model assertions each, separate
known-defect scrollbar diagnostics,27,261 retained full-fixture checks and
GDI56/56. Runner PID2128, start2026-10-12 16:50:32 local, exit0; governance and
diff checks pass. No connected runtime scrollbar repair or publication is
claimed by this model-only implementation step.

## Public query boundary before live adapter integration

The next adapter cannot assume that subclassing SBM_GETSCROLLBARINFO changes
GetScrollBarInfo for a built-in SCROLLBAR. The official
[GetScrollBarInfo contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getscrollbarinfo)
only promises message dispatch for a non-system control with OBJID_CLIENT.
By contrast, the
[SBM_GETSCROLLINFO contract](https://learn.microsoft.com/en-us/windows/win32/controls/sbm-getscrollinfo)
supports querying the full-width tracking position through the window procedure.

The new read-only win16_style_scroll_api.c probe installs identical handlers
on a built-in SCROLLBAR and a custom class, on its own private desktop. Direct
messages are positive controls proving the handler is installed. Public API
calls record dispatch counts and returned geometry/position independently.
It neither changes product windows nor theme settings. The verification driver
builds and runs it separately in both widths with an owned 15-second deadline.

This prevents a false fix in which the painted fixed Win3.1 thumb and custom
hit testing disagree with the system's public scrollbar geometry. The runtime
adapter is not connected merely to make a screen capture pass; choosing how
to preserve this public contract remains part of the live integration work.
The list/ComboLBox wheel and tracking defects remain open.

r124 completes with exit0 on both widths. Native GetScrollInfo dispatches one
message and returns the sentinel track position123456. Native GetScrollBarInfo
dispatches zero messages and returns thumb17..25, ignoring the handler's50..67.
Explicit messages reach both handlers; the custom-class public queries each
dispatch once and return the sentinel values. Thus this is a demonstrated
system-class API bypass, not an uninstalled subclass or a geometry arithmetic
failure. The retained full fixture passes27,261 checks per width with GDI56/56;
the geometry/model fixture passes791,847 per width. Known-leak diagnostics
remain negative evidence, not repair acceptance. No runtime drawing change or
new owner-test delivery is included in r124.

## Creation-time list scrollbar ownership

r125 rules out hiding an already-created native scrollbar as an ownership
transfer. In LISTBOX and all three ComboBox forms, ShowScrollBar(FALSE) removes
WS_VSCROLL and expands the client width17 pixels. A wheel message then leaves
top index0 unchanged. LB_SETTOPINDEX(10) restores WS_VSCROLL, shrinks the client
again and resurrects the original scrollbar. Selection/add/reopen preserve it.
Both widths pass the existing full gates, but this candidate is rejected.

Read-only OpenNT client evidence explains the hidden distinction. lboxrare.c
sets plb->fVertBar from the creation style. lboxctl1.c xxxSetLBScrollParms uses
that retained flag, not just the current window style, to choose between local
position clamping and NtUserSetScrollInfo(...,plb->fRedraw). lb1.c's wheel path
instead tests the current WS_VSCROLL flag. This historical explanation is not
assumed to prove modern-host behaviour; the new native probe tests both paths.
SHA256 identities under O:/repos.external/OpenNT/windows/core/ntuser/client:

- lboxrare.c: E8914F4761ADE70A1A7357B5DC8C17744CE1BC49379D03A99470F815EF85F64D
- lboxctl1.c: 47681BF76054CC55DE0533D29A012D344D96EFCE7AAA60199857A166597B4B73
- lb1.c: 73DD9DFFBF8C7184CC51771E34A4ACE046EC476DE4A5B9DB3EE50FCD08C7C1A7

The r126 candidate creates each list/combo without WS_VSCROLL, then supplies
scroll state through SetScrollInfo(FALSE). Initial observations: wheel advances
top0 to3 in all four cases; setting top and selecting rows still work; native
scroll information stays at the supplied0/39/8 across those operations and
adding an item. Therefore native data/selection need not imply native scrollbar
state ownership. This is not yet pixel suppression evidence or a repair: an
adapter must update range/page/position and own bar interaction, creation-time
handling must include ComboLBox, and retrofitting already-created controls is
not equivalent. No host private layout is accessed or changed by the probe.

r126 completes both-width gates with exit0. r127 adds diagnostic mode4 to the
existing live-pixel boundary fixture: no creation-time WS_VSCROLL, followed by
externally supplied scroll state; no theme override or WM_SETREDRAW bracket.
The observation is still inside the native-message return boundary, before
the outer style repaints. This deliberately leaves bar state fixed so a change
in surface pixels cannot be concealed by updating the expected thumb location.
It is an ownership isolation experiment, not a usable fixed-position bar.

The x86 candidate shows zero mismatched/foreign scrollbar pixels for three
wheels in a normal list and three wheels plus four row motions in each combo
form; top advances0/3/6/9. Surface fingerprints remain unchanged. Necessary
follow-through before adopting this route: externally update native public
range/page/position without direct drawing, use shared tracking for bar input,
cover horizontal/resize/reopen/callback paths, and resolve removal semantics.
In particular, clearing a creation-time native ownership flag is not reversed
by merely restoring WS_VSCROLL on removal. Existing late-attached controls
cannot be claimed fixed by this creation-only diagnostic. Original binaries,
mirrors and runtime control implementation remain unchanged.

r127 subsequently completes x64 with the same mode4 zero-pixel results and
both retained full gates (27,261 checks per width; GDI56/56). Runner PID32360,
start2026-10-12 17:05:19 local, exit0. Geometry/model and API probes pass in both
widths; documentation governance and diff checks pass. This strengthens the
candidate boundary evidence only; bar tracking and a deliverable fix remain
open, with no new user-test demo or production publication.

## External state synchronization is not yet paint suppression

r128 extends mode4 from a frozen bar to actual range/page/position updates.
It fails x86 at the first SetScrollInfo(FALSE) surface-fingerprint assertion
(runner PID44748, start2026-10-12 17:08:23, exit1). Thus the r127 ownership
isolation result cannot be promoted to a complete repair: making the thumb
follow the list reintroduces an intermediate paint despite the no-redraw call.

r129 separates SIF_POS from RANGE/PAGE/POS. On the normal list, the first
position-only update0->3 changes the surface and exposes3500 foreign pixels
(3640 mismatches). Adding range/page in the next call does not change it again.
Later position changes3->6->9 leave the old surface unchanged until our explicit
paint, so a warmed-up test would miss the first-update failure. Readback matches
the actual top index; explicit retro painting matches expected output, neither
of which proves intermediate suppression. These measurements distinguish the
first position update from a page-size change; they do not yet prove which
internal host mechanism causes the paint.

r129 additionally exposes a fixture defect: ComboBox installation sends list
selection messages while its popup is hidden. The expanded observer attempted
a hidden WindowDC capture and failed BitBlt with error6. The probe now observes
only visible surfaces, consistent with the existing popup capture rule; hidden
message delivery is not counted as a passing pixel test. r129 is failed, not a
both-width pass (runner PID28856, start17:09:51, exit1). Known first-update paint
results remain explicit diagnostics rather than assertions claiming a fix.

r130 completes both widths with exit0 after the hidden-surface fixture repair.
The first-update leak remains in the recorded diagnostic; successful position
readback and the27,261 retained full checks per width do not override it.
Both widths complete all four externally managed list cases, three combo row
motion cases and the independent geometry/API gates. GDI remains56/56.
Documentation governance and diff checks pass. This turn changes probes and
evidence only; it does not deliver or claim a repaired standalone scrollbar.

## Connected standalone SCROLLBAR interaction adapter

controls.c now connects scroll_layout/scroll_track for the SCROLLBAR control
class only. It owns button/drag/capture/repeat actions and keyboard notices,
keeps native range/page/position storage, and exposes full-width tracking
position through SBM_GETSCROLLINFO. Each synchronous parent notification is
followed by attachment and gesture-epoch revalidation. Removal stops the owned
timer and releases its capture; native modal mouse tracking is not entered.
No list/ComboLBox or frame scrollbar is silently claimed to use this adapter.

The painter and hit test share fixed-thumb geometry. Disabled arrow state
still comes from native public state. DPI metrics scale17 directly, not a
rounded one-pixel unit multiplied17. USER18:17AE..17BD supplies the original
4*CXBORDER/CYBORDER drag-rectangle expansion; the later NT two-thumb expansion
is not used. Programmatic position/range setters retain their redraw request
while suppressing native direct painting where their documented flag permits.

Live HWND tests exercise both axes: arrow capture/release, initial thumb
notification, movement from position70001, full32bit position readback,
THUMBPOSITION then ENDSCROLL, and style removal during the parent callback.
The disabled-arrow oracle now uses the component's declared fixed metrics,
not the modern system geometry; the original bitmap cell comparisons remain.
The full-width notification checks are behavioural, not screenshot evidence.

r131 compiled but failed at the previously observed outer-menu sample1 capture
deadline, before scrollbar acceptance. r132's tool handle disappeared and its
on-disk build stopped after x86 linking without a terminal test marker; a
privileged process query confirmed no r132 runner before r133 was started.
Neither incomplete run is counted as passing.

Open limitations: built-in GetScrollBarInfo still exposes native geometry;
held-repeat timing, callback destruction/new gesture, page pressed rendering,
focus rendering and every intermediate native setter pixel need additional
tests. The list/ComboLBox first-position paint failure remains unchanged. This
is implementation progress, not S4 completion or a published owner build.

r133 completes both widths with exit0: live horizontal/vertical input and
callback-removal cases pass, original disabled-arrow cell checks pass, and
27,261 retained checks per width end with GDI56/56. Geometry/model and API
probes also complete. This does not turn known-leak diagnostic output into
passing visual acceptance. README/API comments were subsequently corrected
to describe the now-connected adapter and its explicit remaining limits.

## Isolating theme-dependent first-update paint

r134 adds mode6 alongside retained mode4: both create the native list without
creation-time scrollbar ownership and subsequently maintain public scroll
state, but mode6 disables the theme only on that newly owned probe HWND with
SetWindowTheme(empty,empty). No redraw bracket or warm-up scroll is used.
The candidate asserts unchanged existing-surface fingerprints separately after
SIF_POS and after RANGE/PAGE/POS updates, before explicit retro painting.
Only mode6 asserts this: mode4's first-update leak remains a negative control.

The x86 probe completes normal-list first wheel, subsequent wheels, set-top,
selection, insertion/deletion, and three ComboBox forms' wheel/row motion with
those suppression assertions passing. Native top and public scroll position
are synchronized rather than frozen as in r127. This isolates theme-dependent
painting in this experiment; it does not by itself identify an internal Windows
function, certify drag tracking, retrofit existing windows, or preserve an
unknown pre-existing SetWindowTheme association on removal. Production
controls.c does not acquire a theme override from this diagnostic change.

r134 completes x64 with the same mode6 assertions, and both full suites pass
27,261 retained checks per width with GDI56/56. Runner PID18192,
start2026-10-12 17:30:32 local, exit0. Documentation governance and diff checks
pass. Candidate pixel results are not a claim of the full-goal implementation:
native list tracking, safe theme ownership/removal and normal application
creation integration remain required before an owner-test delivery.

## Opt-in native-list interaction adapter

The next implementation step exposes win16_style_list_scrollbar_enable for
already styled, single-column vertical LISTBOX/ComboLBox windows. The creator
must own the per-window unthemed setting; the component does not override or
claim to restore an unknown theme association. Ordinary install remains
unchanged. Horizontal/multicolumn opt-in is explicitly rejected, not silently
treated as implemented. The integrated fixture owns fresh native controls and
restores their default theme after removing the component.

Unlike the mode4/6 experiment, these controls retain creation-time native list
scrollbar ownership. Forwarded list mutations temporarily suspend native redraw
with nesting state; the list remains responsible for range, page, position,
selection and data. Shared fixed-thumb geometry/tracking handles vertical
nonclient input. Thumb requests use full-width LB_SETTOPINDEX; line/page/end
requests go through native WM_VSCROLL. The renderer and input use the same
geometry. Existing raw native diagnostics remain separate negative controls.

r135 compiles and passes the integrated ordinary-list wheel/position/drag and
removal case, then fails capture acquisition in the first popup case. r136
records that the dropdown already owns capture before a bar gesture. Calling
SetCapture on that same HWND interrupts tracking, so the adapter now avoids
the redundant acquisition and retains/restores the prior popup capture rather
than unconditionally releasing it. x86 passes all four forms with the popup
visible throughout and matching native/public top positions; removal restores
ordinary native scrolling. This is not yet intermediate-pixel acceptance of
the connected adapter or completion of the unsupported variants.

r136 completes both widths and all four list forms with exit0 (runner PID19572,
start17:36:38 local). The retained full suite passes27,261 per width and GDI56/56;
documentation governance and diff checks pass. The visible-return comparison
proves the new fixed thumb and content state agree after actions, not that no
native pixels were exposed inside the forwarded mutation.

r137 therefore places a separate observer below the component subclass. It
compares existing scrollbar surface fingerprints immediately before/after the
native mutation returns, before component redraw resumes. It covers the first
wheel, later wheels, thumb top-index requests and repeated row mouse motion.
The fixture requires actual observations, leaves the negative native probes
intact, and separately compares final pixels against the component renderer.
This is a private-desktop probe, not physical held-mouse owner acceptance.

r137 passes both widths,23 intermediate native boundaries for each of four
list forms, plus correct post-action pixels, native top-index/public position,
capture restoration and removal. The observed existing-surface fingerprints
are unchanged during those forwarded native mutations. Runner PID22672,
start17:39:24 local, exit0; full fixtures pass27,261 per width with GDI56/56.
No global theme change or guest/mirror edit is involved. Ordinary visible demo
pages have not yet opted in; whole-control inventory, horizontal/multicolumn
bars, accessibility geometry and physical acceptance remain open.

## Ordinary demo-page integration

r138 opts fresh demo-owned list windows into the verified adapter in sample01
and the LISTS/COMBOS pages. The creator explicitly sets each applicable native
LISTBOX or ComboBox's internal list to an unthemed association, then enables
the component. Horizontal/multicolumn lists remain untouched and visibly
outside this vertical repair. The same preparation runs in interactive and
test modes; no test-only fallback hides the changed implementation.

The existing combined-page OEM arrow oracle now uses17 scaled by DPI for its
arrow length, matching the admitted original-profile metric rather than the
modern system's reported proportional-bar geometry. Bitmap cell values still
come from the pinned original resources. Existing hover, repeated-motion,
wheel, top/selection, disabled/enabled, theme-notification and reopen cases
run against the connected page, alongside the separate native negative controls
and four-form intermediate-pixel fixtures. Per-window theme settings belong
to these freshly created demo HWNDs and disappear with them; no system setting
or unrelated application window is modified.

r138 completes both architectures with exit0 (runner PID12268, start17:42:58
local). Connected showcase hover/repeated-motion/state-transition tests and
the four-form23-boundary fixtures pass, as do27,261 retained full checks per
width; GDI56/56. The generated all-controls capture was visually inspected.
Runnable x64 candidate: build/M0-T445/S4/r138/x64/win16-style-demo.exe,
SHA256 CC8048BDB9B1AC6539FEA984277AFB81CA459B4EB009457BE7F1C00E08250D09.
This is a standalone intermediate candidate, not production publication or S4
closure. Horizontal/multicolumn/edit/frame scrollbar migration, accessibility
geometry and the remaining fidelity/lifetime inventory remain open.

### r139: explicit disabled-arrow input state

Review found that the standalone SCROLLBAR adapter queried native per-arrow
availability, while the opted-in LISTBOX/ComboLBox adapter passed zero disabled
flags to the same tracking model. This allowed an explicitly disabled arrow to
initiate a scroll. The existing query now takes the relevant accessibility
object (OBJID_CLIENT or OBJID_VSCROLL); both adapters use it at gesture start
and during motion/repeat. No mirror, guest or product component changes.

The four-form integrated fixture explicitly disables the down arrow, confirms
STATE_SYSTEM_UNAVAILABLE, sends the nonclient click and checks unchanged top
index and capture. It then re-enables the arrow and proves scrolling plus
capture restoration. This is a message-driven isolated-desktop test, not
physical pointer acceptance. Each form now observes 25 unchanged intermediate
native-paint boundaries instead of 23.

Command: verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r139, launched as
the owned hidden runner PID33120 at 2026-10-12 17:49:30 local; exit0. Both x86
and x64 pass the new four-form cases and the retained 27,261 full checks;
GDI baseline/final remains56/56. Exact logs are r139/<arch>/scroll-test.txt and
test.txt. The runnable x64 win16-style-demo.exe SHA-256 is
3CD19A9FEDE3B646E782B0459C59B5ED5C2267923B673A556AA8CD54BB8259B3.
This closes the per-arrow-disabled input omission only. Horizontal/multicolumn,
edit/frame bars, live repeat/cancellation and the other S4 gaps remain open.

### r140: capture ownership during cancellation and detach

Review found an inconsistent teardown: normal gesture completion preserved a
ComboLBox's pre-existing capture, but control removal unconditionally released
capture when tracking was active. The adapter now shares one capture-release
helper between completion and removal. It clears the saved capture identity,
restores a valid same-thread prior owner only while still holding capture, and
does not release capture borrowed from the same list. Removal marks tracking
inactive and stops its timer before any capture callback; it does not send
scroll notifications through a detached attachment.

The existing four-form fixture now starts an arrow gesture and tests
WM_CANCELMODE, a real SetCapture(parent) transfer, and component removal during
another gesture. Assertions check capture restoration, no stealing from the
new owner, and retention of the popup's borrowed capture. Native data remains
usable after removal. These synchronous tests do not prove physical held-repeat
or every asynchronous application callback.

Command: verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r140. Owned hidden
runner PID17400 started2026-10-12 17:53:14 local, exit0. Both widths pass all
four lifecycle forms,25 intermediate-paint boundaries per form and27,261
retained full checks; GDI56/56. The x64 demo SHA-256 is
14E84A77DF90D7A8F4AFCA027536A60FEA4785779BF6F08D61667603A565D924.
No product integration, mirror changes or new theme policy. S4 remains open
for the other scrollbar families and the remaining fidelity/behavior inventory.

### r141/r142: horizontal list position contract

Before extending the adapter, inspect the actual content position contract.
Read-only OpenNT client/lboxctl1.c xxxSetLBScrollParms reports pixels for a
single-column horizontal list and columns for LBS_MULTICOLUMN. client/lb1.c
dispatches WM_HSCROLL with HIWORD(wParam). client/lboxctl2.c
xxxLBoxCtlHScroll retains xOrigin; client/lboxmult.c
xxxLBoxCtlHScrollMultiColumn multiplies the supplied column by itemsPerColumn.
The latter two input SHA-256 identities are respectively
A99C9EF3B6A7A07B219DDDA18FF2FB6A8A11C42F5378E5F77E10E336AD7D1744 and
FB3569B4DDCD34E17A3426B97F3E23C6C9371FF8F2E874A61FBD7D16B013F363,
under O:/repos.external/OpenNT/windows/core/ntuser/client. These are historical
comparison evidence, not imported code or proof of every Win3.1 behavior.

horizontal_list_probe in tests/app/win16_style_scroll_api.c independently
creates native lists on its private desktop. Both current host widths report:

- Single-column requested position2: scrollbar position2, top item0.
- Multi-column position2: top item14 with7 items per column.
- Packing70000 into the message's16-bit field produces4464.
- SetScrollInfo(SIF_POS,70000) changes the exposed scrollbar position; the
  next native line-right instead reports4472, demonstrating that the native
  list's content origin remained4464. LB_GETITEMRECT stayed at left0 before
  and after; that rectangle alone is not proof of content movement.

Consequently, never use a scrollbar-only position assignment as a content
scrolling implementation. Vertical LB_SETTOPINDEX cannot be blindly reused
for single-column horizontal motion. Multi-column mapping and horizontal
message width must be handled explicitly; wide-position support is not proved.

r141 runner PID17320 (2026-10-12 17:57:23 local) exited1 at compilation:
MSVC /WX rejected the probe's constant-to-WORD truncation. Replace it with
an explicit16-bit mask; no implementation changed. r142 runner PID47164
(17:57:55 local) exits0 with both API probes and both retained27,261-check
full gates passing, GDI56/56. Logs: r142/<arch>/scroll-api.txt and test.txt.
This is boundary evidence only, not delivery of horizontal scrollbar repair.

### r143/r144: multicolumn horizontal adapter connected

The existing list opt-in now also accepts LBS_MULTICOLUMN. Its layout reads
SB_HORZ/OBJID_HSCROLL, uses the original fixed-thumb model and HTHSCROLL input,
and forwards line/page/end through native WM_HSCROLL. Thumb positions remain
full-width column indices and are converted to LB_SETTOPINDEX using current
client height / item height (at least one row); the product is checked in
LONGLONG before conversion. This reuses the same tracker, disabled-state,
capture and repaint-suppression mechanisms, not a second scrolling controller.
Single-column horizontal lists remain explicitly excluded, not silently
truncated to16 bits or assigned a scrollbar-only position.

The owned demo enables this path in the combined showcase and MULTILIST page.
test_multicolumn_scroll creates200 native items, proves line-right advances
one column, drags the fixed thumb and checks first-item == column * rows,
checks capture release, wheel/row-motion pixels and native operation after
removal. The existing below-style observer now handles WM_HSCROLL and samples
the horizontal rectangle for multicolumn windows:14 intermediate native
boundaries stay unchanged in each width. Existing four vertical-list forms
retain their25-boundary and lifecycle assertions. Variable-height, resize/font
transitions, accessibility geometry and physical held-repeat remain open.

r143 runner PID44044 started2026-10-12 18:03:13 local, exit1: /WX rejected
the new fixture's local name samples hiding the gallery's global array.
Rename to boundary_count; no implementation change. r144 runner PID20324
started18:03:43 local, exit0; both architectures pass the new case, existing
API/model cases and27,261 full checks with GDI56/56. The x86
capture.bmp-all-controls-popup.png was visually inspected: the showcase's
multicolumn horizontal bar has the new fixed thumb; edit/frame bars still
have their known older geometry. The x64 demo SHA-256 is
33C9164B16BA12C280DFCA2846BD4729FD7C8F82906698F174D77DD8B24C1154.
S4 remains active, with no product integration or mirror modification.

### r145/r146: EDIT scroll units and focused API selection

Read-only historical MLScroll (OpenNT windows/core/ntuser/client/editml.c,
SHA2565F65E9AFF922918240243C3D91AEEC4C0317010C80A01AF43626F828B0F95DEC)
uses line indices vertically and pixel offsets horizontally. EM_LINESCROLL
accepts character deltas horizontally and line deltas vertically; horizontal
character deltas multiply the EDIT's own average character width. Thumb
messages supply absolute positions; the old implementation has a separate
large-line-count SIF_TRACKPOS path, so this probe does not establish wide
thumb behavior.

The new edit_scroll_probe builds128 long lines, selects characters4..9, then
checks both axes, first visible line, selection and unchanged text length.
r145 PID10388 at2026-10-12 18:08:09 local exits1 on x86: the initial test
incorrectly expected2 * tmAveCharWidth =14 pixels, while native EDIT moved16.
This was a rejected probe assumption, not a component regression.

Source follow-up: ECSetFont in client/edecrare.c
(B50E0F8F305F6456B3A952DDD63C815357136F222328C6D83DD0E76E697B5634)
uses GdiGetCharDimensions, whose implementation in ntgdi/client/cfont.c
(577A0E4CAF54E10F9BB7FCC30328BA4293D47EC73288D18CB481329F0789E625)
derives variable-pitch width from the52-letter alphabet extent. The public-API
probe now checks that measurement instead of substituting TEXTMETRIC's value.
Both widths observe metricAverage7, editWidth8, horizontal16/vertical3;
absolute thumb31px/17lines preserves selection4..9 and text length20736.

verify-win16-style.ps1 gains explicit -ScrollApiOnly: builds/runs only the
independent scroll-api.exe for each width, records selection.json and input
hashes, and retains its15-second process deadline. Default still selects the
full suite; this narrower run is not labelled full acceptance. r146 PID36532
at18:09:17 local exits0 using that flag. For the omitted implementation and
gallery gates,28 component/header/resource and demo/gallery input hashes
match r144 exactly (zero mismatches); reuse its full passing evidence. No
new demo/component behavior is delivered in this iteration. EDIT skin/input
integration and the other S4 requirements remain open.

### r147: provisional vertical EDIT adapter

The shared embedded layout/input functions now serve opted-in multiline EDIT
vertical bars as well as list bars. A separate explicit enable API preserves
the prior late-attachment behavior. Thumb requests use EM_GETFIRSTVISIBLELINE
and an EM_LINESCROLL delta; native EDIT retains the buffer, selection, undo,
caret and ordinary keyboard handling. Native mutations are redraw-suppressed
with the existing nesting/lifetime mechanism, followed by owned bar repaint.
No repaint is forced for ordinary uncaptured client mouse motion. The creator
still owns per-window theme selection; only the demo's fresh windows opt in.

test_edit_scroll builds100 lines, exercises wheel and fixed-thumb drag, checks
scrollbar position == EM_GETFIRSTVISIBLELINE, unchanged selection4..9 and text
length, capture release and undo after removing the attachment. Five native
message boundaries leave the visible vertical bar unchanged in each width.
Owned hidden runner PID7176, start2026-10-12 18:12:51 local, exits0 with both
widths passing the new case and27,261 retained full checks, GDI56/56.
Command: verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r147 (full default,
therefore also verifies that the new focused-selection option leaves default
execution intact). x64 demo hash:
E42719666F741F59DC38B12F4CA144A3463C462B648DA7D93542AEF866F8705E.

Review identifies a semantic gate not proved by these tests: historical
MLScroll skips EN_VSCROLL for SB_THUMBTRACK but not necessarily EM_LINESCROLL.
This adapter currently uses the latter, so do not claim notification parity
or complete EDIT acceptance. Next compare native versus attached parent
notifications, including callback reentrancy, before choosing the final thumb
dispatch. Horizontal EDIT, large line counts and other S4 gaps stay open.
This is a provisional standalone implementation, not product publication.

### r148--r151: EDIT notification correction and diagnostic failures

The native parent probe now counts EN_VSCROLL/EN_HSCROLL. r148 PID11856,
start2026-10-12 18:16:43 local, exit0, confirms both widths: moving by
EM_LINESCROLL notifies once; SB_THUMBTRACK to a different line notifies zero;
SB_THUMBPOSITION to another different line notifies once. These are separate
changed-position requests, not a claim that release at the already-tracked
position emits a notification. The new probe asserts these counts.

r149 PID30600, start18:18:23, exit0, adds a70001-line native text fixture:
synthetic SB_THUMBTRACK42 leaves first visible line0, agreeing with MLScroll's
large-line-count branch that reads USER-owned nTrackPos. The adapter cannot set
that private value through SetScrollInfo. Consequently replace the provisional
EM_LINESCROLL dispatch with original native thumb commands for the supported
line range; do not silently truncate large positions. At65535 or more lines,
retain native geometry/tracking and record the native-skin leak as unresolved.
This is an explicit incomplete coverage boundary, not reduced S4 completion.

r150 PID27380 at18:20:00 exits1: the new eligibility query recursively
dispatched EM_GETLINECOUNT through itself before fixture output. Replaying its
exact immutable x86 binary (PID26476,18:21:19) returns-1073740771
(0xC000041D, unhandled callback exception). Source inspection identifies the
query recursion; limit eligibility queries to actual input/tracking messages,
letting informational messages reach native EDIT directly.

r151 PID30752 at18:21:54 exits1 because its expanded scroll matrix reached the
existing30-second case deadline near the end of multicolumn checks, before
EDIT. Same-binary owned diagnostic PID30584 at18:23:23 completes exit0 in
30.5991428 seconds under a60-second diagnostic deadline. It passes EDIT's
silent thumb tracking/same-position release, one-notification line movement,
content/selection/undo and6 unchanged native-paint boundary assertions.
Therefore extend only the whole scroll-matrix deadline to60 seconds; keep all
cases, negative diagnostics and assertions. This does not alter UI timing or
reclassify either failed run as passed. Full two-width r152 verification follows.

r152 full runner PID43544, start2026-10-12 18:24:20 local, exit0. Both widths
pass API/model tests, all vertical list and multicolumn fixtures, the EDIT
notification/content fixture and27,261 retained full checks with GDI56/56.
The x86 combined-gallery capture was inspected: EDIT's short vertical bar
now uses the fixed-arrow geometry; insufficient room for a full fixed thumb
does not invent a miniature modern thumb. Horizontal EDIT/frame bars remain
the previously declared incomplete surfaces. x64 demo SHA-256:
C00F4205C5E2BDD9E8A8758A365335D1EF4851932AC71F5D29A6306CAE55029C.
No global settings, product images, guest code or original mirrors changed.
This closes the ordinary EDIT thumb-notification mismatch only, not full S4.

### r153: EDIT horizontal adapter and axis isolation

Extend the same independently authored EDIT adapter to WM_HSCROLL using the
previously audited editml.c MLScroll pixel-position contract. No additional
source or guest resources are imported. Rename the trial API to
win16_style_edit_scrollbars_enable; creator-owned unthemed association remains
required. Store the active object's axis so horizontal dragging cannot change
vertical thumb pixels, pressed arrows or message routing. Native EDIT still owns
text, selection, undo, ranges and parent notification semantics. Positions not
representable by the native thumb-message contract remain an explicit native
fallback, not claimed repaired surfaces.

The retained test_edit_scroll fixture now uses100 long lines with both bars.
It asserts actual horizontal position changes, unchanged vertical first line,
both bars' existing-surface pixels, capture/release, silent thumb tracking and
same-position completion, and a one-EN_HSCROLL native line-move positive control.
The lower subclass checks11 intermediate native-paint boundaries, without
asking WM_PRINT to repair the observed surface. Text/selection and undo after
removal remain checked. Pixel helpers now accept an explicit axis rather than
inferring EDIT orientation from LISTBOX style bits.

Full hidden runner PID44172, start2026-10-12 18:33:13 local, exits0. Command:
tests/observation/verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r153.
Both x86/x64 pass791,847 model checks, native API probes, retained list/Combo
and multicolumn cases, new both-axis EDIT checks, and27,261 full gallery checks.
GDI attachment baseline/final remains56/56. The x86 combined-popup screenshot
was inspected: the EDIT horizontal bar now has the fixed-size retro thumb.
Outer frame bars visibly remain the older incomplete surface. This is isolated
probe evidence, not physical owner acceptance or full S4 completion. No product
images, global settings, mirrors or guests changed; no commit/push yet.

### r154: ordinary list horizontal content origin

Further source audit found a separate real painting defect before adapting the
ordinary horizontal tracker. Pinned OpenNT windows/core/ntuser/client/lboxctl2.c
(SHA-256 A99C9EF3B6A7A07B219DDDA18FF2FB6A8A11C42F5378E5F77E10E336AD7D1744)
LBGetItemRect leaves a single-column item's left edge at0 and adds xOrigin to
its right edge. Native drawing separately offsets its DC by xOrigin. Our
independent painter used the item rect without that translation. Fix only the
single-column row coordinates using client.right-row.right; multicolumn
geometry and native buffer/selection/input ownership are unchanged. Deriving
from the item rectangle also avoids mistaking a SetScrollInfo-only change for
an actual content-origin update. No original source code is imported.

test_list_horizontal_content creates a native list with long text and1200px
extent, captures its existing pixels, sends native SB_THUMBPOSITION24, and
compares3904 visible pixels against their pre-scroll positions24px to the right.
The comparison includes1008 ink pixels and asserts the top item stays0. It
does not merely check a scrollbar value or a blank rectangle.

Full hidden r154 runner PID3812, start2026-10-12 18:38:02 local, exit0, uses
tests/observation/verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r154.
Both widths pass the new pixel test, retained list/Combo/EDIT and model/API
checks,27,261 full gallery assertions, and GDI56/56. x64 demo SHA-256:
F2004C0C21BE3BFF9A65D890070A49F6036FB0867FE419D0F70DD325898BF886.

Negative control links the identical r154 x86 demo/gallery objects against
the immutable r153 win16-style.lib under build/M0-T445/S4/r154-negative.
Owned hidden link PID44472, start18:39:48, succeeds. Its isolated scroll probe
PID15212, start18:39:56, exits1 exactly at the new translated-pixel assertion
(gallery line893); prior multicolumn checks pass. This distinguishes the
implementation repair from a trivially passing test. Ordinary horizontal bar
tracking itself remains native/incomplete, as do frame bars and previously
recorded S4 boundaries. No product, mirror, guest or global settings changed.

### r155--r158: ordinary horizontal list tracker

Extend the existing list adapter to ordinary horizontal pixel positions through
native WM_HSCROLL thumb commands; vertical item indices and multicolumn column
conversion retain their prior paths. Share the EDIT horizontal16-bit range
check rather than truncate unsupported positions. Both drawing and input use
the same axis eligibility. Wider ordinary horizontal ranges remain a declared
native fallback, not complete retro acceptance. Sample01 now gives the right
ordinary list both scrollbars, in addition to its multicolumn example.

The horizontal-content fixture retains its translated-pixel comparison and
adds custom thumb capture/motion/release, actual content origin from item rect,
unchanged vertical top index, both bar surfaces and intermediate native-paint
checks. Its extra vertical bar reduces the pixel-comparison area to3488 pixels,
including893 ink pixels; the24px translation is unchanged.

Keep failed attempts distinct: r155 runner PID34444 start18:42:46 exits1
because the new observer was installed outside the style subclass and counted
our final repaint as native repaint. Install that observer before the style
attachment, consistent with the other retained boundary tests. r156 PID12312
start18:43:58 exits1 at the original-arrow pixel assertion for sample01's newly
shortened vertical bar. Same-binary diagnostic PID34596 start18:44:58 runs the
full gallery separately, exit0, with an inspected readable screenshot. r157
PID29368 start18:46:07 exits1 while recording the geometry:54px bar,26px metric,
63 mismatched bottom-arrow samples. Original USER18:07AA shortens its arrows
to min(26,54/2-2)=25px; production already does this, but the OEM probe assumed
the unshortened26px. Correct the probe's source-derived sample rectangle, keep
all578 samples and the assertion, and retain mismatch diagnostics.

Full hidden r158 runner PID36868 start2026-10-12 18:47:00 local exits0 using
tests/observation/verify-win16-style.ps1 -BuildRoot build/M0-T445/S4/r158.
Both widths pass6 horizontal-list native boundaries, content/axis/pixel checks,
the retained list/Combo/EDIT suites,791,847 model checks, native API probes,
27,261 gallery assertions and GDI56/56. The x64 demo hash is
3A5A5721956916150CB5FB6ED4A949C1B9070A85DA9058E9FA6FFD86E184BD89.

Visual limitation: both original r158 all-controls PrintWindow captures were
mostly black despite successful calls. Do not count them as visual passes.
Re-running that exact x64 binary in a fresh isolated --test process (PID4308,
start18:49:58, exit0) produced diagnostic.bmp-all-controls-popup.bmp; its PNG
was inspected and shows the new two-axis ordinary list with retro arrows and
fixed thumb. Preserve both observations: this supplies a readable candidate
view, but does not diagnose or repair intermittent PrintWindow output. Frame
bars, wide-range binding, accessibility and the other recorded S4 fidelity
and lifecycle gaps remain open. No product/mirror/guest changes or commit/push.

### r159: owner-reported blinking residue and unrelated frame repaint

Owner reports that a focused standalone scrollbar's blinking shadow stays at
the old position, and clicking its arrows also flashes the outer frame bars.
Suspend the planned frame-tracker extension to repair these concrete defects.
Source proof: pinned OpenNT windows/core/ntuser/kernel/sbctl.c SHA-256
E60DD4AD4F71631CC312107BD4A3AC13FDD2CE577608E7F555AD68E2F78F44DE creates a gray
caret for focused SCROLLBAR controls. Its SetInfo returns before SetSBCaretPos
when fRedraw is false. Our adapter deliberately passed false to suppress native
painting but left native caret ownership unchanged. The resulting caret used
native thumb geometry and did not follow the retro thumb. Separately,
style_proc repainted the parent frame on every WM_HSCROLL/WM_VSCROLL, including
child notifications whose lParam identifies the child HWND.

The standalone control now owns the ordinary system caret's size/position
using its fixed-thumb layout; native blink timing remains unchanged, with no
new timer or global setting. Focus/size and position updates synchronize it,
capture hides it, completion restores it, and lost focus/destroy/removal clean
it up. Removal on a still-focused control restores the native focus handler.
Native range/data ownership is unchanged. Parent frame repaint now occurs
only for its own scroll notification (lParam0), not for child control traffic.
This does not claim the outer frame tracker is implemented.

test_scroll_focus uses real focused HWNDs on both axes. It checks the system
caret rectangle against the fixed thumb, keyboard and explicit position
updates, hidden caret during capture, and lost-focus cleanup. A negative link
of these same r159 x86 demo/gallery objects against r158 win16-style.lib
(build/M0-T445/S4/r159-negative, hidden link PID7704 start18:57:09; probe
PID12924 start18:57:14) exits1 exactly at the caret-rectangle assertion.
The new implementation passes it in both architectures.

Full r159 hidden runner PID21184 start2026-10-12 18:55:11 local exits0 through
the normal verification script and build/M0-T445/S4/r159. Retained scroll/model,
API, list/Combo/EDIT and27,261 full gallery checks pass each width, GDI56/56.
x64 demo SHA-256 CCF9796E606CE88A6B71FB8084510A6B36806794B2537F77307C190A8F0884A1.
The caret probe is geometry/lifecycle evidence; human temporal smoothness
acceptance remains separate, and the earlier intermittent PrintWindow issue
is not claimed repaired. These are project-owned style changes only; original
mirrors, guest binaries and product components remain untouched. S4 stays open.

## Menu/control font consistency (owner screenshot, r162)

The owner reports sample01's File menu is smaller than its controls. Source
audit finds every menu-bar measurement, hit layout and menu item draw borrowing
the title's explicitly requested16-pixel System font. The title renders into
a base-resolution bitmap and scales it, but menus draw that font directly.
Controls instead use the realized stock SYSTEM_FONT. At this host's144 DPI,
the measured cells are16 and20 pixels respectively; this is a project font
ownership error, not a reason to change VGA.DRV or the installed guest.

Read-only original reference: the previously pinned USER.EXE segment3 at
069F..06A6 calls GDI ordinal87 with stock index13 and stores the result at
data0124. The retained relocation manifest in S3/r029 identifies the import;
the original mirrored wow16/gdi/gdi.def names ordinal87 GETSTOCKOBJECT.
The supplied VGASYS.FON hash remains
7BAEFA14EF8EF2355374D0B5061043DD5F4F8C33FF57680366E4A725F91646F8; its retained
resource audit identifies the16-pixel System face. This static initialization
and resource evidence is not a new trace of every Win3.1 menu caller, nor a
claim that modern stock System glyphs exactly equal the VGA font. No source
or font import is added. The owner-approved host-font exception remains.

The frame now distinguishes an owned base title_font from a borrowed
menu_font. All menu measurements, rendering and navigation (including MDI
handoffs and system popups) use the same stock System font as demo controls.
Title composition is unchanged; the borrowed font is never deleted. The
test computes the expected menu/client boundary and its last hit-test pixel
from a real control's WM_GETFONT, and reports the old/new measured cells.
The hover-routing test also stops assuming a16-pixel menu font.

r160 failed compilation on one missed MDI field rename; r161 then passed
the new font check but exposed the old hover probe's stale16-pixel hit layout.
Both are retained failed evidence, not passing deliveries. r162-negative
links r162 x86 test objects against r159's old library; hidden probe PID10056
at2026-10-12 19:07:41 exits1 exactly at the new menu/client boundary assertion.
This establishes that the test detects the reported regression.

The full-showcase capture is now taken after its existing move to0,0 on the
private desktop, rather than before. This is capture ordering only, not a
claim that all previously intermittent black PrintWindow captures are solved.

Full r162 hidden runner PID14936 start2026-10-12 19:06:44 exits0. Both widths
pass the font check (144 DPI, menu/control20, former-title16, bar24), the
retained27,261 gallery checks and791,847 geometry checks, plus scroll/API and
DC-failure gates. GDI attachment counts stay68/68 x86 and56/56 x64. The
sample01 capture is readable and visually reviewed. x64 demo SHA-256:
E5451EA9DEA33FB4419641380A47C066932D4419921A9DD726D5E3DED2253626.
This is a focused S4 delivery, not closure of the remaining scrollbar and
full-gallery acceptance gaps.

Capture limitation: the readable r162 sample01 image is x86. The x64 initial
capture and a same-binary diagnostic rerun (PID14380 start19:10:13, exit0,
27,261 checks and GDI56/56) still produce the earlier mostly-black PrintWindow
image. Moving before capture alone therefore does not solve that separate
probe problem. Do not present those x64 images as visual acceptance. Both
widths' font/geometry assertions pass; the x86 screenshot supplies the visual
comparison, with x64 physical presentation still subject to owner inspection.
Documentation governance and git diff --check pass after this focused repair.

## Owned capture and WM_PRINT contract audit (r163 onward)

The next goal turn keeps S4's full scope; it first restores useful visual
evidence rather than declaring the earlier black PrintWindow captures valid.
r163 adds a temporary-message/DC observer to sample01: PrintWindow returns
success without observed WM_PRINT/WM_PRINTCLIENT dispatch, and the probe's
930x630 bitmap, clip and origin remain intact. A readable x64 capture returns
in that run, so the failure is intermittent, not proved architecture-specific.
This does not establish the internal cause of the host capture failure.

A separate explicit WM_PRINT capture reveals a project bug: stripping
PRF_NONCLIENT from native delegation also strips the default client's origin
adjustment. The demo additionally lacked a shared WM_PRINTCLIENT paint path.
OpenNT windows/core/ntuser/kernel/dwp.c, xxxDWPPrint, SHA-256
CC48B9CA3303C80EE1B2450DA143E9BCE42F99DC037DC4517114F4B863938022 is read-only
reference: visibility check, nonclient/client/children order, per-region
saved DC, client clipping/erase/print and visible-child dispatch. Its private
PWND/Gre/thread-lock dependencies remain a stopping boundary; no Win32k code
is imported. The standalone frame uses authored public HWND/GDI composition.

Retained experimental dispositions:

- r164: explicit-message capture exposes unpainted client bands; x64's later
  outer-menu probe times out. Not an accepted full gate.
- r165: forwarding all native print flags is tested, but the first coordinate
  assertion incorrectly examines viewport origin alone. The diagnostic is
  corrected to LPtoDP, which includes window-origin changes.
- r166: both full gates pass and client origin measures6,61; visual review
  nevertheless finds native File text below the retro menu. Passing geometry
  alone is insufficient. This native-frame-delegation candidate is replaced.
- r166-negative: new objects against r163's old library exit1 at the actual
  client-origin assertion (probe PID17440,19:20:41).
- r167: the new --capture-test mode constructs only the isolated showcase,
  shortening this diagnostic. PRF_CHILDREN alone draws no children through
  the old default delegation, consistent with the original source's nesting;
  do not rely on that as a split-render workaround.
- r168: authored frame/client/child composition yields a reviewed complete
  x64 explicit-message image without native menu residue (PID39252,19:26:45).

The retained implementation draws its custom frame, maps and clips the actual
client, sends erase/WM_PRINTCLIENT as requested, and prints visible direct
children at their window positions with saved/restored DC state. Owned
windows follow the requested PRF_OWNED flag. It never delegates native frame
painting or modifies visible WM_NCPAINT/theme ownership. Enumeration uses
public HWND APIs; parent/attachment lifetime is rechecked after application
callbacks. Demo WM_PAINT and WM_PRINTCLIENT share their client rendering.

The capture suite retains both PrintWindow and explicit WM_PRINT images with
distinct filenames. It verifies the actual client origin and sample01's blank
bands immediately below the menu and at the client bottom. r169-negative
links the new probe to r166's native-frame candidate and exits1 at the blank
band pixel assertion (PID18428,19:28:43), catching the visible native File
residue that the origin-only test missed. Explicit-message capture is owned
rendering evidence, not physical first-paint/hover acceptance and not a claim
that the original intermittent PrintWindow mechanism has been repaired.

### Correction: black previews were not black capture files

The earlier black-capture attribution is disproved for the inspected files.
After r169's explicit-message pixel assertions passed, the default image-view
tool still displayed only the tiny system cell on black. Reading the SAME PNG
through System.Drawing reports opaque white at10,100 and30,63, agreeing with
the BMP and in-process pixel assertion. Viewing that same path with original
detail instead of the default preview shows the full correct image. Retesting
r162/x64/capture.bmp-all-controls-popup.png in original detail also shows its
full image; r162's diagnostic PNG and r158's x64 capture have the expected
opaque-white sampled pixels. Thus the observed black presentation cannot be
cited as capture-file corruption or an x64 rendering failure. The default
preview pipeline is the affected evidence boundary; its internal cause has
not been debugged. All earlier statements attributing those black previews to
PrintWindow are superseded by this correction. Use original-detail review and
raw-pixel assertions, not the default preview alone.

The independent WM_PRINT defects remain real: r166-negative fails the actual
client-origin assertion, and r169-negative fails the raw blank-band pixels.
These are not inferred from the preview failure. No screenshot retry/sleep,
desktop switch or host-setting workaround is added to the component.

Full r169 runner PID29212 start2026-10-12 19:27:58 exits0. Both architectures
pass27,261 gallery checks,791,847 geometry checks, scroll/API/DC gates, font
consistency and the new origin/blank-band assertions. Both GDI attachment
counts remain56/56. Three fresh x64 --capture-test runs (PIDs18884,43644,19988
at19:31:13/15/16) each exit0. The x64 explicit-message image was reviewed at
original detail. Binary SHA-256:
6F66827E743987062E2B5569D274F76304E001A01A79A993AE33C5B13A97DD90.
Source and tests remain confined to the standalone component/demo. Outer
frame scrollbar tracking and the other admitted S4 gaps are still open.

## Frame scrollbar query/tracking and independent repaint (r170 onward)

Original client/winmgrc.c GetScrollInfo dispatches SBM_GETSCROLLINFO only for
SB_CTL, but uses NtUserSBGetParms for SB_HORZ/SB_VERT. Source SHA-256:
26F0174E6A0DB53D942506CBDC12EFBFEBC34A943D780170E7BF9D95775514A4.
r170 -ScrollApiOnly passes both architectures: both frame axes dispatch zero
query messages; pos/track are31. Attempting to write SIF_TRACKPOS123456 leaves
track31. Replacing mouse tracking alone would break full-width position
queries. No private USER state write or product hook is admitted.

Explicit win16_style_get_scroll_info and win16_style_set_scroll_info bindings
retain native range/page/position ownership. Query overlays only the active/
reporting frame axis's SIF_TRACKPOS; update suppresses native immediate paint
and redraws the affected retro bar. Unstyled windows delegate unchanged.
The same-GUI-thread restriction remains. The demo calls these bindings from
its own frame notification handler; no import patch, process hook or product
integration is implemented. Automatic application binding remains later work.

Frames created with scrollbars reuse the embedded fixed-thumb layout/tracker
through a frame-owned attachment. Notifications retain lParam0 and original
WM_HSCROLL/WM_VSCROLL commands. Queries supply32-bit track positions instead
of HIWORD's16 bits. Palette changes update the attachment; removal cleans up
capture/timer without removing arbitrary unrelated control attachments.

Frame/child notifications no longer unconditionally repaint the whole frame.
WM_CAPTURECHANGED repaints caption buttons only if one was pressed. Each bar
composes into a bar-sized temporary bitmap and copies once, retaining checker
brush phase and releasing resources immediately. Allocation failure retains
the direct-draw fallback. No persistent buffer, new timer or global setting.

r171's focused x64 trial (PID31784,19:40:38) reaches positions130061 and129577
and releases capture. r172 puts a sentinel pixel on the other bar: it must
survive down/move/up, detecting direct GDI repaint that WM_PAINT/WM_NCPAINT
counters alone miss. r171's library with r172 probes fails this assertion
(r172-negative PID46576,19:45:01, exit1). New code passes both axes/widths.
r172 full runner PID39540,19:42:57 exits0 through geometry/API/scroll/DC and
27,261 gallery checks each width. This is not physical temporal acceptance:
probes prove unrelated-axis pixels/positions stay intact; real held-input
smoothness remains separate. Accessibility geometry, wide EDIT/list ranges,
late-added frame bars and SetScrollPos/SetScrollRange/ShowScrollBar/
EnableScrollBar entry-point coverage are still open.

r173 adds real frame cancellation, removal during a captured arrow gesture,
native query fallback after removal and reinstall. Both architectures pass
these plus the retained full suites; runner PID47400 start2026-10-12 19:46:27,
exit0. x64/x86 attachment counts are56/56. The original-detail x64 capture
shows fixed Win3.x outer thumbs, with all existing controls retained. Demo
SHA-256:8706D48A7B0B0C5E76F2EC2B9B3EB827EB987976415589672F690E897E6CCC2A.
These cleanup checks prove capture release/query fallback, not all reentrant
application-callback or removal-notification semantics. S4 remains active.

## Live gesture removal and captured combo clicks (r174--r177)

r174's seven frame callback cases cover destroy, remove, remove/reinstall with
a new gesture, capture transfer, disable, cancel and disappearing range. The
focused x64 probe PID14676 at2026-10-12 19:53:33 exits0, but its diagnostics
show that explicit removal emits no SB_ENDSCROLL. r175 reuses scroll_finish
before detach, with an in-call lifetime reference and recursive-removal guard;
the outer frame removal also checks HWND survival after application callbacks.
Removal now emits the ending notice while the full-width query is still valid.
Destroy itself sends no after-destruction notification. r175 focused output
passes all seven cases, including one end per live gesture and two for the
remove/reinstall case. This is bounded callback coverage, not all reentrancy.

The owner next reports combo popup closure after releasing a scrollbar drag.
The previous integrated fixture injected WM_NCLBUTTONDOWN directly and even
reopened a popup if it had closed. That recovery is removed: popup survival is
now an assertion, not fixture repair. A new test opens both CBS_DROPDOWNLIST
and CBS_DROPDOWN via their button and releases to the actual capture owner.
It then sends the client-coordinate mouse sequence received by the captured
ComboLBox, rather than assuming that every bar click is a nonclient message.

Read-only original reference: OpenNT windows/core/ntuser/client/lboxctl2.c,
xxxTrackMouse near1235--1272, hash
A99C9EF3B6A7A07B219DDDA18FF2FB6A8A11C42F5378E5F77E10E336AD7D1744.
The captured-combo branch converts a click outside the client but inside the
window into a nonclient click, releasing capture before synchronous scrollbar
tracking and restoring it afterwards. Our message-driven tracker returns
between messages; nesting it inside that native synchronous wrapper violates
its ownership assumption. This is a project adapter bug, not a guest defect.
No original source is imported or modified.

The repair normalizes captured client clicks on an owned combo list's native
scrollbar hit regions before native list selection dispatch. It uses the
existing tracker and borrows the popup's existing capture through release.
Item clicks, outside clicks and keyboard closure remain native. No forced
reopen, close-message filter, new tracking loop or repaint timer is added.

r176's initial restricted-desktop probe fails CreateDesktop with error5; it
is not a behavioral result. The first privileged probe exposes an incorrect
test assumption about the opening button's capture owner; the fixture is
corrected to release to that actual owner. Before the library repair, PID6564
at20:04:56 exits1 with top index0->0 and capture=NULL after drag. After repair,
PID10360 at20:05:48 passes with top0->15 and retained popup capture for both
combo types. Expanded x64 probe PID4604 at20:07:14 exits0, additionally testing
thumb release outside the bar, arrow/page clicks, unchanged selection during
scroll, item selection/closure, Escape and outside-click closure.

r176-negative links the identical expanded test objects against the retained
r175 library. PID32220 at20:08:10 exits1 at the retained-capture assertion:
native boundary logging sees capture losses, top remains0, and capture isNULL.
The popup's dropped/visible flags happen still to be true in this synchronous
probe; do not claim it reproduces every physical timing detail of the owner's
visible closure. It proves the broken capture/drag contract and detects the
repair. Tests use native controls on a private desktop, not computer use.

r177 full runner PID29456 at20:07:48 reaches a passing x86 scroll matrix,
including both new combo cases, then fails the gallery's stale active-removal
assertion. It expected only the initial SB_THUMBTRACK; r175 intentionally also
sends SB_THUMBPOSITION and SB_ENDSCROLL before detaching. The revised assertion
requires that exact three-notice sequence, position70000, surviving HWND and
released capture. Its callback attempts removal again during THUMBPOSITION,
exercising the recursive-removal guard rather than merely increasing a count.
r177 is retained failed evidence, not a delivered full pass.

r178 runner PID26968 at20:10:14 passes the full x86 suite and the x64 combo/
scroll suite, then fails the frame-callback whole-process GDI equality check.
The original check did not print its counts, so the failed run does not prove
which object or direction changed. Diagnostic r179 x64 PID34204 at20:13:00
adds counts and passes the complete gallery:60 objects before/after all seven
callback cases,56/56 in the separate repeated-attachment test,27,261 checks.
The test now flushes this thread's GDI batch before both strict equality
snapshots, retaining counts and the original no-growth/equality assertions.
This improves observation ordering; it is not proof that batching caused the
one failed run, or an authorization to erase that failure. No rendering or
resource-lifetime production change is made in response to the unquantified
count failure.

Final r180 runner PID40360 at2026-10-12 20:13:50 exits0. Both x86/x64 pass
the expanded combo/scroll suite,791,847 geometry checks, API/DC gates and
27,261 gallery checks. Frame callback counts are58/58 x86 and60/60 x64;
repeated attachment counts56/56 on both. Three additional x64 capture-mode
runs (PID36756 at20:15:39,16476 at20:15:41,27836 at20:15:44) all exit0 with
both combo contracts and all seven frame callback cases. These repeat passes
do not retrospectively explain or erase r178's resource-count observation.

Owner-test demo: build/M0-T445/S4/r180/x64/win16-style-demo.exe, SHA-256
E00DC711F5ABCF7EDBFD25370C33591AB1B7C53817D7C1DD02134E837437059F.
This is an intermediate standalone S4 build, not an S closure or production
release. Product components, guest binaries and installation directories are
unchanged. Physical owner interaction remains distinct from message probes.

## Scrollbars added after frame attachment (r181--r183)

The remaining late-added-frame limitation is exercised by creating a styled
window with neither WS_HSCROLL nor WS_VSCROLL, then adding each through range
and visibility APIs. Pre-fix r181 x64 PID5660 at20:20:11 exits1: WS_HSCROLL is
set, but GetScrollBarInfo returns an empty rectangle and OFFSCREEN. The
fixture's first draft additionally assumed no OFFSCREEN on the private
desktop; it is replaced with the relevant nonempty/not-INVISIBLE contract.
The failure remains an actual empty-rectangle failure, not that draft check.

Two project boundaries were incomplete: tracker attachment was conditional on
initial style bits, and custom WM_NCCALCSIZE bypassed native calculation
entirely. Original OpenNT windows/core/ntuser/kernel/calcclrc.c,
xxxCalcClientRect, clears and reconstructs WFHPRESENT/WFVPRESENT; its hash is
49C51951B20CF984622AEB512F113BAB1E98D5E8A08D066D35FBD3DB99D82B6C.
kernel/dwp.c's WM_NCCALCSIZE calls that owner; kernel/sbctl.c CalcSBStuff
consumes the presence bits to extend scrollbar rectangles from client bounds.
The already pinned dwp.c/sbctl.c remain read-only evidence, not imports.

Every styled frame now installs the existing tracker once, even without bars.
WM_NCCALCSIZE delegates native calculation for native state before supplying
the custom client rectangle. It does not delegate nonclient painting. No new
late-install state machine, copied geometry cache or private USER access.
Post-fix focused x64 PID34140 at20:22:32 exits0: late horizontal rectangle
11,303,469,329 and vertical443,45,469,303, both state0; independent 32-bit
drag reaches130155/130422, with capture/removal and unrelated-axis pixel gates.
The fixture also hides and shows each bar and posts a bounded release so a
native modal-tracker negative cannot hang indefinitely.

r182 full runner PID20180 at20:23:08 passes x86 scroll checks, then fails
sample01's system-menu gate. This is the default keyboard command route, not
physical mouse evidence. Repeating the identical x86 binary (PID24076 at
20:25:06) passes27,261 gallery checks and56/56 repeated-attachment resources.
The failure is retained; its cause is not established by the passing repeat,
and is not silently attributed to this repair or to the historical native
menu issue. No menu timeout, assertion, behavior or painting is changed here.

r183 runner PID24036 at20:26:32 passes the complete x86 suite. Its x64 frame
callback resource check now captures the previously unexplained direction:
baseline60, final58. GdiFlush does not ensure equality of whole-process native
resources across creation, disable and destruction of multiple HWNDs. A
decrease is not an allocation leak. That diagnostic assertion is corrected
to no growth, retaining the counts and the separate repeated-attachment gate
on a surviving window. This does not claim ownership identification for the
two retired objects or reinterpret the old unquantified r178 result as proven.
No added tolerance permits growth; no product resource/painter code changes.

Final r184 runner PID2380 at2026-10-12 20:29:23 exits0 for both complete
architectures:791,847 geometry checks, API/scroll/DC gates and27,261 gallery
checks each. Late-added bars pass hide/show, capture, 32-bit drag and removal;
the existing combo release contracts also remain passing. Nine nonclient
surface transitions have changed=0 and client-strip dirty=0 on both widths.
Repeated-attachment counts remain56/56. The original-detail sample01 capture
was inspected with the retained menu font and controls intact, not a claim of
physical-input acceptance. The earlier isolated menu failure remains recorded.

Candidate demo: build/M0-T445/S4/r184/x64/win16-style-demo.exe, SHA-256
CED9513936B716CD66C3BE054A4B35C499BB4FFFCACF68F487910D2D1139B8DF.
S4 remains active: broader scrollbar entry-point/accessibility binding,
wide native control ranges and full interaction/fidelity acceptance are not
closed by this late-attachment repair. No product publication or S closure.

## Menu timing instrumentation and missing STATIC text variants (r185--r187)

To distinguish r182's isolated menu failure from capture latency, the gallery
now logs elapsed time, capture time, popup-init/idle/exit counters and final
captured/timeout/HWND flags before its existing assertion. It changes neither
the one-second timer nor popup behavior. r185 x64 full-gallery processes31612
(20:34:19),13436 (20:34:32),18304 (20:34:45),30776 (20:34:59) each exit0.
Their80 outer-menu trials take15--93ms (average32.7625); capture takes0--16ms
(average6.0625), at GetTickCount64 resolution. This run does not reproduce the
failure and does not prove its cause. It does not support slow capture as an
explanation in these observations. Fresh-session native-menu work remainsS7.

The coverage audit then finds original STATIC variants missing/misinterpreted:
SS_SIMPLE is rejected, and SS_LEFTNOWORDWRAP uses DT_SINGLELINE, incorrectly
suppressing explicit newlines. Ordinary labels also omit tab expansion.
Source references are the existing original wow16/inc/windows.h definitions
and read-only OpenNT windows/core/ntuser/client/statctl.c (StaticCallback near
340--390, StaticPaint SS_SIMPLE near652--690), SHA-256
92130CA07381823260432025CD019C6084BAF8DF394BD659414D16A2A94D3F7F.
StaticCallback uses DT_EXPANDTABS; LEFTNOWORDWRAP omits DT_WORDBREAK rather
than adding DT_SINGLELINE. SIMPLE is the separate single-line fast path,
whose original assumptions include always enabled. Public DrawText performs
the admitted text presentation; private PWND/PSM internals are not imported.

The authored adapter accepts SIMPLE and uses distinct formatting flags.
Ordinary left/center/right labels expand tabs; LEFTNOWORDWRAP preserves explicit
line breaks without automatic wrapping. Text, style bits and borrowed native
font are not replaced. Sample01 adds Simple text and a tab/newline label while
retaining the other39 controls (41 total); its three boxes are shortened to
fit the existing Labels and boxes group. README's stale19-entry/native-bar
description is corrected to the actual27-entry gallery.

Six focused vectors compare all220x64 RGB pixels against original-contract
formatting with the same public rasterizer/font, including prefixes/literal
ampersands, left/center/right, wrapping and no wrapping. These are formatting
proofs, not claims of original VGA font glyph identity or every text style.
They also verify text/style/font-handle preservation after removal. r186 x64
PID42156 at20:39:34 exits0. r186-negative uses identical test objects with
r184's retained library; PID2728 at20:40:26 exits1 on the first no-wrap/newline
pixel vector. Icon/bitmap and application-owned drawing remain separate gaps.

r187 full runner PID33088 at2026-10-12 20:41:13 exits0 on both widths: all six
text vectors,41-control automatic attachment, combo/scroll/API/DC gates,
791,847 geometry checks and27,261 gallery checks. Repeated attachment remains
56/56. The original-detail sample01 capture is reviewed after adding the two
labels; explicit newline/tab placement and adjacent controls fit the group.
Documentation governance and diff checks pass. This remains an intermediate
S4 iteration, not a closure or explanation of the earlier intermittent menu
failure. Tested x64 demo build/M0-T445/S4/r187/x64/win16-style-demo.exe SHA-256:
A777025B44C057935071A3BDA849D2A23C6512B055793E0D6E4F0C326C93323F.

## STATIC icon coverage and caller-DC clipping (r188--r190)

The next standard-control inventory gap is SS_ICON. The original Win16
declarations in src/mvdm/wow16/inc/windows.h expose SS_ICON and STM_SETICON /
STM_GETICON, but not SS_BITMAP or CENTERIMAGE. The latter are therefore not
described as original Win3.1 styles. Read-only OpenNT
windows/core/ntuser/client/statctl.c (SHA-256
92130CA07381823260432025CD019C6084BAF8DF394BD659414D16A2A94D3F7F)
supplies the image-paint reference: retain native image storage/loading and
size management, paint the borrowed image into the control bounds, preserve
disabled image colors, and handle the later CENTERIMAGE extension at natural
size. Its private PSTAT/PWND, server image sizing and shared DC cannot be
composed into this standalone HWND library. The finite public binding uses
STM_GETICON, DrawIconEx and temporary GetIconInfo copies; no original code,
module or resource is imported and no guest or mirror is modified.

controls.c now admits SS_ICON and avoids querying image content as window
text. The native STATIC procedure still handles setting/replacing/clearing
the icon and returns the prior handle. Only painting changes. GetIconInfo's
temporary mask/color bitmaps are deleted; the borrowed application image is
never destroyed. The fixture authors its own monochrome cross image with
CreateIcon, keeps it until its showcase HWND's WM_NCDESTROY, and does not
substitute an extracted or modern stock image. Sample01 now has43 automatically
attached controls, with the icon and its label in previously unused space.

r188 full runner PID47980 at2026-10-12 20:53:43 exits0 on both widths. Initial
four-vector image tests cover ordinary/centered and enabled/disabled painting,
replacement, clearing, removal and borrowed-handle survival after destruction.
Both widths retain27,261 gallery checks, scrollbar/combobox gates and56/56
repeated-attachment GDI counts. The x86 original-detail showcase screenshot
is inspected: the added icon/label does not overlap existing controls.

Review adds a second size to those four vectors: an8x6 client with a16x16
centered image. Each WM_PRINTCLIENT test uses a40x36 caller-owned bitmap filled
with a sentinel and checks every RGB pixel, including the area outside the
client. Native BeginPaint supplies a clip, but a caller's printing DC does not
necessarily do so. The painter now intersects the existing clip with the
actual client rectangle, retaining the caller DC through SaveDC/RestoreDC.
r189 negative build PID12636 at20:56:16 links the expanded test objects with
r188's retained library. Probe PID28204 at20:56:23 exits1 at the pixel assertion
for the undersized centered-image case. This establishes that the added test
detects the unclipped boundary rather than only accepting a reference image.
The reference raster uses the public icon primitive with source-derived
placement; it does not claim pixel identity with every original VGA driver.

r190 full runner PID36184 at20:56:38 exits0 on both widths: all eight icon
vectors,43 automatic controls, six text vectors, combo release/scroll/API/DC
gates,791,847 geometry checks and27,261 gallery checks. Repeated attachment
remains56/56. Its x86/x64 showcase PNGs are byte-identical to the reviewed r188
x86 image (SHA-256 C2D7F4A86449CEB83E34B992103B398F0743862646EB9401FAE542082DDA37F2).
The x86 original-detail view is also inspected. An intermittently black x64
tool preview is not the file content: decoded pixels and the complete PNG hash
match the correctly displayed x86 image. No image is retouched.
The tested x64 demo SHA-256 is
D9C09A5DF78F6B77DE11EB37F22ABC01FCDF772CBCA90DE32F934B54E9233C0B.
Bitmap extensions, application owner-draw boundaries and the remaining S4
acceptance audit stay open. This is not S4 closure or a production release.

## Bitmap extension: retain the native painter, isolate its DC (r191--r192)

SS_BITMAP is not in the original Win16 style declarations. It is handled as
a compatible host image control, not relabelled as a Win3.1 control. The same
pinned OpenNT statctl.c above supplies the bounded comparison: ordinary images
stretch to the control, centered images retain natural size, surrounding space
uses the bitmap's top-left pixel, disabled images retain colors, and an empty
bitmap paints nothing. The STK_IMAGE table has no unconditional erase flag for
SS_BITMAP. Clearing its handle is not the SS_ICON background-fill behavior.

The initial experiment delegates bitmap messages to the native STATIC instead
of implementing a second blitter. test_static_bitmap supplies authored24-bit
checker DIBs and compares every pixel with public StretchBlt at the source-
defined placement. Eight combinations cover ordinary/centered, enabled/disabled
and client sizes40x36/8x6, each with initial image, replacement and clearing.
Sentinels outside the client detect escaped drawing; native returned handles,
style preservation and caller-owned bitmap survival after detach/destroy are
asserted. No image replacement, global setting, private USER provider or new
product integration is introduced.

r191 is an incremental diagnostic root. Build PID19944 at2026-10-12 21:02:16
passes; probe PID7768 at21:02:23 exits1 after the ordinary cases pass: centered
replacement retains a gray pixel where the changed image should be blue.
The viewport hypothesis is tested by build PID35552 at21:03:11 and probe
PID19180 at21:03:18. Viewport origin stays0,0; that hypothesis is rejected.
The original centered branch calls ExcludeClipRect after painting the image.
Build PID37004 at21:03:52 and probe PID19852 at21:04:01 confirm its surviving
clip: the image center changes from visible1 to0 on the first print. This
probe exits1 at the new unchanged-clip assertion. Keep all three separate logs;
the diagnostic is not a passing bitmap implementation.

The production-library adjustment is only a saved/client-clipped/restored DC
around native WM_PRINTCLIENT. Ordinary painting and every other bitmap message
still delegate unchanged. Do not delete the borrowed image or duplicate its
loading/alpha handling. SaveDC failure paints nothing without changing the
caller DC. The authored demo bitmap belongs to its showcase HWND and is
deleted only after child destruction at WM_NCDESTROY. Sample01 adds the small
checker beside the frame/rectangle samples, keeping existing control IDs.
r192 full runner PID40856 at2026-10-12 21:04:56 exits0 on both widths. All eight
bitmap vectors and their24 DC-preservation cases pass, together with the eight
icon vectors, six label vectors,44-control scope, combo/scroll/API/DC gates,
791,847 geometry checks and27,261 gallery checks. Repeated attachment is56/56.
The original-detail x86 showcase screenshot is inspected; its newly added
checker and longer group heading fit without overlap. x86/x64 images have
identical SHA-256 F5151DA4685C4EF0317D6D635ED456E266CDF4258A5BF304A16986FF03916D68.
The verified x64 demo SHA-256 is
8D3FDD3C9766B9425473683863CB83D30609FBB6FC36A6724DF9D7DC1FA277FE.
Native alpha-conversion variants and application owner-draw are not certified
by these24-bit fixtures. The rest of the S4 acceptance audit remains open.

## Owner-reported frame marks versus scrollbar edges (r193 investigation)

Owner screenshots identify apparent misalignment between the sizing-band
division strokes and horizontal/vertical frame scrollbar arrows. Do not assume
these independently owned historical boundaries must coincide, or change only
painted arrows while leaving native input rectangles unchanged.

The pinned USER3 disassembly at0F4C..0F79 loads VGA UPARROW/DNARROW/RGARROW/
LFARROW into0606/060C/0612/0618. Helper0881..08BA stores bitmap width/height at
record+2/+4. USER3:1452..145B publishes0608/061C as scrollbar metrics; the
retained byte-exact VGA arrows are17x17. This is direct arrow/thickness evidence,
not an inference from the independently verified17-pixel thumb length.
Caption sizing uses the separately established18-pixel artwork/19-pixel pitch
and sizing strokes documented in resources.md.

Original USER3:24BC..24DD defines inner sizing bands N and full frames F=N+B.
USER18:0F5F..1033 uses N for scrollbar outer insets and the17-pixel arrow metric
for thickness. USER1:A06B..A1DE puts the lower/right sizing strokes at
outer-F-19 in the one-pixel VGA profile. Consequently the vertical bar's left
edge minus the lower stroke is (outer-N-17)-(outer-F-19)=3 original pixels.
The horizontal top edge has the corresponding relationship. These strokes
are corner/edge resize markers, not scrollbar delimiters. Matching them by
stretching the arrows to caption size would not restore this source contract.

r193 adds FRAME METRICS logging to the existing isolated real-HWND frame-scroll
test; it makes no library/layout changes. Background build PID43360 starts
2026-10-12 21:12:06 and exits0; probe PID37480 at21:12:13 exits0. At144 DPI the
930x630 showcase reports border6/unit2, H=(6,598,898,624), V=(898,61,924,598),
arrow26, lower stroke starts x=33/895 and side stroke starts y=33/595. Thus
right/bottom gaps are3 host pixels. The left arrow's final column31 versus
stroke33 is a2-pixel difference, including fractional-scale rounding. The
smaller late-added-bar window gives the same right/bottom relationship.

This proves the mismatch is real and that mandatory flush alignment is not an
original invariant. It does not certify full historical geometry at144 DPI:
the current frame reserves GetSystemMetricsForDpi sizes and uses native
GetScrollBarInfo rectangles, while marks use the scaled historical caption
metrics. Uniform source-derived DPI geometry remains an open fidelity item;
one must reconcile client reservation, shared border, paint and input together,
not cosmetically move marks or widen arrows. No claim of a visual repair is
made by this diagnostic-only change. r192 remains the last full verified demo.

## Logical-96 baseline isolates a shared-border divergence (r194)

Owner supplies a Win3.1 Notepad screenshot and requires even intentional
misalignment to follow the original, not merely look plausibly similar.
The preceding 144-DPI three-pixel result is therefore not an equivalence gate.
The test executable adds --capture-test-96: the existing isolated capture
suite in a DPI-unaware process. This is a logical-coordinate diagnostic, not
a host DPI/settings change or a physical 96-DPI monitor claim. Normal demo
and --capture-test retain per-monitor-v2 awareness. Library bytes are reused
from r192; no new frame geometry is delivered by this diagnostic.

Background build PID40772 starts2026-10-12 21:19:25 and exits0. Logical-96
probe PID44348 starts21:19:53 and exits0. Its 930x630 sizeable frame has
F=4, B=1, arrow17, H=(4,609,909,626), V=(909,42,926,609), lower-right stroke
x907 and right-bottom stroke y607. Both observed gaps are2 logical pixels,
not the original3. The smaller late-added-scrollbar case repeats the gap2.
Combo release, static image/text, frame callback/32-bit tracking and print-DC
checks also pass at this logical DPI; these behavior passes do not certify
historical frame geometry.

The source arithmetic distinguishes the causes without fractional rounding:
original USER18 places the scrollbar's outer edge at outer-N where N=F-B;
current measure_window reserves the whole F and the native bar ends at
outer-F. Thus the current right/bottom bars are one original border unit
too far inward for that original sizeable-frame contract. At96 DPI this
changes the gap from3 to2. Earlier evidence of a gap3 at144 DPI was a
coincidence of two metric/rounding systems, not proof of fidelity.

Read-only OpenNT kernel/sbctl.c CalcSBStuff (same pinned source as above)
explains why the modern binding cannot be repaired only by changing an arrow:
it derives bars immediately outside rcClient, right/bottom, using system
thickness. Current paint, hit-test and tracking also consume GetScrollBarInfo.
Original USER18 instead includes shared boundary lines in its rectangles.
The next geometry change must cover the client reservation, shared edges,
crossing square, drawing and input together, including single-axis and
late-added bars. Keep that repair open; do not move sizing strokes to conceal
the mismatch. The Notepad screenshot alone does not identify whether its
scrollbars belong to the outer frame or an EDIT child, nor its resampling
factor, so it is visual corroboration rather than a pixel-coordinate oracle.

## Shared outer edge repair and negative control (r195--r196)

The first bounded layout correction reserves native scrollbar thickness minus
the scaled shared border, only for the original sizeable non-dialog right-bar
profile. This changes rcClient at its existing owner; paint, native rectangle
query, hit testing and the existing tracker consequently observe the same
right/bottom displacement. There is no drawing-only offset, moved sizing mark,
global metric change or added input implementation. Fixed/dialog frames and
the host-only left-scrollbar extension keep their existing behavior pending
their separate original-contract audit. Native thickness is still used: this
is not completion of the entire historical scrollbar-geometry requirement.

The real-HWND test now independently asserts USER18's outer edge at W-(F-B)
or H-(F-B), and hits the last pixel of the shared outer band. Tiny-client
threshold and client-size assertions reflect the reduced reservation; no
behavior assertion or deadline is removed. Background full runner PID26184
starts2026-10-12 21:22:37 and exits0 at r195. Both widths pass27,261 retained
gallery checks plus the geometry/API/scroll/DC matrices and new shared-edge
checks. Repeated attachment GDI is56/56 x86,59/59 x64. The original-detail
x86 sample01 screenshot is inspected: bars extend over the former extra inner
frame line without covering controls; the corner and caption remain intact.

r196 links the new test objects to the unchanged r192 x86 library. Build
PID41172 starts21:23:44, exits0; negative probe PID26172 starts21:24:18,
exits1 at the exact new outer-edge assertion, after the combo-release gates
pass. Thus the new gate detects the former layout, rather than agreeing only
with a changed test expectation. Positive logical-96 probe PID39584 starts
21:24:53, exits0 using r195 x64. H=(4,610,910,627), V=(910,42,927,610),
strokes x907/y607: right/bottom gaps are now the source-derived3 logical pixels.
The smaller late-added-bar fixture also has gap3. Test output remains under
r196; original installations and production binaries are unchanged.

Verified r195 x64 demo SHA-256:
13B2750B331F8B75F76A2B61E79920276DE71F3DEFD450FD6D7E8E5E905E0951.
Sequential review retains open work: original leading shared border and
two-axis end overlap are not yet represented by native GetScrollBarInfo;
fixed/maximized/dialog and single-axis source rules, arbitrary host metrics
and fractional-DPI equivalence still require verification. This correction
proves the outer-edge relation only, not all four scrollbar endpoints or S4
closure. Native accessibility still describes native rather than fixed-thumb
geometry. Do not rename these limitations as faithful original behavior.

## Unified System24 font feasibility (r197, not yet integrated)

Owner requires the title, menus and controls to share24-pixel type and chooses
scaled System bitmap appearance rather than a different scalable font. Current
separate title stretch and stock menu/control fonts do not satisfy this.
Existing tests/app/win31_font_probe.c demonstrates that public
AddFontMemResourceEx accepts a raw private FNT in this host. Its guest font
bytes are not reused. The new build-owned feasibility probe rasterizes the
host System's CP1252 characters32..255, scales each glyph with COLORONCOLOR
to a24-row cell, and packs an in-memory bitmap font with the same scaled
width/ascent/leading. It registers that resource only for its own process,
then removes it after destroying its font/control. No file font installation,
global setting, screenshot reskin or imported guest asset is involved.

Build PID19680 starts2026-10-12 21:29:22, exits0; first probe PID15504 at
21:29:37 exits1 because a zero-advance glyph was incorrectly rejected. The
diagnostic now preserves zero widths and does not call StretchBlt with an
empty extent. Build PID3364 starts21:29:57 and exits0. The subsequent probe
passes: source cell16, realized target24, ascent20, weight700, private FNT9968
bytes; all224 characters and56,448 pixels compare exactly against the scaled
source. A native multiline EDIT accepts the font and reports24-pixel metrics
and a96-pixel formatting height within a100-pixel client.

This proves a font-based route that lets native EDIT own its caret/selection;
it does not yet prove caret interaction, non-CP1252 text, DPI transitions or
application font replacement/restoration. The source remains a diagnostic
under r197, not a library provider. Next integrate bounded font ownership,
direct title/menu drawing and control bindings, then adjust and rerun the
whole gallery. r195 remains the last full runnable demo and does not implement
the newly required unified24-pixel typography.

## System24 component binding and revised native oracles (r198 onward)

font.c/font.h promote the proven memory-font route into the standalone source
owner, with one reference-counted font/resource per GUI thread. Each character
is rasterized from an explicitly requested16-row host System font and scaled
once to24. Frame title and menu share the same acquired HFONT; the old title
intermediate bitmap/second stretch is removed. Controls give this font to the
native control, including EDIT/list/combo, rather than magnifying only paint.
WM_SETFONT remembers the latest borrowed application font while maintaining
the effective24-pixel style font; detach restores it without deleting it.
The final reference removes the process-private font resource. No global
font change, disk font asset, guest font or product integration is introduced.

Read-only format corroboration: OpenNT windows/core/ntgdi/inc/winfont.h,
SHA-256 EC6A7673E7A4A1684C3A7D4B303243E80E9AACD76A77E1629BD9C2008EAED005,
declares the FNT2 offsets and118-byte header. Original VGASYS's pinned raw
header has widthBytes276, bitsOffset1018, height16, size5441: widthBytes is
the total byte-column count, not one glyph's maximum width. The generated
resource uses total byte-columns; no source body or original resource bytes
are copied. Composition uses public AddFontMemResourceEx instead of importing
the private GDI font driver/loader. The existing standalone font probe already
established this bounded loader boundary.

New tests/app/win16_style_font.c independently constructs expected glyphs with
host System TextOut/StretchBlt and compares every output pixel and width. It
also checks native EDIT24-row positioning/selection, shared handle references,
repeat acquire/release and GDI non-growth in normal and logical96 contexts.
r198 build PID30812 starts2026-10-12 21:33:54, exits0; probe PID40000 at
21:34:28 exits0:224 glyphs,56,448 exact pixels, native row/selection, GDI7/7.
Full-run generator now includes this gate for both widths and both contexts.

Retain failed transitions rather than treating them as passing deliveries:

- r199 runner PID35676 at21:36:42 exits1 on simple-combo visible-bar pixels.
  r200 build PID30168 at21:39:09 and probe PID46576 at21:39:16 confirm52
  differences. Further build PID4016 at21:40:33 and probe PID14352 at21:40:41
  locate first error at child-local192,168. The native list extends to screen
  y325 while its parent's client ends at321; the bar's last two26-wide rows
  are clipped by the ancestor. The memory-DC expected image lacked that clip.
  The observer now intersects ancestor client rectangles and still compares
  every visible pixel. This is not a scrollbar repaint change or discarded
  visible failure. r200 capture PID41776 at21:39:36 confirms candidate24-pixel
  text but old sample spacing clips labels; sample01 spacing/window grows1.5x.
- r201 runner PID13076 at21:41:58 exits1 on old96-DPI caption corner coordinates.
  Twenty-four-row type requires a caption of at least24 rows, six more than
  the original18 at96 DPI. Caption side bands follow that explicit typography
  adaptation; captionless corner coordinates retain the original values.
- r202 runner PID49000 at21:43:59 exits1 on the obsolete invariant that item
  rectangles cannot change after a font change. r203 runner PID20932 at
  21:45:54 exits1 on assuming a font round trip restores all original item
  rectangles: integral-height LISTBOX also changes its HWND height natively.
  The revised test compares every restored rectangle against an independent
  unstyled native LISTBOX given the same System-to24-to-System sequence,
  while asserting24-pixel styled rows, unchanged selection/data and clipping.
  No custom resizing, LBS style toggle or selection rewrite is introduced.
- r204 runner PID28804 at21:48:19 passes the gallery but exits1 in the old
  second-SaveDC fault case. Removing the title-stretch bitmap removed that
  second call. The DC fixture now initializes the real shared font and tests
  failure/success at the sole remaining save boundary, retaining borrowed
  bitmap/clip/text-color/stretch-state checks and callback lifetime cases.

The font-source hash for this candidate is
7657C6B1C70E9B2A9C68EC5B52271582A3EA535038944A755F6389B16B6E5AFE;
font-test source hash is
522D3EF1FC1768E04288EA1BC2BF3D52F6BA20B8290CB494F95BD72AA8B9003B.
Exact pixel certification remains CP1252/ANSI System; wide-Unicode fallback
and non-ANSI source realizations are not certified. Font/geometry adaptations
do not close the remaining S4 scrollbar fidelity/control-variant audit.

r205 full runner PID45292 starts2026-10-12 21:50:20 and exits0. Both widths
pass font normal/logical96 (224 glyphs,56,448 exact pixels each, native EDIT
row/selection, GDI7/7), complete scroll/API/geometry/DC groups and27,261
retained gallery checks. Both repeated-attachment rows are54/54. The actual
showcase control/menu metrics report24, with the same shared font handle;
the latest-borrowed-font restoration test passes. The enlarged x86 showcase
is inspected at original detail: title/menu/control text shares the glyph
size and earlier clipped labels are visible. Both architectures' showcase
PNG SHA-256 is5FFB4F1C2520074745F6E21B23E3EB273EF89C26AB08CA06E86A0831535581F4.
The x64 standalone demo SHA-256 is
1E09F94EF9AADD3CF95FDC55D3F8D2CD10D7C0F655D377936C570A2C3AD750F1.
This is a local tested standalone typography delivery, not a production
release, commit/push or full S4 closure. Unrelated worktree changes remain
untouched. Owner physical acceptance of the new typography remains pending.

### Unified24 focused-gallery layout follow-up

The font transition also affects the old16-row coordinates of focused samples
20--27, not only sample01. A new observer measures their BUTTON/STATIC text
using the actual selected font, original control-glyph allowance and current
client rectangle; it also checks containment inside the sample client.
r206 reuses the unchanged r205 x64 library and rebuilds only the demo/gallery
objects. Build PID44172 starts2026-10-12 21:59:56, exit0. Its private-desktop
gallery PID32904 starts22:00:17, exit1 at sample20's disabled checkbox:
`Disabled checked` requires174 pixels but the label rectangle has173. This
is a proved demo-layout regression, not a font or checkbox-painter defect.

The demo now scales its owned focused-page coordinates by3/2, including native
dropdowns' retained dropped rectangles rather than their closed heights.
Application HWND resizing is not added to the style library. The font remains
24 and resource glyph sizes remain unchanged. The gallery atlas reserves the
actual enlarged sample bounds instead of overlapping/cropping adjacent images.
The observer's sample20 label now has273 pixels and42-row control height;
six checkbox/radio labels, seven button/static labels and four automatically
attached labels pass. Source identity collection also now includes the newly
added font-test source explicitly; r205's sealed manifest is not rewritten.

r207 is the fresh both-width full runner, PID29580, start2026-10-12 22:01:31,
exit0. Both widths pass the complete font, scroll/API/geometry, gallery and
DC suites;27,261 retained checks each and GDI attachment54/54. The x86
focused-page21--24 atlas is inspected at original detail: all labels are
readable, editable/list/combo text uses the same glyph size, and the normal,
disabled and editable dropdown forms retain their separate layouts. This
inspection does not replace physical popup-drag acceptance or close the
remaining original-geometry/control-variant audit.

### Native File hover and menu-click frame damage

Owner reports a host-font File item appearing over the styled menu bar on
hover. The running original is r205 x64 (PID12284), subsequently confirmed
by the owner. r208/r209 add diagnostic-only same-thread dispatch logging,
including host0x90--0x95 messages and visible thread-window identity. The
trace file permits concurrent read access. The r209 baseline trace records
0x0092 sent to showcase HWND0x550BB2 after a real File hover; this is a direct
host menu-drawing route outside WM_NCPAINT/NC-mouse handling. No separate
tooltip identity was established, so it is not classified as a tooltip bug.

The frame subclass now consumes host menu drawing0x91/0x92 for its normal
attached menu bar, without inspecting their opaque payloads. Minimized/no-menu
windows delegate as before; measurement0x93 is not suppressed. This matches
the existing ownership of host caption drawing notifications: USER still owns
menu management, while the standalone painter owns its menu-bar pixels. No
original module, system setting or other product component changes.

r208 build PID29948 at2026-10-12 22:07:46 exits0. Its hidden trace process
PID25484 at22:08:11 reaches its300s owned deadline and is terminated; this is
not an interactive pass. r209 build PID23916 at22:09:23 exits0; the visible
trace PID37200 at22:09:45 later exits0. All desktop actions are scoped to the
demo. A physical Escape interrupts computer use during candidate observation;
no desktop action resumes afterward. r210 full runner PID24344 at22:11:06
exits0 for both widths. Candidate observation PID32052 at22:11:41 exits0 but
does not prove the interrupted hover action. After the owner identifies their
initially retested r205 path, the r210 x64 link is supplied; owner then confirms
hover is fixed and separately reports a whole-window flash on clicking File.

The isolated click probe records one real menu-idle entry, zero client paint,
zero erase and zero UI-state updates. Nevertheless an unrelated right-border
witness pixel changes from0x775325 to0 on r210. Source review identifies full
`paint` calls for menu selection/open/return state. The repair reuses that
painter with a clip limited to the menu bar and system-button rectangle;
caption, sizing bands, client and scrollbars are excluded. Activation and
ordinary frame invalidation retain their full-frame route. No redraw timer,
global double buffer or suppression of application paint is introduced.

r212 full runner PID14740 at22:21:18 exits0 both widths, including27,261
retained checks each and GDI54/54. r213 compiles the same observer against
r210/r212 x86 libraries: build PID17160 at22:23:21 exits0, old probe PID45168
at22:23:27 and candidate PID23800 at22:23:30 exit0. The then-diagnostic witness
logs old0 versus fixed0x775325, with the same zero client/erase/UI-state counts
and one menu-idle entry. The final observer asserts those results explicitly.
The earlier r213 launch without an embedded manifest fails with elevation
required; its stale shell PID/exit output is not a test result. Embedding the
ordinary asInvoker manifest resolves the invocation; no elevation UI is used.

The new menu-dispatch fixture also has an old-library negative: r211 x64
PID42588 at22:15:43 exits1 because r207 forwards the claimed drawing message.
The fixed library passes routing, but initial one-cycle GDI comparisons fail
(10/32 or32/38). Expanded eight-cycle observations show first-cycle growth,
then stable or decreasing counts (r213:44/44 through round3,44/42, then42/42).
This supports bounded first-use state rather than repeated attachment growth;
it does not identify every native cached object. The final test logs round0
and requires non-growth after each subsequent complete cycle. Earlier failed
one-cycle checks remain diagnostic failures, not retroactive passes.

The final regression is now part of verify-win16-style.ps1 and its source
manifest. r214 full runner PID16684 starts22:24:24 and exits0 for both widths,
including27,261 retained checks each and GDI54/54. The final asserted r213
negative run (build15924 at22:26:53) has old-library probe25140 at22:26:59
exit1, and repaired probe39128 at22:27:02 exit0: border0 versus0x775325.
Owner physical acceptance of the new click behavior and full S4 closure remain
open; the r212 executable is supplied as a local trial, not a committed release.

### Control-state and system-button intermediate painting

Owner additionally reports each clicked control flashing and the whole frame
flashing on a system-button click. Source review finds two separate mechanisms:
system-button press/menu-entry/menu-exit calls still repaint the full frame;
BUTTON/STATIC/LISTBOX/COMBOBOX WM_PAINT fills and draws directly into the screen
DC. Native state processing can also draw before the retro repaint.

r215's first hidden-start probe is not a valid negative for native painting:
the window had not been explicitly shown on its private desktop. After fixing
that condition, build4140 at2026-10-12 22:37:19 and probe47936 at22:37:24 exit0.
The r214 baseline emits one native parent colour request for each BM_SETSTATE
(on/off), BM_SETCHECK, focus and focus-loss transition. The family expansion
(build12888 at22:40:20, probe19656 at22:40:26, exit0) additionally observes one
request for list selection, combo selection and combo focus. List focus alone
records0. This is a synchronous native-paint witness, not a claim that a zero
colour-request count proves the absence of every possible host drawing route.

The correction retains native state, capture, focus and notification handling,
using the existing nested WM_SETREDRAW bracket for styled buttons, combos and
lists with or without scrollbars. Application redraw-disable state is retained;
reentrant intermediate repaints are not presented. Native EDIT content and
bitmap statics retain their existing owners. The retro screen painter composes
only the dirty client rectangle in a transient compatible bitmap and presents
it once; it seeds excluded group interiors/combo children from the target.
Borrowed WM_PRINTCLIENT DCs are unchanged. Allocation failure falls back to the
existing painter, rather than suppressing application state changes. There is
no persistent cache, animation timer, global setting or product integration.

Caption-button changes now repaint only that button, using a transient surface.
Held-button motion repaints only when the inside/outside state changes. Ordinary
frame invalidation and activation remain full-frame operations. The system-menu
fixture observes menu entry/exit with the distant border witness preserved and
zero client paints/erases, complementing the normal File-menu case.

New control-presentation tests instrument the real painter's FillRect/BitBlt
calls during WM_PAINT, rather than only checking final WM_PRINT snapshots.
Six control variants each undergo20 measured repaints after20 warmup repaints:
zero screen fills, memory-only drawing and one screen presentation per paint.
Button state and down/up sequences additionally verify native-paint requests,
focus, capture/release and automatic checkbox changes; list/combo selection
and focus have equivalent witnesses. GDI is flushed before resource sampling.
The source is included in the full runner and input manifest.

Intermediate failures are retained: r216 runner24008 at22:33:42 exits1 because
the new probe itself stayed hidden; r217 corrects showing and then exposes a
first-use GDI40/40 steady-state boundary after warmup/flush (earlier33/34 is not
called a pass). r218 runner40000 at22:36:29 exits1 on the x64 menu fixture's
unflushed38/44 resource sample after x86's27,261 checks pass. r219 runner28400
at22:39:33 exits1 on the gallery's one-second system-menu Escape timeout; its
button presentation, capture and checkbox witnesses pass, but that does not
turn the full run into a pass. The gallery timeout is not relaxed or removed.
The r220 runner18396 at22:42:02 times out the x86 gallery after60s. A minimal
automatic-radio group reproduces the hang (r217 build42000 at22:45:54,
probe17868 at22:46:00, owned10s timeout). Bracketing the whole BM_CLICK/input
operation temporarily changes native visibility during radio-group traversal;
this is a candidate regression, not an accepted limitation.

The button bracket is therefore narrowed to native drawing leaves, excluding
BM_CLICK and mouse-button/key dispatch. Nested leaf updates defer retro
repaint until the outer control invocation, without hiding the outer input
operation. r217 build41696 at22:46:35 and probe40320 at22:46:40 exit0: both
radio selections return and group checks are correct; button, list and combo
native-paint witnesses remain0. The automatic-radio regression is retained in
the full suite. Frame-button temporary surfaces are seeded before painting so
an early painter failure cannot copy uninitialized pixels. r221 runner10596
starts22:47:23 and exits1: x86 passes27,261 checks, while the x64 menu fixture
again samples deferred cold GDI growth38/44. Flush alone was insufficient.
The menu test now explicitly performs eight full warmup cycles (all four
window/menu states, draining pending messages), then eight separately measured
cycles that each require non-growth. Cold observations remain logged. This is
not an exemption for growing measured cycles or a claimed object-by-object
classification of native caches.

r222 full runner33092 starts22:49:35 and exits0. Both widths pass27,261 retained
checks, font/scroll/DC gates, both menu-damage cases and the new presentation
suite; repeated-attachment GDI is54/54. Per-paint screen fill count is0 and
presentation count1 for all six variants. Button state/down/up, list/combo
selection and focus have0 observed native colour-paint requests; focus/capture,
checkbox toggles and automatic radio-group switching pass. Menu resource
measurement is stable after the complete warmup. r223 compiles the same menu
test against the old r214 x86 library: build5780 at22:50:25 exits0; probe19964
at22:50:30 exits1 at the system-menu border assertion, observing0 rather than
0x775325. Its measured resource cycles pass, so this negative isolates the old
full-frame system-button repaint rather than the warmup adjustment.

The r221/r214 gallery1 images both contain the existing offset duplicate
child-image capture artifact; it is not used to certify screen motion or
claimed as newly repaired here. No computer-use action resumes. Final source
manifest identity, diff and documentation checks accompany r222 delivery.
Owner physical acceptance and the remaining S4 scope are still open; no
production package, commit/push or S4 closure is claimed by this local trial.
r222 input-manifest recheck has0 mismatches; git diff --check passes.
Documentation governance PID18128 starts22:51:56 and exits0. Standalone demo
SHA-256 identities: x64
B55DCC72917F02B9B1215FF925BEF9A29E2ECA06801505E9E25994996D3866B1;
x86 1F0CC061B6ACB6D44375B420D69BD13A06CBF6B6F4DA9A9CD48C387298212D17.

### Follow-up: printed gallery duplication boundary

The next goal continuation audits the existing gallery1 duplicate/offset child
images rather than declaring the full S4 complete. Current worktree and r222
source are re-read; no runtime source changes in this investigation yet.
The r222 x86 direct `all-controls-message` image is visually clean, unlike
the PrintWindow-produced gallery atlas. Direct WM_PRINT already has an
independent client-offset assertion; clean print output alone does not prove
the physical desktop or identify why the alternate capture differs.

r224 builds a minimal same-thread parent/child trace against the sealed r222
x86 library: build46756 at2026-10-12 22:54:09, probe14572 at22:54:14, exit0.
Direct WM_PRINT flags0x1e reaches root WM_PRINTCLIENT at viewport4,29 and child
WM_PRINT/WM_PRINTCLIENT at44,79. PrintWindow emits none of those observed
messages in this run. The expanded build44536 at22:55:55 and probe5080 at
22:56:01 exit0 and confirm PrintWindow returnsTRUE. Its simple native image
does not reproduce the showcase duplication. Sequential reused-DC image
comparisons have unexpected limited painted regions and are not accepted as
a valid pixel oracle; isolate DC state and use fresh surfaces before drawing
conclusions. All probe outputs remain under build/M0-T445/S4/r224. This neither
changes the owner-facing r222 binary nor claims the capture issue repaired.

r225 removes the shared-surface confounder and records all four routes with
fresh DIBs: PrintWindow, direct WM_PRINT, window-DC copy after explicit redraw,
and PrintWindow after redraw. Build27624 at22:58:46 and probe32416 at22:58:51
exit0; all output DC clips remain0,0,500,300 and viewport0,0. The apparent
missing pixels in the viewer are then isolated to BI_RGB's unused high byte
being interpreted as alpha. Build19024 at22:59:35/probe13152 at22:59:41 normalize
that byte only for serialization, retaining RGB. The subsequent minimal probe
adds an explicit white class background and the gallery's DPI awareness
(build39480 at23:00:50, probe46464 at23:00:57, exit0). The simple parent/button
does not reproduce showcase duplication; its initial PrintWindow and direct
message images are visually consistent. This does not identify the complex
gallery failure or certify window-DC capture on an inactive desktop.

r226 extends only the existing capture-test route of the real gallery, linked
against the unchanged sealed r222 x64 demo object and style library. It keeps
direct WM_PRINT, native capture, explicit full redraw, window-DC copy and a
final diagnostic WS_CLIPCHILDREN variant as separate artifacts. It does not
change interactive gallery styles. Build34448 at23:02:15/probe48552 at23:02:22
exit0. Build32712 at23:03:49/probe19036 at23:03:55 additionally normalize the
serialized BI_RGB high byte after all RGB assertions; this correction remains
in the screenshot helper. The actual native capture still duplicates controls
after explicit full redraw, while direct WM_PRINT remains clean. Build1240
at23:05:23/probe36252 at23:05:29 (exit0) tests WS_CLIPCHILDREN; its capture and
window-DC copy are incomplete, not passing replacements. No runtime clipping
change is adopted. The root cause of native capture duplication remains open;
do not infer visible-desktop corruption from these inactive-desktop artifacts.

### Current source-scope checkpoint after r226

The old incremental README paragraphs are not a completion inventory. Re-read
`controls.c::supported`, `draw_list`, frame installation and the current packet
before S4 closure. Current implementation covers the six stock class families
through supported BUTTON/STATIC/EDIT/LISTBOX/COMBOBOX/SCROLLBAR variants; this
does not mean every style/variant has been implemented or visually certified.

| Boundary | Current evidence / remaining requirement |
| --- | --- |
| String list variants | Multicolumn drawing exists and retains native item geometry; the README's early exclusion is stale. LBS_USETABSTOPS is still explicitly rejected and draw_list has no tab-stop handling. This is a concrete stock-control gap, not an owner-draw exception. |
| Application drawing | Owner-drawn/no-data lists, owner-drawn combos/buttons and custom controls retain application ownership. Do not silently claim their application-defined drawing is automatically Win3.x. Their final scope disposition must be explicit. |
| Text and geometry | Shared System24 has measured CP1252 coverage, not universal Unicode or all DPI/state fidelity. Original frame/scrollbar shared-edge and accessibility limitations remain those recorded above, not erased by the flicker regression pass. |
| Screen versus capture | r222's owner-facing interactive binary is unchanged. Direct WM_PRINT versus native inactive-desktop capture remains a separately identified limitation; neither is a physical-input acceptance substitute. |
| Delivery | Full S4 final review, consolidated current documentation, scoped commit/push and owner acceptance remain outstanding. Unrelated dirty files remain preserved. |

Do not add tab rendering by simply replacing tabs with spaces: application-set
tab positions, horizontal scrolling, selected/disabled rows, font changes and
pre-attachment configuration must first have a supported state-ownership
contract. The next implementation audit should resolve that stock variant
against original sources rather than continuing cosmetic screenshot work alone.

r227 full runner3912 starts2026-10-12 23:06:35 and exits0. Both widths pass the
27,261 retained checks and54/54 attachment GDI row, plus the menu/control
presentation fixtures. Only capture-test observations and serialized image
high bytes changed since r222; no new owner-facing runtime repair is claimed.

### List tab-state source audit and native oracle

Read-only original references are OpenNT windows/core/ntuser/client:
lboxrare.c SHA256 E8914F4761ADE70A1A7357B5DC8C17744CE1BC49379D03A99470F815EF85F64D;
lboxctl2.c A99C9EF3B6A7A07B219DDDA18FF2FB6A8A11C42F5378E5F77E10E336AD7D1744;
lb1.c 73DD9DFFBF8C7184CC51771E34A4ACE046EC476DE4A5B9DB3EE50FCD08C7C1A7;
drawtext.c 014E568B4F4AE57D54B8AFC4502C890CD19EF1A698273561D9627B1B9C9EC10A.
LB_SETTABSTOPS dispatches to LBSetTabStops; explicit DLU positions convert at
set time with MultDiv(value,cxChar,4) into private PLBIV pixel positions.
Count0 removes that array. TabTextOut computes default eight-character stops
from the current font; one explicit value is a repeated pixel interval.
xxxLBSetFont updates font metrics but does not reconvert the existing array.
LBPrintCallback uses xStart0 for multicolumn text versus2 for single-column.

Recovery ladder: these original functions define the comparison contract, but
their PLBIV/private USER state, NtUser DC calls and server globals cannot be
directly linked at the already admitted USER stopping boundary. A small facade
cannot expose a pre-existing host list's private tab array through a public
getter; the audited setter dispatch has no matching tab-position getter.
No external intrusion/private-layout reading is admitted. No replacement tab
state implementation is added in this checkpoint. Changing original guests or
silently resetting pre-existing application tabs is not a valid solution.
Creation-time observation or native rendering must first demonstrate a safe
state-ownership boundary before widening current attachment support.

r228's initial diagnostic build31408 at2026-10-12 23:13:33 fails because `small`
collides with a Windows header macro, not a component defect. Renamed build46580
at23:14:05 and private-desktop probe39004 at23:14:10 exit0: explicit48-DLU stop
is96 pixels with System16; font24 alone retains96; explicitly resetting the
same DLU value produces144; reverting to default font24 tabs produces96.
This distinguishes default dynamic spacing from stored explicit pixels.

The retained tests/app/win16_style_list_tabs.c extends the native oracle with
default16 versus default24 and two explicit stops versus one repeated interval.
It is a native contract test, not a passing implementation test for currently
rejected LBS_USETABSTOPS. The full runner includes its source in the manifest.
r229 runner38972 starts23:16:53 and exits0, both widths passing27,261 retained
checks and54/54 attachment GDI. Native results are default16=64, old explicit48
DLU=96, retained after font24=96, reset under font24=144, default24=96; the third
glyph begins at288 for both explicit48/96 and repeated48. These are measured
current-host comparison results consistent with the original algorithm, not
retail Win3.1 execution or proof of universal host behavior.

### Multicolumn text-origin correction

The same original LBPrintCallback audit exposes a project-owned divergence:
draw_list adds2 to every item label, whereas the original selects0 for a
multicolumn list. r230 links the old r222 x64 library with the native oracle
and an identical styled list: build32736 at23:19:29 exits0; probe15160 at23:19:33
exits1 at the intended equality assertion, native first black pixel0 versus
styled2. Both use the same System24 font, text X and native item geometry.

The correction changes only the label inset predicate in controls.c. It keeps
the existing public-HWND rendering adapter, row/clipping/selection/scrolling
owners and single-column inset2. The original private PLBIV/USER unit still
cannot compose at the documented stopping boundary; no copied executable
code, original mirror diff, new provider or private host-layout access is
introduced. The already admitted project renderer now matches the original
0/2 choice. The retained list test checks both native/styled single-column2
and multicolumn0, separately from the tab-state oracle. r231 full runner14228
starts23:20:26 and exits0. Both widths pass27,261 retained checks,54/54
attachment GDI and the new native/styled text-origin assertions: single2/2,
multicolumn0/0. The tab-state oracle independently retains the same measured
64/96/96/144/96 and288/288 results. Documentation governance runner44900
starts23:22:13 and exits0; diff check passes. The source-manifest recheck has0
mismatches. This remains a bounded standalone delivery, not full tab-list
support, physical acceptance, production deployment or S4 closure.

### Native tab-paint reuse boundary

The r232 documentation-only review corrects stale component README statements
about multicolumn support, fixed-thumb frame bars and unified24-pixel font
ownership. Background governance PID12256 starts2026-10-12 23:29:54 and exits0;
diff check passes. No runtime artifact changes.

r233 asks whether public native WM_PRINTCLIENT can preserve opaque existing
tab state while directly supplying the retro palette. Its diagnostic parent
returns explicit text/background colors through WM_CTLCOLORLISTBOX. Native
ordinary, selected, disabled-selected and disabled rows are captured on an
owned private desktop, with and without a caller-owned per-control untheme.
Build PID8416 starts23:32:03 and exits0; probe20120 starts23:32:22 and exits0.
Both theme rows honor parent colors only for ordinary text/background.
Selected backgrounds/text use COLOR_HIGHLIGHT/HIGHLIGHTTEXT, and disabled
text uses COLOR_GRAYTEXT despite the parent color response. Selection, top
index and font are preserved. No system setting or runtime source changes.

This rejects direct native-image reuse as a complete custom-palette solution;
it does not prove native painting unusable through every possible adapter.
Reading opaque tab state, resetting unknown tabs or remapping arbitrary parent
pixels is not implemented. A proposed adapter still must preserve callbacks,
selection/focus/disabled semantics and the current palette without global state.

The reproducible tests/app/win16_style_list_tabs.c now retains the four-state
native color boundary, font/selection/top-index preservation and a final
unchanged-tab-position assertion. Existing native tab arithmetic and styled
single/multicolumn origin tests remain. The test is already selected by the
full standalone runner. Its source SHA256 is
999703387C9B59AEF48E39854DBDDA231A31C9E61828C4ADF054654A90E10D9C.
r234 focused builds use the sealed r231 width-matched library and unchanged
/MT /O2 /W4 /WX flags: x86 PID16640 starts23:33:34; x64 PID19880 starts23:33:40;
both exit0. Private-desktop tests38636/36532 start23:33:58/23:33:59 and exit0.
Both retain64/96/96/144/96,288/288 and2/2,0/0 origins, with252 text pixels per
three-glyph row across the four color states. This is a native oracle result,
not implemented styled-tab support. The owner-facing executable stays r231.
No full runtime rerun is claimed for this test-only extension; runtime inputs
are checked against r231 before reusing its unrelated gate results.

### Explicit known-state tab attachment

The next implementation keeps the existing source-backed public-HWND adapter
and exposes win16_style_tabbed_list_install in controls.h. The caller supplies
known current pixel positions, not dialog units. The array is copied; no
native setter is invoked at attachment or removal. Unknown existing state
still fails generic installation, rather than assuming default tabs. This is
a local standalone API, not an automatic Hook/creation integration promise.
It parallels EDIT's explicit consumed-border input and preserves the existing
scope's unsupported accounting.

The original LBSetTabStops/TabTextOut evidence above remains the tab semantics
owner. GdiGetCharDimensions additionally comes from read-only OpenNT
windows/core/ntgdi/client/cfont.c, SHA256
577A0E4CAF54E10F9BB7FCC30328BA4293D47EC73288D18CB481329F0789E625,
function at line1024. Its CFONT cache, DC_ATTR and server update dependencies
are the existing NTUSER/GDI stopping boundary, not composable imports. The
small binding uses public font metrics and the same52-letter average formula
for proportional fonts; LBSetTabStops' set-time conversion is retained with
wide checked arithmetic. No private state access, imported executable body,
new source root or mirror diff is added. This is the already-admitted local
renderer extended through its public-API seam, not a second native list owner.

The subclass allocates a candidate pixel array before forwarding ordinary
LB_SETTABSTOPS. Only native success commits it; failure retains the old copy.
Font changes never recompute explicit positions. TabbedTextOutW renders within
the existing native item clipping/origin and retro selection/disabled palette.
Native strings, range, selection, input and notifications retain their owners.
Teardown frees only the copied drawing state and restores the borrowed font.
Invalid count/pointer and unrepresentable conversion fail before native mutation.
Allocation injection and arbitrary callback ordering remain review subjects;
the tests below are not a claim that every failure path has been injected.

r235 full background runner26708 starts2026-10-12 23:38:26 and exits0. Both
widths pass27,261 retained gallery checks,54/54 GDI and the expanded tab test.
The new assertions reject unknown-state generic installation and null initial
array; verify copying the caller array, custom ordinary/selected/disabled
colors, successful one/multiple/default updates, failed update preserving prior
positions and unchanged native tabs after removal. Expected third-glyph
positions are288,240,360,192 as configured; native removal retains192.
This establishes known-state support, not discovery of arbitrary opaque tabs.

The candidate gallery next adds a fresh known-default tabbed list to sample01
through this same public API, then configures it with LB_SETTABSTOPS. Existing
automatic-control rows/counts remain intact; the explicit row is separately
reported. The original multicolumn sample is shortened to make space, not
removed. Its runtime/visual verification belongs to r236, not the r235 pass.

r236 runner36408 starts23:41:31 and exits1. The x86 full row passes27,261
gallery assertions with the explicit tabbed sample; its direct WM_PRINT image
shows both columns, selected row and scrollbar within the Lists group, with
no overlap. This is rendered private-desktop evidence, not physical input.
The x64 control-presentation fixture fails its resource equality at line78:
first push-button GDI count33 before versus40 after. Native intermediate-paint
and capture/focus assertions before that boundary pass. The run stops before
the remaining x64 gates; it is not a complete pass. r235 had passed the same
runtime implementation before the gallery-only addition. Causality of this
first-interaction resource change needs isolation against the previous fixture
and native control; neither warmup nor assertion removal is yet adopted.
Do not promote r236 as the new owner-tested executable. The previously
delivered r231 remains the owner-facing baseline while this gate is resolved.

### Control resource observation correction

r237 isolates process GDI counts per phase on private desktops: native-only,
sealed r231 style and r236 style. Builds43028/18592 start23:46:40/23:46:46 and
exit0. Probes25136/24708/47808 start23:47:05/23:47:05/23:47:06 and exit0.
Native-only totals rise9 to31 during twelve paint/focus/state/click cycles;
both old and current styled totals rise40 to46 then stabilize. Native-only
does not install the component. Thus process-wide first-use totals include
host allocations unrelated to tab rendering; these counts do not identify
each internal USER/theme object or prove a precise private-cache cause.

The fixture previously warmed only paint/focus and then included first button
state/click transitions in its measured total. r238 adds eight complete warm
interaction cycles with queue dispatch. Runner43956 starts23:48:30 and exits1:
the push button stabilizes46/46, but checkbox totals decrease46/44. r239 also
restores baseline parent focus and drains queued messages before final sampling.
Builds13616/10940 start23:50:03/23:50:11 and exit0. x86 probes2520/20960 pass;
6068 reproduces46/44 at23:50:38. This disproves the proposed focus-only fix.
Resource release is not a leak; equality of an asynchronously populated
whole-process total is the wrong sole painter-lifetime assertion.

The final test keeps warmed whole-process non-growth (after<=before) and adds
strict object-identity accounting for the real controls.c painter's compatible
DCs/bitmaps, raw bitmaps and solid/pattern brushes. Successful deletion removes
only its exact recorded handle; a full tracking array is a failure. Every
measured repaint and the interaction boundary require zero live owned objects.
This prevents host resource decreases from masking a painter leak. It does not
claim to instrument GDI allocations in other linked translation units; the
retained font and attachment-lifetime gates cover their own contracts.
Native paint/capture/focus/checkbox/radio and one-presentation assertions remain.

r240 focused builds47524/6776 start23:52:05/23:52:12 and exit0. x86 tests
7040/13152/32284 start23:52:53/23:52:56/23:52:59; x64 tests7020/12524/30692
start23:53:02/23:53:05/23:53:08. All six exit0; each of the six control forms
reports zero owned outstanding objects and no tracking overflow. Warmup logs
retain first-use changes instead of suppressing their evidence.

A formal --leak-negative case allocates and deliberately retains one bitmap
during the measured present. The full runner requires its injection marker,
the resource assertion failure and exit1, independently for both widths.
The negative lives only in the fixture, never the component/demo. A failed
allocation cannot masquerade as successful leak detection. Runtime source
and sample01 remain the r236 candidate; only verification changes in this round.

r241 full runner39236 starts2026-10-12 23:53:44 and exits0. Both widths pass
27,261 retained gallery checks and54/54 attachment GDI, all selected font,
menu, scroll/API/geometry/DC gates, known-state tabs and control presentation.
Both intentional single-bitmap leaks produce the injection marker and fail the
per-paint owned-object assertion at line111 with exit1, as required. Positive
runs retain zero painter-owned handles and no process-total growth after
warmup. These are narrowly defined lifecycle observations, not proof of every
possible host-cache cause or every API allocation-failure path.

All recorded r241 source inputs rehash with0 mismatches. The viewed x64 direct
WM_PRINT showcase image has the new Item/Value tabbed row inside the Lists
group without overlapping other families. Native PrintWindow's separately
recorded private-desktop artifact remains unresolved. This new runnable demo
supersedes r231 for owner side-testing, not S4 closure or production deployment.
x64 demo SHA256:7EC3BBCAF57E4630F0B532D066998899817564734620C8CA412FB09AFE655E4E.
x86 demo SHA256:71C7C7F7139A362AFECF372799E8FD3B4885448C9AF8CF08EC7839786695CA41.

### Tab font-transition and sample inventory coverage

r242 extends the formal list-tabs test with single/multicolumn attachment
transactions starting from the actual16px host System font and existing48-DLU
tabs. Native same-font observations are taken before attachment, then the old
font is restored before invoking the component. Both widths prove initial96,
attached96 and removed96; the original borrowed font handle is restored.
Setting48 DLU while attached uses the24px font and produces144, which remains
144 after removal to the16px font. The second multicolumn column's tabbed
text starts at256 in both the native same-font oracle and styled rendering.
Thus explicit initial state is not accidentally reinterpreted as dialog units
or recalculated on font changes. Tests use existing r241 libraries; no runtime
code change is needed for these vectors.

Focused builds34532/41908 start2026-10-12 23:59:16/23:59:23 and exit0; probes
31396/39820 start23:59:38/23:59:39 and exit0. Formal test source SHA256 is
4A49842CB86E8781DD6B910C8D58F6116F78A91398262EC0BA108F80191E1479.
The sample inventory also exposes existing BS_LEFTTEXT rendering in sample01's
Unchecked and Radio B controls. It changes only demo style flags; native check
state and radio grouping still own interaction. Full candidate verification
is r243; these focused results alone do not certify its complete gallery.

r243 full runner41396 starts2026-10-13 00:00:49 and exits0. Both widths pass
27,261 gallery checks,54/54 attachment GDI, expanded tab transitions and all
selected presentation/font/scroll/DC gates including the intentional leak
negative. The input manifest recheck has0 mismatches. The viewed x86 direct
WM_PRINT image shows Unchecked and Radio B with their glyphs on the right,
without text collision; it also retains the Item/Value tab row. This is
rendered-pixel evidence, not physical input acceptance. No component behavior
changed since r241; only gallery exposure and test coverage changed. r243 is
the new optional standalone side-test build, not a production release or S4
closure. Remaining scope review and owner acceptance are still required.

### S4 closure audit: application keyboard boundary

The current requirement table in the working plan explicitly includes button
keyboard activation, radio group navigation and EDIT keyboard/clipboard use.
The gallery's interactive loop previously used only TranslateMessage and
DispatchMessage. Its tests directly sent control messages, so their passes did
not prove dialog-style Tab/group navigation through the actual application
loop. Sample01 also marked every child WS_TABSTOP, including STATIC and group
boxes. These are authored demo configuration/dispatch defects, not a reason
to intercept keyboard navigation inside the style library.

The candidate adds a demo-local gallery_message path: native IsDialogMessage
for the selector and authored control pages; native TranslateMDISysAccel for
the MDI client. Other frames retain ordinary dispatch. Authored control pages
retain F2/F3/Escape demo shortcuts after native dialog handling; the showcase
and selector report their actual default-button IDs. Only interactive
showcase children are tab stops, group boxes establish group boundaries, and
the selector list participates in Tab navigation. No process/global hook,
product component or renderer input owner changes.

The new private-desktop test invokes the same gallery_message function used
by the interactive loop. It checks Tab from push to default button then past
the disabled button to the checkbox, Space toggling, and Right-key radio
selection/exclusion; labels and group boxes must lack WS_TABSTOP. It saves
and restores thread-local keyboard state and the showcase's check states.
This is queued/message-contract verification, not a physical keyboard claim.
Full verification is r244; a source review alone cannot close these rows.

Current requirement audit (not a reduced S4 scope):

| Requirement | Current evidence or concrete gap |
| --- | --- |
| S3 frames, system-box behavior, native-button suppression | Retained gallery/dispatch tests and owner-closed S3; native-menu session failure remains owner-deferred S7. |
| System/ordinary menus, original glyphs, unified text | Component/menu tests and viewed captures; physical mouse entry/font acceptance remains distinct from keyboard/print evidence. |
| MDI ownership and merged controls | Retained callback/navigation tests; newly wired interactive accelerator path still needs a focused key-path assertion. |
| BUTTON/check/radio keyboard | Previous API-message tests were insufficient for gallery navigation; candidate above closes a specific application-loop gap. Default Enter and reverse-Tab still need explicit vectors. |
| STATIC/images/boxes | Retained source-backed text/icon/bitmap vectors; arbitrary application owner-draw remains application-owned, not claimed reskinned. |
| EDIT input, selection, undo, clipboard | Replace/undo/selection tested; no WM_COPY/WM_CUT/WM_PASTE test was found. Add a real clipboard test in a private window station, not the user's clipboard. |
| LISTBOX/COMBOBOX | Existing selection/scroll/capture tests plus r243 known-state Tab transitions; opaque initial tabs require truthful explicit input. |
| Scrollbar geometry/tracking | Opt-in demo and frame bindings have focused geometry/input/callback tests. Native accessibility geometry, wide EDIT fallback and arbitrary external API binding remain explicit limits, not universal compatibility passes. |
| Typography | Shared actual24px host bitmap font and CP1252 raster tests; wider Unicode/non-ANSI behavior is not certified. |
| Delivery/acceptance | Current standalone candidate r243; final changed-source review, coherent S4 scope disposition, commit/push and owner acceptance are not yet complete. |

The clipboard and untested interactive-key rows are actionable next work;
do not replace them by another summary of the27,261 retained checks.

r244 full runner38508 starts2026-10-13 00:08:12 and exits0. Both widths pass
the new SHOWCASE KEYBOARD assertion,27,261 retained gallery checks,54/54 GDI,
known-state Tab transitions and all selected font/menu/control/scroll/DC gates
including the leak negative. Its source manifest recheck has0 mismatches.
This supersedes r243 as the standalone keyboard-capable side-test candidate,
not as whole-S4 closure. The remaining audit rows above stay open.

### Clipboard fixture isolation boundary

r245 adds tests/app/win16_style_edit_clipboard.c: native/styled single-line
and multiline EDIT copy, cut, paste, undo, read-only refusal and font restoration.
Before any clipboard operation it creates a uniquely named private window
station, binds the process, verifies its name and creates a private desktop.
Failure at this isolation boundary is a failure, never a clipboard-test pass;
there is no fallback to the interactive user's clipboard.

Initial compilation rejected an unavailable DESKTOP_ALL_ACCESS spelling;
GENERIC_ALL fixes that desktop creation argument. Background builds24000 and
15508 (2026-10-13 00:19:04/11 local) exit0 for x86/x64 against the unchanged
r244 archives. Sandboxed tests10924/47448 and elevated tests29124/35696 all
exit1 at CreateWindowStationW with ERROR_ACCESS_DENIED. Reducing station rights
to CREATEDESKTOP, ACCESSCLIPBOARD, ACCESSGLOBALATOMS and READATTRIBUTES does not
change the result: builds16672/31716 exit0; elevated tests42660/26328 at00:20:25
exit1 at the same boundary. No clipboard operation or EDIT assertion ran.

This fixture is retained but is not wired into the passing full runner until
the execution environment supports its isolation precondition. Clipboard
coverage remains explicitly unverified. No renderer or owner-facing demo
changed; r244 remains the offered executable. Continue with the independent
default-Enter, reverse-Tab and MDI accelerator gaps rather than retrying this
unchanged access denial or touching the owner's clipboard.

### Actual gallery keyboard-route verification

The next fixture revision extends test_showcase_keyboard through the same
gallery_message path as the interactive demo. Shift+Tab must return from the
checkbox to the default button, skipping the disabled button. Enter with the
checkbox focused must deliver exactly one BN_CLICKED for the actual default
button HWND/id1202; a temporary observer forwards every message and is removed
before returning. Existing Space and radio-arrow assertions are retained.
An MDI keyboard fixture adds one owned styled child, checks Ctrl+F6 activation,
then Ctrl+F4 destroying only that child and restoring the original active child.
Thread keyboard state is restored; no physical input or global hook is used.

r246 runner1680 (2026-10-13 00:22:37 local) used Windows PowerShell rather than
the previous pwsh host and stopped at the intentional-leak negative gate. Its
stdout already contains INJECTED PAINTER LEAK and stderr CONTROL PRESENT FAIL;
the driver rejection does not show a missed leak or a keyboard regression.
Independent negative probe25548 at00:23:35 returns the expected exit1.
The exact driver rejection cause is not proven; no assertion was removed.
r247 runner42364 at00:23:53 uses the same pwsh host as the preceding successful
full runs. This run, not the failed r246, supplies the new keyboard evidence.

r247 exits0. Both widths pass SHOWCASE KEYBOARD and MDI KEYBOARD,27,261
retained gallery checks,54/54 attachment GDI and the selected font/menu/control/
scroll/DC gates including intentional-leak detection. Input-manifest recheck
has0 mismatches. x64 demo SHA256 is
7A228C2913AC93B711B5B7EB534075FD307654496BEF7945F867F461F69B16D7.
The renderer is unchanged from r244; this is new message-path evidence, not
another claimed visual fix. Clipboard isolation and final scope/source review,
commit/push and physical acceptance remain open. The keyboard gaps identified
above (default Enter, reverse Tab and MDI accelerator routing) now have direct
fixture evidence rather than only presence of API calls in source.

### Installation callback lifetime review

The control installer sends WM_SETFONT synchronously after attaching. Existing
message callbacks already retain control_style through nested removal, but
the installer itself did not. Application callbacks can remove the style or
destroy the HWND during that send; the old installer still returned TRUE.
Combo child installation also crosses synchronous callbacks before assigning
its owner. This is a local lifetime/return-contract defect, not evidence of a
new physical click-flicker cause.

New tests/app/win16_style_control_lifetime.c uses an application subclass that
removes/destroys the control attachment during WM_SETFONT. Against unchanged
r247 x64 library, r248 build44180 (2026-10-13 00:28:47 local) exits0; private
desktop probe47788 at00:29:09 exits1: removal occurs but installed=1. This
negative establishes the false-success regression independently of the fix.

The installer now retains its existing calls counter across child attachment
and font delivery. It checks detached before assigning combo ownership or
invalidating the HWND, releases the retained attempt on every post-attachment
path, and reports cancellation instead of success after callback removal.
The fixture verifies removal leaves a live unstyled HWND that can be installed
again, and destruction leaves no HWND; both must return FALSE. It is included
in the full both-width graph, execution gates and source manifest. No guest,
mirror, global input, product integration or new lifetime framework is added.
r249 full runner37808 begins00:30:23; final result is recorded separately below.

r249 exits0. Both widths report the expected installed=0 for callback removal
and destruction and pass the reinstallation assertion. Both retain27,261
gallery checks,54/54 attachment GDI and all selected font/menu/control/scroll/DC
and keyboard gates. Governance22432 at00:31:45 exits0; diff whitespace checks
pass. This closes this specific installation-lifetime defect, not the remaining
clipboard, wider source review or owner physical-acceptance requirements.

### Nested installation before subclass attachment

The same entrypoint review finds one earlier callback boundary: WM_GETFONT
occurs before SetWindowSubclass. An application may install the component
inside that query. The outer attempt must not overwrite the inner attachment's
subclass data, lose its allocation/font reference or claim a second success.

The retained lifetime fixture now performs exactly that guarded nested call.
r250 build30424 (2026-10-13 00:33:57 local) links unchanged r249 x64 library and
exits0; probe18580 at00:34:19 exits1 with inner=1 and outer=1. Earlier removal
and destruction cases pass in the same negative control. The repair repeats
the existing attachment check immediately after the synchronous font query,
disposes only the uninstalled outer allocation, and returns ALREADY_EXISTS.
No new state, hook or general-purpose reentrancy layer is introduced.
r251 full runner38732 begins00:34:46 with both-width regression coverage.

r251 exits0. Both architectures report inner=1/outer=0 and pass the earlier
removal/destruction/reinstallation cases,27,261 retained gallery checks,
54/54 attachment GDI and selected font/menu/control/scroll/DC/keyboard gates.
Governance12632 at00:35:47 exits0 and diff whitespace checks pass. Review of
control_scope.c confirms that callback-held scope references survive removal
and root destruction disables subsequent matching; no change is made there.
The remaining clipboard isolation/physical acceptance and final delivery
review requirements remain open; this result is not whole-S4 completion.

### Original Win3.1 size-box contract investigation

Rechecked O:/w31/SYSTEM/USER.EXE against the pinned hash above. Retained
r097-user18 and S3/r029 segment evidence have the same identity. No executable
is modified or imported. USER18:085D is the scrollbar procedure:086C..087D
extracts style0x08 into the size-box flag. The paint branch at0A95 chooses
normal scrollbar drawing or0AAD..0AC7, which temporarily sets the size-box
window flag and calls18:0208 with zero frame offsets. It restores that flag
after drawing. Thus this is a real original Win3.1 variant, not a Win95 grip.

18:0208 derives the rectangle from window extent, scrollbar metrics0608/0616
and border offsets. It selects brush[052C]; relocation0235 is GDI ordinal150
(UnrealizeObject),0241 is ordinal45 (SelectObject), and02C0 is ordinal29
(PatBlt). The corresponding original gdi.def names those ordinals. The raster
operation is00F00021 (PATCOPY). The helper uses no diagonal bitmap. The precise
insets must follow this helper rather than a whole-client-fill assumption.

For size boxes,18:0B57 selectsF030 (maximize);0CCD selectsF000 (size).
0CD0..0CF5 converts the point, targets the parent and sends WM_SYSCOMMAND with
the low direction value8. This supports retaining native size-box input and
parent command handling, rather than applying the ordinary scrollbar tracker.
Modern OpenNT sbctl.c retains the separate size-box branch, while its SIZEGRIP
and later DrawSize artwork are not the visual authority.

Color distinction: USER3:0760 begins the brush table at052C and3:0763 begins
the matching color table at0590; the loop increments brushes by2 and colors
by4. Size-box drawing uses the first brush, not an inferred button-face brush.
The raw initial data values at0590 and05CC are zero in the pinned image;
runtime initialization/configuration determines them. Do not treat those
uninitialized bytes as a black-color default. The candidate needs the same
scrollbar-color/pattern contract as the original, with no invented Win95 bevel.
Implementation and native-versus-styled command/geometry assertions remain
next work. This research does not yet add SBS_SIZEBOX support or certify its
runtime color/rectangle pixels.

### Size-box standalone implementation and native oracle

r253 build39332 at2026-10-13 00:48:37 local exits0; native probe3244 at00:48:57
exits0 on its private desktop. Three forms (unaligned, top-left and bottom-right
aligned) deliver SC_SIZE|8 once on button down and SC_MAXIMIZE on double-click.
The parent records commands instead of entering an interactive sizing loop.
Native unaligned60x60 paints only43,43..60,60; aligned forms become17x17 and
paint their whole area. Host pixels are button-face F0F0F0, while host
COLOR_SCROLLBAR is C8C8C8. These are host observations, not original VGA colors.

The candidate accepts SBS_SIZEBOX but still rejects the later SBS_SIZEGRIP.
Its separate painting branch uses original18:0208's inner-border offset,
client clipping and scrollbar system-color brush. It reads the current public
scrollbar/border metrics and retains native creation/alignment dimensions;
there is no silent resize to historical VGA metrics. It does not enter the
normal scrollbar tracker or create a caret/timer. Input and parent commands
delegate unchanged. Screen painting uses the existing transient composition;
WM_PRINTCLIENT restores the borrowed DC. No global color/theme is changed.

Sample01 adds a labelled size box without a Tab stop or meaningless scrollbar
range initialization. tests/app/win16_style_sizebox.c compares the three native
and styled forms, checks exact ink bounds, every painted/sentinel pixel and
the parent command routes. The full runner builds/runs both modes for each
width and includes the fixture in its source manifest. The test does not
claim that synchronous input messages prove physical dragging acceptance.
r254 full runner37048 starts00:51:04 with a300s deadline.

r254 exits0. Both widths pass all three native/styled size-box forms and
retained27,261 gallery checks,54/54 attachment GDI, keyboard/lifetime and
font/menu/control/scroll/DC gates. Source-manifest recheck has0 mismatches.
Governance35520 at00:52:43 exits0; diff whitespace checks pass. Reviewed x86
direct WM_PRINT all-controls-message capture shows the labelled flat size box
within the existing scrollbar group without overlap. Native PrintWindow's
separate capture limitation remains; no physical-drag acceptance is inferred.
The original size-box implementation gap now has a bounded implementation and
message/pixel evidence, while clipboard and final delivery/owner acceptance
remain open.

### Scroll composition failure review

The next sequential review reads controls.c's static/button/list/combo painters
and scrollbar drawing/tracking helpers. It finds an uncovered failure boundary:
draw_scroll_pixels can return before painting when SaveDC fails, but its caller
previously copied the newly allocated, uninitialized bitmap anyway. Likewise,
failure to allocate the checker brush left part of that buffer undefined.
This is a source-proven exceptional-path defect, not evidence that ordinary
owner-observed clicking fails SaveDC or allocates unsuccessfully.

The painter now reports whether it reached a completed drawing path; these
early failures do not publish the temporary image. Bitmap selection is also
checked, with the existing direct-paint fallback and balanced cleanup. This
does not introduce persistent buffers, timers, global settings or production
integration. It does not claim exhaustive protection against every GDI API
failure after painting has begun.

The real-controls presentation fixture injects one SaveDC failure and one
CreatePatternBrush failure into scrollbar composition. Each must preserve all
2,040 sentinel target pixels, perform no presentation and leave no tracked
painter allocation. A following normal draw must publish once. These checks
extend the previously frame-only DC-failure coverage. The full both-width
r255 runner35436 starts2026-10-13 01:00:22 local with a300s deadline; its result
must be recorded separately, not inferred from this review.

r255 exits0. Both widths log SCROLL FAILURE SaveDC=1 pattern=1,
unchangedPixels=4080, normalPresent=1 and objects=0. Retained full gates pass,
including27,261 gallery checks per width and54/54 attachment GDI. Input-manifest
rehash finds0 mismatches. Governance31392 starts01:02:19 local and exits0.
The x64 demo SHA256 is
2102F71F134AF961908C5A434D8FB5EAD674A07D8F28F965B0EB36FC3973DC15.
This candidate supersedes r254 for the exceptional-paint repair only; normal
physical interaction still requires owner acceptance. Clipboard isolation and
the final scoped commit/push remain open, so S4 is not closed.

### Owner black-background and inert-window-scroll repair

Owner supplies an r255 screenshot with black group interiors and reports the
window bars do not move content. Source confirms two separate omissions:
group WM_PAINT copied existing target pixels while leaving the interior
untouched, conflicting with the demo parent's WS_CLIPCHILDREN exclusion;
the demo's window-scroll handler updated only nPos. The old full-window print
fixture prefilled white and therefore did not establish fresh-screen painting.

controls.c now composes the parent WM_PRINTCLIENT background for group screen
painting, with a defined light fallback. It excludes visible sibling rectangles
from the final target clip, so filling the group cannot erase controls enclosed
by it. Borrowed WM_PRINTCLIENT drawing remains transparent. Saved DCs and the
attachment's existing callback lifetime guard cover the new parent callback.
The control-present fixture starts from black, checks a non-white parent
background is restored, and checks an overlapping sibling's pixels remain
untouched; its existing presentation/resource assertions are retained.

The demo owns actual content scrolling: both axes move every direct child,
including currently offscreen children, without intermediate per-child paints;
then invalidate the newly exposed background/children. Extents derive from
content coordinates, page size from the viewport, and resizing clamps position.
Arrow steps, page/end, live thumb position and wheel commands move content.
Interactive sample01 starts within the monitor work area. The renderer does
not acquire application-content ownership. The generic frame32-bit tests now
run on their dedicated SCROLL fixture, rather than corrupting showcase content
coordinates by installing synthetic200000 ranges. Their assertions remain.

New --viewport-test runs the real showcase in a620x380 window, checks actual
child rectangles on both axes, thumb drag/capture release, end clamping, wheel,
resize restoration, and a newly exposed group's live WindowDC background.
It retains a WindowDC capture rather than repainting through WM_PRINT first.
The print-observer assertion is restricted to its requested capture DC; a
nested parent-background WM_PRINTCLIENT legitimately has a different origin.

r257 full runner18744 starts2026-10-13 07:31:23, deadline300s, exit1 at the
scroll fixture's offscreen arrow pixel comparison. Unchanged r255 control
46556 at07:33:08 also exits1 at the same comparison:400/578 mismatches at
x1108..1134. Current monitor work area is640x408 at DPI144, as recorded by the
independent full-test diagnostic21612 at07:34:02. Do not claim the broad suite
passes in this environment. That diagnostic also exposed the overbroad print
observer, corrected as described above. No global resolution/DPI change occurs.

r258 focused runner17248 starts07:36:33, deadline180s, exits0 for both widths.
Its real WindowDC capture is inspected: the scrolled Labels group is white,
labels retain their own backgrounds and the former black bands are absent.
r259 runner47344 starts07:38:12, deadline180s, exits0 for both widths with
the additional real thumb/child-motion assertions. Command:
verify-win16-style.ps1 -ViewportOnly -BuildRoot build/M0-T445/S4/r259.
This explicitly labelled focused mode builds the same complete demo/library
and runs viewport plus control-presentation tests; it does not claim omitted
full-gallery/scroll gates passed. Source-manifest rehash has0 mismatches.
x64 demo SHA256:
7AA455445932B640170AD800D9BD133D29E3FF2D6D28009B2C59CDDD8CD36B31.
The standalone candidate is r259. Physical owner acceptance and the retained
environment-dependent broader regression remain open; no product deployment.

### Complete regression follow-up after display recovery

r260 reuses r259 executables without rebuilding and verifies their input
manifest before testing. Runner40464 starts2026-10-13 07:43:48, deadline300s,
exit1. Both widths pass lifetime, native/styled size boxes, normal/logical96
font, menu dispatch, intentional-leak negative, tabs, scroll geometry/API,
scroll integration and DC failure cases. The monitor work area is now
1920x1008/DPI144; the previous offscreen arrow comparisons pass unchanged.
Both gallery cases instead fail the new viewport nPage==client.width assertion.
USER clamps the page size to the total range when the viewport exceeds the
content extent. The assertion is corrected to min(viewport,range length),
and additionally requires position0 when all content fits. No runtime branch
or old regression assertion is removed. Runner42052 starts07:45:24, deadline300s,
for a fresh full both-width r261 run; its final result is recorded below.

r261 exits0 in full mode. Both x86/x64 pass the viewport/background fixtures,
all retained scroll/font/menu/control/lifetime/DC gates and27,261 gallery
checks each; attachment GDI returns54/54. Source-manifest rehash finds0
mismatches. x64 demo SHA256:
EB97CDF25C9D637E4C94C97C05A33BF488CBE0041CFE3C16374F707B96A5BE7E.
This supersedes the earlier r259 focused-only verification limitation, without
erasing its failed small-display observations. Owner physical acceptance of
the latest black-background/content-scrolling repair remains pending. Clipboard
smoke limitations remain supporting evidence rather than a mandatory closure
gate, as clarified after the owner's question.
