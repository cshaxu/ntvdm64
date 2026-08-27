# M0 T289 S6 — DPMI family matrix closure

S6 is closed.  Every original `53:00..18` selector now has exactly one
current result in the [family disposition matrix](../operations/m0-t289-s6-dpmi-family-disposition-matrix.tsv):

- Sixteen selector bodies have local source-body evidence: descriptor
  normalization/failure (`00`), protected/fault registration (`02`, `0A`),
  DOSX/app/table initialization (`04`, `05`, `0F`), the five XMEM lifecycle
  operations (`07`--`10`, `13`), memory information (`0B`), x86 no-op
  lifecycle (`0C`, `0D`), PM-stack identity (`11`) and VCD version/port query
  (`12`).
- The remaining nine retain an explicit source-proven unavailable
  disposition rather than an invented success or no-op: mode transition,
  fast BOP publication, complete INT21 translation, CPU debug registers, and
  protected DOSX interrupt/fault/IRET delivery.

`Invoke-T289S6DpmiFamilyMatrix.ps1` executed the package matrix using the
original-source fixtures and static audit in one sequence. It passed on both
architectures:

- `-Architecture x64`: exit `0`.
- `-Architecture x86`: exit `0`.

The runner executes S3 protected-machine mechanics, S5 real/protected frame,
registration, stack and source-static checks, the revised S4 composition
fixture, and S6 VDD/misc fixture. It remains selector-disabled: it neither
decodes BOPs nor links DOSX as host code. The source-proven unavailable rows
are first-class matrix outcomes, so the runner does not manufacture stubs to
call them.

## Transfer

S7 must now audit source identity, every registered divergence and mapping
manager use, then attach each remaining unavailable branch to a named future
owner before T289 closes. In particular, it must not turn S6's no-ingress
evidence into a DPMI execution claim.
