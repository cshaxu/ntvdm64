# M0 T310 S22 P2 — host-storage binding and verification

## Question

Can the selected original host-storage bodies compose on Win32/x86 CCPU40
without replacing their controller semantics, opening a real device during
verification, or inventing the historical remote-floppy service?

## Inputs and procedure

- Re-read original and mirror `nt_fdisk.c` and `nt_rflop.c` diffs.
- Re-read `adapter-mvdm-host-out/win32/include/ntioapi.h` and
  `source/ntioapi_facade.c` declaration/implementation pairs.
- Generate `build/M0-T310/S22/formal-host-storage-x86` with
  `New-T310OriginalSoftpcNinja.ps1 -Architecture x86` and run Ninja for
  `original-softpc-host-roots.lib`, `original-softpc-disks.lib`,
  `softpc-win32-bindings.lib`, and `original-softpc-forced-closure.dll`.
- Compile and run `ntioapi-storage-unavailable-fixture.c` in that disposable
  root. It names only `\\DosDevices\\mvdm-no-such-storage-endpoint`.
- Inspect `gfi_ibm.c` and `gfi_sflp.c` preprocessor ownership of every
  `host_rpc_*` call.

## Observations

- `nt_fdisk.c` retains the original `NtOpenFile -> NtDeviceIoControlFile ->
  NtQueryInformationFile -> NtFsControlFile` geometry/BPB order, then the
  original per-PDB lock/retry/read/write/verify/close flow. The only retained
  changes are registered `MVDM-HOST-DIV-006` (fresh two-byte guest lease for
  the PDB scalar) and `MVDM-HOST-DIV-069` (native private aligned allocation).
- `nt_rflop.c` retains its original device, FDC-worker and media operation
  flow; its only retained change is the same DIV-006 PDB scalar lease.
- The existing `ADAPTER-WIN32-016` facade preserves original call spellings
  and 32-bit `OPENNT_IO_STATUS_BLOCK` layout. It resolves public `ntdll`
  exports and copies only native transient completion state back into the
  original host-local record. No handle or native pointer enters guest state.
- The fresh formal graph completed all 369 selected compilation/archive edges.
  Its forced-link audit retained later-owner unresolved symbols under
  `/force:unresolved`, but contains no `host_fdisk`, `nt_fdisk`, `nt_floppy`
  or `host_rpc` unresolved row.
- The focused unavailable fixture compiled and exited zero: a deliberately
  impossible object returned a non-success `NtOpenFile` result. It did not
  open a volume, create a file, or mutate a host resource.
- Every `host_rpc_*` call is enclosed by `#ifdef SLAVEPC`; selected CCPU40
  does not define `SLAVEPC`. No selected definition exists, so this historical
  remote-IBM-PC protocol is condition-unselected and remains unavailable.

## Disposition

| Endpoint | Final S22 disposition |
| --- | --- |
| `host_fdisk_*` | Direct original no-fixed-disk callback contract: configuration succeeds; reads/writes return zero. |
| `nt_fdisk_*` | Original body, binding-only through existing ADAPTER-WIN32-016 and the registered current-PDB synchronous lease. |
| `nt_floppy_*` | Original body, binding-only through existing ADAPTER-WIN32-016 and the registered current-PDB synchronous lease. |
| `host_rpc_*` | Original `SLAVEPC`-only branch is condition-unselected / exact unavailable; no protocol replacement. |

## Interpretation and follow-up

S22 is source and local-binding closed. It does not prove a guest disk I/O
transaction: selecting and exercising real fixed/removable media remains
integrated S49 work with a separately admitted safe resource policy. No
Bochs, MONITOR, kernel VDM, synthetic-media or new remote-service route was
selected.
