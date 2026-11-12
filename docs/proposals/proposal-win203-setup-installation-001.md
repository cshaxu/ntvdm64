# Proposal — Windows 2.03 Setup patch package

## Status and owner objective

The owner requests `tools/win203-setup`: a Setup patch package that enables
Windows 2.03 to install and run on the project runtime. This is an unnumbered
candidate in Queue, not an active task or a claim of compatibility.
Queue owns its current position; the active task remains unchanged.

## Scope

1. Identify the user-supplied Windows 2.03 media and establish the current
   installation and runtime baseline; attribute actual failures before changing
   code or configuration.
2. Reuse applicable `tools/win101-setup` and `tools/win31-setup` mechanisms and
   released assets. Implement `tools/win203-setup` with CMD entry points and
   existing add-on tools; consume package materials from `assets`, not temporary
   build outputs. Do not assume drivers or configuration from another Windows
   version are compatible.
3. Prepare a user-selected installation workflow and installed launcher with
   only necessary configuration or independently authored adaptations. Preserve
   original media and guest binaries under the existing source policy; any new
   exception requires explicit approval. Do not change unrelated setup packages.
4. Verify original Setup completion, Windows 2.03 startup, display, keyboard and
   mouse interaction, representative bundled applications, normal exit and
   restart. Include basic missing/unsupported-input and repeated-application
   checks, and report exact supported media and remaining limits.

## Delivery and acceptance

At admission, define the bounded implementation and integration-test plan.
Completion requires a working end-to-end installation and interactive runtime,
not only script checks or an existing preinstalled image. Follow the current
cached build, applicable regression, asset/tool publication, manifest, commit
and push rules. No source import, product repair or release is performed by
this queue-only update.
