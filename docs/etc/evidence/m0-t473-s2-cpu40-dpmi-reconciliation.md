# M0 T473 S2 — CPU40 and DPMI source reconciliation

## Scope and method

This is the first bounded group of the S2 plan.  It reads the formal selected
source lists, linker composition and all production callers of the CPU40/DPMI
carriers.  The result distinguishes original execution semantics from the
modern-host values that cannot be represented by an NT4 kernel VDM layout.
It changes no source and makes no runtime-completion claim.

The selected original owners are complete at the source-selection level:

- CCPU40 selects 130 original translation units plus its explicit `localfm`
  state carrier; its historical `ntstubs.c` alternative is excluded because
  it duplicates the real CPU/FPU/SAS providers.
- DPMI32 selects its 15 portable production units.  The `i386/dpmi386.c` and
  `i386/dpmimem.c` paths require passive kernel `CONTEXT`/`VDM_TIB` state and
  are not missing CPU40 sources.
- The current `ntvdm.exe` product selection contains the corresponding
  original owner objects; the r27 inventory records 350 selected NTVDM
  sources overall.  The CCPU/DPMI manifest, extraction and edition proof is
  retained in the [T420 CCPU audit](m0-t420-s1-ccpu386-package-audit.md) and
  [T420 DPMI recovery](m0-t420-s18-dpmi32-package-recovery.md).

## Carrier decision table

| Carrier | Original caller/semantic owner | Missing or incompatible NT4 facility | Current authority and disposition |
| --- | --- | --- | --- |
| `mvdm_softpc_execution.c` | original `obj.vdm/ntvdm.c` and `host_start_cpu` / `host_applClose` | Original process exit would dispose of the complete VDM; the product keeps a worker process/session alive until the centralized lifecycle chooses retirement. | **Retain.** It binds one active session, brackets the unchanged original entry with one termination escape and runs the original close cohort.  It owns neither CPU loop nor BOP meaning. |
| `mvdm_softpc_guest_memory.c` | CCPU40 SAS and original XMS/COMMAND/WOW callers | NT4 exposes durable raw process aliases; current session epochs require bounded native access. | **Retain.** CPU40 is the sole byte-store authority; the carrier only creates/revokes numeric leases and copies through `c_sas_*`.  Its ordinary and non-local end is paired by the execution carrier. |
| `mvdm_softpc_physical_mapping.c` | original `nt_mem` / CCPU physical-address callers | `PhysicalPageREC` and kernel VDM mappings are not available to a CPU40 user-mode worker. | **Retain.** It maps only session-owned numeric page spans, registers exactly one teardown owner and resolves no guest algorithm itself.  It may be removed only with an admitted whole replacement for the unavailable kernel mapping provider. |
| `mvdm_softpc_effective_address.c` | original CCPU address and selector routines | CPU30 monitor descriptor table and host process aliases are unavailable. | **Retain.** Protected addresses are decoded by selected `selector_outside_GDT_LDT`/`read_descriptor_linear`; real/VM86 arithmetic is the original numeric form.  The carrier does not select a segment cache or invent descriptor state. |
| `mvdm_softpc_descriptor_fields.c` | original BIOS `tape_io.c` INT 15 move-block helper | The exact four-output historical helper body is absent from the selected user-mode source closure. | **Retain.** It presents the historical output layout around original CPU40 `read_descriptor_linear`, which remains decoder and fault owner. |
| `mvdm_softpc_fast_bop.c` | portable DPMI CALLF/BOP state and selected CPU40 dispatch | The original endpoint is a kernel monitor CS:IP identity; CPU40 has no equivalent executable kernel address. | **Retain.** It reserves one LDT identity through the selected DPMI publication path and redirects only the three named endpoints to the existing dispatcher.  It is not a DPMI scheduler or general call gate. |
| `mvdm_softpc_ccpu_wait.c` and selected `fmstubs` leaves | original CCPU HLT/event consumers | Original event transport assumed the NT4 host/kernel relationship; selected sources lack an activity-provider body. | **Retain narrowly.** The wake event is a single host wait object with begin/signal/end ownership.  The absent activity provider is the exact selected void fallback—not a timer, poll loop, yield policy or synthetic interrupt. |

## Authority and removal tests

There is one authority in each disputed area: CCPU40 owns register,
descriptor, SAS and execution semantics; DPMI32 owns policy and source
transitions; the session owns lifetime and lease validity; the carrier owns
only conversion of a missing host/kernel resource.  No table row has both a
carrier state machine and a competing original state machine.

Consequently no body is removable merely by importing a file with the same
function name.  The only valid removal conditions are: a selected original
user-mode provider with a complete CPU40-compatible dependency closure, or a
replacement modern resource boundary that preserves the listed original
caller's normal, rejection and teardown results.  Neither condition is true
for this group.  Reintroducing CPU30, kernel VDM, raw aliases or a blanket
host-pointer interpretation would expand authority and violate the S1
boundary matrix.

The existing source fixtures cover the relevant local contracts:
`ccpu_bounded_execution_fixture`, `ccpu_thread_lifecycle_test`,
`original_external_memory_test`, `physical_mapping_observation_test`,
`fast_bop_mode_fixture` and the DPMI descriptor-domain fixtures.  Because this
reconciliation does not modify production code, they are retained evidence
rather than rerun as a substitute for later package behavior proof.  S3 must
exercise the remaining CCPU/DPMI lifecycle and protected-mode paths through
their production callers before it can claim runtime completion.

## Result

No source import, removal or production build selection change is justified
for CPU40/DPMI.  This is a positive minimization result: it prevents a
misleading "original recovery" that would either duplicate selected original
CPU semantics or revive prohibited private-kernel dependencies.  The next S2
group is DOS/COMMAND/XMS, where the same source/caller/authority test applies.
