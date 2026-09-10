# M0 T228 S1 — Guest DOS multi-program integration and owner-domain repair

## Objective

Run one immutable, source-built NTIO/NTDOS/COMMAND/SHARE parent-child-return
sequence in Direct and Readonly, then repair a demonstrated failure only in
its complete original owner domain. The sequence follows original NTDOS
`$Exec`/PSP/`retf`, normal child `INT 21h/AH=4Ch`, NTDOS cleanup/`50:3C`, and
COMMAND `54:0B` parent return.

## Preconditions

T227 has closed local BOP code completion: the current 203-entry ownership
and local Direct/Readonly behavior were formally rebuilt and tested. T225
S44/S45/S55 retain the original/current lifecycle map and source-built
four-image identities.

## Implementation and test boundary

Use a fresh immutable staging root and bounded observation. A defect report
must first classify the whole owner domain (Guest DOS/COMMAND, bx-vdm host
provider, bx-mantle machine lifecycle, bx-core mechanics, or CLI profile
composition), then apply the source-recovery ladder and repair the complete
affected package. Direct and Readonly run the same input separately.

## Exclusions

No guest-byte mutation, synthetic EXEC/PSP/parent return, host child process,
trace-selected BOP leaf work, input-specific Bochs patch, Overlay
implementation, or Virtual mode is permitted. A trace is a verification and
classification input, never the architecture or task-backlog selector.