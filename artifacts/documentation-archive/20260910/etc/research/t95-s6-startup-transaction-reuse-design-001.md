# T95 S6 Startup-Transaction Reuse Design 001

## Decision

No new BOP/SVC mechanism is needed. The existing adapter host-session path
already provides the only admitted startup transaction, `DEM 50:11`. The
missing work is **same-version composition evidence** connecting that existing
adapter path to the registered generic Bochs #UD/write mechanics while running
the source-built NTIO prefix. It is not a request to add a handler, a service
catalogue entry, or a second adapter API.

## Existing transaction, end to end

| Boundary | Existing owner and interface | Bounded behavior |
| --- | --- | --- |
| Exact marker/service recognition | `bx_ntvdm_host_service_catalog_v1_lookup` | Accepts the catalogued `50:11` path from the copied instruction window; other values pass through. |
| Source-derived preparation | `bx_ntvdm_host_service_plane_v1_prepare` | Real mode only; validates resource identity; computes historical `DI:0000`; preflights one NTDOS write. |
| One-shot ownership | `bx_ntvdm_host_session_v1_dispatch` / `take_pending_write` | Binds pending data to the copied #UD boundary and CPU snapshot, consumes it once, and resumes at `fault_rip + 4`. |
| Runtime installation | `bx_ntvdm_adapter_runtime_v1_dispatch` / `take_pending_write` | Owns the installed session and delegates only the existing session path. |
| Generic mechanics | registered Bochs #UD/write consumer in `refs/bochs/cpu/exception.cc` | Accepts an opaque pending write, validates matching boundary/length/resume, copies only ordinary RAM, and applies the generic resume. |

The bounded transaction is therefore:

```text
copied #UD window C4 C4 50 11
  -> catalogue / host-session (adapter)
  -> one identity-checked NTDOS write at DI:0000 (adapter)
  -> generic ordinary-RAM commit and resume at fault + 4 (Bochs mechanics)
```

Bochs receives neither selector/service meaning nor NTDOS/DOS/DEM terminology.
The adapter receives no Bochs C++ object, memory pointer, or CPU object.

## Reuse evidence and limits

The unit tests already exercise this contract. In particular,
`tests/bx-ntvdm-adapter/bx_ntvdm_host_session_test.c` uses the exact five-byte
window `C4 C4 50 11 F4`, verifies a `+4` resume, rejects a mismatched CPU
snapshot, checks one-time consumption, and checks the NTDOS destination
derived from `DI`. `bx_ntvdm_host_service_plane_test.c` rejects another DEM
service, payload-length mismatch, aperture overflow, and protected-mode use.

The retained r12 fixture is runtime proof that the same adapter transaction
can pass through the generic Bochs commit seam, but its ROM is a synthetic
`mov di; C4 C4 50 11; hlt` marker. It proves neither source-built NTIO prefix
execution nor later NTDOS initialization. Conversely, the r6 observation uses
the real NTIO prefix but intentionally links only the narrow startup-session
closure and leaves `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`; it cannot consume
the transaction.

## Minimum future observation

One future, separately admitted observation may use a fresh same-version
companion closure containing exactly:

1. the already linked r6 Bochs container objects plus the existing registered
   generic #UD/write consumer compiled with its explicit opt-in;
2. the existing adapter runtime/session/catalogue/plane and their declared
   same-island dependencies, with no new adapter source; and
3. the immutable source-built NTIO/NTDOS/COMMAND identity set and the already
   admitted cold-start plan.

It must make one execution only. Acceptance can establish only: NTIO reaches
the existing `50:11` transaction; the NTDOS image is atomically committed to
the source-derived `DI:0000` destination; and the guest resumes at the
post-service byte. It may not claim NTDOS initialization, DOS command support,
host filesystem behavior, or a runnable CLI profile.

## Explicitly rejected expansion

- a BOP-aware branch in Bochs;
- another selector parser or service dispatcher in the adapter;
- linking SoftPC, CCPU, `MS_bop_0`, `DemDispatch`, or `demLoadDos`;
- a special firmware, IVT, IRQ, FPU, CPU, device, or memory mapping repair;
- reusing a prior r12/r6 root or converting their evidence into a build claim;
- any additional DEM or COMMAND service.

The next gate is a fresh build/observation admission that freezes the exact
object/link closure. Until then this record is design and source evidence only.
