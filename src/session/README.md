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
