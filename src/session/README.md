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
