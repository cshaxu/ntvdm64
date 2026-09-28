# product-abi

This header-only root contains shared product identity/version declarations and
the worker/frontend Console I/O, mouse and video contracts. It owns no runtime
state, session resource or package-layout implementation. Broker RPC policy and
protocol remain owned by ntsrv; frontend I/O behavior remains owned by ntkvm.
