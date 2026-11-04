# Win32 integration libraries

`rpc/` owns generic local Windows RPC binding, asynchronous call, and authenticated local peer process matching mechanisms. `io/` owns bounded pipe
transfers, Console process-list sampling and final native path resolution.
These libraries do not depend on common, worker-base or product components.
Protocol and authorization policy remain with their respective owners.

`process/` owns Win32 process attribute-list allocation and cleanup; callers
retain attribute values and all process creation, admission and lifetime policy.
