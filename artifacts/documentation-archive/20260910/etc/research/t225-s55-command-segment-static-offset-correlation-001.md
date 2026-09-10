# T225 S55 COMMAND segment static offset correlation

Date: 2026-08-17
Status: source/image attribution only; no execution or repair claim.

## Question

The paired native first-fault evidence reports CS:EIP 0C41:2357 while the
same invocation reaches several COMMAND-family BOPs from CS 0C41. Can the
controlled source-built COMMAND.COM artifact establish whether that segment
uses the ordinary COM PSP-plus-100h offset mapping?

## Fixed inputs

- Same controlled source-built COMMAND.COM identity: 50,384 bytes, SHA-256
  908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43.
- Same-run COMMAND BOP locations retained by T225 S35/S37:
  0C41:1B0E for 54:02, 0C41:1BFE for 54:0D, and 0C41:03A6 and 0C41:03CB
  for 54:0F.
- Paired first-fault record: 0C41:2357. S30 establishes that its copied EIP
  is not an instruction-boundary witness.

## Static correlation

The immutable artifact contains the exact BOP encodings at these file
offsets:

| Artifact file offset | Bytes | Runtime COMMAND IP |
| --- | --- | --- |
| 1A0E | C4 C4 54 02 | 1B0E |
| 1AFE | C4 C4 54 0D | 1BFE |
| 02A6 | C4 C4 54 0F | 03A6 |
| 02CB | C4 C4 54 0F | 03CB |

All four independent locations obey runtime IP = file offset + 0100h, the
ordinary COM layout selected by the original NTDOS COM branch. The source
contract for that branch is msproc.asm lines 981-1087: it reads the COM image
at Exec_DMA:0000 and transfers to PSP:0100.

Applying the same verified mapping to the first-fault EIP yields artifact
file offset 2257. The bytes there are:

    69 6E 74 65 72 70 72 65 74 65 72 20 70 65 72 6D

They are ASCII text, the continuation of Help6Msg in
src/opennt/base/mvdm/dos/v86/messages/comimsg.inc:16:

    Makes the new command interpreter permanent (can't exit).

## Disposition

This correlation proves neither that the CPU fetched those bytes as code nor
that COMMAND is corrupt. The existing first-fault event does not contain an
instruction window or pre-instruction history, so its EIP cannot be promoted
to a direct fetch attribution.

It does exclude treating 0C41:2357 as a source-mapped COMMAND instruction
solely from the current first-fault record. The remaining S55 question is
therefore the whole original COM control-flow/segment-continuity path after
the source-owned Exec transfer, not any BOP provider, real-mode wrap rule, or
adapter-created guest state. No implementation change is admitted by this
record.
