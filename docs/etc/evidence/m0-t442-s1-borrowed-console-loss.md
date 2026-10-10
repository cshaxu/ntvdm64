# M0 T442 S1 — borrowed Console-loss evidence

## Reproduction and cause

The retained report was: launch a DOS task from a visible CMD Console, let the
DOS task return, then close that outer Console.  NTMON still showed the NTCON
root and resident worker.  The previous NTCON implementation selected a local
`console_anchor`, waited that one process, and re-sampled members on exit.  It
could therefore retain the Console object itself and make a local retirement
decision outside NTSRV's lifecycle authority.  Separately, NTSRV retained the
authenticated root member handles but did not use them for root loss, so a
resident worker suppressed its workerless grace path indefinitely.

## Repair evidence

- NTCON no longer owns `console_anchor`, `next_console_anchor`, a member wait,
  or liveness re-sampling.  Its remaining member read is the one-shot
  NTSRV-requested candidate-launcher admission check.
- NTSRV registers `WT_EXECUTEONLYONCE` waits over the authenticated initial
  external identity, and callbacks only signal `frontend_lifetime_changed`.
  Lifecycle arbitration verifies all external handles terminated before
  setting `frontend_closing`; normal worker shutdown then remains unchanged.
- `basesrv-service-reservation-test.exe --borrowed-console-identity-loss`
  passed: an actual external process exit closes a borrowed root and signals
  its worker shutdown event, while an owned root remains open.
- `basesrv-service-reservation-test.exe --frontend-authority` passed after the
  member snapshot was moved to NTCON's actual report-before-lease order.
- AMD64 service and frontend closures built from the changed source.  The
  service fixture passed again from the final closure; `frontend-session-
  arguments-test.exe` passed 48 checks.  `console-channel-lifetime-test.exe`
  is not counted as a pass: this host rejected its test-only 80x60 Console
  buffer with `ERROR_INVALID_PARAMETER` before the channel case began.
- The ten-component package staged at `build/M0-T442/S1/r018-runtime` retains
  the eight unchanged, verified release images and replaces only the
  source-built NTSRV and NTCON images.  It was published to both
  `assets/release` and `O:/winnt/system32`; all ten entries match the updated
  release manifest.  The prior ten-image asset/runtime sets are retained at
  `build/M0-T442/S1/r019-publication-recovery`.

## Pending gates

The remaining acceptance observation is a real visible-Console close smoke
using the published package.  The legacy
`Verify-BrokerFinalLifecycle.ps1 -ConsoleClose` wrapper could not reach its
required live DOS prompt in this noninteractive agent session, so it is
recorded as unavailable rather than passed.  The historical
`--frontend-notification-baseline` fixture is deliberately a negative
lost-wakeup reproduction and remains unsuitable as a passing gate.
