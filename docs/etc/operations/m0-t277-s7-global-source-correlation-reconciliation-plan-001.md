# M0 T277 S7 — global source-correlation reconciliation plan

S7 joins the original 3,697 T276 S5 source-call correlations to the completed
S3–S6 family dispositions. It produces one effective row per source call:

- exactly 325 `reviewed-future-host` rows, each linked to its actual S3, S4,
  S5 or S6 family ledger record; and
- exactly 3,372 `non-runtime-or-profile-excluded` rows, retaining original
  caller/path/disposition evidence instead of silently omitting them.

The 4,803 textual declaration candidates remain separately visible as
lexical-only evidence. They are not upgraded to a reached dependency by this
join. This audit neither enables a source body nor manufactures a build edge;
it is the controlled input to package-edge and build-profile reconciliation.
