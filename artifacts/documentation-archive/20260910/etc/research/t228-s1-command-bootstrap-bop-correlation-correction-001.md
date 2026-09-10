# T228 S1 P32 — COMMAND Bootstrap BOP Correlation Correction

## Question

Do the retained startup records `54:0F` and `50:10` identify a current
host-service-provider failure that could explain the later zero transient
segment?

## Inputs

- `src/opennt/base/mvdm/inc/cmdsvc.h:16..34`
- `src/opennt/base/mvdm/inc/dossvc.h:92..166`
- `src/opennt/base/mvdm/dos/command/cmddisp.c:16..33`
- `src/opennt/base/mvdm/dos/dem/demdisp.c:92..161`
- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:557..572`
- Hash-locked source-built `COMMAND.COM` disassembly at runtime offsets
  `03A6h`, `03B9h`, `03CBh`, and `0401h..0428h`
- P22 retained BOP and software-interrupt observation
  `build/M0-T228-S1/p22-bop-and-software-int-observation-20260818c/readonly.stdout.log`
- P26 and P31 results.

## Original Identities And Call Form

`cmdsvc.h` assigns service `0Fh` to `cmdGetInitEnvironment`; `cmddisp.c`
places it in the original COMMAND dispatcher. `rdata.asm` issues it once with
the initially allocated guest environment buffer and, when the returned
paragraph requirement is larger, frees that buffer, allocates the requested
size, and issues it again. The source-built binary retains that exact shape:

```text
03A6  CMDSVC 54:0F
03B5  INT 21h AH=49h                 ; free first buffer
03B9  INT 21h AH=48h                 ; allocate retry buffer
03CB  CMDSVC 54:0F                   ; fill retry buffer
03E2  ... EndInit continues
0401  INT 21h AH=48h, BX=FFFFh       ; query largest block
0409  INT 21h AH=48h                 ; allocate it
0420  STD; REP MOVSB; CLD
0426  INT 21h AH=49h                 ; release it
```

P22's BOP records are copied at ingress, before a provider's result is
applied. Its first `54:0F` has entry `BX=0010h`; its second has entry
`BX=010Fh`. This is the original capacity-retry form: the latter value is the
guest's already allocated capacity, not a value returned by the second
provider invocation. The records cannot prove an incorrect second return.

`dossvc.h` assigns service `10h` to `SVC_DEMGSETMEDIAID`, and `demdisp.c`
maps it to `demGSetMediaID`. `SVC_DEMEXITVDM` is `3Dh`, not `10h`. The P22
`50:10` event is therefore a DEM media-ID request, not a VDM termination
request or a COMMAND allocation failure terminal.

## Result

The retained BOP sequence does not identify a current `54:0F` provider or
`50:10` lifecycle defect. In particular, it provides no source basis to
change BOP resume length, synthesize a guest allocation, alter the media-ID
provider, or attribute the later zero transient to the adapter.

The remaining question is unchanged and complete-owner scoped: whether the
original `EndInit` largest-block copy and subsequent `LodCom` allocation reuse
preserve the original transient payload under the current guest COMMAND/NTDOS
arena lifecycle. P31 already proves payload clearing is not normal original
`$ALLOC`/`$DEALLOC` behavior. Any further observation must remain default-off,
selector-blind, and mechanical; this correction selects no implementation.