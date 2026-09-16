# T417 S2 SoftPC patch-boundary recovery

## Result

`src/mvdm-softpc-patch` is retired.  No new source file was added below either
`src/mvdm` or `src/opennt-host`.

The three unavailable selected-x86 generated declaration headers now live at
`src/adapter-mvdm-host-out/softpc/include/generated/x86/prod/`.  The four
selected missing CCPU provider forms live in the named SoftPC adapter carrier
`mvdm_softpc_ccpu_fallback.c`: the three invalid vector tails retain the
immediate debug-break failure direction and `ActivityCheckAfterTimeSlice`
retains its exact void no-op.

This supersedes the preliminary S1 destination hypothesis that proposed a new
canonical MVDM carrier.  The owner clarified and the governing rules now state
that neither mirror may gain a new file; the selected upstream source union has
no existing file which owns these absent generator outputs/providers.

## Verification

- Fresh `New-T310OriginalSoftpcNinja.ps1` Win32/x86 formal graph:
  `build/M0-T417/S2/formal-x86-003`.
- Full sequential formal build completed and produced `run16.exe`,
  `basesrv.exe`, `ntvdm.exe`, and `ccpu-halt-reset-test.exe`.
- `ccpu-halt-reset-test.exe` returned zero and reported original CCPU HALT
  RESET with `AX=beef`.
- The generated `build.ninja` and source manifest contain no retired-root,
  old evidence-library, or old selector reference.

## Boundary accounting

- Removed from the retired production root: three generated headers (1,654
  declaration lines), `fmstubs.c` (142 lines, only 31 selected semantic
  lines), two provenance patches, and its README.
- Added to the adapter/ABI boundary: the same three declaration headers and a
  41-line selected fallback carrier.  This is a relocation of required input,
  not new MVDM/OpenNT-host source behavior.
- New mirror files: 0 in `mvdm`; 0 in `opennt-host`.
