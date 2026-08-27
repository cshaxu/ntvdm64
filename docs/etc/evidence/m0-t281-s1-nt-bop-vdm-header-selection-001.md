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
implementation. The caller is selected from the OpenNT baseline. The two
client headers are byte-identical to one another but declare Base
process-creation helpers rather than `VdmServiceClass`; the OpenNT-4.5 private
header has only the obsolete nine-value enum prefix. The public OpenNT
internal candidate supplies the full reached declaration layout and is
therefore selected.

It was copied byte-identically to
`src/opennt-platform-abi/source/opennt/public/internal/base/inc/vdm.h` and
verified at the source SHA-256 above. This declaration carrier belongs in
`opennt-platform-abi`, never in `adapter-vdm-monitor`.

## Follow-up

S1 may now advance the unchanged x86/x64 compiler observation with this
declaration root. No adapter body, selector handler, or monitor semantics are
admitted by this evidence.

## Compiler continuation

With the selected declaration root and the already-mirrored MVDM `inc` root,
the original source reaches code generation on both x86 and x64. Modern x86
MSVC alone rejects three historical implicit `FARPROC`/`MYFARPROC` assignments.
`MVDM-HOST-DIV-008` makes those three already-intended conversions explicit;
it preserves the raw export pointer and does not enable the WOW or
installable-BOP branches whose provider ABIs remain later-owner work.
