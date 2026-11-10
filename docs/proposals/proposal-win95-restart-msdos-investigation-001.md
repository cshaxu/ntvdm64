# Proposal — Win95 restart into MS-DOS investigation

## Status and owner request

This is an unnumbered candidate at the front of the T queue. The owner asks
why the Windows 95 shutdown option “Restart the computer in MS-DOS mode” does
not work in this project. Admission remains separate from the active task.

## Objective

Reproduce the selected shutdown option on an appropriately isolated Win95
installation and determine its expected and observed behavior. Trace the
guest shutdown and restart request through the original Windows and OpenNT
paths and the project's host, frontend and lifecycle boundaries. Attribute
the failure to the actual responsible component before proposing a remedy.

Distinguish returning to an existing DOS environment, starting another DOS
session, restarting the guest, and exiting the host process. Record which
behavior the original guest requests and which behavior the project can
provide; do not assume they are interchangeable.

## Boundaries and evidence

- Preserve original guest binaries and media; do not modify or rebuild them.
- Use existing launch and diagnostic facilities and isolated test copies.
- Record the exact option, configuration, observable result, control paths,
  component ownership and any unverified lifecycle or input/output boundary.
- Compare a normal Windows shutdown and a normal DOS return where applicable.
- Provide a minimal, evidence-backed solution and its compatibility tradeoffs
  if a project-owned gap is found; this candidate does not presume a root cause
  or authorize a guest workaround.
- Retain the active multitask task and all existing queued candidates.
