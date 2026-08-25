# M0 T266 S3 — reached OpenNT input recovery evidence

## Inputs and procedure

- Imported 57 exact original inputs into named local owners; the machine-readable origin, target, byte count and SHA-256 ledger is `m0-t266-s3-opennt-input-disposition.tsv`.
- Re-rooted the current guest stager, BOP selector inventory, provider map, DEM provider map and owner manifest away from `refs/opennt`.
- Preserved full original dispatcher sources only as governance parse inputs in `docs/etc/legacy_code/opennt-bop/original/`; they are not production provider bodies.

## Reproducible verification

- `Export-OpenNtBopInventory.ps1` produced `build/M0-T266-S3/r002/inventory/opennt-bop-inventory.json`.
- `Export-OpenNtBopProviderMap.ps1` produced r006 provider map SHA-256 `7a5af515d7671a792cf51c54ef49658a2aa4d9a74ac11898bb5909aeb6ad630e`.
- `Export-OpenNtDemProviderMap.ps1` produced r006 DEM map SHA-256 `c8ee25b3535339b8a0657de023f442abbb92fda1088169100c73328bc8b853a8`.
- Both r006 commands exited 0. The resulting maps are static source mappings only and assert no provider link/runtime behavior.
- `Stage-T235S5GuestBundle.ps1` produced r008: 113 artifacts (68 local original OpenNT, 45 explicit external OpenNT 4.5), manifest SHA-256 `80af5f3a2bc59b49ddf0493e061d6788405058f2c7150b5dd3baa6475e16d4eb`, and no `refs/` path in the manifest.
- `Export-T260S1OwnerManifest.ps1` produced r011: 1,740 rows, SHA-256 `febfe519113f71a4626bfce84107a116793daf36e987b772fa5d3de50cffa7de`, and no `refs/opennt` owner/provenance path.
- Independently compared all 57 ledger targets with their selected local source baseline; all SHA-256 values match.

## Scope boundary

- External OpenNT 4.5 payload inputs remain explicitly external guest-artifact sources; this packet removes only repository-local `refs/opennt` consumers assigned to S3.
- Remaining historical tools, probes, tests and `refs/archive` consumers remain assigned to T266/S4; `refs/` deletion is exclusively S5 work.
