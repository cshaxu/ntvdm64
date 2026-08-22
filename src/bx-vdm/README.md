# Bochs/NTVDM Adapter Boundary

This directory is the sole integration boundary between the adopted Bochs 2.6
runtime and the project-owned NTVDM/OpenNT side.

It may contain:

- a versioned, fixed-width ABI;
- a copied, mode-tagged x86 register/selector snapshot at a declared
  instruction boundary;
- pointer-free, preflight-only physical guest-write descriptors;
- immutable session-resource descriptors supplied by the CLI BYOB admission;
- one adapter-local, installation-time host-drive policy snapshot, with no
  guest-request rescan or Bochs visibility;
- native Bochs container lifecycle glue;
- validated guest-memory transfer helpers; and
- the declared adapter NTVDM host-service catalogue and common transaction
  preflight; and
- one same-island host session which owns pending transaction/payload state
  until the generic backend commit boundary consumes it.

It must not contain CPU interpretation, Bochs configuration/model substitutes,
BIOS or device behavior, DOS kernel/filesystem algorithms, or WOW application
policy. Its declared host-service plane may rehost source-derived historical
BOP/DEM/COMMAND host contracts only through the shared catalogue and
transaction model; it must not pass
C++ objects, host pointers, CRT-owned storage, or raw cross-architecture
function pointers across the boundary.

Changes below `refs/bochs` are exceptional. They require evidence that a core
mechanism is hard-linked to an excluded peripheral and that an adapter-side
solution cannot express the separation without changing behavior.
## BOP Implementation Layout

`bop/` contains the sole current source-parity provider work.  Completed BOP
owners do not retain a parallel legacy provider tree: their historical
experiments are available only in Git history.  Generic machine, guest-memory
and session ABI implementations remain at this directory root.
