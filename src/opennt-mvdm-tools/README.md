# opennt-mvdm-tools

Canonical original standalone MVDM-tool mirror. Its final retained
upstream-relative package set is `vdmutils`, including `forcedos`, `graftabl`,
`pifedit`, `win`, and their original resources.

The current T276 audit keeps the byte-exact source union temporarily under
`src/opennt-mvdm-host`; no source is copied, changed, linked, or deleted by
this skeleton. A dedicated hash-verified `git mv` packet must establish this
component. Independent tool builds may later be admitted, but this component
may never become a main-program or MVDM host-runtime link input.

## Divergence register

None. No upstream source has entered this root yet.
