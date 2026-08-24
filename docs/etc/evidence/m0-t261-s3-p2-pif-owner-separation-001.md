# M0 T261 S3 P2 — PIF owner separation

## Question

Can the existing PIF bridge be separated into its true owners without changing
OpenNT parser or COMMAND behavior?

## Change

- Moved the `PIF_DATA` layout bridge from `opennt-host` to
  `opennt-bop/command/opennt_pif_composition.{c,h}`. It remains a distinct
  project composition source; it is not an OpenNT mirror.
- Moved the public-Win32 OEM/path/title facade to
  `adapter-win32/facade/opennt_pif_oem_facade.c` and placed its independent
  ABI declaration in `adapter-win32/include/legacy-api/`.
- Updated the original COMMAND PIF caller, runtime session, fixture and formal
  module manifest to consume the new owners.
- Kept `softpc.new/host/src/nt_pif.c` and its two headers unchanged in
  `opennt-host`.

## Verification

- `ConvertFrom-Json` accepts the formal component manifest.
- Fresh formal graph `build/M0-T261-S3/r002` compiles both moved objects:
  `opennt_pif_composition.c` in `opennt-bop` and
  `opennt_pif_oem_facade.c` in `adapter-win32`.
- A second Ninja invocation for those exact object targets reports `no work to
  do`.
- No live source/test/build input refers to the retired
  `command_pif_shim.{c,h}` path; remaining references are historical evidence
  only.

## Interpretation

The host component now contains the unchanged historical PIF parser input,
not BOP layout translation or unavailable-Win32 implementation.  The bridge
is explicitly recorded as `BOP-DIV-032`; the public API replacement is
`WIN32-DIV-003`.
