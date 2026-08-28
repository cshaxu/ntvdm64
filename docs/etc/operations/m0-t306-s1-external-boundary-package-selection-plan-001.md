# M0 T306 S1 — MVDM external-boundary package selection

## Objective

Turn the existing physical direct-exit evidence (`zero-ledger1` and
`one-ledger1`) into a finite, source-based adoption and adapter worklist.
Every direct non-MVDM call identity must receive one final planning
disposition: public Win32/CRT leaf, small local closure, importable non-MVDM
OpenNT package slice, named adapter family, or non-invasive hard boundary.
An import decision is complete only when it names the exact original files and
interfaces that preserve MVDM's source shape, their finite outward closure,
the `opennt-host` target root, and any current source-derived workaround that
must be migrated, retained, or deleted.

## Required work

1. Validate the ledger's physical caller/callee identity rule and remove no
   rows by spelling-only merge.
2. Partition direct exits by original source location, declaration form,
   linkage and package root; distinguish public leaves and source-absent forms
   from import candidates.
3. For each import candidate, breadth-first inspect its required original
   slice until every reached branch stops at public Win32/CRT, a demonstrably
   small local closure, a named adapter family, or a hard boundary.  Record
   source file/path, reached exported forms, declaration/layout requirements,
   required imports, proposed component root, and the reason no larger parent
   package is imported.
4. Reconcile existing Base VDM, monitor, SoftPC, Win32 and app-derived seams:
   record whether each can be deleted/migrated when an original package is
   selected.  Do not implement the selected provider in this S.  Do not turn
   a package decision into a new generic compatibility layer.

## Non-goals

No BOP service implementation, source-body import, adapter behavior, guest
trace, kernel/CSRSS recreation, second SoftPC executor or generic replacement
API is admitted.  The scan does not recursively classify all of OpenNT beyond
the documented stopping forms.

## Exit

The direct external frontier has a complete final disposition; every proposed
package has an exact required source slice and bounded external closure; and
the resulting package/adapter worklist is sufficient to order further source
recovery without a new global BFS gate.  Later owner tasks may inspect their
own immediate edge for implementation, but may not reopen perimeter discovery
as a substitute for recovery.
