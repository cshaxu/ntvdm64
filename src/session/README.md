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

M0 T291 S4 disposition register:

- `session.c` and `session.h`: `remove temporary generic route`. The previous
  numeric-operation dispatcher conflated BaseSrv-shaped command acquisition
  with kernel-VDM monitor control. `adapter-mvdm-host-out/monitor` now owns
  separate typed bindings for `GetNextVDMCommand` and `NtVdmControl` while
  `session` retains only neutral lifetime, thread binding, leases and mapping.
  Future multi-caller command brokerage belongs to the source-shaped monitor/
  broker package, never to a generic session callback table.

M0 T303 S2 disposition register:

- `session.c` and `session.h`: `small neutral extension`.  A bound session may
  retain an opaque finite mechanical-resume budget and the copied status of its
  last mechanical resume.  It does not identify the machine implementation,
  a BOP selector, or an OpenNT service; `adapter-mvdm-host-out/softpc` remains
  the owner of the historical `host_simulate(void)` spelling.

M0 T310 S3 disposition register:

- `session.c` and `session.h`: `small neutral extension`. Each session may
  make one fixed-width machine-backend selection while ready. The choice is
  immutable after activation and carries neither a native pointer nor an
  executable callback; `session` does not construct either backend.

M0 T310 S8 disposition register:

- `session.c` and `session.h`: `small neutral extension`. A ready session may
  retain immutable, app-selected roots for firmware, DOS media and Win16
  media. They are UTF-8/ANSI path copies only: session neither locates files,
  loads guest bytes nor interprets any OpenNT service. The app chooses the
  executable-relative roots; source-shaped adapters consume them synchronously.

M0 T388 S5 disposition register:

- `session.c` and `session.h`: `small neutral extension`. The existing
  session video event carries one graphics-ready and one host-display-toggle
  kind alongside invalidate/palette/activity. Session still neither creates
  a window nor interprets SoftPC/PIF/guest state; the adapter identifies the
  original graphics/input boundary and app owns the public presentation
  response.

M0 T317 S3 disposition register:

- `session.c` and `session.h`: `small neutral extension`. An active session
  may register a bounded sequence of owner-private thread bind/unbind hooks.
  Session invokes those hooks only while establishing or releasing its
  existing TLS binding and rolls back an unsuccessful bind in reverse order.
  The facility has no BaseSrv, COMMAND, SoftPC or worker policy: each original
  owner registers and validates its own thread-local record.

M0 T317 S3 disposition register:

- `session.c` and `session.h`: `small neutral extension`. An active session
  may register a bounded sequence of owner-private thread bind/unbind hooks.
  Session invokes those hooks only while establishing or releasing its
  existing TLS binding and rolls back an unsuccessful bind in reverse order.
  The facility has no BaseSrv, COMMAND, SoftPC or worker policy: each original
  owner registers and validates its own thread-local record.

M0 T310 S27 disposition register:

- `session.c` and `session.h`: `small neutral extension`. An active session
  may dispatch a typed host-video invalidation, palette or activity event to
  one app-installed presenter callback.  The event carries native host values
  only within the host boundary; session neither creates a window, interprets
  SoftPC video data nor exposes a guest identity.  An absent presenter is an
  explicit unavailable result, preserving the original caller's failure path.

M0 T310 S8 disposition register:

- `session.c` and `session.h`: `small neutral extension`. A ready session may
  retain immutable, app-selected roots for firmware, DOS media and Win16
  media. They are UTF-8/ANSI path copies only: session neither locates files,
  loads guest bytes nor interprets any OpenNT service. The app chooses the
  executable-relative roots; source-shaped adapters consume them synchronously.
