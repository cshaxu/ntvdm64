# T189 S6 COMMAND bootstrap boundary map 001

Date: 2026-08-12  
Packet: M0 T189 S6  
Disposition: source-only map; no source change or new trace run.

## Question

What is the first reached, unresolved BOP boundary after the corrected
`54:0C` boot-file write, and is it a machine capability or a host-service
composition contract?

## Inputs and procedure

The only runtime input is the valid frozen T189 S5 trace. Its first
post-commit COMMAND boundary is `C4 C4 54 02`; the adapter observes it then
passes through to the real-mode #UD vector. The source map compares
`base/mvdm/softpc.new/host/src/nt_bop.c` (`MS_bop_4`),
`base/mvdm/dos/command/cmddisp.c` (table index 2),
`base/mvdm/inc/cmdsvc.h` (`SVC_CMDCOMSPEC = 2`), and
`base/mvdm/dos/command/cmdmisc.c` (`cmdComSpec`) with the existing adapter
COMSPEC seam. No guest input was reread and no diagnostic was added.

## Observations

`cmdComSpec` is a COMMAND host-service operation, not BIOS, CPU, PIC, DOS
kernel, or Bochs device behavior. On its first call it reads the NUL-ended
string at guest `DS:DX`, writes `COMSPEC=` plus that string to its host-side
buffer, records its byte count, and sets AL from the original console/WOW
condition. It does not require a fixed path to `COMMAND.COM`.

The current adapter seam is correctly placed in the host-service plane and
uses a checked gather/read/resume transport. Its rejection is narrower than
the historical contract: `prepare_comspec` constructs a profile path and its
completion requires gathered guest bytes to equal that expected path. The
valid T189 S5 trace has a provider present, but `54:02` remains pass-through,
then reaches the unchanged real-mode vector-06 path. This is consistent with
that equality restriction; it is not proof of a Bochs machine deficit.

## Interpretation and transfer

T189 closes with a complete answer to its diagnostic question: the `54:0C`
decline was stale derivative composition, and the next reached boundary is a
separate COMMAND host-service contract mismatch. No source change is licensed
by this map.

The next package must recover `cmdComSpec` holistically with its paired
`54:0F` environment consumer: a bounded `DS:DX` gather, explicit ASCII/NUL
and size handling, source-derived `COMSPEC=` buffer/result semantics, and the
existing checked resume/transaction path. It must preserve the adapter as
host-service owner, never expose host environment, hardcode a guest path, or
move command/DOS semantics into Bochs.
