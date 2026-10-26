# T461 S2 — Shell and layered Registry binding design

## Decision

Retain every selected original `wshell.c`, `wkernel.c`, `mapembed.c`,
`wdde.c` and `wow32fax.c` body unchanged.
The existing `mvdm_shadow_registry` component remains the sole owner of the
layered Registry state.  A small WOW32-DLL binding maps guest-visible DWORD
tokens to that component's private parent-process keys; it has no Registry
tree, parser, persistence state, host Registry access, or Shell policy.

This is the smallest source-shaped boundary because the original bodies already
own Win3.1 error conversion, `Remove_Classes`, guest mappings/flushes,
buffer-retry rules and recursive delete ordering.  Directly linking a second
copy of the façade into WOW32 would create two state authorities. Replacing
the bodies with an autonomous Registry implementation would discard original
behaviour.  Neither is allowed.

## Ownership and ABI

| Layer | Owner | State and rule |
| --- | --- | --- |
| Original WOW callers | `wshell.c` ordinals 1--7, `wkernel.c` selected registry thunks, and the existing `mapembed.c`/`wdde.c`/`wow32fax.c` Registry call sites | Retain original calls, argument conversions, error conversion, guest buffer flushes and `WOWRegDeleteKey` recursion. |
| Parent façade | `mvdm_shadow_registry.c` in `ntvdm.exe` | Owns all raw key objects, the read baseline and future mutable `NTVDM.REG` overlay. It exposes only fixed Win32-shaped private imports to its late-loaded WOW32 child. No raw key leaves this component. |
| WOW binding | one `wow32-dll` Registry-binding source and header | Owns a bounded worker-local token table only. It resolves a guest DWORD token immediately to a parent opaque key, calls the parent façade, and never stores a raw key in guest memory. |
| Guest | original DWORD `HKEY` fields | Receives nonzero worker-local tokens. Tokens are monotonic and never reused during the worker lifetime; stale/foreign values fail as invalid handles. |

The token table is worker/process scoped, not task scoped. Win16 registry keys
are process handles and can be passed through shared memory between tasks;
eagerly disposing them at one task's retirement would change that original
ability. Explicit original `RegCloseKey` removes a token. WOW32 DLL detach
closes all remaining parent keys before its table is discarded. Worker teardown
then destroys the parent façade. This gives normal-close, stale-token and
abnormal worker-retirement cleanup without inventing a task-private Registry.

## Required parent imports

The existing generated parent import surface gains these `WINAPI` operations:

`mvdm_shadow_reg_open_key`, `mvdm_shadow_reg_create_key`,
`mvdm_shadow_reg_close_key`, `mvdm_shadow_reg_delete_key`,
`mvdm_shadow_reg_set_value`, `mvdm_shadow_reg_query_value`,
`mvdm_shadow_reg_enum_key`, `mvdm_shadow_reg_set_value_ex`, plus the existing
`open_key_ex`, `query_info_key` and `enum_value` used by original
`WOWRegDeleteKey` and KERNEL thunks.  WOW32 resolves this fixed private parent
ABI only from its loaded `ntvdm.exe`; there is no static second façade copy or
independent Registry provider.

The WOW binding presents the corresponding historical `Reg*` signatures to
the original source through a source-specific forced-include/build mapping.
This avoids edits to the imported OpenNT bodies. The generated parent `.def`
and provider import aliases document the only dependency direction:
`WOW32.DLL → NTVDM.exe`; the parent never imports from its late-loaded child.

## S2 mutable operation contract

The parent façade extends its existing loaded overlay with one worker-local
tree view. It accepts only `HKEY_CLASSES_ROOT` and the explicitly admitted
machine roots. A successful create/set/delete changes that in-memory overlay,
never Windows Registry. Reads resolve:

1. key/value tombstone in the mutable overlay;
2. mutable value/key;
3. existing `NTVDM.REG` value;
4. admitted read-only host snapshot; then
5. the original caller's unchanged missing-value branch.

The parent exposes direct-child enumeration and query-info only so the
unchanged `WOWRegDeleteKey` retains its original recursive ordering. It does
not call a host recursive-delete helper. S2 does **not** persist mutations;
S3 serializes commits across workers and writes atomically to `NTVDM.REG`,
including tombstones. Thus no S2 test may describe a worker restart as a
persistence pass.

## Verification group after the complete connection

One post-connection build/test group will cover: original Shell callback
success/failure and guest return; `FindExecutable` normal and <=32 error;
create/set/query/enumerate/delete/close through the original Shell thunks;
recursive deletion of a nested key; stale-token rejection; parent/DLL teardown
without native Registry writes; retained DOS/WOW/Win3.x frontiers. A source
scan must confirm no selected original caller still imports the system
mutation APIs. S3 owns restart, crash, atomic-commit and multi-worker tests.
