# T454 WOW32 resource discovery and loading plan

## Authority

T454 admits Queue row W05. [Current](../../states/CURRENT.md) is the sole
active-packet authority. The external `ntvdm64-wow32-audit` remains read-only
source evidence. The objective is to reconnect the selected original resource
algorithm and callback boundary with the smallest valid host binding, not to
replace it with a second resource manager or modern PE-resource service.

## State ownership

The Win16 KERNEL resource table and its `rn_usage` accounting remain guest
authority. `wres16.c` owns the host-side `RES` bridge records and the lifetime
of converted buffers or scoped aliases. T452's W03 provider remains the sole
owner of module association and allocation/alias state. Native Windows owns
only real native resource/cursor/icon handles. A callback result is a
direction-specific value, not a common raw resource result.

## Stages

| Stage | Scope | Exit |
| --- | --- | --- |
| S1 | Lookup boundary | Finite original owner/caller/state matrix for Find/Load/Lock/Unlock/Free, including normal, failure and retirement routes. |
| S2 | Load and lock | Reconnect only matrix-selected original load/lock paths through W03 aliases and prove guest identity/data/lock behavior. |
| S3 | Free and rollback | Complete matrix-selected free, partial-load rollback and module-associated release without stale RES/alias state. |
| S4 | Foundation acceptance | Exercise the selected loading contracts and hand the verified foundation to later menu/dialog consumers without absorbing conversion/drawing. |

## Boundaries

`FreeResource` is not one operation: guest `IFreeResource`, guest-to-host
`WK32FreeResource`, and host-to-guest `FreeResource16` have independent return
normalization and ownership effects. A modern `FreeResource` result cannot
stand in for Win16 NE accounting. Resource-specific MENU/DIALOG conversion,
cursor/icon materialization, and lifetime semantics remain separate consuming
owners unless the S1 matrix proves a finite prerequisite.

Guests and guest media remain immutable. Every production mirror difference
requires an adjacent stable divergence marker and register entry. No stage may
declare runtime success from a symbol match, static source review or mock
alone.
