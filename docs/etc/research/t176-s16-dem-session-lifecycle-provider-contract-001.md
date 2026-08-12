# T176 S16 — DEM Session-Lifecycle Provider Contract 001

## Question

Can the finite boot-namespace composition close the reached `C4 C4 50 3C`
(`SVC_PDBTERMINATE`) boundary without becoming a VDD/device host or claiming
that the historical NT host composition is present?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdisp.c:159` maps the service to
  `demTerminatePDB`.
- `src/opennt/base/mvdm/dos/dem/demsrch.c:659-690` reads the PSP from BX,
  runs VDD notification (except first call), invokes host cleanup, then frees
  the PSP-owned FindFirst/FindNext list; an absent list is a successful no-op.
- `VDDTerminateUserHook` walks registered callbacks in
  `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c:1250-1271`; the same
  file's `HostTerminatePDB` invokes floppy/fdisk cleanup at `1411-1416`.
- `bx_ntvdm_boot_namespace_provider_v1` owns the immutable profile snapshot
  and search transaction. Its `bx_ntvdm_search_sessions_v1` has at most
  sixteen private continuation slots, each labelled by `owner_pdb` and owning
  a finite heap snapshot (`bx_ntvdm_search_session.h/.c`).

## Contract

The future provider is a source-derived **DEM session-lifecycle provider**.
It is not a PDB manager, VDD dispatcher, device cleanup facility or DOS
kernel service.

### Admission and selection

It owns an invocation only when all conditions hold:

1. common BOP ingress identifies real-mode `50:3C` from an exact four-byte
   `C4 C4 50 3C` CPU exception-window event;
2. registry selection and DEM-plane classification name this source-derived
   lifecycle provider in the DEM namespace component;
3. the installed profile selected `boot_namespace_provider_v1`; and
4. that provider is the complete namespace composition: no registered VDD
   callback owner, floppy/fdisk/device cleanup backend, ambient host-directory
   backend, host path or host handle exists.

The final condition is a named-provider fact, never an inference that missing
historical work is harmless. A profile adding any such owner must decline this
provider pending separately admitted original-owner composition evidence.
All mismatches retain the existing unavailable/deferred result; no adapter-made
success is allowed.

### Exact owned state and effect

The provider reads only BX low word already copied in the typed CPU state. It
does not gather guest memory or inspect DOS structures. Its same-island
operation is:

```text
for every occupied finite-search slot where slot.owner_pdb == BX:
    free slot.entries
    clear token, owner, indexes and pointer
leave all other slots unchanged
```

It is the finite-provider equivalent of the original final search-list stage.
No matching slot is a successful no-op, like original `GetPspFFindList` absence.
The operation preserves container magic/version/validity; it must not release
the whole transaction, snapshot, readonly namespace, or another PDB's slot.
An adapter-local released-slot count is test-only and never crosses an ABI.

### Result contract

Original `demTerminatePDB` specifies no guest result. After owned cleanup, the
provider performs the ordinary typed resume at `fault_rip + 4`, with no
register delta, flag change, guest write, bulk transaction, host callback or
machine operation. In particular, it must not borrow the CF-clearing contract
of unrelated original DEM no-op services.

### Explicit exclusions

- VDD, `HostTerminatePDB`, floppy and fdisk cleanup remain unavailable
  historical composition, not adapter behavior.
- No guest PSP, DTA, FCB or DOS-memory read is introduced.
- Normal file tokens, COMMAND state, DTA registration, target lifecycle,
  Bochs state, CLI drive projection and FCB search are unaffected.
- If FCB search later gains a finite continuation owner, it needs a successor
  contract or this same release primitive before claiming PDB cleanup.

## Test matrix for S17

| Case | Required result |
| --- | --- |
| Exact admitted `50:3C`, two owned slots plus one foreign slot | Release exactly the owned snapshots; foreign slot/container valid; generic four-byte resume only. |
| Exact admitted `50:3C`, no slot for BX | Successful generic resume and no state change. |
| Wrong vector/mode/opcode/selector/service/ingress/registry/plane or missing finite provider | Decline unchanged. |
| VDD/device/ambient-search/host-handle composition | Not selectable; no synthetic success. |
| Repeated same-PDB termination | First releases; second is the absent-list no-op. |

## Interpretation and next step

Confidence is high that these slots are the sole current state with the
original service's PSP-indexed lifetime. The missing VDD/device calls are not
reimplemented; selection makes their absence explicit. T176 S17 may add the
same-island release primitive, provider wrapper and tests through the common
plane. It must not add a special BOP recognizer, guest-memory ABI, Bochs/OpenNT
change or runtime retry.
