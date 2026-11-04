# Win32 integration libraries

`rpc/` owns generic local Windows RPC binding, asynchronous call, and authenticated local peer process matching mechanisms. `io/` owns bounded pipe
transfers, Console process-list sampling and final native path resolution.
These libraries do not depend on common, worker-base or product components.
Protocol and authorization policy remain with their respective owners.

`process-attributes/` owns Win32 process attribute-list allocation and cleanup; callers
retain attribute values and all process creation, admission and lifetime policy.

`handle/` provides explicit-process same-access duplication and best-effort
remote handle close; caller-owned allocation, authorization, and transfer policy
remain outside the library.

`process-hook/` owns suspended-child payload transfer and same-/cross-width
Detours installation; the caller retains process ownership and rollback.
The pinned Detours source and license are carried under this subcomponent.

`process-observation/` retains an observation handle, reads process identity
and image information, and registers/drains a one-shot exit wait; callbacks,
process ownership, task identity and decisions remain caller-owned.
