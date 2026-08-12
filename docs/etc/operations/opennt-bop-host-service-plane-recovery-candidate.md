# OpenNT BOP Host-Service Plane Recovery Candidate

## Status

This is an unnumbered candidate-task plan for the first entry in
`docs/QUEUE.md`. It does not admit work, allocate a numeric task identifier,
or replace the single active packet in `docs/STATUS.md`.

## Objective

Replace the accumulating per-BOP recognizer pattern with one coherent,
adapter-owned routing and recovery of the unavailable OpenNT NTVDM
host-service composition. The adapter first connects independently composable
original OpenNT host code; it rehosts a contract only after proving that the
original composition cannot be used non-invasively. The result must preserve
OpenNT's dispatcher/module ownership, calling conventions, result layouts,
order, and failure behavior while keeping Bochs generic and keeping DOS, WOW,
CPU, firmware, and devices outside the adapter.

The passive `C4 C4 selector` listener and its complete definition inventory
already exist. Trace is therefore a prioritization and regression input; it
does not determine the architecture or authorize a new one-off handler.

## Required Shape

```text
Bochs #UD fact (copied event + instruction window)
  -> adapter BOP ingress and observation
  -> static OpenNT source-derived selector/service module map
  -> module-local source-derived contract
  -> profile capability backend or explicit unavailable disposition
  -> typed result / checked guest-memory transaction / controlled stop
```

The ingress identifies only the copied selector and, for a service family,
the copied service byte. It has no host path, guest-memory scan, historical
function pointer, or implementation-specific state change. The static map is
not a transplantation of `BIOS[]`, `MS_bop_*`, CCPU, or SAS: it selects a
provider only after the relevant original dispatcher and owner are evidenced.

## Provider Precedence

Every endpoint has exactly one recorded provider disposition, selected in this
strict order:

1. Call independently composable original OpenNT host implementation through
   adapter ABI copying and typed result return.
2. Retain the original implementation and replace only declared intrusive host
   platform facilities (for example file, console, or time access) with a
   contained CLI capability backend.
3. Use a minimal source-derived provider only when a recorded CCPU/SAS, old
   ABI, VDD/GUI, or unavailable NT-host composition edge prevents options 1
   and 2. It reproduces only the documented contract.
4. Preserve the original evidenced unavailable/failure disposition or defer.

Neither a definition nor a trace hit authorizes progression down this list.
Every fallback records why the preceding option is unavailable.

## Module Families

| Family | Historical evidence to recover | Candidate disposition |
| --- | --- | --- |
| DOS/DEM (`50:xx`) | `demdisp.c`, DEM service modules, NTDOS callers | One DEM plane; profile-backed file/drive/search services or explicit source-proven failure. |
| COMMAND (`54:xx`) | COMMAND dispatcher/service modules and startup callers | One COMMAND plane; startup information and command-delivery contracts migrate from existing isolated services. |
| XMS (`52:xx`) | XMS dispatcher/service boundary | Module map and profile disposition first; no XMS manager is implied. |
| DPMI (`53:xx`) | DPMI dispatch boundary | Module map and profile disposition first; no protected-mode host is implied. |
| Redirector (`57:xx`) | Redirector dispatch/load boundary | Module map and profile disposition first; no network/ambient host filesystem is implied. |
| Debugger and top-level notification (`56`, `59`--`5F`, `FD`, `FE`) | `nt_bop.c` and specific original owners | Explicit notification, stop, unavailable, or pass-through disposition; no debugger/VDD/GUI implementation by default. |
| SoftPC BIOS selectors | `bios.c` table plus reached caller/owner evidence | Preserve provenance ambiguity until control-flow evidence proves a Bochs-native owner, an admitted adapter host service, or unsupported behavior. |

## Delivery Sequence

The future admitted task should use module-scale subtasks, not service-number
subtasks:

1. Create a complete selector/service to original dispatcher/module/owner map,
   including existing isolated endpoint migrations and explicit uncertainty.
2. Define one adapter ingress, provider-selection, result, and unavailable
   disposition ABI. Prove old direct recognizers either route through it or
   remain quarantined pending migration.
3. Recover the DOS/DEM host-service plane as one module family.
4. Recover the COMMAND host-service plane as one module family.
5. Classify XMS, DPMI, redirector, debugger/notification, and SoftPC-BIOS
   families against the first profile; add only profile-required module
   contracts.
6. Run full selector/service/owner/capability/trace regression proving no
   unimplemented endpoint is accidentally successful and no implemented
   endpoint bypasses the shared plane.

Each step remains subject to an admitted Status packet and can stop if source
evidence shows that the requested behavior belongs to Bochs, guest OpenNT, or
an unavailable external NT host facility.

## Non-goals And Guardrails

- No generic historical dispatcher execution, dynamic handler registration,
  `BIOS[]` reconstruction, or CCPU/SAS import.
- No service implementation merely because a selector exists or appears in a
  trace.
- No Bochs CPU, memory, firmware, device, or configuration semantics in the
  adapter; no BOP/DOS/WOW semantics in Bochs.
- No DOS kernel/filesystem or WOW reimplementation. A host capability can be
  rehosted only where the original host component owned it and the profile has
  admitted its bounded contract.
- Every module endpoint has one disposition: original provider, original
  provider with contained capability backend, source-derived provider,
  delegated native Bochs/guest owner, explicitly unavailable, or pass-through
  pending evidence. "Observed" is not an implementation state.

## Admission Evidence

The future task must cite the immutable BOP inventory, the complete passive
listener result, original OpenNT dispatcher/module source, the active profile
capability matrix, and focused positive/negative tests. It must also reconcile
the existing wording that a catalogue is not a generic historical dispatcher:
the catalogue remains identity-only; the separately admitted host-service
plane supplies the static, source-derived module map and only its individually
closed contracts may produce results.
