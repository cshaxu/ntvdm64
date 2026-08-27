# session

Dependency-neutral per-instance lifecycle, mapping, resource, event and
teardown component.

M0 T272 S2 disposition register:

- `session.c` and `session.h`: `replace`. The former lifecycle shape informed
  this minimal explicit-instance implementation, but its process-global
  identity allocator and callback-without-context contract were not
  multi-instance-safe.
- `session_cancellation.cc` and `session_cancellation.h`: `retire`. Their
  process-global cancellation state is replaced by fields owned by each
  `session` instance.
- `session_input.c` and `session_input.h`: `split`. Their startup, environment
  and re-entry payload is not neutral lifecycle state and awaits its app or
  same-shaped adapter owner.

M0 T272 S3 disposition register:

- `mapping_manager.c` and `mapping_manager.h`: `new neutral replacement`.
  They provide the one generic mapping implementation required by the
  rebootstrap architecture.  A session owns three separate instances:
  `guest_memory_mappings`, `host_resource_mappings`, and
  `completion_callback_mappings`.  The manager stores opaque native-sized
  values only; it does not acquire guest memory, close host resources, or
  attach callback semantics.
- Historical `guest_pointer_manager` and `host_handle_manager` records remain
  in `src.old/` as reference-only material.  They combined process-global
  registries with guest-memory lease and host-resource ownership semantics, so
  they cannot be reused as the neutral, per-session manager.

M0 T272 S4 disposition register:

- `guest_memory_lease.c` and `guest_memory_lease.h`: `new neutral
  replacement`.  The old guest-pointer manager supplies only the checked
  copied-memory lifecycle reference.  Its process-global session identity,
  real-mode helper, host-handle registry and provider-specific ownership were
  deliberately not carried forward.
- `session.c` and `session.h`: `small extension`.  They expose the lease
  through the owning session and terminate leases before mapping and session
  teardown.  The API accepts only caller-provided read/write callbacks and
  keeps a bounce pointer within the synchronous lease lifetime.

M0 T280 S5 disposition register:

- `session.c` and `session.h`: `small extension`. They expose a synchronous,
  dependency-neutral external control callback for source-shaped adapters.
  The callback has an explicit context and operation number; it owns neither
  guest-memory mapping nor native resource lifetime. `adapter-mvdm-host-out/win32` uses it
  for the reached `NtVdmControl(VdmQueryDir, ...)` declaration contract.

M0 T291 S3 disposition register:

- `session.c` and `session.h`: `small neutral extension`. One session may now
  register a bounded set of unique numeric-operation routes before its older
  fallback dispatch is consulted. The route record owns no MVDM/WOW/COMMAND,
  Bochs, pointer, handle, worker or mapping semantics. It only lets two
  independently owned source-shaped *control-request* contracts coexist in
  the same session without either becoming a process-global broker. It is the
  modern carrier for `GetNextVDMCommand`-style callers, not for WOW
  `CallBack16` synchronous guest re-entry.
