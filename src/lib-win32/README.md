# Win32 integration libraries

`rpc/` owns generic local Windows RPC binding, asynchronous call, and authenticated local peer process matching mechanisms. `io/` owns bounded pipe
transfers, Console process-list sampling and final native path resolution.
It also creates connected local overlapped byte-pipe pairs; client identity
validation, protocol and channel admission remain caller-owned.
These libraries do not depend on common, worker-base or product components.
Protocol and authorization policy remain with their respective owners.

`process-attributes/` owns Win32 process attribute-list allocation and cleanup; callers
retain attribute values and all process creation, admission and lifetime policy.

`handle/` provides explicit-process same-access duplication and best-effort
remote handle close; caller-owned allocation, authorization, and transfer policy
remain outside the library. It also resolves Windows object-handle comparison;
callers decide how an unavailable function or unequal objects affect policy.

`file/` compares existing file identity using volume and file indices; callers
own the decision attached to a match and the supplied path policy.

`process-hook/` owns suspended-child payload transfer and same-/cross-width
Detours installation; the caller retains process ownership and rollback.
The pinned Detours source and license are carried under this subcomponent.

`process-observation/` retains an observation handle, reads process identity
and image information, queries inherited parent identity, and registers/drains a one-shot exit wait; callbacks,
process ownership, task identity and decisions remain caller-owned.
