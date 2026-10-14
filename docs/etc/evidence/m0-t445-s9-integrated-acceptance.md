# T445 S9 integrated style acceptance

## Admission

Owner confirms S8 activation in run16 --win16-style winmine and admits S9:
uniform20px title/menu/control System bitmap type replaces24px; investigate
native menu-item hover leakage and the About/winver appearance. Baseline is
466d816bc and the thirteen-image release manifest. Single-person implementer
then reviewer; probes/private desktops preferred, background builds/tests,
coherent publication and commit/push before claiming delivery.

No guest or original mirror change, global font/theme setting, new public
option or unrelated WOW recovery. Existing source policy/resources retained.
Host About content is not to be falsified as Windows3.1; determine actual
producer/task scope before deciding its presentation binding.

## Investigation

Original src/mvdm/wow32/wshell.c WS32ShellAbout marshals the guest call to host
ShellAbout. This is source evidence only until the actual WINMINE invocation
and HWND ownership are observed. The S8 WOW scope covers registered creation,
dialogs and MessageBox; ShellAbout does not yet have that scope. Native menu
hover suppression exists in NTW16S; its actual WOW message path must be traced,
not presumed equivalent to the standalone demo.

## Implementation and evidence so far

All paths below are relative to build/M0-T445/S9. No guest or mirror file is
changed. The production changes are the NTW16S shared20px System bitmap font,
custom-menu hit-test ownership, dialog initialization attachment timing and
WOW32's finite ShellAbout binding. No new public argument or global setting.

- r004: retained S8 package, actual pointer motion on an activated private
  desktop. window-3.bmp reproduces the small native Game overlay, unlike
  messages-only hover. About belongs to the same NTVDM process and has no
  style marker. Its original guest caller reaches WS32ShellAbout.
- r005/r006: first candidate builds and actual pointer hover no longer shows
  that overlay. About is styled but its initialization formats an already
  marked caption, producing a duplicate marker. This is not final acceptance.
- r008: rebuilt candidate delays standard-dialog attachment until completion
  of WM_INITDIALOG. WOW32 binds only wshell.c's ShellAbout call, retaining the
  current Win16 task policy and the host's actual version information.
- r009: final-candidate physical test cannot switch the desktop. It fails
  before pointer input and is not a hover pass. Previous physical evidence
  does not establish final-candidate physical acceptance.
- r018: real WINMINE with style off/on/all passes messages-only menu traffic,
  task-owned About policy, exactly one title marker, and About dismissal
  while its main window remains alive. PrintWindow captures retained. This
  does not substitute for r009's missing physical test.
- r002/r007/r010 retain failed independent regressions whose oracles still
  assumed24px tab spacing, corner-band growth or title vertical centering.
  Updated oracles use the requested20px geometry; reruns remain required.

The custom menu advertises neutral HTBORDER instead of HTMENU, preventing
USER's native hot tracking from owning the same pixels. Actual clicks in the
measured menu rectangle still reach the existing menu command path. The
menu-dispatch regression now queries the hit test before clicking and retains
its popup, no-client-repaint and untouched-frame assertions. Existing HMENU,
keyboard and command handling remain in place; no repaint timer is added.

Background execution: admission PID45060 (19:30:10,180s) passed; r008 pipeline
PID from final.out was not retained in that empty stream, with logs showing
19:46:08--19:46:46 and r009 failure. Resumed PID18760 (19:50:38,3600s) passed
r018 then failed r010's old24px title oracle. PID19080 (19:53:13,3600s) runs
the corrected independent and product suites serially. All times local on
2026-10-13. Candidate remains unpublished; S8 is still the deployed baseline.

r019 passes x86's27261 main checks, then x64's control-presentation total-GDI
assertion fails44->46 with painter-owned objects0. No assertion was relaxed.
r020, resumed by PID23544 at19:55:45 with3600s pipeline deadline, passes the
complete independent x86/x64 suite, including the injected-leak negative.
Both main HWND suites pass27261 checks; font tests compare224 glyphs and39100
pixels exactly with realized height20, including96DPI and EDIT selection.
The non-reproduced total-GDI fluctuation remains recorded, not erased by the
passing rerun and not claimed to be a diagnosed product leak.

r011 passes both-width real Hook creation/descendant policies, A/W windows,
modal/modeless dialogs, dynamic titles, later GUI threads and six MessageBox
entrypoints. r012 exercises the exact r008 DLLs: nested Off, construction and
later controls, theme reentry/restoration, explicit detach/unload, failure and
30 lifetimes pass with GDI3/3. r013 verifies exact candidate DLL/demo ABI and
load boundaries. r014 passes18 real WOW shared/separate relay variants across
both native widths; r015 passes five shared-task/child policy variants with
captures. These are integration outcomes, not SOL/WRITE functional acceptance.

