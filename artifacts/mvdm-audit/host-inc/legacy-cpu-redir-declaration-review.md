# Legacy CPU and redirector constants: host profile limits

These paths remain pending in the live shard decisions at review time. The evidence below supports at most profile-scoped file-role recommendations; it does not exclude corresponding DOS, CPU, or redirector functionality. No shared ledger or product/guest source was modified.

## `inc/enumapis.h`: excluded candidate for the inspected host build only

Current/OpenNT SHA-256 `864C27F32FC218FDBC29CD137111253686AF65F6DBF77DE365FEF95EEE8F33DB`. The header declares LAN Manager redirector local API subfunction constants. No literal include was found in the inspected current MVDM sources, and `shard-1/carrier-review.csv` records no exact selected dependency in its two captured databases. Those negatives do not rule out macros, generated, external or unbuilt profiles. Original DOS redirector `resident.asm`, `redir.asm`, `netapis.asm` and others instead include their assembler `enumapis.inc`.

`redir-enum-compare-observer.py` independently compared the two current textual lists: 51 C and 51 assembler hexadecimal definitions, all 51 names and values matching; hashes and counts are in `redir-enum-compare-evidence.json`. This is source data comparison, not proof an installed immutable guest was built from the `.inc`, and not proof all operations work. Recommend excluding only this redundant C header role in the inspected selected host profile, subject to wider profile review; preserve guest redirector operation and ABI as separate obligations.

## `inc/v86def.h`: excluded candidate for the selected CPU40 branch only

Current/OpenNT SHA-256 `5BC822B487F5F1A236E6BE8E107AF5832FD680E026CF7231E0AE61E13595937E`. `inc/softpc.h` includes this `X86CONTEXT` carrier only under `#ifdef i386`. `tools/build/New-T310OriginalSoftpcNinja.ps1` explicitly selects CPU40/CCPU and does not define `i386` product-wide; the recorded release dependency census has no exact selected edge. This supports only a scope-limited exclusion for the inspected ordinary CCPU40 profile, not any claim that context, CPU execution or alternate historical i386 profiles are unnecessary. Target-local compilation exceptions or future profiles must still be checked.

## `inc/x86def.h`: keep pending

Current/OpenNT SHA-256 `F5D8AB04433599401682B0E4F7AE45104B8C5195168CB12201D8278F1549F38D`. It defines a distinct historical x86 emulator `X86CONTEXT` layout. No literal include or exact valid selected dependency was found in the observed census. Absence alone does not prove no conditional, generated or different-profile consumer, or that replacing it by `v86def.h` would be ABI-correct. Keep pending until original consumers and selected alternatives are conclusively reconciled.

## `inc/monregs.h`: keep pending

The selected `nt_thred.c` contains an include, while no direct register accessor use was found in that file. `softpc.new/host/inc/host_cpu.h` includes a same-named header only within CPU30/MONITOR conditionals. There are same-name headers and include-search differences, so the observed single dependency must not be generalized to all paths or used to assert behavioral restoration. See `xms-redir-export-role-review.md`; exact resolved producer and alternate-profile use remain open.
