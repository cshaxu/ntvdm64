# T225 S10 Native Run-Control and Observation Map

## Question

Why did the bounded primary-image run reach only reset-era output, and which
current code path can provide the required NTIO, 50:11, and NTDOS continuity
facts without introducing a BOP semantic or modifying source-built guest bytes?

## Inputs

- S10 Direct/Readonly result:
  t225-s10-primary-bootstrap-observation-result-001.md.
- Native CLI: src/cli/ntdos64_native_cli.c and
  src/cli/ntdos64_engine_worker_v1.c.
- Engine and machine stage:
  src/bx-mantle/bx_ntvdm_engine_{contract,run}_v1.[ch] and
  src/bx-mantle/bx_ntvdm_machine_stage_v1.[h,cc].
- Current composition and adapter observations:
  src/bx-vdm/bx_ntvdm_composition_runtime_v1.[ch],
  bx_ntvdm_vdm_generic_ud_bridge_v1.c, and
  bx_ntvdm_bop_sequence_observation_v1.[ch].
- Current primary profile:
  build/M0-T225-S10/primary-bootstrap-inputs-r2/profile.json.
- Existing finite-run and engine tests listed in the formal Ninja module graph.

## Existing native control path

| Step | Current code and copied contract | Result |
| --- | --- | --- |
| CLI policy | ntdos64_native_cli.c initializes instruction_tick_budget to exactly 1000000. Its public parser accepts no budget option. | Every normal CLI run uses the same fixed internal budget. |
| Worker lifecycle | ntdos64_engine_worker_v1.c starts the engine on an owned thread and waits for either it or the Ctrl+C cancellation event. | External process termination bypasses normal result classification. |
| Engine pass-through | bx_ntvdm_engine_run_v1.c copies the request budget unchanged to bx_ntvdm_machine_stage_v1_execution_request. | No CLI/engine policy is added after parsing. |
| Mantle watchdog | bx_ntvdm_machine_stage_v1_execute registers machine-stage-stop for exactly the copied tick count and returns EXECUTION_BUDGET only after cpu_loop() returns. | The timer is an instruction-tick mechanism, not a wall-clock timeout. |
| CLI terminal | Only after the worker joins does the CLI print its terminal/detail/lifecycle/presentation line. | S10's external watchdog can leave stdout empty even when early execution has occurred. |

The two ten-second S10 runs cannot show whether the internal one-million-tick
watchdog would eventually return. They establish only that it did not return
within that external wall-clock bound.

## Existing observation facilities

| Facility | Owner and activation | Why it cannot satisfy S10 by itself |
| --- | --- | --- |
| Armed entry copy | bx_ntvdm_machine_stage_v1_copy_real_mode_entry is mantle-owned and copies the requested/armed CS:EIP. | It proves staging state, not that CPU execution fetched the entry. |
| Profile machine observation | byob_profile and bx_ntvdm_machine_profile_abi accept bounded ranges, but only a profile-declared neutral UD2 (0F 0B) trigger can publish one. | The source-built S10 profile declares no such trigger; inserting UD2 would alter guest bytes and violate the exact-image premise. |
| BOP sequence record | bx_ntvdm_bop_sequence_observation_v1 is adapter-owned, default-off, and records only after an accepted C4 C4 BOP route. | Its enable/copy calls appear only in fixture code; engine/CLI expose no copied post-run record. |
| Generic UD bridge | bx_ntvdm_vdm_generic_ud_bridge_v1 invokes the current boot/native composition, then optional adapter observations. | It is already outside Bochs, but it yields no public lifecycle report for an ordinary CLI invocation. |
| Generic STOP marker | bx_ntvdm_mantle_generic_ud_stop_observation_* distinguishes an accepted typed STOP from the machine watchdog. | It is intentionally selector-free and returns only an execution status, not the event sequence or CPU state. |

The primary S10 profile has neither machine_observations nor a neutral-UD2
trigger. No existing mechanism publishes an NTIO fetch marker before the
first UD. Therefore the current runtime has no observation path for the
first requested marker.

## Boundary and recovery decision

This is not an OpenNT historical-service recovery. It is a current CLI/mantle
run-control and copied-observation gap; the OpenNT source-recovery ladder does
not authorize a substitute DOS, DEM, COMMAND or BOP implementation here.

- Original OpenNT source: not applicable; historical NTVDM does not own the
  modern x64 CLI's wall-clock/run-budget control.
- Smallest current seam: retain the existing engine request's fixed-width
  instruction_tick_budget and machine-stage statuses; admit a bounded CLI
  parser/control surface only if needed to select that already-defined field.
- External-code intrusion: rejected. No bx-core/Bochs CPU, decoder, memory,
  device or firmware modification is indicated.
- New behavior: not admitted by S10. A new CLI option or public observation
  result is a material active-packet ABI/behavior expansion and requires a
  separately admitted S.

## Required next admission

The next S must be explicitly limited to **generic native run control and
copied observation**:

1. preserve the default one-million-tick behavior for ordinary CLI calls;
2. allow a bounded, validated caller-selected tick budget solely for
   reproducible native observation;
3. return an existing normalized machine/engine terminal result without
   external process termination;
4. if a new report is required, carry only stage/status and existing
   fixed-width facts; do not decode BOP selectors in bx-core/bx-mantle, inspect
   arbitrary guest memory, or add a guest-byte trigger;
5. test Direct and Readonly, invalid budget rejection, default compatibility,
   and Overlay refusal.

A separate later adapter-owned package may expose the already-recorded BOP
sequence after run control is proven. It must not be folded into the first
CLI-budget change.

## Follow-up

S10/P3 proves the first divergence cannot currently be classified with the
existing CLI surface. No BOP, provider, firmware, device or source-built guest
change is admitted by this record.
