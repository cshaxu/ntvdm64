# M0 T263 S7 P2 — unconnected OpenNT host-package withdrawal

Date: 2026-08-24

## Correction

T263/S5 imported seven original OpenNT host extension packages solely as
source-identical preservation. The owner has corrected that admission: an
original host package does not belong in `src/opennt-host` until its provider
is actually admitted for composition and has a source/ABI/failure plan.

The following 138 tracked files are therefore withdrawn from the production
component, not deleted from the historical source baseline:

- Debugger packages: `bde` (13), `dbg` (3), `vdmdbg` (6), `vdmexts` (25).
- VDD packages: `vdd` (47), `ieuvddex` (23).
- Host extension package: `fax` (21).

The authoritative original copies remain at
`O:\repos.external\opennt\base\mvdm\{bde,dbg,fax,ieuvddex,vdd,vdmdbg,vdmexts}`
at Git `5c5b979e`. Future owner-package work must re-import only the reached
source subset as part of the admitted composition packet, then record every
necessary divergence in `opennt-host/README.md`.

## Result

`src/opennt-host` now contains only the established original host capability
bodies and their declared minimal source subsets. It contains no dormant VDD,
debugger or fax product package merely for convenience or source preservation.
T263/S5's hash record remains historical provenance evidence but is superseded
as a source-component admission by this record.
