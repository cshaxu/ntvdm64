# M0 T293 S1 — zero-degree function audit plan

## Scope

Build the complete current `mvdm-host` component definition graph, including
files already classified tool-only or not-host-runtime so their functions can
receive an explicit final exclusion. The audit reads the provenance-selected
C/C++ files named by the live MVDM file ledger; the current mirror supplies
only component-path membership, never parsed source bytes. It does not treat
`src.old`, compiler output or an alternate OpenNT edition as a definition
source.

## Outputs

- `mvdm-host-zero-degree-definition-ledger.tsv`: one row for each selected
  function definition identity, including path/hash/line, linkage evidence,
  direct call sites, leaf basis and final recovery disposition.
- `mvdm-host-first-degree-candidate-ledger.tsv`: one row for each direct call
  from zero degree whose callee is not a zero-degree definition and is not a
  proven non-function expression. It records the caller definition identity
  and exact line, but does not inspect the callee body.
- `mvdm-host-zero-degree-source-coverage-ledger.tsv`: one row for every
  component C/C++ file, including files with no function definition.

## Identity and resolution

The exporter identifies function bodies only after comment/string/preprocessor
masking. A source definition identity contains selected source path, SHA-256,
definition line, spelling, linkage/storage evidence and call-form evidence.
Same spelling is never enough to merge rows. A direct call resolves internally
only when its selected source definition identity is unambiguous in the
available source context; otherwise it becomes a first-degree candidate with
an explicit ambiguity reason.

## Disposition and leaf rules

Each zero-degree row reuses its enclosing file's selected final disposition as
the initial product disposition, then records whether its body can be retained
byte-exact, binding-only, adapter-backed, overlay-required or excluded from the
host runtime. A leaf has no direct function call, only public modern Win32/CRT
calls, or only a separately recorded finite shallow replacement. Every other
external callee makes the caller non-leaf until the next task proves otherwise.

## Boundary

T293 creates no first-degree definition row, no package edge, no adapter and
no runtime implementation. Candidate 2 alone may inspect the first-degree
function bodies emitted here.
