# M0 T225 S54 P1 — Existing BOP sequence load-address observation

The existing default-off BOP sequence was run once in Direct and Readonly on
the immutable four-image input. Both records agree that the first accepted
`50:11` has `EDI=08B0`, which is the typed NTDOS copy destination used by
`bx_ntvdm_dem_load_dos_service_v1_prepare`.

The same records later reach `50:36` at `0041:5A70`; sequence capacity
exhausts after 128 records. This observation proves only the initial copy
address and profile neutrality. Per the original sysinit/DosInit relocation
rules recorded in `t225-guest-image-layout-lifecycle-correction-001.md`, it
cannot map a later runtime CS:IP directly to a final NTDOS file offset.
No BOP/provider/machine behavior changed.
