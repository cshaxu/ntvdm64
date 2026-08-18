# Proposal: OpenNT COMMAND Owner-Package Completion

## Purpose

Recover the complete COMMAND host-service family (`54:xx`) as one package:
bootstrap, registration, launch/CMDINFO, console, execution, environment,
current directory, error/exit and parent-return lifecycle.

## Boundary

COMMAND guest code remains guest-owned.  `bx-vdm` selects source-derived
OpenNT host providers and passes fixed-width copied contracts only.  Normal
Win32 console, environment and directory APIs are permitted capabilities of
the selected OpenNT owner; no historical GUI/NT4 host composition is loaded.

## Admission Plan

1. **S1 — audit:** map all `54:xx` services and dependent `nt_bop.c` routes
   to original COMMAND source, layouts, continuation/failure semantics and
   profile dispositions.
2. **S2 — code complete:** close the complete bootstrap/launch/console/
   environment/lifecycle provider package, preserving original ordering and
   rejecting unsupported capabilities explicitly.
3. **S3 — verification:** execute whole-package Direct/Readonly local
   regressions and one source-built COMMAND native integration run.

## Exit Rule

Individual endpoints such as `54:04` or `54:0E` count only as package tests;
they cannot independently close this proposal.
