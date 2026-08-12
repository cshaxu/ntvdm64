# T95 S7 BOP 54:01 Source Response Contract

## Source Route

`CMDSVC SVC_CMDGETNEXTCMD` encodes `C4 C4 54 01`. COMMAND calls it with
`DS:DX` naming the packed 36-byte `CMDINFO` record. `cmdGetNextCmd` is the
OpenNT owner (`cmdmisc.c:25-364`); COMMAND tests Carry immediately after the
four-byte BOP (`tcode.asm:557-567`).

The contained CLI replacement does not reproduce `GetNextVDMCommand`, PIF,
console, current-directory, handle, or process policy. For the first command
only, it publishes the already selected immutable `TARGET.COM`/`TARGET.EXE`
into the original guest record shape, then lets COMMAND use its ordinary DOS
open/EXEC route.

## Checked First-Delivery Plan

The adapter first gathers exactly `CMDINFO`'s 36 bytes at `DS:DX`. The copied
record is accepted only for the source-shaped first COMMAND layout:

| Target | Source field / binding | Required bounded output |
| --- | --- | --- |
| Raw command buffer | `CmdLineSeg:CmdLineOff`, `CmdLineSize == 128` | At offset 1: count byte, `TARGET` basename, optional ASCII tail prefixed by one space, CR/LF, and NUL. Byte 0 remains COMMAND's initialized maximum. |
| Executable path | `ExecPathSeg:ExecPathOff`, `ExecPathSize == 273` | ASCII `C:\\TARGET.COM`/`C:\\TARGET.EXE`, including NUL; record size is path bytes excluding NUL. |
| CMDINFO response words | record offsets 04, 06, 16, 22, 26, 32, 34 | Current contained drive, frozen drive count, zero standard-handle mask, zero redirector pointer, zero batch state, executable length, and original COM/EXE extension classification. |
| Prior command registration | `SVC_CMDSETINFO` bindings | `SCS_ToSync = FFh` and DOS-binary byte = `01h`. The already registered FD-access word is not changed. |

The transaction stages all writes before queueing one generic multi-write
result, resumes at `fault_rip + 4`, and clears Carry. A missing registration,
wrong real-mode BOP/window, malformed layout, non-ASCII declaration, absent
or already delivered command, bad span arithmetic, or transaction-preflight
failure creates no transaction and does not consume the command.

No environment write occurs here: the selected first profile has already
completed `SVC_GETINITENVIRONMENT`, matching the original first-call branch
in `cmdmisc.c:71-75`. The historical environment-capacity retry remains a
separate response state and is not fabricated for this profile.

## Exact UCOMBUF Bound

OpenNT declares `COMBUFLEN = 128`, allocates `UCOMBUF` as `COMBUFLEN + 3`,
and requires the delivered count to be at most 127 (`tcode.asm:525-558`,
`spc.asm:255`, `cmdmisc.c:315-334`). The first contained target basename is
six bytes. A nonempty tail therefore requires `6 + 1 + tail + 2 <= 127`, so
the actual profile maximum is **118 ASCII tail bytes**. The earlier generic
126-byte CLI declaration limit is insufficient for this concrete COMMAND
consumer and must be narrowed before a 54:01 response is enabled.

## Boundary

This contract uses only copied `CMDINFO`, copied launch metadata, opaque
addresses from the existing `CMDSETINFO` registration, existing generic
gather/multi-write mechanics, and a fixed CPU result. It adds no Bochs BOP
semantics, device, direct guest pointer, host path, host handle, or OpenNT
host component.
