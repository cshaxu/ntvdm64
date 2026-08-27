# M0 T281 S1 — `nt_bop.c` `vdm.h` declaration-carrier selection

## Question

Which original `vdm.h` is the same-shaped declaration carrier required before
the byte-exact `softpc.new/host/src/nt_bop.c` can enter the formal host graph?

## Inputs

- Original caller: `src/opennt-mvdm-host/softpc.new/host/src/nt_bop.c:5`.
- T274 direct-include ledger rows 208, 275 and 448, which correctly retain
  four same-basename candidates as unresolved.
- `O:\repos.external\opennt\public\internal\base\inc\vdm.h` — SHA-256
  `C614C03A004F974C1EEF37F1B29D3A8E1D7CFC33D04E7E7304BEFCBD6FAAFD18`.
- `O:\repos.external\opennt-4.5\nt\private\inc\vdm.h` — SHA-256
  `6920D648547BC1E2B0F26BEE68C4BD475C61F41A914377CE8E055E170ED1CFF6`.

## Procedure and observations

The unmodified original `nt_bop.c` was compiled outside the sandbox with
MSVC 17.13.5 `/MT /W4 /showIncludes` for both host widths. Both stopped at
the same first source diagnostic:

```text
nt_bop.c(5): fatal error C1083: Cannot open include file: 'vdm.h'
```

No original source changed and no BOP selector/provider route ran. The public
OpenNT internal candidate defines the full 14-value `VdmServiceClass` enum;
the OpenNT-4.5 private candidate defines only its older nine-value prefix.

## Interpretation and disposition

This is a declaration-carrier selection, not a request for a new VDM monitor
implementation. The caller is selected from the OpenNT baseline, so the
public OpenNT internal candidate is the leading source-identity candidate;
the similarly named client/private alternatives must still be compared against
the reached definitions and consuming layouts before import. A copied exact
carrier belongs in `opennt-platform-abi`, never in `adapter-vdm-monitor`.

## Follow-up

S1 next audits the remaining two same-basename candidates and the precise
`nt_bop.c` symbols/layouts that need `vdm.h`. Only after one exact original
carrier is selected and manifested may it be imported and the unchanged
compiler observation advance. No adapter body, selector handler or monitor
semantics are admitted by this evidence.
