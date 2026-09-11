# Proposal: CPU40 BOP entry-contract audit

## Decision sought

Classify every selected `HOST_BOP_IP_FUDGE` use before any further BOP repair.
The audit must distinguish a host-constructed BOP/trampoline entry from a
guest-registered direct callback or continuation. It does not authorize a
global removal or restoration of the two-byte BOP convention.

## Scope

For every production caller and definition reachable in the selected x86
CPU40 composition, record the source owner, purpose, operand origin, whether
the address is host-constructed or guest supplied, CPU40 fetch/entry shape,
and disposition:

- retain the BOP adjustment only when an actual preceding BOP/trampoline is
  demonstrated;
- retain direct entry for a guest-registered callback; or
- mark the route unresolved and make no change.

The newly delivered INT 33h `mouse_io.c` entry is the confirmed direct-entry
reference case. CPU30, generic CCPU changes, guest binary rewriting, and
inserting BOP bytes into guest memory are excluded.

## Evidence bar

Use selected original source plus the CPU40 build's real macros/call graph.
Where source alone does not prove an entry convention, inspect the responsible
host stub construction or bounded runtime bytes; do not infer a BOP from the
name of a constant. The output is an indexed audit record and a proposed
per-caller disposition, not a behavioral patch.
