# M0 T277 S4 — DOS DEM/COMMAND source-form family audit

S4 groups all 73 original `dos` host caller correlations by eight source-form
families. The grouping is determined by original callee body and call shape,
not by BOP selector or trace order.

- 24 rows are session controlled-termination calls. Their historical
  standalone process body is excluded; the successor is a session typed
  controlled stop with caller-visible ordering retained.
- Three rows are top-level direct-device error calls. They retain the original
  reason code and per-thread duplicate-suppression contract through
  `nt_error`, public Win32 UI and session controlled stop.
- 20 rows are ordinary CRT allocation/environment calls and retain their
  existing caller-side `NULL`/absent-value paths.
- 14 rows are COMMAND event, standard-stream notification, or console-exit
  calls. They are session-local plus public Win32, with private VDD, WOW and
  CSRSS product paths explicitly deferred to their named adapters.
- Four rows are original local pipe EOF/data queries. The original
  `cmdredir.c` provider remains the owner; stream identity is resolved only by
  the session `host_resource` mapping instance, while Redirector protocol is a
  named later owner.
- Five rows are host device reset/cleanup calls; host resource state remains
  session owned. Two bounded-machine calls reuse S3's sole machine-run
  disposition.
- Two rows initialize original LPT flush state.
- One row is the original package-internal `CMDInit` call.

No source body is enabled. The generated ledger preserves all 73 stable S2
work IDs and records the original provider form, unique successor, mapping
rule and source-derived failure disposition for each.

## Reproducibility

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T277S4DosFamilyDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S4DosFamilyDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
