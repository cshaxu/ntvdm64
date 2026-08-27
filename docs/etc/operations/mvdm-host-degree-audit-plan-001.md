# MVDM host degree-by-degree audit plan

## Purpose

Replace broad lexical dependency expansion with two source-defined function
audit packages. They first establish the complete selected `mvdm-host`
definition set, then audit only its direct non-zero callees. Neither package
implements providers or discovers a second-degree package opportunistically.

## Function identity

A row denotes one **definition**, not merely a spelling. Its immutable identity
is the selected original source path, selected source SHA-256, definition line,
linkage/storage form, declared calling form/signature evidence and source
package. Two static functions with the same spelling in different source files
remain distinct. Two definition candidates with the same spelling in an
unselected edition remain candidates until a source path is selected; they are
never silently merged.

## Candidate 1: zero degree

`mvdm-host` zero degree is the complete set of selected original C/C++ function
definitions. Its ledger has one row per definition identity and includes:

- source package/path/hash/line and definition identity;
- all direct call expressions, resolved first against the zero-degree set;
- leaf outcome: no call; modern public Win32/CRT only; or bounded shallow
  replacement after its dependency is independently proved finite;
- final product disposition: byte-exact mirror, binding-only, adapter-backed,
  overlay-required, tool/firmware/guest-only, or not-host-runtime; and
- recovery form: original source, same-shaped binding, source-derived adapter,
  registered overlay, or explicit exclusion.

Any direct callee not resolved to a zero-degree definition is emitted exactly
once per call-site identity into the first-degree candidate ledger. Therefore
the zero-degree definition ledger and first-degree candidate ledger are
mutually exclusive by construction. A source-preserved function selected as
not-host-runtime is not deleted from the historical mirror; “delete” means
remove it from the eventual host-runtime recovery/link selection only.

## Candidate 2: first degree

Candidate 2 consumes the frozen first-degree ledger. Each row resolves to one
of: exact selected/original external definition; modern public Win32/CRT leaf;
declaration-only hard boundary; finite shallow replacement; or explicit
unresolved/source-unavailable result. It records path/hash/line, package,
final recovery disposition, owner and any outgoing call expression. A newly
found non-leaf outgoing call is recorded as a prospective second-degree edge,
not expanded or implemented in this package.

## Leaf rule

A function is a leaf only if it has no direct function call; calls only a
modern public Win32/CRT API; or calls a separately documented shallow
dependency whose entire replacement is finite and does not introduce a new
historical package. A macro/function-pointer spelling is not automatically a
leaf: its actual target or non-call status must be documented.

## Verification and limits

Both packages scan only provenance-selected source files and approved original
OpenNT trees. They mask comments, strings and preprocessor forms; distinguish
declarations from definitions; preserve source line and SHA-256; and assert
cardinality and absence of unclassified live rows. `src.old` is never an
input. Neither audit imports, compiles, links or enables historical source.
