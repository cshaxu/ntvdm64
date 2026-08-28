# OpenNT historical build tools

This root contains only provenance-admitted original OpenNT build-tool
closures. The complete 33-path `base/mvdm/tools16/` package is retained below
`tools16/`. Seven paths are byte-identical in both approved OpenNT editions;
the remaining 26 are one-sided OpenNT-4.5 carries. Its current per-file
source/hash provenance is recorded in
`docs/etc/operations/m0-t296-s1-tools16-provenance-manifest.tsv`.

It is not a production source component and must never be linked into the
modern host runtime. A tool is added only with its original path, source
identity, hash/provenance record and declared guest-build consumer.

The historical tool closure may support reproducible OpenNT guest builds; it
does not authorize a change to guest, BOP, host or Bochs runtime semantics.

T301 S1 P21 adds any byte-exact `opennt-src-2` products missing from the
carry, retaining their original-relative names for provenance only.  These
files are not host build, link or runtime inputs; see
[`m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv`](../../docs/etc/operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv).
