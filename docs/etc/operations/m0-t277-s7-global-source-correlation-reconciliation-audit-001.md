# M0 T277 S7 — global source-correlation reconciliation audit

The effective source-correlation ledger now gives every one of the 3,697
original T276 S5 caller/declaration correlations exactly one current tracker
state:

- **325 `reviewed-future-host`** records join exactly one completed S3, S4,
  S5 or S6 source-form family.  Their provider form, owner, mapping rule and
  failure disposition remain implementation-disabled evidence.
- **3,372 `non-runtime-or-profile-excluded`** records retain their original
  source path and final file disposition (`firmware-only`, `not-host-runtime`
  or `tool-only`).  They are not silently dropped, but no provider may claim
  them in the current host graph.
- **4,803 lexical declarations** remain separately recorded in the interface
  ledger. They are not reached merely because a same-spelling caller or
  definition exists.

The first run exposed that all family ledgers correctly retain the stable S2
work-item IDs. The reconciliation generator now explicitly joins that stable
identity to the original T276 `MVDM-BOUNDARY-*` identity; it does not invent
new per-S identity variants.

This closes source-correlation status reconciliation. It does not yet declare
package-edge topology or x86/x64 compile/profile enablement; those are the
next independent tracker layers.

## Reproducibility

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T277S7EffectiveCorrelationLedger.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S7EffectiveCorrelationLedger.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
