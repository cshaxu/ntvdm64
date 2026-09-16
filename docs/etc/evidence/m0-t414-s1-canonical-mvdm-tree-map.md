# M0 T414 S1 — canonical MVDM tree map

## Question

Can all selected original OpenNT `base/mvdm` content be mechanically composed
under one `src/mvdm/` source root while preserving bytes, provenance and the
current host/guest/tool/firmware build boundaries?

## Inputs and method

- Baseline: `9162b97fe`, immediately before T414 source work.
- Selected MVDM source union: pinned `O:\repos.external\OpenNT\base\mvdm`
  and `O:\repos.external\OpenNT-4.5\nt\private\mvdm` per source policy.
- Mapping/checker: `tools/governance/Test-T414CanonicalMvdmTree.ps1`.

The checker enumerates tracked files in the six selected current carries,
derives their canonical path, rejects target collisions, and compares each
source blob to `BaselineRef:<old-path>`. It supports both the pre-move old path
and the post-move new path, so S2 cannot silently modify a source while moving
it.

| Current carry | Canonical destination | Role |
| --- | --- | --- |
| `mvdm-host/` except listed exclusions | `mvdm/` | executable MVDM host source |
| `mvdm-guest/dos/v86/` | `mvdm/dos/v86/` | load-only DOS/V86 guest |
| `mvdm-guest/bin86/` | `mvdm/bin86/` | load-only Bin86 input |
| `mvdm-guest/wow16/` | `mvdm/wow16/` | load-only WOW16 input |
| `mvdm-tools/vdmutils/` | `mvdm/vdmutils/` | independent original tool |
| `mvdm-softpc-firmware/softpc.new/` | `mvdm/softpc.new/` | immutable firmware input |

## Explicit exclusions

- `mvdm-host/kernel-vdm/monitor_printer.c` is an outside-`base/mvdm` semantic
  carrier. T414 does not disguise it as MVDM content or reclassify it.
- `mvdm-guest/font16/` originates in `win16/fonts`, not `base/mvdm`; it is not
  put below the canonical MVDM comparison tree in this task.
- Component READMEs are project governance text, not selected original source.
  S2 replaces their root description with one canonical-tree description;
  historical records are not bulk-rewritten.
- `mvdm-host-overlay` is re-rooted separately to `mvdm-overlay`; it is private
  non-original implementation and must remain visibly excluded from direct
  original-tree comparison. `opennt-host-overlay` is outside T414.

## Required S2 and S3 proof

Run before the move:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/governance/Test-T414CanonicalMvdmTree.ps1 -Phase before -BaselineRef 9162b97fe
```

Run after the move with the same baseline and `-Phase after`. S2 also checks
that all current formal build/stage consumers move to canonical paths while
preserving their build role. S3 rebuilds the formal x86 CCPU40 package and
verifies source/tree comparison, staging and selected regressions.

The current formal source manifest identifies
`tools/build/New-T310OriginalSoftpcNinja.ps1` as its generator and selects the
two original manifests below `src/mvdm-host/softpc.new`. The only current
production staging input list is `tools/build/Stage-OriginalSoftpcRuntime.mjs`.
S2 changes those current consumers and their generated output only. Historical
one-off generators remain historical evidence and are not bulk-rewritten.

## Interpretation

The requested one-tree shape is feasible for the selected `base/mvdm` union;
the only apparent source-root exceptions are correctly excluded non-MVDM
content, not omissions. The physical re-root must not change ownership:
`src/mvdm/` is a canonical comparison root with manifest-declared logical
host/guest/tool/firmware roles. No source content has been moved in S1.
