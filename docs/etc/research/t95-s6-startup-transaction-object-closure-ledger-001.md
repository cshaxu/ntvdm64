# T95 S6 Startup-Transaction Object-Closure Ledger 001

## Result

The existing r6 companion cannot simply enable its #UD macro: its execution
plan deliberately uses the narrow `startup_session` endpoint, while the
existing #UD block calls the broad `adapter_runtime_v4` dispatch path. Linking
that block would bring unrelated read, observation, multi-write, debug, drive,
DPB and COMMAND-capability paths into the first startup experiment.

That is an object-boundary failure, not evidence that `50:11` needs another
service implementation. The smallest acceptable future composition is a
dedicated **adapter-owned startup transaction bridge** plus a narrow,
default-off generic Bochs #UD/write consumer. It reuses the existing `50:11`
plane and must not introduce another catalogue or dispatcher.

## Source-to-object classification

| Class | Existing source/object family | Reason |
| --- | --- | --- |
| Required Bochs mechanics | `main.cc`, `cpu/exception.cc`, `memory/misc_mem.cc` | Existing cold-start plan, generic copied #UD boundary, and ordinary-RAM write only. |
| Required adapter input | `bx_ntvdm_startup_session_environment.c`, `bx_ntvdm_startup_session.c`, `byob_profile.c`, `byob_identity.c`, `byob_image.c` | Reads the child-only immutable profile/resource set and produces the existing NTIO plan. |
| Required adapter transaction | `bx_ntvdm_exception_abi.c`, `bx_ntvdm_cpu_state_abi.c`, `bx_ntvdm_instruction_window_abi.c`, `bx_ntvdm_guest_range.c`, `bx_ntvdm_guest_write_abi.c`, `bx_ntvdm_session_resource_abi.c`, `bx_ntvdm_host_service_catalog.c`, `bx_ntvdm_host_transaction_abi.c`, `bx_ntvdm_host_service_plane.c` | Fixed-width boundary, exact four-byte catalogue input, resource identity, `DI:0000` preflight and one write descriptor. |
| Existing shared session, not yet a clean member | `bx_ntvdm_host_session.c` | Its `initialize`, `dispatch`, `take_pending_write`, and `prepare_startup_plan` are required, but the same translation unit also owns observation, snapshot, guest-read, gather-read and multi-write APIs. |
| Explicitly rejected for first startup | `bx_ntvdm_adapter_runtime.c` and all its secondary service owners | Its v1 dispatch falls through to debug/BIOS/profile logic; v2/v3/v4 further invoke COMMAND, drive, DPB, hard-error, DTA/read and gather paths. It is not the narrow first-startup owner. |
| Default but unneeded product closure | GUI/plugin/device/firmware additions, SoftPC/CCPU/OpenNT host objects | No required call site in this transaction; they must not be selected to resolve a link. |

## Why a narrow bridge is required

`main.cc:30-32,80-94` calls only
`bx_ntvdm_startup_session_v1_prepare_from_environment`; it has no host-service
ownership. The broad `cpu/exception.cc` block instead calls
`bx_ntvdm_adapter_runtime_v4_dispatch` before trying v3/v1 paths, and then
accepts multiple pending-operation kinds. The two existing endpoints therefore
cannot together establish a minimal first-startup composition.

The bridge must be adapter-local and provide exactly these same-island actions:

1. install the immutable profile/resource images once;
2. return the existing NTIO execution plan;
3. route a copied #UD event/window only to the existing host-session `50:11`
   preparation; and
4. hand out its one pending ordinary-RAM write only at the identical boundary
   and CPU snapshot.

It has no guest-memory access and no BOP/DEM/NTDOS term in the Bochs ABI.
The Bochs consumer receives only a generic result, write descriptor, payload
and resume value.

## Required source separation before any build

The four required functions in `bx_ntvdm_host_session.c` share local helpers
with the other operation kinds. A future adapter-only source move may isolate
them into one startup-session translation unit without changing their public
headers, behavior, records, or callers. That move is preferable to relying on
`/Gy` and `/OPT:REF` to make a broad host-session object disappear, because the
latter would prove linker accident rather than the intended hard boundary.

Separately, the Bochs exception change must be registered before code: a new
compile-time default-off block may consume only the bridge's generic
dispatch/take-write operations. It must not retain the present broad
observation/read/multi-write paths in the startup companion. This is a new
mechanics-only intrusion, not a decode or CPU-semantic modification.

## Future build preconditions

Before one fresh same-version companion build can be admitted, all of the
following must exist and be statically verified:

- adapter startup bridge and narrow host-session object have focused positive
  and negative tests;
- the new Bochs intrusion is registered with default-off source checks;
- `main.cc` and the #UD consumer use that same bridge endpoint;
- a copied-root shim names only the classified required objects, with no broad
  runtime, service, OpenNT or product object; and
- its full object/archive/link ordering and expected CRT imports are frozen.

No compiler, archive, linker, build root, or guest run was used for this
ledger.
