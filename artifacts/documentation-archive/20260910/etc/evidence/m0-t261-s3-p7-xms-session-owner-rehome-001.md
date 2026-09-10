# M0 T261 S3 P7 — XMS session owner re-home

The native XMS session only validates a generic `#UD` event, copies CPU state,
calls the imported XMS provider, and emits a typed resume/stop outcome.  It is
BOP ingress composition, not host capability.  It moved from
`opennt-host/xms` to `opennt-bop/ingress`; its XMS provider facade remains
component-qualified in `opennt-host` pending the separate XMS facade split.

Fresh graph `build/M0-T261-S3/r007` compiled the moved object; the repeated
Ninja invocation was a no-op.  Component-manifest and production-tree gates
also passed.  No provider or machine behavior changed.
