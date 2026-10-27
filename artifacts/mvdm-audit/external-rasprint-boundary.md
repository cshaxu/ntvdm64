# External rasprint inventory: print-owner boundary

The frozen external WOW audit contains 14,304 rows beneath
`windows/core/ntgdi/printers/rasprint/`; the complete suffix census is
`external-rasprint-suffix-census.csv`, derived from the frozen external
ledger whose SHA-256 is
`949D696E96F088A69EA437A41F15A09D50C8218BA29983A15A7B31E25AF95623`.
This is a path census, not a classification: suffixes alone neither prove
file content nor authorize bulk exclusion. In particular 4,344 `.ifi` and
4,211 `.pfm` paths coexist with C source, driver data, manifests and other
materials. Each relevant feature and provider must still be traced.

T468 S1 (`docs/etc/evidence/m0-t468-s1-printing-spool-owner-matrix.md`)
identifies the selected original WOW32 print conversion and spool entrypoints.
Raw spool operations call modern `WINSPOOL.DRV`, which owns actual native
printer/job resources; device setup similarly crosses the existing spooler
boundary. The `rasprint` directory contains NT4 printer-side material,
not evidence that all of its files are linked into the WOW worker. Conversely
this owner boundary does not establish that modern printer drivers implement
every old printer model, escape, font or device capability.

Next review should separate original guest and WOW print behavior from
individual device-driver resources and trace any actually referenced escape,
font or provider. Existing T468 printing witnesses must be checked for each
claimed semantic result; source-selection, a spooler export or one successful
printer does not establish all devices. The entire 14,304-row group remains
pending relevance assessment and is not silently excluded from the audit.