## Current delivery gate

The PID23544 pipeline completes successfully: final.exit=0. r016 Product
passes wow-frontiers, Console-17 and Window-17 (165291ms overall); r017 Control
passes every selected gate (211330ms), including real EDIT/CMD/DOS input,
immediate relaunch, independent Console survival and worker/frontend loss.
These retain the S8 application frontiers, not new SOL/WRITE functionality.
Review governance PID38436 (20:03:50,180s deadline) passes; git diff --check
passes. Production ownership remains NTW16S and the finite WOW32 binding;
there are no guest/mirror changes and no new shared ABI or public CLI option.

Final physical retry r021, PID47380 (20:15:38,240s deadline), again fails at
OpenInputDesktop/SwitchDesktop before real pointer input. The test-created
WINMINE has the correct candidate DLL and marker. The missing interactive
desktop prerequisite is not diagnosed as a product failure, nor waived.
Physical input acceptance remains pending even though messages-only and
independent/integration/product suites pass. No further retries are useful
without a desktop-state change. The publication script is prepared but has
not run; it requires physical.exit=0 and all package gates, backs up the
coherent13-image set, validates hashes, and restores on failure.

## Desktop recovery and publication

Owner restored desktop availability. r023 physical retry PID39440
(20:39:42,240s deadline) passes on the exact r008 candidate: real pointer
hover compares44556 nonclient bytes with changed pixels0; no native menu item
appears. About has exactly one marker and closes while WINMINE remains alive.
This supersedes the missing-desktop gate above, without discarding failures.

First publication r022 PID32392 (20:40:27,300s) encounters a locked unchanged
nthook64.dll. Rollback also reports that lock, but a complete backup/hash
comparison proves every destination restored to its previous bytes. The
publication runner now skips byte-identical copies/restores while still
requiring complete13-image verification. PID30224 stops before changes on a
transient deployed process; a subsequent read confirms no such process.
r024 PID37624 (20:41:43,300s) publishes successfully to assets/release and
O:/winnt/system32, including the release manifest, recoverable backups and
three-way SHA-256 equality. Post-publication DLL/demo boundary tests pass.
No unrelated process was terminated to release the unchanged Hook DLL.

Sequential review confirms minimal project-owned changes, unchanged original
mirrors/guest and no new CLI/wire ABI. Tests cover the changed font oracle,
actual menu click ownership, physical hover, task-owned ShellAbout, both-width
creation/lifetime and the unchanged product baseline. S9 P1 carries this
verified release and evidence; owner visual acceptance remains pending.

Fixed host About client layout can still clip enlarged
text; S9 does not authorize an application-layout engine or falsified version
content. No claim that SOL/WRITE's known startup errors are repaired.

## P2 owner-requested demo proportions

Owner accepts P1 then requests one additional commit: restore the smaller20px
demo appearance before T closure. P1 is266067264. Only W16Style's authored
geometry changes:16px grid multiplication3/2 becomes5/4, associated list
columns/extents, viewport padding/line step and default frame size follow it.
The extra tabbed list uses original grid coordinates rather than24px-derived
absolute values. Both the overview and focused-control pages are included.
No renderer, Hook, WOW, guest or arbitrary-application layout changes.

r025 builds both demo widths using the dependency cache and snapshots a new
package from r008; every other release image is hash-identical to P1. r026
checks the exact release candidate; r027 checks both-width standalone demos.
Both pass27 pages, real scrolling, normal exit and DLL boundaries. The
regression now requires page01's push button145x50, tabbed list310x45 and
20px content movement. It captures the actual shipped demo by PrintWindow;
visual inspection confirms the reduced three-column layout. This is an
off-screen layout capture, not new physical-pointer evidence. P1's physical
hover and font proofs apply to unchanged DLL inputs.

Background PID49148 (20:56:13,1800s deadline) reaches the product gate but is
blocked by an existing unowned deployed broker; no product failure is inferred.
After authorized closure of exact deployed project processes21160/28444,
PID43744 (20:58:33,1200s) resumes serial Product/Control gates with new logs
r030/r031. r030 Product passes all gates (161432ms), but stage cleanup fails
on a retained nthook64.dll handle; Control is not reached. This is not a
successful complete delivery. Preserve that run, not a silent retry/pass.

### Owner-retired caption witness

Before P2 delivery the owner additionally requests removal of every generated
`(Win16 Style) ` prefix. This supersedes the earlier marker requirement, not
the two style options or propagation policy. The sole automatic caller is
NTW16S attachment. Remove its title-state/calls and the now-unused151-line
module/header and diagnostic exports, rather than changing the prefix to an
empty string or stripping application text. No Hook/WOW routing change,
guest/mirror edit, global state mutation or application geometry change.

