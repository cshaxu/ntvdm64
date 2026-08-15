# Governance tools

These PowerShell tools generate or verify tracked governance evidence.  Their
inputs are repository authorities and retained source inventories; their
outputs are either a caller-selected `docs/etc/` evidence record or the
generated documentation inventory.  They do not compile runtime code, create
runtime artifacts, or create a disposable `build/` tree.

`Export-T219S2BoundRouteLedger.ps1` consumes the frozen T200 203-entry
canonical inventory and emits a complete current source-route reconciliation
ledger for T219 S2.
