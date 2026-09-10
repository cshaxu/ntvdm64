# M0 T301 S1 P7 — Original MVDM Definition-Form Index

## Scope

This evidence corrects a source-language limitation discovered while resolving
the T301 declaration-shaped first-degree frontier.  It reads only the approved
original source union:

- `O:\repos.external\OpenNT\base\mvdm`
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`

It does not read `src.old`, a product mirror, an adapter, a build artifact, or
a runtime trace.  It imports or enables no source body.

## Method

The local Universal Ctags C/C++ parser constructs a physical function-definition
index for both roots.  The generated
`mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv`
filters that index strictly to the 122 spellings previously classified as
`function-declaration-candidate` by P6.  Each row keeps its source root,
relative path, SHA-256, line and detected definition form.  Edition-equivalent
files remain separate physical rows.

## Result

- 122 requested declaration-candidate spellings;
- 128 physical original-MVDM definition candidates;
- 64 spellings with at least one original-MVDM definition;
- 24 bodies whose brace follows the parameter list directly;
- 36 bodies recognized as K&R declaration-list bodies; and
- 68 additional historical function bodies recognized by Ctags where the
  lightweight form classifier deliberately remains conservative.

For example, the index records both approved-edition identities of
`softpc.new/base/bios/cmos_bis.c:cmos_read` (line 143) and
`softpc.new/host/src/nt_graph.c:high_stretch3` (line 1401).  These are concrete
evidence that a name-only “not found in MVDM” conclusion would be wrong.

## Interpretation and next gate

This is a **candidate-body index**, not a reclassification by spelling.  A
T300 physical call can move from first degree back into the zero-degree MVDM
closure only after its caller include/frontier, linkage and compatible physical
definition identity are proved.  Ambiguous spelling variants remain first-degree
records until that per-call proof exists.  The remaining 58 requested spellings
have no function-definition candidate in this original-MVDM index and continue
through the declared T301 source-union outcome procedure.

The index is the reusable parser input for that correction and later BFS work;
it replaces no original source authority.
