# M0 T346 S4 P1 — fixed presentation observation

## Fixed run

One console-owning, non-debug observation used the newly linked x86 product:

- product: `build/M0-T343/S3/record-x86/original-softpc-process.exe`;
- fixed stage: `build/M0-T346/S4/runtime-x86-r1`;
- fixed media manifest SHA-256:
  `15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`;
- staged product SHA-256:
  `c2eef68690844a74b7c32ee9706d676a09aff20b233495067bd1c91082506718`;
- observer: `build/tools/console-startup-observer.exe`, with its unchanged
  eight-second timeout.

The raw report and sidecars remain untracked research material at
`artifacts/research/m0-t346-s4-presentation-fixed-container.txt*`.

## Result

The observer ended in its ordinary timeout-controlled stop (`0x53504354`).
Its only guest markers were `50:11`, `50:3B`, `50:0F`, `50:1B`, and `54:05`.
It did not observe a graphics surface allocation, source invalidation, window
repaint, Alt+Enter, graphics-mode workload, or a later guest continuation.

## Disposition

This is a valid unchanged-profile non-regression observation of the selected
x86 product, but it is not graphics proof. `54:05` is original
`SVC_CMDSETINFO` / `cmdSetInfo`, the registration of the DOS `SCSINFO` state;
it is not CONFIG processing. `CONFIG.NT` is the later `54:0C`
`SVC_GETCONFIGSYS` / `cmdGetConfigSys` service and was not reached. The active
blocking edge is therefore the pre-configuration original registration/startup
continuation after `54:05`; it must be recovered by that owner package, not by
adding a trace-selected display, video or window workaround. T346 keeps its
S4 runtime workload exit condition open until that upstream continuation
permits it.
