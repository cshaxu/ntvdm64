# Selected build graph: excluded C source cross-check

This is a current build-graph and object inspection, not a claim that a compiled object is linked or executed. The inventory is a provisional snapshot; the four shard ledgers remain authoritative. No file classification was changed by this check.

Inventory SHA-256: `50C992A5634DBEB46EF222D28B5F8686E738DD9A1FAD1F3DB87E788066E7894B`.

## ntvdm.exe

Build graph SHA-256: `7336D5FFDA67810253CD3362B2E7AE39DCA998958ED16719CE46C84CE544F151`. Binary SHA-256: `86835839AC83A4FF61493C049264FAD15B3255AAC74C11D3E0E2F75A8B547E4B`.
Link map SHA-256: `7CB58413E0907CF60F6D3F23C729DF743C375C60FEEDD1BC471BA5C1C6DE1232`.
Ninja commands name 351 distinct MVDM C sources; 22 have an excluded disposition in the snapshot. Object code sizes below are sums of nonempty `.text*` sections from the current COFF objects.

| Source | Object code bytes |
| --- | ---: |
| `dos/dem/demmsg.c` | 0 |
| `dpmi32/debug.c` | 0 |
| `softpc.new/base/ccpu386/c_getset.c` | 0 |
| `softpc.new/base/ccpu386/ccpupig.c` | 0 |
| `softpc.new/base/ccpu386/nop.c` | 3 |
| `softpc.new/base/comms/prt_scrn.c` | 0 |
| `softpc.new/base/debug/btrace.c` | 0 |
| `softpc.new/base/debug/dasm.c` | 0 |
| `softpc.new/base/debug/debuggng.c` | 0 |
| `softpc.new/base/debug/ega_dump.c` | 0 |
| `softpc.new/base/debug/pigyoda.c` | 0 |
| `softpc.new/base/debug/profile.c` | 9 |
| `softpc.new/base/debug/yoda.c` | 6 |
| `softpc.new/base/disks/gfi_ibm.c` | 0 |
| `softpc.new/base/disks/gfi_sflp.c` | 0 |
| `softpc.new/base/disks/gfi_test.c` | 0 |
| `softpc.new/base/support/get_env.c` | 122 |
| `softpc.new/base/video/ega_trcr.c` | 0 |
| `softpc.new/host/src/cpucstbs.c` | 0 |
| `softpc.new/host/src/nt_sas.c` | 0 |
| `softpc.new/host/src/nt_sec.c` | 155 |
| `softpc.new/host/src/x86_emm.c` | 0 |

## wow32.dll

Build graph SHA-256: `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278`. Binary SHA-256: `CBE582384F6BB97A748826B32C1A126A91D40990B2390473F1AA3E2C0E0CB096`.
Link map SHA-256: `4D626B08DD329999DAD6C4DD8CDA2DC142E588006375A2808D01383D554DCB51`.
Ninja commands name 77 distinct MVDM C sources; 7 have an excluded disposition in the snapshot. Object code sizes below are sums of nonempty `.text*` sections from the current COFF objects.

| Source | Object code bytes |
| --- | ---: |
| `wow32/wgman.c` | 0 |
| `wow32/wmmedia1.c` | 0 |
| `wow32/wmmedia2.c` | 0 |
| `wow32/wmmstru1.c` | 0 |
| `wow32/wmmstru2.c` | 0 |
| `wow32/wmmstruc.c` | 0 |
| `wow32/wthman.c` | 0 |

The five worker objects with nonzero code require the recorded selected-profile rationale, rather than an assumption that compilation produces an empty object: `nt_sec.c` is an alternate mapping provider, `get_env.c` an unselected provider, `profile.c` and `yoda.c` contain non-debug stubs, and `nop.c` has a separate helper while the selected decoder implements NOP inline. The existing shard evidence records the respective qualification; this check did not reproduce each behavior or determine whether archive members were extracted and then discarded.

Commands were taken from the target dependency closures, not every build target. They establish compilation inputs, not actual runtime reachability, all build configurations, header use, full functionality, or correspondence of these unsealed binaries to a separately sealed released package. Earlier alternate-owner and capability obligations remain in S11–S19.
