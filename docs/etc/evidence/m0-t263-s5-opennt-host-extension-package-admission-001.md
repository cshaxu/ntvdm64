# M0 T263 S5 — OpenNT host extension-package admission

Date: 2026-08-24

## Decision

The following original OpenNT MVDM host product packages are retained
source-identically below `src/opennt-host/`:

- `bde` (13 files), `dbg` (3), `vdmdbg` (6), `vdmexts` (25): debugger and
  debugging extension packages.
- `vdd` (47), `ieuvddex` (23): VDD and VDD extension packages.
- `fax` (21): historical host extension package.

They are optional original provider packages, not evidence that their NT4
product shells are linked, available or enabled. A runtime feature requires a
future owner-package admission and its own source/ABI/failure evidence.

## Provenance and verification

- Source root: `O:\repos.external\opennt\base\mvdm`, Git `5c5b979e`.
- Target root: `src/opennt-host/` with each original package directory and
  internal relative layout retained.
- Count: 138 files across seven packages.
- Verification: recursive file-count and SHA-256 equality was checked for
  every upstream-relative path after copy. There are no local changes and no
  new `HOST-DIV` row.

## Boundary result

The packages belong to `opennt-host` because they are original host product
extensions, even where they supply debugger support. They are not placed in
`bx-mantle`, `adapter-softpc`, or an unapproved generic adapter. Their
presence creates no Ninja target and no default runtime dependency.
