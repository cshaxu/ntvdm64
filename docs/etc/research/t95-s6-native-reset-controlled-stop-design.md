# T95 S6 Native Reset-to-Controlled-Stop Design

## Question

What is the smallest native lifecycle observation that can prove the r4
source-built Bochs container enters its original reset/simulation path and
terminates under an internally owned bound, without activating the adapter,
guest payload, host capabilities, or a replacement machine loop?

## Inputs

- r4 source-built native container evidence:
  `t95-s6-intact-native-build-observation-003.md`.
- Existing generated r4 profile and its two copied Bochs ROM inputs.
- Imported Bochs 2.6 `main.cc:702-705,1306-1313,1058-1118` and
  `pc_system.cc:409-414`.
- Historical UCRT-only reset-fixture tool, retained as behavioral evidence but
  not as an executable input for the MSVC r4 island.

## Original Mechanism

`-q` selects Bochs's original quick-start route, avoiding the configuration
interface.  `-benchmark n` stores a numeric benchmark value and forces the
same quick-start mode.  During ordinary native hardware initialization,
`main.cc` registers Bochs's original one-shot `benchmark.timer` for
`n * 1,000,000` emulated ticks.  Its original `bx_pc_system_c::benchmarkTimer`
sets `kill_bochs_request`; the unchanged single-CPU native loop observes that
flag, emits `cpu loop quit, shutting down simulator`, calls the original
`bx_atexit`, and returns.

This is a bounded *emulated-tick* mechanism, not an instruction-count
facility.  No source-backed exact instruction-budget command is identified in
the imported native lifecycle.  The first observation must state this fact
rather than falsely call one million ticks one million instructions.

## Proposed One-Shot Observation

Use only the r4 executable, the r4 copied `intact-native-runtime-profile.bochsrc`,
and its copied BIOS/VGA ROM files in a new evidence-only fixture directory.
The exact process argument shape is:

```text
ntdos64-native-container.exe -q -f intact-native-runtime-profile.bochsrc -benchmark 1
```

`1` is the smallest nonzero original command-line unit: approximately one
million emulated ticks.  The runner records stdout, stderr, Bochs log, exit
code, input/output SHA-256 values, elapsed wall time, and the process command.
It may impose a host wall-clock watchdog only to terminate a non-returning
process and classify that outcome as a failure; a watchdog termination is not
accepted as Bochs's controlled stop.

## Required Acceptance and Negative Predicates

The observation succeeds only if all apply:

1. The executable and copied profile/ROM hashes match the r4 evidence.
2. The log reports original benchmark-mode activation.
3. The log reports the original CPU-loop shutdown marker.
4. The process exits without host watchdog termination.
5. The command/environment contain no adapter opt-in, BYOB profile/root,
   CLI, OpenNT payload, BOP, DOS/WOW/DEM or guest-disk input.

The observation does **not** prove a specific reset-vector instruction,
firmware completion, disk boot, host-service dispatch, `#UD` handoff, DOS,
WOW16, CLI composition, or NTVDM usability.  Any configuration parse failure,
panic, GUI/configuration prompt, unbounded process, or unexpected adapter
term is a terminal classified result and must not be repaired in place.

## Boundary and Follow-Up

All stop mechanics remain in the imported Bochs lifecycle; the adapter is not
linked or loaded.  No Bochs source change, build-graph change, configuration
feature enablement, firmware generation, or adapter/OpenNT change is needed.
The future fixture is a separate tool under `tools/` plus a fresh evidence
directory, not a product runner.  A successful result permits only a later
decision on a stable adapter/CLI composition surface; it does not authorize
guest loading or service work.
