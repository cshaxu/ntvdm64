# OpenNT historical build tools

This root contains only provenance-admitted original OpenNT build-tool
closures, initially from `base/mvdm/tools16/`.  It is not a production source
component and must never be linked into the modern host runtime.  A tool is
added only with its original path, source identity, hash/provenance record and
declared guest-build consumer.

The historical tool closure may support reproducible OpenNT guest builds; it
does not authorize a change to guest, BOP, host or Bochs runtime semantics.
