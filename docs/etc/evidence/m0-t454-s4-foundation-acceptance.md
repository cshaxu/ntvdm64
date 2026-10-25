# M0 T454 S4 — resource foundation acceptance

## Question and scope

Can W05 close the selected original resource discovery/loading foundation
without claiming menu/dialog conversion, resource drawing, a new resource
manager, or a guest change?

## Inputs and procedure

- S1's [owner matrix](m0-t454-s1-resource-owner-matrix.md), S2's
  [load/lock witness](m0-t454-s2-resource-load-lock-witness.md), and S3's
  [free/rollback witness](m0-t454-s3-resource-free-rollback-witness.md).
- Published S3 source/package revision `5ede6c0e4`; its ten-image manifest was
  checked against both `assets/release` and `O:\winnt\system32`.
- The current selected x86 production WOW32 and worker object graphs:
  `build/.ninja/msvc-x86-release/wow32` and `worker`.
- `tests/observation/verify-wow-task-lifecycle.ps1` with
  `-Case window-borrow -RealDialogLifecycleOnly`, output rooted at
  `build/t454-s4/real-dialog-disposition-r1`.

The final command rebuilt the current selected fixture objects, exercised the
actual registered dialog/resource owner and exited zero. It is a source-shaped
native fixture, not a substitute for S3's immutable-guest sixteen-round proof.

## Observations

The broad registered-owner row now passes all required markers:

```text
WOW_REAL_DIALOG_SCOPE errors=0 ...
WOW_REAL_DIALOG_ROLLBACK errors=0 cases=4 physical_free=1 publications=2 vetoes=2 no_app_callback=1
WOW_REAL_DIALOG_RESOURCE_OWNER errors=0 normal_consume=1 exit_invoke=1 conflict_rejected=1 reentrant_once=1
WOW_NATIVE_DIALOG_RESOURCE_OWNER errors=0 no_original_fnid=1 resource_once=1
WOW_REAL_DIALOG_LIFECYCLE errors=0 init=3 terminal=2 nested=1 exception_local=1 callback_cache=1 pdlg_retired=1 dying_callback_denied=1
```

This resolves S3's retained fixture disposition. The earlier non-pass happened
after the dialog markers at process-owner disposal; current selected objects
complete the cleanup. No generic cleanup fallback or resource-consumer change
was added in S4.

The aggregate evidence is therefore:

| Contract | Actual evidence | Result |
| --- | --- | --- |
| Original lookup/load/lock and W03 alias boundary | S1/S2 source matrix and immutable-guest `CDL2` lifecycle | passed |
| Original free/rollback/module release | S3 16-round immutable-guest modeless create/destroy witness | passed |
| Registered dialog resource/physical rollback owner | `real-dialog-disposition-r1` current production-object fixture | passed |
| Build/package/product regression | S3 full ten-component/idle-incremental and Product suite records | passed; inputs unchanged in S4 |
| Exact deployed identity | S3 ten-row candidate/assets/`O:\winnt\system32` SHA-256 audit | passed; inputs unchanged in S4 |

## Interpretation and boundary

W05 is complete at high confidence: original `wres32.c`, `wres16.c`,
`wcall16.c`, `wkman.c` and the WOW16 callback shim retain behavior/resource
authority; W03 alone owns allocation and aliases; native USER owns real HWNDs.
The only S3 production correction removed adapter-only whole-desktop
projections from two modeless leaves and terminal `DestroyWindow` while
preserving the native-call scheduler handoff and return values.

No guest file, `assets/winnt.zip`, resource provider body, resource drawing,
accelerator conversion, menu conversion or broad dialog feature changed.
Those remain the bounded later W07/W09 and W10+ consumer owners. S4 itself is
documentation/evidence only, so it does not rebuild or republish the already
verified S3 package.

## Follow-up

The next unadmitted queue package is W07 GDI identity, DC and drawing. It must
use this W05 resource foundation rather than duplicate a resource manager.