Caption tests now exercise unchanged initial/dynamic A/W text, empty titles,
Get/Set roundtrips, genuine application-supplied prefix text, rejection,
reentry, removal and destruction. Native integration separately checks actual
Ntw16sWindowAttached state; cross-process immutable-WOW probes check exact
module identity and per-window DWM NC policy, plus captures and caption
absence. These are test-only observations, not new production witness state.
The first r039 cross-process observer mistakenly queried the set-only DWM
NC policy attribute. Its first Off case failed the observation, not a product
caption assertion. r048/r049 use the queryable NCRENDERING_ENABLED Boolean,
report HRESULT and compare native/custom frame state independently of text;
the initial Off case now proves S_OK and enabled native rendering. No product
or guest changed for this probe correction.
r033 initially exposed an incorrect fixture assertion on SetWindowText's API
return rather than the window procedure result; the test now uses WM_SETTEXT
as its predecessor did. The fresh r043 run retains this failed run honestly.

Owner explicitly permits retaining the unstyled native-menu animation
fresh-login investigation in TODO and closing the T after this delivery.
That issue remains unproved/unfixed; styled menu tests do not discharge it.

### P2 final candidate verification

Candidate r034/package retains P1's ten core images and replaces only both
NTW16S DLLs and W16Style.exe. Background runs12964 (2400s) and18544 (1800s)
execute serially with the probe correction above, not simultaneous UI tests.

- r043: both-width actual DLL caption-preservation cases pass, including
  explicit absence of the retired exports.
- r035: complete both-width controls/menu/scroll/font/lifetime regression and
  shipped-demo boundary pass. Its injected painter leak still fails as
  expected; that deliberate negative is not a product regression.
- r036: both native widths, A/W, modal/modeless, dynamic captions, later GUI
  threads, six MessageBox APIs and cross-width child propagation pass; actual
  Ntw16sWindowAttached state is checked separately from unchanged text.
- r037/r038: exact candidate attachment/lifetime/unload and DLL/demo boundary
  pass; GDI3/3 over30 lifetimes,27 demo pages and20px viewport movement.
  r038's actual demo capture has the restored compact three-column geometry.
- r048: all18 real Win16/native/Win16 shared/independent and A/W relay chains
  pass. r049: five shared-task/direct-child policy variants pass with captures.
- r041 Product:159275ms, all WOW-frontier, Console17 and Window17 gates pass.
- r042 Control:213114ms, all11 gates pass, including version negatives,
  strict directory, EDIT/cooked returns, rapid/interactive relaunch,
  session isolation, retirement wiring and nested window handoff.

All runs retain source/fixture identities and process-owned cleanup.

- r044 finds the lifecycle fixture's link list missing shell32.lib after
  P1's ShellAbout integration. Add the same host import used by the product;
  no production modification. r050 then passes task policy, A/W dialogs,
  dynamic captions, same-runtime Off peer and parent retention with errors=0.
- r046: actual WINMINE About with Off/Win16/All passes; styled title is exactly
  About Minesweeper, known module path and custom NC state are verified, and
  closing About leaves the game alive. Off-screen captures and message-only
  menu checks are not represented as physical mouse tests.
- r047-Off/Win16/All: ordinary real-game flag/counter changes, F2 reset, open
  cell, WM_CLOSE and Run16 completion all pass. The observer checks unchanged
  caption plus the independent frame state, not a textual style marker.

Extra probes run in background PID17560 with900s owned deadline. No product
input changed after r034; the last fixes affect test assertions/link inputs.

### Publication and closure

r045, background PID5072 with300s deadline, first preserves all13 destination
images and the old release manifest. It copies only changed hashes, verifies
all13 against tested inputs at both assets/release and O:/winnt/system32,
then passes deployed both-width DLL boundaries and27-page demo checks.
Its exit.txt is0 and verified-manifest.json records exact identities. The
only changed release images are W16Style.exe and ntw16s32/64.dll; the ten
core images remain P1-identical. No deployed project process was running
at publication preflight. Recoverable backups remain in r045.

Sequential review confirms the requested20px demo correction and removal of
automatic caption rewriting, unchanged policy propagation, no guest/mirror
diff and no new application-layout engine. Documentation governance and
diff checks precede the final commit/push. The owner-directed
[T closure](../../history/m0-t445-window-style-closure.md) retains known
limitations and leaves no active packet or automatic next admission.
The final documentation/link governance and diff checks pass. The previous
32KiB status chronology is preserved in history, not deleted; Queue order
is unchanged and CURRENT explicitly records task intermission.
