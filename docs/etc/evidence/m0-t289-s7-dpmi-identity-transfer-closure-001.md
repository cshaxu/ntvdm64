# M0 T289 S7 — DPMI identity, exception and transfer closure

S7 closes T289's source-identity and deferred-transfer exit gate; it does not
claim that protected-mode DOSX execution is enabled.

`Test-T289S7DpmiMirrorAudit.ps1` proves that all 25 selected DPMI paths are
byte-identical between the two canonical OpenNT editions.  Nineteen current
mirror files are exact.  The remaining six modified mirror files carry their
in-file `DIVERGENCE(...)` marker and map to the eight registered
`MVDM-HOST-DIV-012` through `MVDM-HOST-DIV-019` entries in the mvdm-host
README.  The audit additionally rejects direct Bochs/historical-product
references in the DPMI mirror and a mapping-manager bypass in its overlay.

Durable DPMI host-resource identities go only through the session-owned
`mvdm_host_identity` facade.  The source mirrors retain raw historical alias
forms only where their bodies remain selector-disabled; those forms are source
evidence, not product-visible pointers.

Every S6 unavailable selector now has one named future owner in the
[identity, exception and transfer ledger](../operations/m0-t289-s7-dpmi-identity-exception-transfer-ledger.tsv):

- `53:01`, `53:03`, `53:06`, and `53:14..18` transfer to the already queued
  protected runtime/product-composition work, with their narrower monitor or
  DEM/COMMAND requirements recorded there.
- `53:0E` transfers to the queued VDD/debugger package and a future
  selector-blind Bochs debug-register seam.

Therefore T289 has no ownerless DPMI branch.  Its verified result is local
source-body and failure-direction closure on both x86 and x64; a guest BOP
ingress, DOSX executor, host LDT/IDT mutation, raw pointer path, or second CPU
executor remains explicitly out of scope.
