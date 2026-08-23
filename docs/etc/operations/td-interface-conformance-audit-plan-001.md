# Td S2 P2 — BOP Interface-Conformance Audit Plan

## Purpose

Audit every canonical BOP and every tracked supporting dependency against the
OpenNT-shaped compatibility-facade rule adopted in Td S2 P1. This is an
inventory and disposition task. It does not add a BOP route, change a mirror,
or make a machine capability available.

## Authoritative inventory

The audit overlay lives in `docs/etc/bop-list.md`. It covers all 203 canonical
BOP rows and all 118 `BOP-DEPENDENCY-*` rows.  P3 adds dependency 118 because
the four historical guest-pointer APIs are a shared interface family, not a
family-private implementation detail.  A row may not be called
interface-conformant merely because its provider returns a compatible result.

Each row receives these five fields:

| Field | Required content |
| --- | --- |
| Interface disposition | `direct-public-api`, `opennt-shaped-facade`, `registered-exception`, `migration-debt`, `deferred`, or `not-applicable-guest`. |
| Original interface evidence | Exact original function/macro/structure and source call site, or `none` for a pure guest artifact. |
| Divergence | Pointer, signature, layout, ordering, lifetime, host API, or failure-semantic difference; `none proven` is permitted only with a citation. |
| Exception | Registered recovery/intrusion record ID, or `none`. |
| Migration conclusion | `retain-facade`, `retain-generic-mechanics`, `migrate-facade`, `replace-with-mirror`, `delete`, or `deferred-owner-package`. |

## Execution sequence

1. Establish the overlay schema and derive its full 321-row inventory from the
   existing tracker without changing any current BOP status claim.
2. Audit shared guest-pointer, handle/token, CPU-result and generic machine
   records first. These rows are common prerequisites and may not be hidden
   inside a service-family conclusion.
3. Audit DEM and COMMAND mirrors, then XMS and DPMI source groups.
4. Audit Redirector, WOW16, Debugger/VDD and top-level/machine rows, retaining
   explicit unavailable/deferred dispositions rather than inventing facades.
5. Reconcile cardinality, every listed exception and every non-mirror route;
   record the frozen T257 S5 disposition and publish a closure matrix.

## Boundaries

- `GetVDMAddr` / `Sim32*` use only the Td S2 P1 bounded mapping-lease model.
- A compatibility facade preserves the original source call shape, but does
  not recreate a CCPU/SAS/CSR/NTVDM product shell.
- A newly-shaped `bx_ntvdm_*` interface requires an explicit recovery
  exception. Existing interfaces are audit subjects, not precedent.
- `bx-core` and `bx-mantle` remain selector/DOS/BOP/OpenNT blind.
- The audit does not delete old code; a `delete` conclusion identifies a later
  owner-package cleanup action.

## Exit criteria

Td S2 P2 closes only when all 321 rows have exactly one disposition and one
migration conclusion; every `registered-exception` cites an existing record;
every direct/facade claim cites an original interface; and every row left
deferred identifies its receiving owner package. A cardinality check, the
documentation gate and `git diff --check` must pass. The closure may then
re-admit or reject T257 S5; it does not itself implement the DPMI frame path.
