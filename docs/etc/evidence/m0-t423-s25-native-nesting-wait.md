# M0 T423 S25 — Native Nesting and Wait Evidence

## Question and baseline

Why does the S24-accepted direct NTCON package pass sequential native reuse
but time out when a CMD batch starts another `run16 cmd` before its own CMD
exits? The baseline is the owner-accepted protocol-23 seven-file package;
the S24 evidence records its hashes and passing direct tests. This record
does not change or invalidate that bounded acceptance.

## Inputs and procedure

The retained `build/M0-T423/S24-helperless-clean/native-reuse.cmd` launches
three inner `run16 cmd /c` commands sequentially, checking their return codes.
The private-desktop Console observer starts `run16 cmd /d /c` with that batch
and captures a screen, timeout process snapshot and process exit. The S25
reproduction uses a copied accepted package under `build/M0-T423/S25/` and a
temporary short drive mapping. The owner subsequently required **Z:** as the
only such mapping; it was removed after the test. No guest file was modified.
The long-path setup returned 5 even for `cmd /c ver` and is a known separate
limitation; it is not used to diagnose native nesting.

## Observations

- Short-path `run16 cmd /d /c ver` returned 0 and displayed the Windows
  version text. This controls for package startup and visible output.
- Short-path `run16 cmd /d /c native-reuse.cmd` timed out after 30 and then
  45 seconds, with no completed batch text. The latter observer report is
  `build/M0-T423/S25/z-nested-live.txt`; the package remains unmodified.
- At the 45-second run, the outer CMD remained alive, its first inner
  `run16 cmd /c ver` remained alive, and both NTCON and NTKVM remained alive.
  The process snapshot showed outer CMD parented by NTCON and inner run16
  parented by that CMD. Only test-owned mapped-drive leftovers were ended;
  unrelated installed-package processes were not touched.
- `src/ntcon-exe/main.c` calls `ntcon_executions_wait_idle()` before every
  `worker_base_get_next_command()`. The wait is infinite until all active
  direct serving threads end. Meanwhile the outer CMD waits for the inner
  run16, so its direct serving thread cannot end. The inner request needs
  the same NTCON to call GetNext. The broker's same-root path already permits
  an in-flight direct record, and NTCON's execution owner already counts
  multiple active requests; the main-loop idle gate is the divergent barrier.

## Interpretation and next proof

This is a high-confidence host-side circular wait, not a guest defect or
evidence that the 30-ms hidden-Console output sampler caused the hang.
S25 must allow an authenticated nested direct request to be taken while the
outer target is still active, preserve one shared Console/frontend and the
real Windows parent/child exit codes, and keep failed broker completion a
worker fault. A finite startup deadline alone would merely convert this
deadlock into a timeout; it is not the repair. Before production publication,
add a deterministic nested test, prove both inner and outer completion and
visible text, then rerun the accepted S24 and full project gates. This is an
investigation result, not a claim that the production fix is implemented.
