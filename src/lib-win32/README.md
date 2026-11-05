# Win32 integration libraries

`rpc/` owns generic local Windows RPC binding, asynchronous calls and local peer process matching. `pipe/` provides connected local overlapped byte-pipe pairs and bounded transfers. `file/` owns final native path resolution and file identity comparisons. Client identity validation, protocol and channel admission remain caller-owned.
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

`crt-compat/` provides historical unbounded CRT formatting and scanning entry
points for callers that require those spellings, using the modern UCRT ABI.

`thread/` provides the generic x86-compatible thread entry and optional
per-thread lifecycle callbacks; it owns no product session or policy.

`process-observation/` samples Console process membership, retains an observation handle, reads process identity
and image information, queries inherited parent identity, and registers/drains a one-shot exit wait; callbacks,
process ownership, task identity and decisions remain caller-owned.

`process-image/` provides Win32 target-process machine queries and image file validation; hook installers own selection policy. The imported component is shared byte-for-byte with Win16-style.
