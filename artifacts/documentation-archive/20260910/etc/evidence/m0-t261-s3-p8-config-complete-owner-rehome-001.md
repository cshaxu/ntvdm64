# M0 T261 S3 P8 — Configuration-complete owner re-home

`UMBNotify` is the source-parity configuration-complete notification reached
from the OpenNT BOP route.  It has no independently useful host capability
semantics and retains its original empty body.  It moved from `opennt-host`
to `opennt-bop/softpc` without changing behavior.

Fresh graph `build/M0-T261-S3/r008` compiled the moved object and a repeated
Ninja query was a no-op.  Component-manifest and production-tree gates passed.
