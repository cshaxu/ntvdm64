# M0 T261 S8 P6 — VDMREDIR host-body disposition

## Question

After the local `vrmslot.c` bodies moved to their original BOP owner, which
remaining live `opennt-host/vdmredir` functions can be made source-faithful
inside S8, and which require a later whole Redirector/VDD product package?

## Baseline

- Original `refs/opennt/base/mvdm/vdmredir/vrnmpipe.c` is 3,412 lines;
  the current re-rooted reached subset is 213 lines.
- Original `refs/opennt/base/mvdm/vdmredir/vrinit.c` is 748 lines; the
  current static-composition provider is 30 lines.
- Therefore their present paths and function spellings are useful owner
  markers, but neither file may be described as a byte-identical full
  translation-unit recovery.

## Per-function disposition

| Original provider group | Present live use | S8 disposition | Reason / successor |
| --- | --- | --- | --- |
| `VrIsNamedPipeName`, `VrConvertLocalNtPipeName` | DEM named-pipe classification and local-name conversion | Recover next as direct source bodies. | They require only their input buffers and public Win32 computer-name calls; no BOP selector, VDD, ICA, NetBIOS or DLC state. |
| `VrIsNamedPipeHandle` | local record classification | Direct original list lookup active. | It is coupled only to `OPEN_NAMED_PIPE_INFO`, not a guest-handle allocator. |
| `VrpAdd/Get/RemoveOpenNamedPipeInfo`, `VrAdd/RemoveOpenNamedPipeInfo` | current static named-pipe metadata | Direct original record/list bodies active. | The original record's `DosPdb` member is retained, but the source does not initialize or consume it in this reached non-debug group; no new PDB/mapping seam is invented. |
| `VrTerminateNamedPipes` | current session-uninitialize cleanup | Keep as an explicit source-derived cleanup divergence. | The historical `VrTerminateNamedPipes(DosPdb)` body is intentionally empty. The current all-record teardown cannot be mislabeled as that original body and moves only with the later session/Redirector lifecycle decision. |
| `VrReadNamedPipe`, `VrWriteNamedPipe` | synchronous DEM pipe interception | Keep their declared S8 source-derived exceptions until the Redirector/VDD lifecycle package is admitted. | Original `OVERLAPPED_PIPE_IO` uses VDD cancellation registration and its worker-completion lifecycle; the current bounded session replacement must not be silently relabeled as original. The recovered metadata record group does not supply cancellation/interrupt completion. |
| `VrInitialize`, `VrUninitialize` and the rest of `vrinit.c` | 57:00/01 static provider state | Retain the explicit source-derived single-session gate; do not call it direct recovery. | Original body requires `VDDInstallUserHook`, NetBIOS/DLC initialization, VDM load-info guest write, queued ICA interrupts and the historical VDD suspend/resume hooks. These are a whole Redirector/VDD/ICA lifecycle, not an S8 helper seam. |

## Consequence

S8 may recover the first two helper bodies and the named-pipe record group
without inventing a CCPU/SAS identity seam. It must not pull
VDD cancellation, NetBIOS, DLC, ICA or a multi-session broker into
`opennt-host` merely to inflate source coverage. Those remain explicit
follow-on owner work, presently represented by the Redirector IPC/network and
VDD/debugger package lines in `docs/etc/bop-list.md`.

This evidence corrects the earlier broad phrase “re-rooted original named-pipe
helper surface”: it means a re-rooted **reached provider surface**, not a claim
of complete original function-body recovery.

## P6 progress — helper recovery

`VrIsNamedPipeName` and `VrConvertLocalNtPipeName` are now copied into the
re-rooted `vrnmpipe.c` in their original control-flow order. `HOST-DIV-020`
records the only required x64-safe edits: defined same-array pointer
subtraction, explicit `strlen` narrowing, and a parenthesized assignment test
for the warning-as-error build. Their private historical-header macros remain
translation-unit local, so they cannot alter unrelated OpenNT providers.

The existing formal Redirector fixture now exercises positive and negative
pipe-name classification and forward-slash canonicalization. Fresh formal
`r012` compiles the complete affected graph, links the fixture and the fixture
exits successfully.

The original `OPEN_NAMED_PIPE_INFO` record (including its historical
`DosPdb` field), `VrpAddOpenNamedPipeInfo`, `VrpGetOpenNamedPipeInfo`,
`VrpRemoveOpenNamedPipeInfo`, `VrAddOpenNamedPipeInfo` and
`VrRemoveOpenNamedPipeInfo` are now active as well. This removes the prior
duplicate-handle admission and standalone list algorithm. The original
`VrTerminateNamedPipes(DosPdb)` body is deliberately empty, so the existing
all-record session cleanup is not rebranded as an original recovery and remains
the explicitly deferred lifecycle divergence (`HOST-DIV-021`). Likewise the
small static `vrinit.c` provider is explicitly `HOST-DIV-022`, rather than a
claim that VDD/ICA/NetBIOS/DLC initialization has been restored.
