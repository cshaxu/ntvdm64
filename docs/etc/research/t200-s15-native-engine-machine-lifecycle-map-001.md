# T200 S15 — native engine machine-lifecycle map

## Question

How must the direct native engine attach the minimal mantle machine to the
already-real composition without reviving the full Bochs product shell, the
legacy adapter runtime, or fixture-only finite-run behavior?

## Inputs

- `src/bx-mantle/bx_ntvdm_minimal_machine.cc` and
  `bx_ntvdm_finite_run.[ch]`;
- `src/bx-mantle/bx_ntvdm_generic_ud_bridge.cc`;
- `src/bx-vdm/bx_ntvdm_composition_runtime_v1.[ch]`,
  `bx_ntvdm_startup_plan_abi.[ch]`, `bx_ntvdm_startup_session.c`, and
  `bx_ntvdm_ntio_preentry_v1.[ch]`;
- retained `src/bochs/main.cc` startup hook;
- T200 S14 direct composition closure.

## Existing-source findings

| Concern | Reuse / owner | Required decision |
| --- | --- | --- |
| Machine construction/reset/A20/port-space | `bx_ntvdm_minimal_machine_c` in bx-mantle | Reuse internally in a later C++ engine implementation; never expose its object through the C engine contract. |
| Checked NTIO publication + real-mode entry | `bx_ntvdm_startup_plan_v1` in bx-vdm | Reuse the pointer-free plan and its preflight contract.  The engine/mantle performs only the mechanical copy/preserve/CS:IP actions. |
| Current direct composition | `bx_ntvdm_composition_runtime_v1` in bx-vdm | Retain install/reset ownership.  It owns images and BOP package sessions, but currently exposes no direct startup-plan producer for its owned NTIO image. |
| Generic exception stop | bx-core/mantle typed generic bridge | Keep selector-blind.  A stop is an engine terminal category, not a CLI success or a BOP/service rule in Bochs. |
| Finite runner | `bx_ntvdm_finite_run` in bx-mantle | Fixture-private only: it copies an entire entry byte array and carries diagnostic snapshot controls. It is not the product engine entry or result ABI. |
| Old startup implementation | `bx_ntvdm_adapter_runtime` + `src/bochs/main.cc` | Reject: it installs from environment and injects adapter/startup semantics into full-product Bochs `main`. It cannot be the replacement path. |

## Product lifecycle order

The later engine implementation must follow this order:

```text
validate copied engine request
  -> bx-vdm direct composition install (owns source-built images/sessions)
  -> bx-mantle minimal machine initialize/reset
  -> bx-vdm direct startup-plan prepare from the installed composition
  -> mantle mechanical checked RAM publication + real-mode CS:IP entry
  -> mantle-owned bounded CPU loop
  -> copy typed terminal category (no BOP vocabulary in result)
  -> bx-vdm unbind/release session state
  -> bx-mantle machine cleanup
```

Failure before CPU entry follows the same reverse-order cleanup.  The engine
must distinguish composition rejection, machine failure, startup-plan rejection,
bounded execution terminal, controlled guest terminal, and future ordinary
guest completion; none automatically maps to process success.

## Missing direct boundary

The old `bx_ntvdm_startup_session_v1_prepare_ntio_plan` proves the correct
generic plan geometry (`0x700` payload, real-mode `CS=0x70`, and checked
preflight), but it belongs to a separate environment-installed session.  The
S14 direct composition owns another validated NTIO image privately.  There is
no API that prepares a plan from that installed image, and the existing
`bx_ntvdm_ntio_preentry_v1_prepare` requires a fixture-provided preentry input
plus the private finite-run request type.

Therefore the next implementation must be a **direct composition startup-plan
provider**, not a call back into `adapter_runtime`, `startup_session`, or
finite-run.  It shall produce the existing pointer-free startup-plan data plus
an adapter-owned, bounded payload view.  The product preentry recipe must be
separately source-mapped; test-fixture IVT/BDA bytes cannot silently become a
product default.

## XMS cleanup boundary

Minimal-machine initialization enables the mantle A20 and extended-memory
lifecycle.  Current native XMS sessions reserve/allocate mantle resources
only when their services run, but the current composition reset has no explicit
native-session release operation.  Before admitting CPU execution, the next
package must specify and test release of any such allocation while the mantle
lifecycle is still active.  This is a lifecycle closure requirement, not a
reason to move XMS semantics into mantle.

## Procedure

Read the listed lifecycle, startup-plan, generic-stop and old-product sources;
compare their state ownership and cleanup paths against the S14 direct path.
No implementation, machine initialization, or guest run was performed.

## Interpretation and confidence

High confidence on the lifecycle split and on rejection of the old `main.cc`
hook.  High confidence that a direct startup-plan provider is the first missing
product boundary.  The actual product preentry recipe and normal terminal
semantics are not yet proven and remain explicitly open.

## Follow-up

T200 S16 is admitted to design and implement the direct composition
startup-plan provider and its release-safe lifecycle contract with focused
x64 `/MT` tests.  It may not initialize a machine, start a CPU loop, or reuse
the environment/finite-run path.
