# M0 T306 S1 — MVDM external-boundary package selection

## Objective

Turn the existing physical direct-exit evidence (`zero-ledger1` and
`one-ledger1`) into a finite, source-based import and adapter worklist.  Every
direct non-MVDM call identity must receive one final planning disposition:
public Win32/CRT leaf, small local closure, importable non-MVDM OpenNT package,
named adapter family, or non-invasive hard boundary.

## Required work

1. Validate the ledger's physical caller/callee identity rule and remove no
   rows by spelling-only merge.
2. Partition direct exits by original source location, declaration form,
   linkage and package root; distinguish public leaves and source-absent forms
   from import candidates.
3. For each import candidate, breadth-first inspect only until public
   Win32/CRT, a demonstrably small local closure, a named adapter family, or a
   hard boundary.  Record files, reached exported forms, required imports and
   proposed component root.
4. Reconcile existing Base VDM, monitor, SoftPC, Win32 and app-derived seams:
   record whether each can be deleted/migrated when an original package is
   selected.  Do not implement the selected provider in this S.

## Non-goals

No BOP service implementation, source-body import, adapter behavior, guest
trace, kernel/CSRSS recreation, second SoftPC executor or generic replacement
API is admitted.  The scan does not recursively classify all of OpenNT beyond
the documented stopping forms.

## Exit

The direct external frontier has a complete final disposition, every proposed
package has a bounded external closure, and the resulting package/adapter
worklist is sufficient to order further source recovery without a new global
BFS gate.
