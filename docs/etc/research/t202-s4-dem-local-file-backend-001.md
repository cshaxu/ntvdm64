# T202 S4: DEM local-file backend composition

## Question

Can the admitted DEM file-provider substrate compose one bounded DOS path,
one selected real-host root, and one adapter-private opaque file token without
adding an endpoint BOP handler or bypassing the shared mutation profile?

## Inputs and procedure

- T202 S2 admitted-root `bx_ntvdm_host_namespace_v1`.
- T202 S4 bounded OEM path resolver and opaque file-token session.
- T201 shared mutation profile and DEM owner registration.

`bx_ntvdm_dem_local_file_backend_v1` accepts only an OEM DOS path, copied
access/disposition values and an adapter-private session. It resolves the path
before host access, opens only below the admitted root, and immediately adopts
the returned `HANDLE` into the session token table. It never returns a host
handle or pointer.

For creation or write, it resolves DEM's `NAMESPACE_CONTENT` mutation class
through the common profile: direct continues; readonly returns its explicit
refusal; overlay and virtual return their explicit not-yet-backed outcomes.
Readonly reads retain ordinary root-relative host access; an unadmitted or
invalid path produces no token.

`Invoke-T202S4DemLocalFileBackendProbe.ps1` builds all involved units and a
focused real-host fixture with MSVC x64 `/MT`.

## Observations

`artifacts/build/t202-s4-dem-local-file-backend-r1` compiled and returned
zero. The fixture creates a temporary real-host file, opens and reads it
through the direct backend, then separately proves:

- readonly read opens the same admitted file and yields an opaque token;
- readonly write returns `READONLY` with token zero;
- overlay write returns `OVERLAY` with token zero until the shared overlay
  receives a file backend;
- virtual access returns `VIRTUAL` with token zero; and
- a parent-directory escape is rejected before a token is issued.

## Interpretation and follow-up

This is a single DEM package substrate, not a `50:xx` implementation and not
a substitution for the remaining FCB/search/error contracts. It uses direct
modern user-mode file access under the CLI-admitted host policy, which is
consistent with the project's non-invasive definition: no Windows component,
driver, service, system file, or installation-style Registry change is
required.

The next S4 work must compose this backend with checked guest gather/write
transactions and the complete 29-identity owner provider, including source
shaped failure forms, search continuations and the explicit `50:47/48`
Redirector defer outcome. No native trace is admitted before that whole
package regression passes.
