# M0 T309 S2 P3 — original PIF source-composition audit

## Question

Can the original `softpc.new/host/src/nt_pif.c` be admitted to the current
DEM/COMMAND provider-composition investigation without changing its PIF
algorithm; and does `nt_reset.c` belong to the same current source group?

## Inputs and procedure

- The byte-exact `mvdm-host/softpc.new/host/src/nt_pif.c` was compiled through
  `New-T309OriginalProviderCohortsNinja.ps1` under MSVC `/MT` for x86 and x64.
- The build supplies the historical `WINNT` preprocessor selection and the
  existing original public `pif.h` declaration carrier.  Neither changes the
  source file or selects a replacement PIF provider.
- The PIF-only static archive was linked with `/force:unresolved` to identify
  the complete immediate source boundary.  This deliberately non-runnable DLL
  is audit evidence only.
- `nt_reset.c` was separately attempted as the reset neighbor of the PIF
  source.  Its original include chain reaches `CpuH -> CpuInt_c.h` before a
  current Bochs-only composition boundary can be formed.

## Observations

- `nt_pif.c` compiles unchanged on both target architectures.  The PIF 3.1
  structure is exposed only when its original `WINNT` conditional is selected;
  no body crop, pointer-width rewrite, or provider shim was required.
- Each architecture emits the same eleven immediate external forms:
  six public CRT forms (`memmove`, `strcat`, `strcmp`, `strcpy`, `free`,
  `malloc`); `ExpandEnvironmentStringsOem`; `RcMessageBox`; and the original
  PIF/session state forms `pfdata`, `bPifFastPaste`, and `DosSessionId`.
- The six CRT forms remain public runtime leaves.  The OEM expansion form has
  the existing `adapter-mvdm-host-out/win32` owner.  `RcMessageBox` selects the
  next whole original `nt_error.c` group.  The final three forms are PIF/reset
  session state and require source-shaped session/app binding, not an invented
  PIF algorithm.
- `nt_reset.c` is not a suitable companion in the present graph: its original
  body owns full SoftPC reset, console/UI, heartbeat and termination lifecycle,
  and the required CPU include chain is the original SoftPC executor boundary.

## Disposition

`nt_pif.c` is an original-source candidate with a finite, named immediate
boundary.  Its next implementation decision must consider the full PIF
configuration/session group and may not replace `GetPIFData`,
`GetPIFConfigFiles`, or `SetPifDefaults` with a trace-selected helper.

`nt_reset.c` is transferred intact to the queued original-SoftPC backend
package.  That package will admit a CLI-selected SoftPC or Bochs backend under
one MVDM host composition.  T309 must neither crop nor rewrite `nt_reset.c` to
erase the SoftPC alternative.

## Confidence and follow-up

High confidence for the source identity and immediate boundary: both formal
x86/x64 build logs are under `build/M0-T309/S2/cohorts/<arch>/` and list the
same eleven PIF forms.  This audit does not claim a runnable PIF path or a
composed reset lifecycle.  The next P3 group is original `nt_error.c`.
