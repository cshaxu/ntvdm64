# Populated WOW registry token shutdown

The selected `wow_registry_binding.obj` was rebuilt through the current cached
WOW32 graph and linked into `wow_registry_shutdown_fixture`. The test supplies
only a controlled parent export boundary: two successful opens yield distinct
parent handles, explicit close retires one, and binding shutdown retires the
other once. Both stale guest tokens are rejected; a repeated shutdown does
not close anything again. A subsequent open receives a different token and a
further shutdown closes its parent once. Unused parent operations return an
error if called and are not part of this witness.

The formal runner exited zero and reported `WOW_REGISTRY_POPULATED_SHUTDOWN_OK`.
Source, definition, selected object and executable hashes are recorded in
`inputs.csv`. This directly exercises the current token implementation, but
uses a synthetic parent and invokes shutdown directly: it does not establish
real-worker explicit unload with populated tokens, concurrent unload safety,
guest behavior, or product-wide acceptance. No guest or product image changed.
