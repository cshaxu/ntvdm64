# mvdm-host

Temporary exact source-union carrier for the selected OpenNT MVDM mirror. M0
T274 S5 recovered its original package topology, headers and static resources
byte-identically; S6 then recovered every selected original provider body and
remaining input without changing its contents. This root remains the canonical
audit carrier only until the approved file-identity re-root moves support
(`dirs`, `makefil0`, `inc`, `oemuni`, `suballoc`), standalone tool
(`vdmutils`), and selected `softpc.new` firmware/ROM/data paths to their
dedicated mirror components. It has no locally
authored provider replacement.

## Provenance

- [S1 canonical union ledger](../../docs/etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger.tsv)
  selects one source for every host path.
- [S5 topology manifest](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-manifest.tsv)
  records all 1,689 host rows, including imported/deferred disposition, source
  path and SHA-256.
- [S5 closure evidence](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md)
  records the topology/body boundary.
- [S6 host-input manifest](../../docs/etc/operations/m0-t274-s6-mvdm-host-input-manifest.tsv)
  records the 639 S5-deferred selections and their copied-file hashes.
- [S6 closure evidence](../../docs/etc/operations/m0-t274-s6-mvdm-host-package-supply-closure-001.md)
  records full selected-host coverage and verification.

The root carries the current host-owned selection described by the topology
manifests. Firmware-owned `softpc.new/base/keymouse` is intentionally rooted
in `mvdm-softpc-firmware` instead. This physical layout is not a final
runtime-owner claim.

## Divergence register

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-001 | Store a native x86 `PVOID` find-list reference in the DOS DTA/FCB reserved field. | The original field is 32 bits by process-pointer coincidence; on x64 `PVOID` widens and corrupts the original DOS layout. | Retain the original 32-bit field with `ULONG`; its value is an opaque host-resource surrogate, not a pointer. | `dos/dem/dosdef.h` |
| MVDM-HOST-DIV-002 | Persist, compare and release a `PFFINDLIST` pointer across DOS FindFirst/FindNext calls. | Native pointer persistence is width-unsafe and forbidden across the MVDM/guest boundary. | The original source flow publishes, resolves and releases an opaque identity through the S15 adapter-mvdm-host-out/softpc facade; `FFindId` stays original numeric data and source resource-failure branches are retained. | `dos/dem/demsrch.c` |
| MVDM-HOST-DIV-003 | Use 32-bit host pointer casts for DEM directory-handle sentinels, pathname/device offsets and directory-buffer traversal. | Those values are host-local native-width pointers/HANDLEs on x64; their original arithmetic truncates them even though no value belongs in guest state. | Retain original algorithms, data flow and failure branches while using native-width HANDLE sentinel and byte-pointer/`uintptr_t` arithmetic. | `dos/dem/demsrch.c` |
| MVDM-HOST-DIV-004 | Retain and write the DOS extended-error location installed by `demSetDTALocation`. | A native `GetVDMAddr` pointer cannot cross a callback or an x64 ABI boundary. | Preserve DS:CX as an `adapter-mvdm-host-out/softpc` numeric guest descriptor; `demRead` takes a fresh nine-byte bounded lease, writes the original fields in order, and commits/releases before it continues. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demhndl.c` |
| MVDM-HOST-DIV-005 | Retain the NTDOS DTA far-address cell and use its current target for DEM search/FCB I/O. | The original `pulDTALocation` is a persistent native pointer, and DTA bytes must not outlive a guest-memory lease. | Preserve DS:AX and its four-byte far-address cell numerically; use fresh bounded DTA leases for FindFirst, FindNext and FCB I/O, committing only guest-write paths. Lease failure follows existing DEM failure flow with `ERROR_INVALID_ADDRESS`. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demsrch.c`, `dos/dem/demfcb.c` |
| MVDM-HOST-DIV-006 | Retain the current DOS PDB scalar for DEM file/find and SoftPC disk/floppy ownership. | The original `pusCurrentPDB` is a persistent native pointer; its two-byte guest value must not survive a callback as host memory. | Preserve DS:DX as a numeric descriptor and take a fresh two-byte read lease at each reached consumer. Existing false/zero/negative error directions are retained when the scalar cannot be read. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demsrch.c`, `dos/dem/demfile.c`, `softpc.new/host/src/nt_fdisk.c`, `softpc.new/host/src/nt_rflop.c` |
| MVDM-HOST-DIV-007 | Retain the DEM SFT chain and implement VDD SFT/JFT handle operations. | The original persists `pSFTHead` and returns direct guest aliases that WOW writes after the call; its 32-bit `SFT_NTHandle` also truncates x64 handles. | Keep the SFT head as numeric DS:offset.  Active VDD wrappers use the adapter-mvdm-host-out/softpc session-owned shadow/commit contract; reached WOW caller writes have explicit final commit/discard points. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demfile.c`, `wow32/wkfileio.c` |
| MVDM-HOST-DIV-008 | Resolve dynamically loaded WOW and installable-BOP entrypoints through the historical `FARPROC`/`MYFARPROC` variables. | The historical x86 compiler accepted incompatible implicit function-pointer assignments; modern MSVC rejects them even though `GetProcAddress` supplies the same raw export address. | Three explicit casts retain the original selected pointer representation and call order. They do not enable WOW or installable BOP providers; their distinct calling contracts remain owned by those later packages. | `softpc.new/host/src/nt_bop.c` |
| MVDM-HOST-DIV-009 | Write `cmdGetCurrentDir`'s `DS:SI` result directly through an unbounded `GetVDMAddr` pointer. | A guest pointer cannot be retained as host memory on x64 or across the modern component boundary. | Retain the original drive/environment order and CF/AX outcomes; stage the at-most-67-byte OEM result in a local buffer, then use the session-owned adapter-mvdm-host-out/softpc numeric descriptor and exact synchronous guest write. | `dos/command/cmdmisc.c` |
| MVDM-HOST-DIV-010 | Persist `pHimemA20State` as a native `GetVDMAddr` pointer for later A20 state-byte writes. | The pointer is not width-safe and cannot survive beyond a scoped guest-memory lease. | Preserve the AX:BX numeric location and A20 write ordering; the matching private `mvdm-host-overlay` records only that location and uses a fresh one-byte session lease for each write. | `xms.486/xmsa20.c`, `xms.486/xmsumb.c`, `xms.486/xms.h`; `../mvdm-host-overlay/xms.486/xms_a20_state.[ch]` |
| MVDM-HOST-DIV-011 | Read the XMS move structure through a retained `GetVDMAddr` alias, then copy its numeric linear operands through host pointers. | The structure and both operands are guest memory; native aliases are forbidden across the x86/x64 component boundary. | A matching private overlay reads the original twelve bytes under a bounded session lease and invokes the same-shaped numeric `xmsMoveMemory` seam.  Invalid lease/length conditions return the existing failure result in AX. | `xms.486/xmsblock.c`; `../mvdm-host-overlay/xms.486/xms_block_move.[ch]` |
