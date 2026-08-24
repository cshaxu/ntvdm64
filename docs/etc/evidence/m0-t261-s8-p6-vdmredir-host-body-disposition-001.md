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
| `VrIsNamedPipeHandle` | local record classification | Recover with its original list lookup when the record group is recovered. | Its source is coupled to `OPEN_NAMED_PIPE_INFO`, not a guest-handle allocator. |
| `VrpAdd/Get/RemoveOpenNamedPipeInfo`, `VrAdd/RemoveOpenNamedPipeInfo`, `VrTerminateNamedPipes` | current static named-pipe metadata | Recover as one record/PDB lifecycle group. | Original records retain `DosPdb`, while the current subset does not yet bind the caller's original process identity. This needs an existing CCPU/SAS PDB seam, but no new mapping manager. |
| `VrReadNamedPipe`, `VrWriteNamedPipe` | synchronous DEM pipe interception | Keep their declared S8 source-derived exceptions until the record/PDB group is present. | Original `OVERLAPPED_PIPE_IO` uses VDD cancellation registration and its worker-completion lifecycle; the current bounded session replacement must not be silently relabeled as original. A later Redirector/VDD package owns cancellation/interrupt completion. |
| `VrInitialize`, `VrUninitialize` and the rest of `vrinit.c` | 57:00/01 static provider state | Retain the explicit source-derived single-session gate; do not call it direct recovery. | Original body requires `VDDInstallUserHook`, NetBIOS/DLC initialization, VDM load-info guest write, queued ICA interrupts and the historical VDD suspend/resume hooks. These are a whole Redirector/VDD/ICA lifecycle, not an S8 helper seam. |

## Consequence

S8 may recover the first two helper bodies and then the named-pipe record/PDB
group only if its original CCPU/SAS identity seam is proven. It must not pull
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
