# M0 T310 S4 P4: initialization-group interface disposition

## Input and method

This pass consumes the two formal P3 `/WHOLEARCHIVE` logs and records each
physical unresolved form in a separate architecture ledger:

- `m0-t310-s4-p4-x86-forced-link-ledger.tsv` — 264 forms.
- `m0-t310-s4-p4-x64-forced-link-ledger.tsv` — 263 forms.

The exporter resolves by physical library/object/caller/symbol rather than by
symbol spelling alone. It consults the selected original MVDM source ledger,
then applies source-family classification. The P4 output is deliberately
separate from the S2 ledger because P2 added original BIOS/keymouse membership
and the old S2 classifications assumed the pre-T310 Bochs-only executor plan.

## Result

Every physical form now has one source-first disposition. Counts are stable
between architectures except for one public host boundary form.

| Disposition | x86 | x64 | Meaning |
| --- | ---: | ---: | --- |
| select original MVDM source | 131 | 131 | select the physical original MVDM provider package before writing an adapter |
| select original SoftPC machine source | 57 | 57 | direct original machine owner for a SoftPC-selected session; no Bochs substitution |
| select original SoftPC host-control source | 39 | 39 | select original control/provider package before a public-API binding |
| select original SoftPC BOP source | 16 | 16 | retain original `nt_bop` dispatcher ownership; no parallel handler |
| select original MVDM provider | 7 | 7 | DEM/COMMAND/XMS/debug/config provider package selection |
| select original XMS source | 4 | 4 | original XMS flow first; only memory/A20 seam may be adapter-backed |
| adapter Win32/host boundary | 6 | 5 | named public-API/host contract (`ExitVDM`, `GetNextVDMCommand`, process/loader/TEB forms) |
| debugbreak patch default | 4 | 4 | retained `fmstubs.c` immediate-break contract; not a runtime replacement |

No row remains `exact-owner-review`. This classifies source ownership; it does
not mark any currently absent package runnable or authorize an interface stub.

## Next package order

The first direct composition candidate is the original PIC/PIT/IOS path
(`base/system/ica.c`, timer/IOS owner sources), because it is now reached by
both CCPU and the newly linked keyboard/mouse source. Its package audit must
first establish its original initialization order, source membership and all
new outbound forms. It may not bind Bochs for the SoftPC-selected path.
