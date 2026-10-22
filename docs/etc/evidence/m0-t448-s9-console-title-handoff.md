# T448 S9 Console title handoff

The owner accepted S8 functionality and requested correction of the carrier
caption observed for `run16 cmd` in CAF. On the S8 package, the NTVWM hidden
Console and its CAF caption both contained the `ntvwm.exe` path. A native
program's explicit `title` command was reflected correctly. DOS and one
generated PIF used the shared native Console and PIF title respectively.

## Ownership and implementation

NTCON owns a bounded copied title at an authenticated channel handoff. A
returning channel's current-epoch title wins; otherwise the most recently
released owner's title is inherited, or the attached native Console supplies
the initial title. When the root Console is parked for its outer owner and
reacquired, the prior channel title epoch is invalidated and the current
outer Console is authoritative. The copy is a presentation handoff, not an
execution owner or a separate policy. Titles remain limited to the existing
`CONSOLE_IO_TITLE_BYTES` ANSI boundary.

NTVWM reads that title during the existing screen snapshot over Console I/O,
sets its hidden native Console before publication, and thereafter observes
ordinary native title changes. New operation `CONSOLE_IO_GET_HANDOFF_TITLE_A`
uses protocol version 28; it is a data operation on the existing channel,
not a new transport. DOS `GET_TITLE` and PIF behavior are not replaced. Errors
abort the existing acquisition; no retries, timeout or guest change is added.

## Evidence and limits

- `s9-build-r1/result.json`: thirteen images and 1454 unchanged intermediates
  on the second incremental build. The wrapper initially failed only its
  documentation gate because CURRENT lacked an S8 closure link; that link was
  added and the governance verifier subsequently passed.
- `s9-focused-r3/presentation.txt`: 526 checks, zero failures; the frontend
  dispatcher test also passed. The previous fixture run failed one obsolete
  expected request count, corrected for the three new snapshot reads.
- `s9-channel-r1.txt`: actual frontend channel tests cover root title,
  nested-owner inheritance, parent restoration, and park/reacquisition.
- `verify-s9-verify-r1-result.json`: both-width frontend fixtures, actual
  channel and text handoff, and all eight CAF natural/management/self-delete
  cases pass against the complete staged package.
- `s9-title-live-r2-*.txt.timeout-live.txt`: private desktop `run16 cmd` CAF
  and hidden Console both show the inherited `NTVDM observation` title rather
  than the carrier executable; an explicit `S9-EXPLICIT` title appears in both.
  These held interactive cases were terminated by the owning observer and do
  not prove normal exit. The first attempted live probe had an incorrectly
  splatted command argument and is retained as invalid test evidence.
- `s9-runtime-r1-result.json`: DOS17 and Minesweeper pass; WRITE is an observed
  startup frontier and SOL is startup-only, consistent with existing limits.
- `s9-title-dos-r2-*.txt.timeout-live.txt`: the new package shows the root
  Console title for `COMMAND.COM`, and `S9-PIF` for the generated test PIF.
  The first PIF attempt failed to create its fixture directory and did not
  launch; no installed guest was modified. Both held probes are observations,
  not normal-exit tests.

Original mirrored source and guest files are unchanged. The pre-existing SOL
layout, WRITE editing/exit and intermittent direct WOW exit limits remain;
passing these tests does not establish all applications' title behavior or
all concurrent handoff interleavings. The complete thirteen-image candidate
was published to `assets/release` and `O:/winnt/system32`, and both destinations
were checked against candidate hashes (`s9-publication.json`). The
`s9-publication-backup` retains the prior package. Delivery commit `d8a8a9d4a`
contains the sources and release manifest and was pushed to `origin/main`.
The owner subsequently approved S9 closure. This approval does not close T448
or expand the stated test coverage.

## SOL integration geometry follow-up

Owner requests closure of the original-behavior small-desktop sizing concern
and environment-aware integration coverage. This follow-up is test-only and
does not change S9 title behavior, original guest, mirror or product assets.

The installed `O:/winnt/SOL.EXE` and the retained S3 copy share SHA-256
`68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`.
Read-only NE resource and instruction inspection finds its 71x96 bitmap,
the startup seven-card 585-pixel client-width calculation, AdjustWindowRect,
and a separate initial-height limit based on GDI VERTRES. OpenNT
`windows/core/ntuser/kernel/createw.c::xxxAdjustSize` clamps tiled/sizeable
windows to the values of WM_GETMINMAXINFO, whose default MAXTRACK derives
from CXMAXTRACK/CYMAXTRACK in `minmax.c`; `inctlpan.c` derives those metrics
from screen and frame metrics. SOL's relevant window-procedure dispatch uses
the default handler. The 427x320 historical desktop records 447x340 maximum
track and 447-pixel actual outer width, consistent with that original path.
Its actual minmax response was not captured; this is source-and-observation
attribution rather than a claim of a directly captured original message.

An independent hidden native-window control set maxtrack to447x340 and
requested601x443; the resulting outer rectangle was447x340. With the limit
raised to1000x1000, the same request produced601x443. This demonstrates the
host mechanism, not the result of an injected SOL intervention. A subsequent
direct installed-SOL private-desktop attempt did not find its Solitaire
window and timed out; its owned worker and broker were stopped. It is not a
successful SOL test.

`wow_sol_geometry_test.c` checks sufficient geometry, either or both limiting
axes, unexplained clipping, and different awareness contexts. MSVC compiles
the real integration probe at /W4 /WX. Real package S9 private-desktop run
`build/sol-size-integration-r1` observed screen320x607, maxtrack340x627,
initial and post-resize client324x384 and outer340x443; target and observer
had matching DPI contexts. The probe captured the limited window, closed it,
and the observer recorded `result=exited` and `exit=0x00000000`. The runner
records `PARTIAL ... full gameplay UNVERIFIED` and exits non-success: no deal,
stock or full-gameplay claim. Full-size gameplay is still required when a
sufficient desktop is available; historical larger-desktop evidence remains
historical, not a fresh run of this test change. The owner's separately
reported SOL layout symptom has no measured geometry and remains distinct.

Initial early-close rounds r2/r3 reached the same limited geometry but failed
their close assertion; the observer reported exit `0xffffffff`. Do not erase
or reinterpret those failures. The probe was then changed to wait for SOL's
existing deal-menu readiness assertion before requesting close and to report
close-post/close-timeout failures explicitly. Final-source rounds r5 and r6
each reached deal-menu enabled, closed the window, and recorded the explicit
partial result plus launcher exit0. Those two observations do not prove the
root cause of r2/r3 or every timing interleaving; normal close failure remains
a failure, not an environment skip. Full-size gameplay remains unexecuted for
this test revision because the available desktop is320 pixels wide.
