# T189 S1 v5 runtime-predicate map 001

Date: 2026-08-12  
Packet: M0 T189 S1  
Disposition: source map closed; one adapter-only diagnostic requirement is justified.

## Matched facts

T188/S10 observes `C4 C4 54 0C`, vector 6, real mode, RIP `0x732`, and
DS:DX `8DC8:33D9` (physical `0x91059`). It also observes ingress class 3,
COMMAND family 4, callable service `0C`, `installed=1`, provider=1, and
`pending=0`.

These facts satisfy the visible conditions in:

| Source | Condition | Result |
| --- | --- | --- |
| `bx_ntvdm_bop_ingress_v1.c:37-45,77-85` | four-byte `54:0C` is callable COMMAND mapped-deferred ingress | matched |
| `bx_ntvdm_command_plane_v1.c:20-30` | mapped COMMAND ingress has original/deferred provider and service 12 | matched by ingress classification; registry remains static source fact |
| `bx_ntvdm_adapter_runtime.c:543-553` | provider, COMMAND plane gate, prepare, and queue must all succeed before a result is returned | the compound chain declined |
| `bx_ntvdm_cmd_boot_file_service.c:28-55` | ABI validity, vector 6, real mode, exact bytes, aperture-safe DS:DX, ASCII profile path, add/resume/preflight | all externally observable terms matched |
| `bx_ntvdm_multi_write_transaction.c:20-31` | copied transaction must pass ABI/event/CPU/write/result preflight | only copied ABI validity remains unobserved |

The controlled T188/S2 fixture uses the same fault RIP, DS:DX, bytes, real
mode and v2 route and obtains the expected `C:\CONFIG.SYS` write. It proves
the provider's ordinary source path is viable, but not that all runtime copied
records have identical reserved/metadata fields.

## Exact remaining uncertainty

The compound chain can still decline at only these internal predicates:

1. `bx_ntvdm_boot_namespace_provider_v1_valid` checks provider magic/version
   plus `search_snapshot` and `search_transaction` validity.
2. `bx_ntvdm_exception_event_v1_valid` checks copied ABI metadata and kind.
3. `bx_ntvdm_cpu_state_v1_valid` checks copied ABI metadata, execution-mode
   enum and `reserved0`.
4. transaction construction/preflight or session queue rejects a copied
   record.

None is exposed by BX-TRACE-060, and forcing any one would be a semantic
change. The minimum next proof is an adapter-owned, default-off stage code
for the existing `54:0C/0D` compound chain. It may report only the first
failed category (provider, gate, prepare, queue) and, inside `prepare`, only
the above validation category; it must not expose selector/service/payload,
guest memory, host paths, or change dispatch/result/session state. No Bochs
change is warranted.
