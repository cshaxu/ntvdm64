# M0 T276 S14: W3 `xms.486` package disposition plan

## Objective

Audit every selected original `xms.486` path as one owner package. Record the
original build membership, XMS service/data role, exact SoftPC/SAS/A20/RAM/IVT
and UMB dependencies, and the resulting prerequisite or explicit
Bochs-replacement exclusion before enabling any XMS service body.

## Fixed scope

The current canonical ledger selects exactly 15 `xms.486/**` paths. This is a
complete original package review, not a response to a reached `52:xx` BOP.

## Required evidence

1. reproducible per-path source/build/subpackage disposition for all 15 paths;
2. family-level source evidence for XMS dispatch, A20, conventional/extended
   memory, UMB, interrupt/IVT and guest-memory contracts;
3. named ownership for every external boundary: `adapter-bop`,
   `adapter-softpc`, `adapter-bochs`, `session`, `adapter-win32` where actual
   source evidence reaches it, or explicit original-style profile exclusion;
4. a dependency handoff that distinguishes XMS source recovery from monitor,
   physical machine and later DPMI work.

## Non-goals

No XMS source move or edit, source-body enablement, BOP implementation,
machine-memory change, Bochs change, adapter body, build edge, Ninja run,
guest execution, host-memory allocation or trace-led service work.

## Acceptance

Every selected path has one reproducible non-enabled disposition. Every actual
external XMS family has a single named owner or explicit machine/profile gate;
the audit must never expose a host pointer, recreate a memory controller or
infer an XMS implementation from one BOP entry.
