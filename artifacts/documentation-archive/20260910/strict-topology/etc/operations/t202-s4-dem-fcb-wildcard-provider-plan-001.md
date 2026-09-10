# T202 S4 DEM FCB wildcard provider plan

## Scope

This plan closes the remaining `demfcb.c` namespace subpartition within the
already admitted T202/S4 provider: `50:07` (`demDeleteFCB`) and `50:20`
(`demRenameFCB`).  It is neither a new T/S packet nor a BOP ingress switch.
Both services remain provider-internal until the single remaining FCB/pathname
package switch permitted by the T202/S4 whole-provider plan.

## Original contract

| Service | Original input | Original behavior retained by the provider |
| --- | --- | --- |
| `50:07` | copied `ES:DI` FCB wildcard pathname; `AL` extended-FCB flag; `DL` extended attribute mask | Enumerate every 8.3 match below one admitted root. A normal FCB skips read-only, hidden and system entries. An extended FCB admits only requested special attributes; it clears read-only before deleting an admitted match. Success if at least one deletion succeeds. If nothing succeeds, return access denied when filtering rejected at least one candidate, otherwise file-not-found. |
| `50:20` | copied `DS:SI` source wildcard; copied `ES:DI` destination wildcard template | Enumerate every source 8.3 match. Form a destination basename by the original `?`/`.` substitution rule, reject an identical result with access denied, and rename each matched source only within the same admitted drive. Exhaustion after successful enumeration is CF clear; path/enumeration and host rename errors retain source-shaped failure. |

The evidence source is `src/opennt/base/mvdm/dos/dem/demfcb.c`:
`demDeleteFCB` and `demRenameFCB`.  The source's `GetVDMAddr`, OEM find
handles and host paths are not usable composition inputs.  The new provider
instead consumes copied bounded bytes, `bx_ntvdm_host_namespace_v1_enumerate`,
and root-relative namespace operations only.

## Boundary and disposition

- Parse an OEM absolute DOS wildcard path through the existing strict 8.3
  search decoder. Wildcards are accepted only in the final 8.3 component;
  reparse traversal, ambient CWD, long-name fallback and host absolute paths
  remain rejected.
- Enumerated `dos_name` values are the only match candidates. Build each
  root-relative source/destination from the already-validated directory and
  8.3 candidate; no host path or handle crosses bx-vdm.
- Namespace content is resolved once through the shared DEM mutation profile:
  direct performs the recorded host operation; readonly returns `AX=5,CF=1`;
  overlay and virtual return their explicit currently-unavailable
  `AX=1,CF=1` result. No direct-host write is permitted in either latter mode.
- Volume-label deletion (`extended FCB` with `DL=08`) is not represented by
  the admitted root namespace. It returns the explicit source-derived
  unavailable result rather than pretending that a file is a volume label.
- The partition owns no BOP window, RAM access, CPU mechanics, host handle,
  search continuation, or legacy route. Its caller supplies only copied
  input/output records.

## Required regression

One source-built MSVC x64 `/MT` whole-provider fixture must create temporary
8.3 host entries and prove all of the following before this subpartition is
called complete:

1. normal-FCB wildcard delete removes ordinary candidates while preserving
   hidden/system/read-only candidates;
2. extended-FCB delete accepts the requested special attributes, clears a
   read-only candidate only to delete it, and reports file-not-found versus
   access-denied with the original aggregate rule;
3. wildcard rename applies `?` substitution and rejects an identical source
   and destination in the provider; the fixture proves substitution and that a
   cross-drive request fails before any namespace operation;
4. readonly, overlay and virtual profiles cause no host deletion, rename or
   attribute mutation; and
5. no raw handle, host path, guest pointer or BOP selector is present in the
   subpartition ABI.

After those checks, `50:07/20` joins the existing FCB local-handle subset in
the final package regression. It still does not authorize an independent
ingress route or a native trace.

## r33 evidence

The source-built MSVC x64 `/MT` whole-provider fixture r9 now supplies this
subpartition's provider-internal evidence. It creates temporary 8.3 entries
below one admitted real host drive; normal FCB delete removes only ordinary
matches, extended delete separately removes hidden and read-only matches, and
the read-only attribute is cleared only immediately before deletion. It also
proves wildcard rename substitution, rejects a cross-drive target with
`AX=17,CF=1`, and proves that readonly, overlay and virtual profiles leave the
real entries in place with their specified errors. The implementation's
per-candidate identical-name rejection is source-derived, but a separate
short-name-enumeration witness is still required before calling that edge case
fixture-covered. This is not an ingress switch or complete FCB closure.
