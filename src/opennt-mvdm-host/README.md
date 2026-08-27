# opennt-mvdm-host

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

The root currently carries all 1,689 selected paths across 23 original package
roots: 1,050 topology/header/resource inputs and 639 S6-recovered inputs
(612 C/C++/assembly provider bodies plus 27 remaining forms). This physical
layout is not a final runtime-owner claim.

## Divergence register

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-001 | Store a native x86 `PVOID` find-list reference in the DOS DTA/FCB reserved field. | The original field is 32 bits by process-pointer coincidence; on x64 `PVOID` widens and corrupts the original DOS layout. | Retain the original 32-bit field with `ULONG`; its value is an opaque host-resource surrogate, not a pointer. | `dos/dem/dosdef.h` |
| MVDM-HOST-DIV-002 | Persist, compare and release a `PFFINDLIST` pointer across DOS FindFirst/FindNext calls. | Native pointer persistence is width-unsafe and forbidden across the MVDM/guest boundary. | The original source flow publishes, resolves and releases an opaque identity through the S15 adapter-softpc facade; `FFindId` stays original numeric data and source resource-failure branches are retained. | `dos/dem/demsrch.c` |
| MVDM-HOST-DIV-003 | Use 32-bit host pointer casts for DEM directory-handle sentinels, pathname/device offsets and directory-buffer traversal. | Those values are host-local native-width pointers/HANDLEs on x64; their original arithmetic truncates them even though no value belongs in guest state. | Retain original algorithms, data flow and failure branches while using native-width HANDLE sentinel and byte-pointer/`uintptr_t` arithmetic. | `dos/dem/demsrch.c` |
| MVDM-HOST-DIV-004 | Retain and write the DOS extended-error location installed by `demSetDTALocation`. | A native `GetVDMAddr` pointer cannot cross a callback or an x64 ABI boundary. | Preserve DS:CX as an `adapter-softpc` numeric guest descriptor; `demRead` takes a fresh nine-byte bounded lease, writes the original fields in order, and commits/releases before it continues. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demhndl.c` |
