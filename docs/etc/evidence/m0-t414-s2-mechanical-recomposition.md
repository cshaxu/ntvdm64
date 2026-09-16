# M0 T414 S2 Mechanical MVDM recomposition

## Result

The selected OpenNT MVDM union is now physically rooted at `src/mvdm/`, using
its original `base/mvdm`-relative directory layout.  This is a mechanical
move, not a source rewrite.

| Former selected root | Canonical destination | Files |
| --- | --- | ---: |
| `mvdm-host` | `mvdm` | 1,975 |
| `mvdm-guest/dos/v86` | `mvdm/dos/v86` | 877 |
| `mvdm-guest/wow16` | `mvdm/wow16` | 1,658 |
| `mvdm-guest/bin86` | `mvdm/bin86` | 19 |
| `mvdm-tools/vdmutils` | `mvdm/vdmutils` | 45 |
| `mvdm-softpc-firmware/softpc.new` | `mvdm/softpc.new` | 41 |
| **Total original selected files** | **`src/mvdm/`** | **4,615** |

The private overlay moved as a paired, explicitly non-original tree from
`src/mvdm-host-overlay/` to `src/mvdm-overlay/`.  It is not inside
`src/mvdm/`, so a visual comparison of the latter against OpenNT remains
unambiguous.

The only intentionally retained split locations are outside the selected
OpenNT MVDM union: `src/mvdm-host/kernel-vdm/` is the external kernel-VDM
carrier, and `src/mvdm-guest/font16/` is sourced from OpenNT `win16/fonts`.
They are not presented as part of the canonical MVDM mirror.

## Consumer repair

The current formal generator and runtime stager now select `src/mvdm/...`.
The only production source includes needing path correction were the WOW
callback, redirector header, and three overlay `localfm.c` includes.  Their
providers, source contents, and link roles are unchanged.  Retired generators
and historical evidence were deliberately not rewritten: their old paths are
chronology, not current build inputs.

## Verification

- `Test-T414CanonicalMvdmTree.ps1 -Phase after -BaselineRef 9162b97fe` passed:
  `files=4615`, with the six categories shown above and six explicit
  exclusions.  It compares every destination blob to the pre-move baseline.
- A fresh x86 `/MT` formal graph built all three programs from the new paths:
  `run16.exe`, `basesrv.exe`, and `ntvdm.exe`.
- Seven selected broker, CCPU, DPMI, external-memory, and C-VID fixtures
  passed.  In particular, the original BaseSrv Check/Update/Get/ExitVDM
  lifecycle, original EMS/DIB mappings, and all 38 C-VID accessor pairs passed.
- Documentation governance passed.  The legacy standalone `ntvdm --runtime`
  manifest probe is intentionally not an acceptance route: after T412,
  `ntvdm.exe` is a broker-connected worker and rejects that pre-broker CLI
  contract.  This layout-only S packet does not alter that product behavior.

S2 therefore closes with no original-MVDM byte change and hands only final
source-layout review and package-level closure to S3.
