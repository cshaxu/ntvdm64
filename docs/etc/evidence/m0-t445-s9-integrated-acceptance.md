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
