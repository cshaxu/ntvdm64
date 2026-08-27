# M0 T290 S3 P3 — Original named-pipe source parse and pure helper

## Question

Can the complete original `vrnmpipe.c` translation unit parse on both supported
host widths without editing its source, while one dependency-free helper is
executed from that exact object?

## Inputs and procedure

- Exact mirror input: `mvdm-host/vdmredir/vrnmpipe.c`.
- T290 adds byte-exact OpenNT public-internal `align.h`; the older OpenNT-4.5
  candidate truncates pointers and is not selected for the modern x86/x64
  source cohort.
- The sole source binding is the same-shaped unavailable thread form:
  original cdecl async callback input is rejected with
  `ERROR_CALL_NOT_IMPLEMENTED` until S4 supplies a session-worker thunk.
- Fixture-only static-link stubs satisfy uncalled owner groups.  They neither
  provide a Redirector service nor alter the imported body.

## Observation

- x64 fixture `build/M0-T290/S3/vrnmpipe-name-x64/bin/t290-s3-vrnmpipe-name-fixture.exe` exited `0`.
- x86 fixture `build/M0-T290/S3/vrnmpipe-name-x86/bin/t290-s3-vrnmpipe-name-fixture.exe` exited `0`.
- Both compile the complete original source unit and execute its exact
  `VrIsNamedPipeName` body for remote `\\server\PIPE\name`, slash form and
  rejected local/malformed/ordinary paths.
- x64 emits the expected historical `DWORD`/`HANDLE` and pointer narrowing
  warnings in service branches.  No such branch executes or is enabled by
  this fixture.

## Interpretation and boundary

This is source-parse and pure-helper evidence, not synchronous named-pipe
service closure.  The warnings identify precisely why the selected later S3
composition must use the existing session host-resource identity and bounded
guest leases.  Async callback execution remains intentionally unavailable;
its thunk belongs to S4.  No `57:02..08`, `57:20..21`, `57:23..24` or ingress
route is enabled.

## Follow-up

The next S3 part must compose an original synchronous named-pipe operation
through opaque handle resolution and an explicit pointer scope, then verify its
source-defined register/error ordering on both targets.
