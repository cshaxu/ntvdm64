# OpenNT historical build tools

This root contains only provenance-admitted original OpenNT build-tool
closures. The seven-file source-identical `base/mvdm/tools16/` package is
retained below `tools16/`; its hash/provenance and deferred-consumer decision
are recorded in
`docs/etc/evidence/m0-t263-s6-opennt-tools16-closure-admission-001.md`.
It is not a production source component and must never be linked into the
modern host runtime. A tool is added only with its original path, source
identity, hash/provenance record and declared guest-build consumer.

The historical tool closure may support reproducible OpenNT guest builds; it
does not authorize a change to guest, BOP, host or Bochs runtime semantics.
