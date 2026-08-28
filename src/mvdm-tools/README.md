# mvdm-tools

Canonical original standalone MVDM-tool mirror. Its final retained
upstream-relative package set is `vdmutils`, including `forcedos`, `graftabl`,
`pifedit`, `win`, and their original resources, plus the independent
`v86/util/QUIT.COM` assembly utility.

The current T276 audit keeps the byte-exact source union temporarily under
`src/mvdm-host`; no source is copied, changed, linked, or deleted by
this skeleton. A dedicated hash-verified `git mv` packet must establish this
component. Independent tool builds may later be admitted, but this component
may never become a main-program or MVDM host-runtime link input.

T301 S1 P21 additionally preserves missing byte-exact `opennt-src-2` tool
products under their original-relative paths.  They are retained only as
historical tool evidence and remain outside the main-program link graph; see
[`m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv`](../../docs/etc/operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv).

T310 S6 moved `v86/util` here byte-for-byte from the former host mirror. It is
not a host-runtime input.

## Divergence register

None. All files in this mirror remain byte-exact upstream sources.
