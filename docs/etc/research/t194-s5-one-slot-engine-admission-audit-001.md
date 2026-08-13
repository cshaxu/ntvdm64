# T194 S5 one-slot engine-admission audit 001

Date: 2026-08-12  
Packet: M0 T194 S5  
Disposition: source evidence complete; one adapter-composition omission blocks
one-slot engine handoff.

## Question

Does the frozen profile-to-engine handoff carry the explicit v6 one-slot plan
through the existing adapter installation boundary?

## Inputs and procedure

The audit compared the v6 producer path in `src/cli/ntdos64_run.c` and
`src/cli/byob_launch_plan_v2.c` with the install boundary in
`src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.c`, using the retained v5
runtime-install fixture as a control.

## Observations

1. `ntdos64_run` validates the selected profile, builds the plan from its
   declared target count, and places it unchanged in
   `NTDOS64_ADAPTER_LAUNCH_PLAN`. For v6 this is the already-tested `2,1,…`
   encoding.
2. `byob_launch_plan_v2_from_environment` accepts both one and two slots.
3. `bx_ntvdm_adapter_runtime_v1_install`, however, wraps all of the following
   in `if (selection.declared_target_count == 2u)`: environment-plan parsing,
   target-kind validation, immutable TARGET loading, terminal-QUIT loading,
   assignment of `launch`/`has_launch`, and the required v5 QUIT identity.
4. The following boot-namespace-provider initialization independently also
   requires `selection.declared_target_count == 2u`.

Thus a valid v6 profile is not rejected as malformed at this boundary. Its
installation can proceed without a parsed launch plan, target image, or boot
namespace provider. The CPU/engine therefore has no valid one-slot COMMAND
composition to consume. The existing v5 fixture is not a v6 acceptance test:
it deliberately supplies `2,2,c,00`, `QUIT.COM`, and asserts the v5 provider.

## Interpretation

The owner is the adapter's profile composition assembly, not Bochs, the guest,
the BOP dispatcher, a device, or the CLI. This is a coherent residual
two-slot assumption from the original finite controlled-stop profile. It is
not evidence to add a fallback QUIT, alter `54:11`, change CPU execution, or
start a runtime retry.

## Follow-up

T194 S6 may change only this one assembly boundary as a unit: require and
validate the supplied one- or two-slot plan against the declared profile,
always load the declared TARGET, conditionally load/validate QUIT only for
two slots, and initialize the same provider for a four- or five-file
namespace. It must add focused v5/v6 install tests. It remains outside Bochs
and does not authorize a guest run.
