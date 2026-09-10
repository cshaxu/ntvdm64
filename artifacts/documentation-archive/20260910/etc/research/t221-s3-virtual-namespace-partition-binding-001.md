# T221 S3 P36: Virtual namespace-partition binding

## Question

Can the completed Virtual private namespace be selected by the existing
`demfile.c`/`demdir.c` partition as one provider view, rather than being
implemented as separate BOP exceptions?

## Inputs

- `src/opennt/overlay/base/mvdm/dos/dem/demfile.c` and `demdir.c` owner
  families represented by `01,03,04,05,06,12,17,22,44`.
- T221 S2 shared view contract and P30--P35 Virtual private-root foundations.
- Existing Overlay partition binding, used only as an ownership/dispatch
  shape; no Overlay host resolver is imported into Virtual.

## Procedure

Added the Virtual private metadata backend and selected it, together with the
existing Virtual open/create and mutation backends, from the one namespace
partition after the package file-view selection.  The selected paths have no
host namespace, path or handle argument.  The existing whole-provider fixture
now exercises private directory creation, private file create, attribute
set/query, check-path, rename, delete and directory removal under the Virtual
profile.

## Observations

`Invoke-T202S4DemWholeProviderProbe.ps1` passed in
`build/M0-T221-S3/112` with MSVC x64 `/MT`.  The test retains Virtual FCB
wildcard refusal because that distinct FCB owner package is not yet bound.

## Interpretation

The namespace/path subfamily now has a coherent Virtual provider route.  This
is not whole-provider closure: typed virtual handle I/O, FCB lifecycle and
search enumeration/continuations still need their own package-level binding
and regressions.  CLI/engine activation and a native trace also remain out of
scope.

## Follow-up

Bind `00,02,08,16,1E,27` through the existing opaque `VIRTUAL_FILE` token
kind as one handle-I/O subfamily, then proceed to the FCB and search subfamilies.
