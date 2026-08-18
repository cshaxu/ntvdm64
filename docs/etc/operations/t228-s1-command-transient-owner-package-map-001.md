# T228 S1 — COMMAND transient allocation and reload owner-package map

## Scope

This map records the complete owner package selected by P6.  It is a source/ABI/failure map for repair admission, not an implementation claim and not a new BOP task.  The bounded native run has proved only that a resident COMMAND far pointer correctly names the dynamically selected transient segment while that segment contains zeros.

## Original source flow

| Stage | Original source | Contract | Failure / continuation |
| --- | --- | --- |
| Initial transient move | `rdata.asm` EndInit | Copy `RESGROUP:TranStart..TRANGROUP:TranSpaceEnd` to the top of the largest temporary DOS allocation; set `TrnSeg`; free that allocation; enter `LodCom_Trap`. | `nomem_err -> Alloc_error`; the subsequent resident allocation must preserve/reacquire the selected area. |
| Resident allocation / relocation | `command2.asm` `LodCom` through `LodCom1` | Query largest DOS block, allocate it, calculate `MemSiz`/`Res_Tpa`/`LTpa`, retain or move transient bytes when the selected segment changes, then set `TrnSeg`. | Inadequate allocation goes to `BadMemErr`; no host provider chooses a segment. |
| Integrity and reload | `command2.asm` `ChkSum`, `LoadCom`, `ReadCom`, `WrongCom` | Check transient checksum. On mismatch, open COMSPEC, seek `RESGROUP:TranStart`, read `TRANGROUP:TranSpaceEnd-100h` bytes to `TrnSeg:100h`, close, retry on expected failure. | Preserve original retry/prompt/fatal paths; a short read or open/seek/read error must not masquerade as success. |
| Return to transient | `command2.asm` `HavCom` and `TJmp` | Copy `TranVars` into `TrnSeg:HeadCall`, adjust PDB block length, `jmp Trans`. | Only after checksum/reload success. |
| Resident/transient bridges | `uinit.asm`, `stub.asm`, `tcode.asm` | Relocation table and `LodCom`/`TrnLodCom1` stubs preserve return entries; `TRANGROUP` is explicitly code that can be overwritten by user programs. | This is guest COMMAND behavior, not a BOP protocol. |

## Demonstrated correlation

P6 records `SS=0C41`, `BP=CAE0`, and the predecessor `FF 9E 4A 97`.  The original 16-bit far-call operand resolves to `0C41:622A`; source-built `COMMAND.COM` file offset `612Ah` stores `F9 5E 5F 5A`, i.e. `5A5F:5EF9`.  That target maps to static file offset `5DF9h`, inside `TRANCODE`.  Its nonzero bytes contrast with P5's same-moment zero successor window.

## Existing composition comparison

The current `bx-vdm` boot namespace already has a typed DEM file-service family:

- `50:12` open, `50:00` seek, `50:16` read, and `50:02` close enter `bx_ntvdm_boot_namespace_plane_v1` and `bx_ntvdm_dem_readonly_file_v1` through copied request/result transactions.
- The runtime loads the source-built COMMAND bytes as a startup resource (`bx_ntvdm_adapter_runtime.c`), but that resource placement is not itself the original transient relocation/reload lifecycle.
- The current namespace composition is a finite selected file set.  It must be examined for whether the original COMSPEC open/seek/read sequence reaches the same identity and whether Direct/Readonly select the correct host capability backend.  No single service is to be patched in isolation.

## Repair boundary and acceptance

The repair belongs in `bx-vdm` as one COMMAND transient composition provider plus its selected DEM file-service dependency.  `bx-core` remains selector-blind; `bx-mantle` remains ordinary-RAM/CPU lifecycle owner; the guest keeps DOS allocation and COMMAND control flow.

A repaired package must prove all of the following before native tracing:

1. source-built COMMAND identity is admitted as the COMSPEC reload source without an address-specific RAM seed;
2. the `50:12/00/16/02` file lifecycle preserves original open/seek/read/close and short-read/error semantics in Direct and Readonly;
3. the full transient byte range reaches the guest-selected `TrnSeg:100h` only through the original guest read contract;
4. existing zero/short read and unavailable paths remain typed failures; and
5. provider-level Direct/Readonly tests cover the whole lifecycle.  Overlay is interface-compatible but remains unimplemented.

Only then may one bounded native trace test whether the guest's original checksum and `TJmp` reach nonzero `TRANGROUP` code.