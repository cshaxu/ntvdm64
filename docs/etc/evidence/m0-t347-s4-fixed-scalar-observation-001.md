# M0 T347 S4 — Fixed scalar-cohort observation

## Fixed inputs

One console-owning, non-debug observation used:

- newly linked selected CPU40/x86 product staged under
  `build/M0-T347/S4/runtime-x86-r1`;
- the unchanged media staging script and original MVDM media set;
- unchanged `build/tools/console-startup-observer.exe` eight-second timeout;
- the normal `-f -o --ordinary-child` activation.

The raw report and console sidecar are untracked research evidence at
`artifacts/research/m0-t347-s4-scalar-fixed-container.txt*`.

## Result

The observer returned its ordinary timeout-controlled result `0x53504354`.
The console records original ingress markers `50:11`, `50:3B`, `50:0F`,
`50:1B`, and `54:05`, with no later BOP marker, explicit scalar-adapter error,
or product exception report.

`54:05` is the pre-dispatch original COMMAND marker. It proves the same
ingress reached by the prior fixed run, but it does not prove that the original
cmdSetInfo body completed or identify a branch after it. The observation
therefore neither validates scalar guest execution nor selects a leaf repair.

## Disposition

T347's source/link contract is closed independently by S1–S3. This unchanged
run finds no evidence that a retained raw scalar alias remains or that the
scalar adapter failed. The next work remains the existing package-first queue;
any eventual post-54:05 continuation work must begin with source-owner review,
not another altered observation or BOP-specific workaround.
