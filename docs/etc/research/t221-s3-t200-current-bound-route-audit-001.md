# T221 S3 P29: T200 current bound-route audit

## Question

Do all 203 OpenNT BOP identities retained by T200 still have an evidenced
current executable source route, and which of those routes now contain a
later shim, an obsolete fallback, or a shared host-mutation profile path?

## Inputs and procedure

- The frozen 203-row T200 identity set and T219 reconciliation in
  [T219's ledger](t219-s2-t200-bound-route-reconciliation-001.json).
- The current generic bridge, boot and native compositions, and the DEM,
  COMMAND, XMS and DPMI package sessions.
- Run:

  ```powershell
  powershell.exe -ExecutionPolicy Bypass -File `
    tools/governance/Export-T221S3T200CurrentRouteAudit.ps1 `
    -RepositoryRoot O:\repos.hobby\ntdos64 `
    -ReconciliationPath O:\repos.hobby\ntdos64\docs\etc\research\t219-s2-t200-bound-route-reconciliation-001.json `
    -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s3-t200-current-bound-route-audit-001.json
  ```

The exporter rejects a non-203-row or non-unique identity set and requires
the present bridge-to-composition and composition-to-owner-session symbols.
It emits one source-proofed route record for each identity. This is static
source evidence: it neither runs a guest nor upgrades a bound route into a
provider-compatibility or runtime-success claim.

## Observations

The generated [203-row audit](t221-s3-t200-current-bound-route-audit-001.json)
contains 125 `confirmed`, 77 `changed`, and one `confirmed-with-gap` route.

- DEM has 31 current route labels that select the whole-provider seam. The
  admitted Direct, Readonly and Overlay subfamilies use its selector-blind
  profile/view owner where their own package contract permits; Virtual remains
  explicit unavailable. The count includes routes whose detailed failure or
  pipe disposition remains open, so it is not a statement that the complete
  DEM package is closed.
- COMMAND remains routed through one package session, XMS through one native
  package session, DPMI through the explicit-stop package session, Redirector
  through the reachable unavailable family, and debugger through its deferred
  facade. None is upgraded from a typed route to a complete historical
  provider.
- `50:3C` remains the one confirmed gap: it only tears down finite boot
  namespace state and cannot release Direct file/search resources because the
  necessary guest PDB/JFT/SFT ownership transfer is absent.
- 65 entries still expose a retained `cli-unavailable` terminal/fallback
  branch. Those remain source-visible package debt, not silently successful
  compatibility behavior.

## Conclusion and follow-up

T200's owner/package classification remains usable; its old endpoint-local
and fixture-only descriptions are superseded where the 77 changed rows name
later composition shims. The audit also confirms that subsequent work must
remain package-owned: no row authorizes trace-led BOP patches, generic
fallback success, or an adapter-local DOS implementation.

T218 is already closed and is not reopened by this audit. Its conclusion that
`$Exec` is guest DOS work remains intact. The active work therefore continues
under T221 S3's admitted DEM provider package, with this audit as a current
all-family guard; any later guest-execution work requires a newly admitted
task rather than reusing a closed identifier.
