# M0 T312 S4 — selected SoftPC formal closure

Date: 2026-08-30  
Status: closed

## Formal graph

`tools/build/New-T310OriginalSoftpcNinja.ps1` generated a fresh x86 CCPU40
graph in `build/M0-T312/S4/r2`. The graph selected 131 CCPU sources and lists
the retired machine roots only in its forbidden-input assertion.

## Result

- `ninja -C build/M0-T312/S4/r2 original-softpc-candidate` completed all
  399 declared steps without `ninja: build stopped`.
- `ninja -C build/M0-T312/S4/r2 original-softpc-forced-closure.dll` completed
  its forced-link audit successfully.

The compiler/linker still emits pre-existing source diagnostics such as
historical width conversions and duplicate CRT symbols. They are not suppressed
or reclassified here; this packet proves only that deleting the retired Bochs
route did not break the selected CCPU40 source/build closure.
