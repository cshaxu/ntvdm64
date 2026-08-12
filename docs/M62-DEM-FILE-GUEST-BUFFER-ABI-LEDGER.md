# M62 DEM File and Guest-Buffer ABI Ledger

Status: source-derived `demOpen`/`demRead`/`demWrite` record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/dem/demfile.c`
- `src/opennt/base/mvdm/dos/dem/demhndl.c`
- `src/opennt/base/mvdm/dos/dem/demerror.c`
- `src/opennt/base/mvdm/dos/dem/dem.h`

## Common Historical Form

The three services use a 32-bit NT handle split across two 16-bit guest
registers: `AX` holds the high word and `BP` the low word. `GETHANDLE(AX,BP)`
reconstitutes it. This is a historical NT host representation, not a safe
external ABI: a future wrapper must use an opaque per-session token with the
same guest register layout, never disclose a host handle value.

`demClientError` is the shared normal-failure exit. It sets CF and AX to a
DOS-visible error code; error mapping and hard-error/INT 24 policy depend on
the global `pHardErrPacket`/device-chain state established elsewhere. It is
therefore not valid to treat a raw Win32 error code as the complete service
result contract.

## SVC `12h`: demOpen

| Aspect | Source-established behavior |
| --- | --- |
| inputs | `DS:SI` is a NUL-terminated full path; `BL` is access/share mode; `ES:DI` nominally names an EA buffer; `AL` says whether EAs are specified. |
| unsupported input | A nonzero `AL` emits the historical EA message and returns without the documented success/failure register update. This is an original limitation to preserve as evidence, not a modern API model. |
| path treatment | Forward slashes are copied and converted to backslashes. The code otherwise gives the resulting host path directly to `CreateFileOem`; it does not perform contained-root authorization. |
| access/share | `BL` selects read/write/read-write and sharing mode. Compatible sharing adds delete sharing. |
| named pipes | A pipe-looking name activates VDMREDIR, converts local pipe syntax, uses overlapped I/O, and records pipe metadata. This is an optional redirector feature, not part of the first local-file cut. |
| CD-ROM exception | A first read/write open of a CD-ROM file is retried read-only. This is legacy media policy. |
| success | `AX:BP` receives the raw 32-bit handle; non-pipe file size is returned in `BX:CX`; `DX=0` for regular files and `DX=1` for named pipes; CF clears. |
| failure | Calls `demClientError(INVALID_HANDLE_VALUE, firstPathCharacter)`; CF/AX are set by shared error policy. |

No SFT/JFT lookup occurs on this service path. The separately exposed VDD
helpers in the same source manage DOS SFT state for other consumers; they do
not make raw `AX:BP` handles safe to export across a modern runtime boundary.

## SVC `16h`: demRead

| Aspect | Source-established behavior |
| --- | --- |
| inputs | `AX:BP` handle, `CX` requested byte count, `DS:DX` guest destination, `BX:SI` 32-bit seek position, ZF indicating that no pre-read seek is needed. |
| seek | When ZF is clear, performs `SetFilePointer(FILE_BEGIN)` to `BX:SI` before I/O. |
| transfer | Calls `ReadFile` directly with the pointer produced from `GetVDMAddr(DS,DX)` and count `CX`. |
| normal success | CF clears and AX is the number of bytes read. |
| broken pipe | `ERROR_BROKEN_PIPE` maps to AX=0 and CF clear. |
| pipe mode | VDMREDIR named-pipe reads can populate the shared extended-error structure, including the `ERROR_MORE_DATA` case. This is excluded from the first local-file tier. |
| memory lifecycle | The original path calls `Sim32FlushVDMPointer` then `Sim32FreeVDMPointer` after success or read failure. Thus the historical interface already distinguishes a writable guest mapping whose modifications require publication. |
| other failure | Calls `demClientError(handle, -1)`. |

## SVC `1Eh`: demWrite

| Aspect | Source-established behavior |
| --- | --- |
| inputs | `AX:BP` handle, `CX` byte count, `DS:DX` guest source, `BX:SI` seek position, ZF indicating that no pre-write seek is needed. |
| seek | Same `SetFilePointer(FILE_BEGIN)` behavior as read when ZF is clear. |
| zero length | `CX=0` invokes `SetEndOfFile`: DOS truncates or extends at current file position; CF clears on success. |
| transfer | Calls `WriteFile` directly from the pointer produced by `GetVDMAddr(DS,DX)`. |
| normal success | CF clears and AX is the number of bytes written. |
| disk full | `ERROR_DISK_FULL` is normalized to AX=0 and CF clear. |
| pipe mode | Named pipes route through VDMREDIR and are deferred with the redirector tier. |
| other failure | Calls `demClientError(handle, -1)`. |

## Required Neutral Boundaries

The source evidence requires three distinct future contracts. None may be
implemented locally as a replacement SVC dispatcher:

1. **Contained file object capability:** open and opaque-token lifecycle,
   access/share/seek/truncate behavior, and an explicit local-file tier;
   pipes/VDMREDIR remain separately opt-in.
2. **Directional guest buffer access:** read obtains a validated writable
   destination and publishes exactly the reported byte count; write obtains a
   validated readable source snapshot. The historical raw-pointer path is not
   an acceptable software-backend substitute.
3. **DOS-result adapter policy:** conversion of capability failures into the
   original CF/AX and extended/hard-error state requires the rest of DEM's
   state contract. It cannot be reduced to a platform file error alone.

This reinforces M58: checked query/read/write primitives alone are insufficient
for a file service whose read direction needs controlled write publication and
whose open/read/write sequence carries per-session opaque state. The next
source slice should inspect `demQueryCurrentDir`, `demSetCurrentDir`, and
`demGetDrives` to establish the contained drive/current-directory state rather
than guessing it from host paths.
