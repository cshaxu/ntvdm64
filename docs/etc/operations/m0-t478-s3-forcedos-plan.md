# T478 S3 — ForceDOS host integration

## Question and bounded owner

Finding 20 asks whether the original explicit CreateProcess flag can reach
mvdm-host COMMAND. The supported candidate caller is a native program already
launched inside the product, using its existing hooked CreateProcessA/W.
No standalone FORCEDOS utility or new Run16 CLI is required for this route.
The original [audit](../../../artifacts/mvdm-audit/shard-3/forcedos-original-owner-audit.md)
is historical evidence; its C++/Detours names are stale after T475. Current
source uses C and process-hook but retains the same restrictive flag gates.

## Admission-time source observations

- nthook32-dll/create_process.c: finish retains only CREATE_SEPARATE_WOW_VDM.
  legacy_application rejects new-console/detached/no-window paths and uses
  nthook_legacy_type, which admits DOS/WOW/PIF but not OS2 NE.
- common/native-hook/installer.c and packet.c reject flags other than separate
  WOW. Expanding only producer flags cannot cross the current consumer gate.
- run16-exe/main.c classifies and chooses frontend/GUI behavior before
  frontend_scope.c consumes its one-hop Hook context. Merely forwarding the
  bit after begin_gui would be too late: forced WOW must choose DOS ownership
  before GUI Console detachment. Refactor the seed ownership explicitly, not
  by rereading/duplicating one-hop handles or adding process-global state.
- launch_vdm currently forms new-console/separate-WOW flags, omitting forcedos.
- Original COMMAND cmdmisc.c consumes CREATE_FORCEDOS into
  DontCheckDosBinaryType. Original broker CHECK/GET carries creation flags;
  the historical codec test proves serialization only, not complete execution.
- Pinned OpenNT base/win32/client/process.c sends forced Win16 to ForceDos;
  forced OS2 NE bypasses its OS2 branch. A valid native PE still uses native
  execution. These are classification semantics, not blanket forcing by suffix.

## Implementation sequence

1. Bind current classifier/context fixtures to normal and explicit-force
   inputs; establish allowed combinations and both process widths. Keep native
   PE and ordinary launches unchanged. The existing shared/non-shared Console
   exclusions must be documented and tested rather than silently expanded.
2. Make the existing launcher seed available before image dispatch, with one
   explicit owner for attached handles and deterministic failure cleanup.
   Select DOS only for original eligible forced legacy classes; then forward
   the original flag through BaseCheckVDM and existing NTSRV transport.
3. Verify actual service CHECK/GET and COMMAND bypass/reset. A forced command
   must not leak policy into a later ordinary task. Preserve original PIF
   overwrite semantics unless a separately proved host defect requires action.
4. Exercise an instrumented native parent, original immutable Win16 image/DOS
   stub or independently authored correctly classified NE input, plus ordinary
   contrast, child execution, output and completion. Reject a synthetic NE
   classified as DOS as proof of the forced NE route. Validate ANSI/Unicode,
   working directory and inherited streams. Run the coherent product gate,
   publish only worktree assets/release and commit locally.

## Status and limits

Implemented and closed in [S3 evidence](../evidence/m0-t478-s3-forcedos.md).
The admitted supported caller route is connected and tested. The evidence
separates actual NE/COMMAND execution from static OS2 and same-worker reset
review, and retains existing new-console/debug/PIF limitations. Both the S2
repair and main changes are now in the tested and locally published package.
No O:/winnt publication, guest modification or push is authorized.
