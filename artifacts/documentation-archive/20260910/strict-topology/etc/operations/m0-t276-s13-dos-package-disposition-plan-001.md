# M0 T276 S13: W3 `dos` package disposition plan

## Objective

Audit every selected original `dos` package path as the next complete MVDM
owner package in the dependency/interface tracker. Record its original
subpackages, literal build evidence, provider/dispatcher role, reached external
interface families, dependency edges and provisional x86/x64 composition
disposition before enabling a DEM, COMMAND or DOS BOP implementation.

## Fixed scope

The canonical file ledger contains exactly 40 selected `dos/**` paths. This
includes the original host-side DEM and COMMAND provider/control source that
is separate from the load-only guest DOS image. The audit is package-led: no
trace-selected service is an independent work item.

## Required evidence

1. a reproducible 40-path source/build/subpackage ledger;
2. original dispatcher/provider/utility/source classifications, with no
   source-presence-to-runtime inference;
3. family-level caller/declaration and adapter/internal dependency records;
4. explicit links to already-audited SoftPC/SAS, Win32, session, monitor and
   BOP boundaries where reached;
5. a package dependency/topological handoff that distinguishes DEM and
   COMMAND ownership without enabling either implementation.

## Non-goals

No source move or modification, BOP service implementation, adapter body,
build edge, Ninja run, guest image change, Bochs change, host filesystem
operation or guest execution.

## Acceptance

Every one of the 40 selected paths has one reproducible provisional owner and
composition disposition; every actual external family observed in its source
is assigned to a named existing adapter/internal owner or explicit exclusion;
no provider body becomes enabled.
