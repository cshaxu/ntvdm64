# M0 T258 S1 — Shared Guest-Pointer Manager Source/ABI Map

## Question

Which active OpenNT-shaped pointer call sites can use the one synchronous
`bx-vdm` mapping lease immediately, and which require an owner-specific
copy/refusal decision first?

## Inputs

- `BOP-DEPENDENCY-118` in `docs/etc/bop-list.md`.
- Td mapping governance and the active DEM, COMMAND and XMS shim sources.
- Historical `GetVDMAddr`, `Sim32GetVDMPointer`, `Sim32FlushVDMPointer` and
  `Sim32FreeVDMPointer` call shapes retained by the imported bodies.

## Source-Recovery Decision

| Rung | Result |
| --- | --- |
| Original source | Retained unchanged at each active imported call site. |
| Same-shaped facade | Admitted: one mapping-manager implementation, through the session's fixed `guest_memory` instance, backs the historical pointer expression while CCPU/SAS is unavailable. |
| External-code intrusion | Rejected: neither bx-core nor bx-mantle changes. |
| New behavior | Only the session-local bounce lease is authored; it supplies unavailable SAS backing and changes no owner algorithm. |

## Migration Matrix

| Current surface | Current form | T258 S1 disposition |
| --- | --- | --- |
| `demhndl_shim.c` fixed DTA, FCB, CDS and GSET spans | family-local allocated bounce buffer plus ad-hoc final write-back | **Migrated:** manager lease has real-mode address, span, read/write access, epoch and explicit release. |
| `demhndl_shim.c` ordinary read/write buffer | source uses `Sim32Flush/Free` ordering | **Migrated:** the `guest_memory` instance provides its exact CX-sized bounce lease; flush remains source-ordered and free retires the lease without duplicate write-back. |
| Session-owned opaque data | no present OpenNT mirror has admitted a payload through this generic seam | **Implemented but unbound:** the fixed `session_data` instance publishes, looks up and releases opaque 32-bit IDs with an optional host-private destructor. It does not enable a BOP or expose a pointer. |
| DEM pathname, loader and DASD multi-span mapping | NUL-bounded path, 16 KiB loader guard or multiple sector payloads | Staged: each requires an explicit bounded-span or multi-lease decision; no forced generic migration. |
| `demerror_lock_shim.c` persistent hard-error locator | copied VHE/SYSDEV retained across calls | Staged/refuse: a mapping lease cannot persist; preserve copy/locator semantics until DEM error owner migration. |
| COMMAND `cmdmisc/cmdconf/cmdenv/cmdexec/cmdkeyb` | service-specific multi-buffer copies, multisz and child-worker handoff | Staged: migrate only after every source pointer has an exact span and synchronous lifetime. Async child work remains prohibited. |
| XMS `xmsa20` | current code retains a guest byte address across A20 calls | Staged/refuse: it is cross-call state, not a synchronous pointer lease. It must become a source-proven copy/locator contract, never a retained native pointer. |
| DPMI source | historical protected/persistent `Sim32*` usage | Explicitly refused in S1; belongs to the future whole DPMI package. |

## Verification Plan

The new local fixture proves the three distinct fixed instances, 16:16 and
32-bit-linear copied acquire, seeded write bounce, write-back, out-of-range
refusal, expired-epoch refusal, and session-data ID lifecycle. The existing DEM direct
mirror fixture is the regression target for the first migrated source caller.
Formal Ninja runs use an isolated disposable `build/M0-T258-S1/` root.

## P1 Verification Record

### Procedure

1. Generated the formal MSVC x64 `/MT` Ninja graph in the disposable
   `build/M0-T258-S1/formal-r12/` root.
2. Built the mapping-manager fixture plus the affected imported DEM and
   COMMAND fixtures through that graph.
3. Ran the mapping-manager, host-handle, DEM handle/file/search, COMMAND
   console/lifecycle and local-child fixtures.

### Observations

- The formal graph compiled and linked the requested targets.
- The mapping-manager fixture proves three distinct session instances,
  real-mode and 32-bit-linear bounce/write-back, expired-epoch and overflow
  refusal, session-data release, and rejection of a hand-embedded object that
  merely copies the guest-memory instance kind.
- The host-handle and affected imported DEM/COMMAND fixture set passes
  unchanged in observable service behavior.

### Interpretation

P1 establishes the shared mechanics without adding a BOP route.  In
particular, the guest-memory manager is a fixed session instance, not an
internally multiplexed key namespace; no family may create a look-alike
instance.  COMMAND and XMS pointer helpers remain staged according to the
migration matrix above.

## P2 Session-Lifecycle Verification

P1 review found that the fixed instances also need one outer lifecycle point;
family-local handle reset is not a substitute for session ownership.  The
manager now exposes `bx_ntvdm_session_mapping_registry_reset()`, which retires
all guest-memory leases and releases the session's host-handle and session-data
entries together. `bx_ntvdm_engine_run_v1` invokes it before admission and on
every engine cleanup path. The T258 fixture publishes a second opaque data
object and an owned native event, proves teardown invokes the data release
callback, proves its former ID no longer resolves, and proves the host handle
has been closed. The native runner and affected DEM/COMMAND fixtures relinked
and passed.

## Follow-up

After the DEM regression passes, update dependency 118 and the affected DEM
rows from generic migration debt to an explicit partial migration statement.
COMMAND and XMS are separate bounded P deliveries; no new BOP is enabled.
