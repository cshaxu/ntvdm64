# T420 S29 communications capability closure

## Current profile and source authority

S29 closes the complete selected original
`src/mvdm/softpc.new/base/comms` unit: `com.c`, `printer.c`, `printer_.c`,
`prt_scrn.c`, and `rs232_io.c`.  The five selected source bodies are
byte-identical to the pinned OpenNT source.  No communications mirror body,
overlay, adapter algorithm, serial emulation, printer emulation, or virtual
device was added.

The selected standalone profile has no provisioned COM or LPT medium.  The
original host providers are still the owners: `nt_com.c` lazily opens a COM
device on the first guest request, `nt_lpt.c` supplies the LPT table, and the
original reset path publishes COM1--COM4 and LPT1--LPT3.  A real missing COM1
therefore follows the original error-panel path; it is not converted into a
synthetic port or a false successful transfer.

## Complete capability disposition

| Family / selected body | Current-profile disposition | Evidence |
| --- | --- | --- |
| COM BOP/table initialization and status: `com.c`, `rs232_io.c` | Real pass through the original no-medium failure contract | The S29 DOS witness invokes INT 14h initialize and status.  The original lazy COM open presents its error panel; the observer selects its existing Ignore result, after which guest execution returns normally. |
| LPT initialization and status: `printer.c`, `printer_.c` | Real pass through the original current table | The same guest witness invokes INT 17h initialize and status, returns to COMMAND, and makes no host-printer write. |
| Print Screen: `prt_scrn.c` and printer data transmission | Source-proven current-profile exclusion | No COM/LPT medium is configured and there is no selected printer provider.  S29 adds neither a fake device nor autonomous spooler behavior.  Actual byte transmission requires an explicitly provisioned host or virtual device and is outside this profile. |
| Device error handling | Real pass | The COM1 open failure is observed in a real guest run.  The test uses the original error panel's Ignore route rather than changing product behavior; the marker, subsequent MEM, and exit prove recovery. |
| Normal teardown | Real pass | Original `nt_event.c`, `nt_term.c`, and `nt_reset.c` close COM/LPT provider state.  The dedicated witness and the full COMMAND regression finish with no `run16.exe`, `basesrv.exe`, or `ntvdm.exe` process left. |

## Real guest witness

`tests/observation/comms_capability.asm`, built by
`tools/build/Build-T420S29CommsGuestTest.ps1`, is a test-only DOS COM image.
It calls INT 14h functions 00h/03h and INT 17h functions 01h/02h.  It neither
writes a byte to a serial/printer device nor provides a replacement provider.

The fresh-product transcript `O:\winnt\logs\m0-t420-s29-comms-r3.raw`
contains:

```text
S29_STEP_COM S29_STEP_LPT S29_INT14_OK S29_INT17_OK S29_COMMS_OK
```

The terminal observer additionally records `dialog-ignore=1`, `marker=1`,
and `guest-failure=0`: this is explicit proof that the missing-COM original
failure path was reached and recovered through its original user decision.
The witness then runs `MEM` and `exit` in the same session.

## Formal build and regression

Fresh graph `build/M0-T420/S29/formal-x86-r1` built all 526 declared edges
and emitted `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, and
`VDMREDIR.dll`.  Their deployed SHA-256 records are in
`O:\winnt\logs\m0-t420-s29-formal-artifacts.json`.

Fresh deployment passed all 17 established transcript-gated COMMAND routes:
empty/native COMMAND, expected missing-command recovery, native streams and
EOF, direct/interactive/nested/repeated MEM, COMMAND `/c`, guest exit 7, and
EDIT return followed by MEM.  The machine-readable result is
`O:\winnt\logs\m0-t420-s29-command-r2-summary.json`.

S29 changes only test/build observation material.  Product mirror delta is
zero, overlay delta is zero, and no production autonomous implementation was
introduced.  This closes the full selected communications package under its
explicit no-device-medium profile; provisioning real serial or printer media
is a separate product capability, not an unrecorded S29 omission.
