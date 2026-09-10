# M0 T276 S2 original build-input and include-graph evidence

## Scope and method

This evidence parses only byte-exact selected mirror files. It inventories source translation units, selected original build-control files named `dirs`, `makefil0`, `makefile` or `sources`, and literal C/C++ preprocessor include lines. A textual include is not a compiler-resolved include path, library dependency, link edge or interface-recovery conclusion.

## Counts

- Selected file-ledger rows: 1689
- Source translation units: 612
- Selected original build-control inputs: 127
- Original NT `sources` descriptions: 58
- Literal declared source tokens: 489
- Selected source files observed in a literal `sources` description: 487
- Selected source files not observed in a literal `sources` description: 125
- Literal include edges: 5171
- Package roots: 23

### Textual include classifications

- `selected-mvdm-relative`: 292
- `toolchain-or-sdk-textual`: 2754
- `unresolved-textual`: 2125

### Include edges by caller package

- `bde`: 9
- `dbg`: 14
- `dos`: 138
- `dpmi32`: 49
- `fax`: 14
- `ieuvddex`: 22
- `oemuni`: 21
- `sim32`: 4
- `softpc.new`: 4271
- `suballoc`: 8
- `v86`: 76
- `vdd`: 19
- `vdmdbg`: 10
- `vdmexts`: 46
- `vdmredir`: 261
- `vdmutils`: 26
- `wow32`: 154
- `xms.486`: 29

## Limits

- Quoted include resolution is intentionally limited to a normalized path relative to the caller that exactly matches a selected mirror path.
- Angle-bracket classification is lexical and does not claim a currently available SDK declaration.
- Original build descriptions may use macros, generated sources, assembler tooling, conditional compilation and external library inputs that require S3 symbol/interface and later build-profile audit.
- `SOURCES` parsing is literal and macro-unexpanded: it is a historical input record, not a complete target recipe.
- No final composition classification, adapter requirement, x86/x64 build decision or enabled package follows from this extraction.
