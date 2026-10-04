# T427 system-root and application-search isolation plan

Owner admits this package on 2026-10-04 after accepting T426. The
[proposal](../../proposals/proposal-ntvdm-system-root-path-isolation-001.md)
owns requirements; [Status](../../states/CURRENT.md) owns the only active S.

## Sequential stages

1. S1: audit each user/host/product path caller and provenance. Freeze original
   DOS extension/search order, Win16 directory roles and reachable non-ROM
   resource paths. Reproduce package shadowing with actual selected-image
   evidence. Decide the smallest shared owner and per-caller failure tests.
2. S2: implement actual-own-EXE root derivation and internal component/media
   binding; use bounded guest projection only where source proves it. Verify
   relocation, incorrect inherited roots and missing local internal files.
3. S3: remove implicit package-first user command lookup; retain CWD/PATH order
   and explicit-path authority. Make product-generated COMMAND invocation
   explicit without changing guest EXEC/parser or native host environment.
4. S4: sweep callers, verify direct/nested/relocated paths and retained product
   matrices/WOW frontiers, publish coherent package, review diff and hand off.
5. S5: install all six EXEs and two host DLLs in system32 by owner request.
   Derive Windows root from the parent of the loaded EXE directory; preserve
   original guest layout, root SYSTEM.INI, user search and Registry state.
   Verify relocation/internal launches and retained product gates before
   recoverable publication and reviewed delivery.

Each implementation S requires its own admission, affected x86 build, focused
tests, retained product gates and coherent O:/winnt publication/recovery before
reviewed commit/push. Audit S1 does not claim a new product capability.

## Boundaries

Every process derives its root from its actual loaded EXE, not another EXE's
path, argv, CWD, PATH or inherited authority. Native host Windows directories
remain host-owned. Internal media roots never gain implicit user-search
priority; the package participates only through CWD, explicit PATH placement
or an explicit user path. Owner correction excludes new RPC package-directory
identity checks; existing authentication/version policy remains unchanged.

No guest patch, new helper/component, global environment/Registry mutation,
DLL-loader overhaul or lifecycle/scheduler change. Preserve other sessions'
edits. New probe/output directories stay below build; use only subst Z: and
remove it finally. Unresolved source or materially broader policy stops the
stage rather than silently expanding it.
