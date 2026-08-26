# session

Dependency-free, per-VDM lifecycle foundation.  It owns the active session
identity, neutral completion/cancellation state, teardown ordering and the
identity/lifetime of registered opaque resource instances.  It has no BOP,
DOS, WOW, VDD, Redirector, Win32 or Bochs vocabulary.

`session_input.{c,h}` owns a separate copied input/publish record shared by
host-facing compatibility facades. Its fields are deliberately neutral text,
metadata and multisz lifetime; the record does not expose a guest pointer,
native handle or any guest/host product protocol.

`app` creates and activates the one supported instance.  A component may
register a no-argument teardown hook for a resource it implements, but the
hook is invoked only by this component's lifecycle.  The registration record
contains no host handle, guest pointer or machine object.

## Local divergence register

This is project-authored neutral runtime code, not an imported mirror.  No
imported-source divergence is registered here.
