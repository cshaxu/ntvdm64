# M0 T291 S1 — Original WOW owner-package source/ABI admission audit

## Purpose

Freeze the complete original `mvdm-host/{wow32,fax}` host-owner surface before
any WOW body, broker, GUI or guest route is enabled. This is a whole-package
source audit, not a trace-selected thunk implementation.

## Inputs

- The selected canonical union mirrors under `src/mvdm-host/wow32` and
  `src/mvdm-host/fax`.
- Existing complete-file disposition baselines for WOW32/FAX, the Wave 8
  plan, the closed COMMAND, monitor, DPMI and Redirector records, and source
  policy.
- Current `session`, `adapter-mvdm-host-out/{win32,softpc,wow}` and
  `adapter-bochs` public boundaries.

## S1 work

1. Enumerate selected active WOW32/FAX translation units, declarations,
   resources and inactive product forms, preserving original package topology.
2. Map each reached historical external call and callback to one of: public
   current Win32 direct use, same-shaped outgoing adapter, selector-blind
   Bochs mechanic, session lease/identity, broker record, private historical
   product dependency, or a currently unresolved boundary.
3. Record every guest pointer, persisted pointer and 16/32-bit handle crossing
   against the appropriate existing session mapping-manager instance.
4. Classify the original owner subdomains and dependency order for the later
   code-complete packages. No imported body is rewritten or enabled in S1.

## Explicit exclusions

No generic WOW/Win16 API implementation, WOWEXEC/CSRSS recreation, GUI/USER/
GDI policy, selector ingress, guest load, BOP service, raw native identity,
new mapping manager, direct Bochs call, or `src.old` input.

## Exit evidence

A reproducible file/subdomain/interface ledger gives every selected WOW32/FAX
item one package role and one final source-recovery disposition, names all
external boundaries and mapping instances, and identifies the smallest
admissible next original-owner group. The evidence must distinguish a
source-preserved unavailable historical dependency from a merely unimplemented
current feature.
