# Redirector worker bindings

This directory owns only bindings that must execute in the one `ntvdm.exe`
worker address space.  It is not the `VDMREDIR.DLL` target boundary; the
DLL-local bindings live in `src/vdmredir-dll/`.

## Retained worker-local bindings

| ID | Original purpose | Standalone boundary | Implementation |
| --- | --- | --- | --- |
| ADAPTER-REDIR-003 (worker half) | VDMREDIR NetAPI result bodies write through `ES:DI` aliases. | The DLL must not retain a guest alias or create a second worker session/TLS binding. | `mvdm_redirector_worker_copy.c` resolves numeric 16:16 addresses through the worker's CCPU-aware guest-memory lease; `vdmredir-dll` imports its bounded copy operations. |
| ADAPTER-REDIR-004 | Asynchronous named-pipe work retains and later uses `GetVDMAddr` aliases. | A guest lease cannot cross the BOP/worker lifetime, and durable native aliases are invalid. | `mvdm_redirector_async.c` snapshots write data or stages read data, then obtains a fresh worker lease only for completion copies.  The original queue, completion order and ICA calls remain in `mvdm/vdmredir/vrnmpipe.c`. |

No redirector provider, protocol implementation, DLL loader entry, RAP policy,
or original VDMREDIR algorithm belongs here.
