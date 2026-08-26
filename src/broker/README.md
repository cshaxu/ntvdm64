# broker

Versioned, fixed-width cross-process coordination contract.  The current
implementation is an in-process contract fixture only: it contains no named
pipe, RPC, Win32 security token, MVDM provider or machine pointer.

M0 T272 S5 disposition register:

- `wire.c` and `wire.h`: `new neutral contract`.  There is no reusable
  project-local broker source.  The fixed-size record deliberately excludes
  native pointers, handles, local surrogate IDs and guest pointers.
- `broker.c` and `broker.h`: `new neutral registry`.  It gives stable,
  monotonic broker IDs, binds each client to an already-authenticated fixed
  user key, and removes the record on disconnect.  A later transport must
  authenticate the user key using public OS facilities before dispatch; it
  cannot manufacture a cross-user route.

Project-owned per-user cross-process coordination component. It exchanges only
versioned fixed-width copied records and never carries local resources.
