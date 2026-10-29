# S10 device follow-up: system, keyboard/mouse and disks

This review owns only `softpc.new/base/system`, `base/keymouse`, and
`base/disks` C/H rows in the current `host-ch-scope/inventory.csv`. It does
not change the shared inventory, product, or guest. The current snapshot has
28 host candidates in these directories: 3 pending, 22 partial, 1 full and 2
excluded. All three pending paths are in `base/disks`; this is not proof that
the already partial or full rows are behaviorally complete.

Both external input audits list all three files; the DOS catalog calls them
`not_reviewed`, and the WOW32 source catalog calls them `located-unreviewed`.
Those labels establish inventory membership, not behavioral acceptance. The
DOS catalog SHA-256 is
`7DAC7F535A737988BE4B3D859C82D58C89EE54E4C3207B529E0875FE29A76938`;
the WOW32 catalog is
`A3A4F44ACB7430FEB7CDE997E367E6EFCE8CBE26D5A67932128219BD435E9BE0`.
The reconciled inventory SHA-256 is
`7C47B46ACFEA13D211C1B4467376A7DC344EF3E78B3C935D1FE05B7DD28DBDB5`.

| Exact original file | Source identity and reached role | Recommended classification and remaining obligation |
| --- | --- | --- |
| `base/disks/gfi_ibm.c` | Current, OpenNT and OpenNT-4.5 identities all `234D4FDAC97C117A01EE642242DBAB691E6809D0A3D25E63DF4604D5FBBD955F`. Under `SLAVEPC` it serializes remote floppy-controller calls through `host_rpc_action`/`host_rpc_reset`. | **Excluded physical implementation in the selected NT/CCPU40 production profile**; no remote-controller execution is claimed. Do not interpret this narrower recommendation as exclusion from every historical SoftPC profile. S17 must explicitly decide any retained non-NT external-server capability. |
| `base/disks/gfi_sflp.c` | Current and both original identities all `396C621A6C6ABF135FDEC730A0BE91C8BC5582DE51687ED006CA5DF97D354DC5`. `SLAVEPC` activation registers callbacks and routes FDC/DMA/IRQ behavior to the same remote IBM-PC server. | **Excluded physical implementation in the selected NT/CCPU40 production profile**, paired with `gfi_ibm.c`; it is not replaced semantically by the local floppy provider. S17 retains the distinct transport/profile decision. |
| `base/disks/gfi_test.c` | Current and both original identities all `CFBA520508D5E274AC23D471AF6656B7BC41AF1A8B0606B3B7020570AB2E2210`. Under `!PROD` its pseudo-provider logs GFI calls. `gfi.c` invokes it conditionally for verbose tracing and ignores its command return before real function-table dispatch. | **Excluded physical implementation in the selected PROD profile**; this does not exclude real floppy service. S17 retains the non-PROD diagnostic-profile decision; a tracing success value is not a disk implementation. |

All three original paths are byte-identical, so none is a current mirror
diff; the mandatory mirror-diff partial rule does not apply to these files.
Their selected-profile textual retention is 100%, but **executable restored
behavior for these gated bodies is 0% in that profile**, not a claim about
runtime functional quality or alternate build configurations.

The captured worker graph SHA-256
`835D8D25248FFBE2C4B4B147C056A43EF7C2F3624A85E09F141258F45FA0410C`
contains `/DPROD`, `/DSPC386`, no `/DSLAVEPC`, and compiles all three into
`original-softpc-disks.lib`. The currently cached objects have no executable
section or global implementation symbol (`objdump -h` and `-t`); their hashes
are `62801BB5D352576758C85C5B7B1C68D48DDD14ED5934A65CB4B031690C21E642`,
`F86874B51F2BC711484AA6BCDED581E1F59156F4AF3B22403380DFB5ECF9547B`,
and `3FD49DF37C61938F2A8C3FF33CC0E591E98A9FD4F25DEC1C157828F3B4FB58F9`,
respectively. Source-file archive membership therefore must not be counted as
active behavior. The currently cached `ntvdm.exe.map` hash is
`BA35E2163CA561232E7467094E50FB6A7581055013A47F7EC50AD80F974F3B91`; it maps
`host_gfi_rdiskette_active` to `nt_rflop.obj`, not to the remote files. This
map is a build observation, not proof of identity to a released executable.

Original OpenNT `base/disks/sources` also lists these inputs, while its
checked build recipes did not contain an `SLAVEPC` definition. Original and
current `gfi.c` gate remote configuration and calls with `SLAVEPC` and trace
calls with `!PROD`. The selected NT host `host_gfi_rdiskette_active` initializes
the real host provider, or terminates it and switches to the empty provider.
The latter is a different resource owner and is not an implementation of the
remote protocol. See [prior provider review](host-ch-scope/gfi-alternative-provider-review.md)
and [original NT profile review](shard-3/slavepc-floppy-nt-applicability-review.md).

Retain the functional work separately: S13 must verify the selected GFI/FDC,
DMA/IRQ and local media lifecycle, including real or expressly owned test
media, errors, deactivation and restart; actual-object failure injection and
no-media reset do not prove read/write/format. S17 owns any decision on
remote serial-controller and non-PROD trace profiles. The fixed-disk and
floppy counter-release failure seam in [functional handoff](host-inc/gfi-functional-handoff.md)
is a controlled observation, not a proven live guest defect. No production
or device test was run during this read-only follow-up.
