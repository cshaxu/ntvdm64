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
