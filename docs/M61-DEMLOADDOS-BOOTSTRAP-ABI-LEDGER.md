# M61 DEMLOADDOS Bootstrap ABI Ledger

Status: source-derived bootstrap service record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm`
- `src/opennt/base/mvdm/dos/dem/demmisc.c`
- `src/opennt/base/mvdm/dos/dem/dem.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c`
- `src/opennt/base/mvdm/inc/softpc.h`

## Guest Call Contract

The original `msinit.asm` computes `dos_load_seg`, places it in `DI`, and
emits `SVC SVC_DEMLOADDOS` (`11h`). Its comment is explicit: `DI` is the
segment at which DOS is loaded and a failure does not return.

The corresponding `demLoadDos` implementation establishes the following
historical ABI facts:

| Item | Source-established behavior |
| --- | --- |
| input register | `DI` is the NTDOS load segment. |
| guest target | `GetVDMAddr(DI, 0)` is the first destination byte. |
| source identity | The old host derives a mutable `pszDefaultDOSDirectory`, appends `\\ntdos.sys`, and opens it through `CreateFileOem`. |
| transfer | Repeated synchronous `ReadFile` calls request 16,384 bytes directly into incrementing guest destination addresses until a zero-byte read. |
| result registers/flags | No success register or CF change is made by `demLoadDos` itself. Successful return continues the NTIO instruction sequence. |
| failure | Open or read failure calls `TerminateVDM`; the routine does not arrange a guest-visible DOS error return. |
| debug side effect | Debug builds retain a sibling `ntio.sys` path in `pszBIOSDirectory`; non-debug success frees the mutable default DOS directory. |

The caller is therefore a loader protocol, not a normal file SVC. In
particular, its image length comes from EOF, not a guest-provided count, and
the destination can cross arbitrary historical memory regions unless the
monitor's address model prevents it.

## Earlier NTIO Placement Contract

`InitialiseDosEmulation` in `nt_msscs.c` shows the distinct pre-entry phase:

1. it initializes host I/O and reset state;
2. it obtains the old system directory and opens `ntio.sys`;
3. it obtains the file size, reads the entire image at
   `NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET` via `get_byte_addr`;
4. it restores the fixed virtual-interrupt state overwritten by that file
   load; and
5. it sets CS and IP to that NTIO load address.

This differs from `demLoadDos`: NTIO was originally host-preloaded before
guest execution, whereas NTDOS is loaded by an NTIO-originated service call.
The two stages must remain separately modeled in any future profile.

## Modern Boundary Consequences

On original x86, `GetVDMAddr(seg, off)` is a raw `seg << 4 + off` address and
the legacy V86 memory model makes `ReadFile(..., pbLoadAddr, ...)` a direct
write. That behavior cannot be imported literally into an x64 software
backend. A future profile adapter needs all of the following, without giving
the historical service arbitrary host memory access:

1. a BYOB-selected immutable NTDOS file handle obtained before guest entry;
2. a validated ordinary-RAM destination range for the entire NTDOS image;
3. all-or-none publication of the image bytes before allowing guest execution
   to continue; and
4. a backend-owned stop/fault result when validation or I/O fails.

The first two requirements correspond directly to the two still-gated neutral
backend capabilities: checked guest memory and a paused ordinary-RAM
multi-span transaction. The historical direct `ReadFile` loop is evidence for
the required semantics, not a permissible replacement implementation.

The old `GetSystemDirectory` lookup, mutable global path, and `TerminateVDM`
exit policy are historical host-product choices. They do not belong in core or
in a generic backend. A contained wrapper must instead provide an
already-authorized BYOB root/file object and translate a failure to its own
bounded diagnostic/termination policy.

## Next Evidence Slice

The next source pass should cover `demOpen`, `demRead`, and `demWrite` as one
file-and-guest-buffer family. It must document every register, buffer span,
error/CF convention, and handle-table dependency before proposing a namespace
or stream capability. It must not implement those services locally.
