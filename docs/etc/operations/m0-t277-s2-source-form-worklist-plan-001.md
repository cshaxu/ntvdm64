# M0 T277 S2 — source-form reachability worklist plan

## Why this narrowing is valid

T277 S1 preserved 3,697 caller/declaration name correlations. Most belong to
final `not-host-runtime`, `guest-only`, `tool-only` or `firmware-only` source
paths. They remain evidence but cannot decide the next host recovery package.
S2 selects only callers whose final file disposition is `adapter-backed`,
`overlay-required` or `binding-only`; it does not claim any of those callers
are linkable.

## Required result

The worklist has one stable row per future-host caller correlation with the
original caller and declaration location, final file disposition, correlation
state and a package-order key. The companion summary accounts for the complete
3,697-row population, including excluded/deferred rows.

## Review order

1. `suballoc` support forms;
2. `xms.486` and `dpmi32` machine-bound forms;
3. `dos` DEM/COMMAND forms;
4. `softpc.new/host` control forms;
5. `vdmredir` and `wow32` product-bound forms.

This is an evidence order, not permission to implement a package before the
source-form, ABI and failure review closes.
