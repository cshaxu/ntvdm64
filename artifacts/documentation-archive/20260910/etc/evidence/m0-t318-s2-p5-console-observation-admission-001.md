# M0 T318 S2 P5 — Console-qualified original-entry observation admission

## Question

Can the existing pipe-captured product observations distinguish a stop before
the original `InitialiseDosEmulation` DOS-image load from a stop caused by the
historical console contract itself?

## Inputs

- The resource-complete x86 product staged from
  `build/M0-T318/S2/runtime-r3`.
- A second staged layout identical except for removal of `dos/NTIO.SYS`.
- Original `softpc.new/host/src/{nt_reset.c,nt_hosts.c,nt_event.c}` and
  `softpc.new/base/support/main.c`.

## Procedure

1. Run the resource-complete process with original `-f` and a five-second
   owned-process watchdog, with the harness standard streams captured.
2. Repeat from an otherwise identical layout with only `dos/NTIO.SYS`
   removed.
3. Repeat the complete-media run with original `-f -o`, which disables the
   original stream-I/O branch in `host_applInit`.
4. Compare the three outcomes with the original startup sequence.
5. Repeat the complete-media `-f -o` observation in a hidden real-console
   process rather than with captured pipe handles.
6. Read the next original pre-loader path in `nt_msscs.c`, then rebuild with
   only the resulting source-shaped configuration binding changed.

## Observations

- The missing-NTIO run remained alive until its watchdog and ended only when
  that owned tree was terminated. Its recorded arguments were `-f`; it had no
  process-crash output.
- The complete-media `-f -o` run likewise remained alive until its watchdog.
  This excludes the optional stream-I/O allocation/enable call as the common
  cause of the silent observation.
- In `nt_reset.c`, the remaining common prefix after `-f -o` is
  `DuplicateHandle`, ICA initialization, `init_host_uis`, and
  `nt_start_event_thread`; `base/support/main.c` reaches configuration and
  `InitialiseDosEmulation` only afterwards.
- `nt_hosts.c::InitScreenDesc` treats non-character standard handles as
  redirection and opens `CONIN$`/`CONOUT$`; failures use the original
  `DisplayErrorTerm` path. `nt_event.c` then configures an original console
  input thread. The capture harness deliberately supplies pipe handles, not
  an interactive console.
- The hidden-console observation also stayed alive until its owned three-second
  watchdog. Console-handle shape is therefore not the only common condition.
- The original `InitialiseDosEmulation` first calls `SetupInstallableVDD`.
  This host lacks the retired
  `HKLM\\SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers` key, for
  which original code displays a modal error before `scs_init` and `NTIO.SYS`
  loading.
- The selected source retains original VDD enumeration, but a narrow
  configuration binding treats only an absent key as an empty VDD set; other
  open failures retain the original error route. Both formal product links
  pass. A fresh `runtime-r6` hidden-console observation still ended only at
  its watchdog, which is not a DOS-loader result.

## Disposition

The watchdog results are **not a valid positive or negative test of the
original DOS-media loader**. The hidden-console control removes the first
console-shape ambiguity and identifies a concrete retired VDD configuration
boundary. Its source-shaped absent-key disposition is build-closed; the next
owner boundary is original `scs_init` / Base VDM command acquisition. No DOS
loader, BOP route, CPU loop, or guest state machine is changed by this finding.
