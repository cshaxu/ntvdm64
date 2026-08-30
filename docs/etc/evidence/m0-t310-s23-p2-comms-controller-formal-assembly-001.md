# M0 T310 S23 P2 — communications-controller formal assembly

## Question

Does the complete original `softpc.new/base/comms` package assemble under the
selected Win32/x86 CCPU40 profile without inventing a controller or activating
an S24 host endpoint?

## Inputs and procedure

Generated a fresh formal graph with
`tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86` in
`build/M0-T310/S23/formal-comms-x86`.  Outside the sandbox, invoked Ninja for
`original-softpc-comms.lib` and `original-softpc-forced-closure.dll`.  Inspected
the original `sources` manifest, generated archive membership, selected flags,
forced-link log and produced output sizes.  The forced link intentionally uses
`/force:unresolved` as an ownership audit only; it is not a runnable-image
claim.

## Observations

- Ninja completed all 369 configured edges.  The original communications
  archive contains exactly the five selected original manifest members:
  `com.obj`, `printer.obj`, `printer_.obj`, `prt_scrn.obj` and `rs232_io.obj`.
  `original-softpc-comms.lib` is present at 27,330 bytes.
- The selected flags are Win32/x86 `/MT`, `CCPU`, `CPU_40_STYLE`, `C_VID` and
  `PROD`.  `CPU_30_STYLE` remains only the already documented source
  compatibility carrier.  Neither `PRINTER` nor `SLAVEPC` is defined.
- The forced-link audit emitted a 3,014,144-byte
  `original-softpc-forced-closure.dll`.  Its retained unresolved owners are
  later CCPU, video, console, DEM/COMMAND, VDD/debug and lifecycle packages.
  It contains no unresolved `host_com_*`, `host_lpt_*`, `host_print_*`,
  `com_init`, `com_post`, `rs232_io`, `printer_*` or `prt_scrn` symbol.
- The original serial controller's one registered mirror difference is
  `MVDM-HOST-DIV-068`: host-local ioctl pointer/scalar transport uses
  `intptr_t` rather than the historical `long` cast.  It preserves request
  values, caller ordering and output-pointer role, is not guest-visible and
  introduces neither a mapping identity nor a controller replacement.
- Focused negative result: all original printer/parallel bodies are retained
  and compiled from the original package manifest, but `PRINTER` remains
  unselected.  The selected BIOS therefore keeps the original no-printer
  equipment result and does not call `printer_init`/`printer_post`.  No port,
  printer handle or synthetic status was exercised.

## Interpretation and follow-up

The selected original serial controller and INT 14 front end have source and
archive closure.  The formal forced-link audit has no communications-owner
unresolved edge; unrelated unresolved symbols remain visible rather than being
hidden.  The exact selected printer result is an original unavailable
condition, not a missing implementation.  Real serial/parallel host endpoint
selection, native handle lifetime and public Win32 device binding are still
S24; integrated guest communication behavior is mandatory S49 work.
