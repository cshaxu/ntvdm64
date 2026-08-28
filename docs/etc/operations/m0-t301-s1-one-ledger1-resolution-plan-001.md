# M0 T301 S1 P18 — `one-ledger1` Physical Definition Resolution Plan

## Admission

The owner directed continuation after the true-zero correction and approved
the `one-ledger1` workset.  The complete historical zero ledger remains
immutable; `zero-ledger1.tsv` is the selected 3,720-definition source-graph
input.  Its four bodies that need separate historical syntax/conditional
handling remain explicit in `one-ledger1-unparsed-zero-body-ledger.tsv`.

## Objective

For every `outside-zero-ledger1` direct call, find physical function-definition
candidates in the approved OpenNT/OpenNT-4.5 union, or record that no original
source body is available.  Preserve the call-site identity and every candidate
file identity; do not merge by spelling.

## Method

1. Build or validate a reusable C/C++ definition index from the two approved
   original source roots.
2. Resolve only the spelling at each physical `one-ledger1` call site against
   that index and record every matching source root/path/hash/line/linkage
   candidate.
3. Record separately whether the candidate lies in retained MVDM, a
   Bochs-replaced/extracted SoftPC region, another OpenNT package, or has no
   original C/C++ definition.  These are source facts, not provider choices.
4. Keep `zero-ledger1-binding-ambiguous` rows out of the external-body pass;
   they require original declaration/linkage binding before degree assignment.

## Non-goals

No callee body traversal, second-degree ledger, package import, adapter or
overlay implementation, build/link admission, BOP enablement, guest trace,
runtime route, or Bochs change.

## Acceptance

Every true external call has one source-resolution row and zero or more
physical definition-candidate rows.  Every row retains original caller and
candidate identity.  The result explicitly separates source absence from
public platform/API disposition to be decided by the following package/ABI
audit.
