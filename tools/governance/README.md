# Governance tools

These PowerShell tools generate or verify tracked governance evidence.  Their
inputs are repository authorities and retained source inventories; their
outputs are either a caller-selected `docs/etc/` evidence record or the
generated documentation inventory.  They do not compile runtime code, create
runtime artifacts, or create a disposable `build/` tree.

`Export-T219S2BoundRouteLedger.ps1` consumes the frozen T200 203-entry
canonical inventory and emits a complete current source-route reconciliation
ledger for T219 S2.
`Export-T226S1BopConformanceManifest.ps1` consumes the frozen T225 S57
203-endpoint owner ledger and emits the tracked T226 S1 owner-package/local-
fixture contract. `Test-T226S1BopConformanceManifest.ps1` regenerates that
manifest in a temporary location and rejects stale, duplicate, unbound, or
profile-incomplete entries. Neither tool compiles or runs guest code.
