# Generic platform integration libraries

`rpc/` provides local Windows RPC binding and async call lifetime mechanics.
`io/` provides checked overlapped pipe transfers and Console process-list
sampling, and final native file-path resolution. Both are selected independently as `lib-rpc.lib` and `lib-io.lib`.
They depend only on platform APIs; product protocols, authentication decisions,
frontend frames, scheduling and endpoint lifetimes remain in their owners.
Neither library depends on `common`, worker-base, or executable-private code.
