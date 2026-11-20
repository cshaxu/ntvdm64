# Redirector worker bindings

This directory owns only bindings that must execute in the one `ntvdm.exe`
worker address space.  It is not the `VDMREDIR.DLL` target boundary; the
DLL-local bindings live in `src/vdmredir-dll/`.

## Retained worker-local bindings

| ID | Original purpose | Standalone boundary | Implementation |
| --- | --- | --- | --- |
| ADAPTER-REDIR-003 (compatibility exports) | VDMREDIR NetAPI result bodies write through `ES:DI` aliases. | The DLL must not retain a guest alias or create a second worker session/TLS binding. | `mvdm_redirector_worker_copy.c` retains the original exported entry points for compatibility. VDMREDIR itself resolves addresses and copies through the shared guest-memory contract. |
| ADAPTER-REDIR-004 | Asynchronous named-pipe work retains and later uses `GetVDMAddr` aliases. | A guest lease cannot cross the BOP/worker lifetime, and durable native aliases are invalid. | `../../compat/mvdm_redirector_async.c` owns write snapshots and read staging, validates the bound host context and its epoch, and takes fresh leases for completion. The original queue, completion order and ICA calls remain in `../../mvdm-host/vdmredir/vrnmpipe.c`. |

No redirector provider, protocol implementation, DLL loader entry, RAP policy,
or original VDMREDIR algorithm belongs here.
