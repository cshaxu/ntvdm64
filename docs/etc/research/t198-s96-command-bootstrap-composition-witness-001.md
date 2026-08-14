# T198 S96 COMMAND bootstrap composition witness 001

## Question

Can the contained, source-derived COMMAND bootstrap pair (`54:02` and
`54:0F`) be repaired and attached to the current `bx-vdm` composition through
the global BOP classification and existing selector-blind mechanical-action
ABI, without changing Bochs or admitting ambient host environment policy?

## Change

`bx_ntvdm_cmd_comspec_bootstrap_service` now owns an explicit three-state
first-profile lifetime:

1. `WAIT_COMSPEC` performs one fixed 64-byte checked `DS:DX` read for
   `54:02`.
2. `ENVIRONMENT_READY` returns a paragraph retry for too-small `54:0F`
   buffers, then publishes the bounded `COMSPEC=<guest string>\0` payload at
   `ES:0` through the existing checked multi-write transaction.
3. `ENVIRONMENT_CONSUMED` makes later `54:02` an original-compatible resume
   with no register delta and later `54:0F` an original-compatible `BX=0`
   resume.

The helper no longer carries an unused readonly-namespace parameter.  The
current composition owns this state and calls it only after common ingress,
provider selection and `bx_ntvdm_command_plane_v1_classify` accept the
original COMMAND/`DEFERRED` record for service 2 or 15.  The manifest now
explicitly compiles the service.  No selector, OpenNT, DOS or SoftPC term
enters Bochs or mantle code.

## Verification

1. `artifacts/build/t198-s96-command-bootstrap-composition-r2` was built and
   run by `tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` after adding the
   service to that current composition closure.  It is an MSVC x64 `/MT`
   source-built executable and exits zero.
2. The composition fixture proves first COMSPEC capture, `54:0F` size retry,
   checked payload publication, later `54:02` no-op resume, later `54:0F`
   `BX=0`, and the existing DEM/stream/command regressions.
3. The focused service test was independently built and run at
   `artifacts/build/t198-s96-command-bootstrap-service-r3`.  It verifies
   malformed unterminated input rejection, state transition, retry, checked
   write preparation, consumed state and repeat results.

The first r1 composition attempt linked successfully but exposed a stale
test-fixture assumption: its stream test populated the K.SYS table at
`DS:SI`, while the source-derived stream service correctly reads `CS:SI`.
The fixture now uses `CS=DS=1000h` for the positive case and an out-of-aperture
`CS` for the negative case; r2 proves both the repaired bootstrap and the
preserved stream behavior.

## Scope and result

This is a source-derived contained provider, not a direct modern build of
`cmdComSpec`/`cmdGetInitEnvironment`.  It intentionally supplies only the
COMSPEC string and does not expose host variables, conversion policy, console,
WOW, allocation or historical fatal behavior.

No Bochs/core/mantle source changed, no guest ran, and no other COMMAND service
was admitted.  The next separately admitted item may rebuild and make one
bounded exact-input source-built guest observation to determine the next BOP
boundary.
