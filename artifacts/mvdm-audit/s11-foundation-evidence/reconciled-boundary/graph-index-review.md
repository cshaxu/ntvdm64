# Transaction-local graph index review

Scope: current project-owned WOW adapter, not an original USER behavior
replacement. This is a source review supplemented by the controlled and guest
tests linked below; it does not establish every concurrent interleaving.

The index in `wow_user_borrow_client_graph` exists only on the calling thread's
stack. Its pointers refer to sample records owned by active borrow scopes; the
cache adds no reference or permanent state. A slot collision falls back to the
scope list. A cached edge is reused only for the same pinned backing and only
after that generation's view was read in this traversal. Each incoming edge
pin is released on this path; new samples retain a backing and binding until
scope retirement. The page-domain relation lookup pins an existing withdrawn
generation instead of consulting a handle that might now name a replacement.
Nested graph publication moves sampled borrows to the outermost active scope,
so resumed outer code retains the new relationship targets.

`page_domain_menu_context` checks the current owner and page-directory state;
it is not, by itself, proof that every other thread or native callback is
excluded. Terminal retirement can release the runtime lock around original
control/dialog finalizers. The currently retained tests cover collision,
generation reuse, dead-root handling and selected nested dialog retirement;
they do not force destruction or reentry during this precise traversal and
publication interval. The recorded intermittent native-return error 1400 has
no stage or object identity and cannot be attributed to this index from the
available evidence. Do not call this a proven race or a complete concurrency
proof.

Evidence: `dialog-lifecycle/` contains the current
selected-object dialog lifecycle run. The earlier selected-lifecycle controls
and exact object hashes are in
`../../s11-dialog-performance/idle-shared-regression/selected-lifecycles/`;
the actual same-worker guest dialog/retirement witness and package identity
are in the local `reconciled-idle-dialog-*` receipts. The current package
also passed three SOL Options cycles (`sol-options/`). These establish only
their stated paths.

A current selected-object graph-snapshot rerun passed dead-root rejection, withdrawal with retained readers, same-scope nested publication, preparation failure, and native and managed handle-slot reuse. [Trace and input hashes](graph-snapshot/) identify this controlled run. It does not deterministically inject concurrent destruction in the traversal-to-publication interval or identify the historical 1400.

Current selected-object window-reclaim verification passed three-way window index collision, middle removal and same-address reuse; class index collision and forced retirement failure retention; terminal retry; and empty index on teardown. [Result and hashes](index-reclaim/) bind the run. The window-index collision uses synthetic metadata and does not prove guest-level concurrency.
