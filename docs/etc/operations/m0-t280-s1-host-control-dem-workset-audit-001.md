# M0 T280 S1 — host-control and DEM workset audit

## Purpose and source of truth

This audit turns the final expected-integration disposition of the original
DEM and reached SoftPC host-control paths into the bounded source-owner
workset for M0 T280.  It is an admission record only: no BOP selector,
provider, original body, machine operation, or guest trace becomes enabled by
this document.

The generated
[`workset ledger`](m0-t280-s1-host-control-dem-workset-ledger.tsv) is the
machine-readable source of truth.  It is derived only from
[`T276's final integration disposition ledger`](m0-t276-final-integration-disposition-ledger.tsv),
selecting `dos/dem/*` and `softpc.new/host/src/*`.

## Reconciled population

- 81 total selected paths.
- 22 DEM paths: 16 `adapter-backed`, four `binding-only`, and two
  `not-host-runtime`.
- 59 reached SoftPC host-control paths: 33 `adapter-backed`, six
  `overlay-required`, one `binding-only`, and 19 `not-host-runtime`.
- Across both owner packages: 49 `adapter-backed`, six `overlay-required`,
  five `binding-only`, and 21 `not-host-runtime`.

The T276/S25 document's 25/6/7 count is retained as accurate evidence for its
explicitly smaller population: **38 remaining SoftPC control bodies**.  T280
uses a wider final source-path population of 59 host paths.  Its extra 21
paths account for eight adapter-backed paths, one binding declaration, and 12
historical/non-runtime paths.  Therefore 25 versus 33 is a scope difference,
not permission to discard either classification.

## Implementation order and boundaries

1. `01-bop-ingress-control`: the original `nt_bop.c` host-control entry shape
   is reached only through `adapter-bop`; Bochs remains selector-blind.
2. `02-softpc-machine-facade`: original SoftPC/CCPU/SAS call forms bind only
   to `adapter-softpc`, then `adapter-bochs`.  No OpenNT body receives a
   Bochs object or starts a second CPU executor.
3. `03-dem-*`: original DEM dispatch/data, namespace/file, and device/state
   bodies retain their source control flow.  Numeric guest addresses use the
   session guest-memory mapping-manager instance only at their actual
   VDM-address boundary; host-local temporary pointers never do.
4. `04-*`: host event/stop and public capability bodies bind through
   `adapter-win32` and `session` only where the original declaration proves
   the boundary.
5. `05-machine-transport-overlay`: six original bodies remain mirror
   evidence until the matching overlay can replace direct CPU/RAM/page
   transport with typed `adapter-softpc -> adapter-bochs` operations.

The 21 `not-host-runtime` paths remain source evidence and are never made
linkable merely to improve a count.  `binding-only` paths carry declarations;
they do not authorize a provider implementation.

## S2 admission preconditions

S2 may begin only after this ledger and its verifier pass.  Its first product
deliverable must be a complete source-shaped declaration/import map for one
bounded group, including the original call form, named adapter, mapping owner,
and explicit unavailable outcome.  It must not create a trace-selected
single-service shim.
