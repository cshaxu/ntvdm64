# session

Dependency-free, per-VDM lifecycle foundation.  It owns the active session
identity, neutral completion/cancellation state, teardown ordering and the
identity/lifetime of registered opaque resource instances.  It has no BOP,
DOS, WOW, VDD, Redirector, Win32 or Bochs vocabulary.

`app` creates and activates the one supported instance.  A component may
register a no-argument teardown hook for a resource it implements, but the
hook is invoked only by this component's lifecycle.  The registration record
contains no host handle, guest pointer or machine object.

## Local divergence register

This is project-authored neutral runtime code, not an imported mirror.  No
imported-source divergence is registered here.
