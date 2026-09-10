# M0 T310 S8 P4 — CVIDC generated-table ABI audit

## Scope

This audit covers the complete original `softpc.new/base/cvidc` `sources`
package in `mvdm-host`, including all `SINIT*`, `SEVID*`, glue, generated-rule
and table inputs.  It is limited to x86/x64 call and pointer representation;
ordinary historical compiler warnings remain visible.

## Selected composition

The original package supplies the generated rule bodies.  Its original
`evidfunc.c` and `vglfunc.c` table definitions remain byte-exact mirrors, but
the formal product graph selects the registered `MVDM-HOST-DIV-042` generated
typed table carrier instead.  That carrier preserves original table order and
rule entry sequence while passing native-width values through a typed
four-word call boundary.

## Results

- The 16 historic `C4152` function/data pointer conversion records are absent
  from clean formal x64 and x86 package rebuilds.  They are closed as
  `closed-typed-cvidc-table`.
- The generated setter cases that issue bare `return`, the old implicit
  `int` declarations in `evidfunc.h`, and diagnostic-only unprototyped calls
  have no selected host-width truncation or changed calling convention.  They
  remain visible in compiler output and are classified
  `not-x86-x64-retained-source-warning`; this audit neither suppresses nor
  edits those original sources.
- The CCPU generated `VGAGLOBALSETTINGS` rule-storage repair is separately
  recorded by `MVDM-HOST-DIV-051`; it removes the only selected private
  code-address storage mismatch between original CCPU accessors and the
  generated record.

## Verification

After deleting every CVIDC object and archive, formal Ninja rebuilt and
archived all 41 selected objects into `original-softpc-cvidc.lib` on each of
the x64 and x86 graphs.  The builds succeeded.  This is a source/ABI closure,
not a claim that the video device has completed its runtime integration.
