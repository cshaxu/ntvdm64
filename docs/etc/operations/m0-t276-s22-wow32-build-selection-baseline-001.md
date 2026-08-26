# M0 T276 S22: `wow32` build-selection baseline

The selected WOW32 mirror contains 201 paths: 85 source files, 105 declaration
or table files, and 11 resource/build inputs.  Its original `sources` text
contains 87 apparent source/resource entries, but nine table-source entries
are enclosed by `!IF 0`.  The active build selection is therefore 78 C bodies
plus `wow32.rc`, not 87 bodies.

Any subsequent WOW32 disposition ledger must parse conditional build blocks and
record all four groups separately: active bodies, deliberately inactive source,
declarations/tables, and resource/build input.  A raw textual `SOURCES` match
is insufficient evidence for recovery or composition.
