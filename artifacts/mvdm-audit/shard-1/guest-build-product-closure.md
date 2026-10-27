# Immutable guest source and build product closure

Frozen source universe SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `guest-build-product-closure.csv` covers all 624 shard-1 entries pending at start: per-file source role, 71 family groups, frozen/current SHA256, nearby original recipe paths and direct filename references, current four-host-target input checks, package-c same-hash and same-name matches, prior guest review and finite next action. All 624 current bytes match frozen records; no source-to-image claim follows from names or recipes.

Roles: 309 source/declarations, 182 retained intermediates, 99 resources/messages/support inputs, 25 historical guest build recipes or linker inputs, 9 image candidates. The 25 recipe/link inputs are excluded **as host product build-input files only**: current four checked Ninja target closures do not consume them and package-c does not contain identical bytes; the original guest features and binary lineage remain open. The 182 objects/resources/libraries/maps/symbols remain pending as guest intermediates until actual use in guest build/packaging is established, rather than assuming all unconsumed. No original guest file was modified or built.

Six entries have exact package hash matches: GRAPHICS.COM, COUNTRY.SYS, DOSX.EXE, VGA.DRV, WINSPOOL.EXE and historical share/SHARE.EXE, whose hash matches package FASTOPEN.EXE despite a different name. The last is byte identity only and is consistent with the explicitly staged original FASTOPEN stub in `tools/build/Stage-OriginalSoftpcRuntime.mjs`; it is not proof of SHARE service capability. COMMAND source-family witness and installed COMMAND image identity remain documented in `guest-command-dosx-review.csv` even though COMMAND.COM itself is not in this 624-row cohort.

The original recipe exact-name hits are textual links, not an attestation that the frozen artifacts were generated from those sources. Cases without such hits, shared include trees, guest graphics, runtime selectors and any mismatched package content need further source-to-image and behavior evidence. Historical archive manifests can establish provenance only when exact hashes are matched; the existing `guest-source-review.csv` supplement fields have been preserved, not invented. Package-c staged guest payloads and host tools are not assumed functionally equivalent.

## Largest bounded groups

- Win16 write: 69 entries.
- Win16 kernel31: 64 entries.
- Win16 mmsystem: 31 entries.
- DOS device keyboard: 30 entries.
- DOS kernel dos: 27 entries.
- Win16 user: 25 entries.
- Win16 ddeml: 22 entries.
- Win16 inc: 22 entries.
- DOS command command: 21 entries.
- DOS guest shared inc: 15 entries.
- Win16 toolhelp: 15 entries.
- Win16 win87em: 15 entries.
- Win16 commdlg: 14 entries.
- DOS kernel bios: 12 entries.
- Win16 drivers: 12 entries.
- Win16 ole: 12 entries.
- DOS command debug: 11 entries.
- DOS command graphics: 11 entries.

Remaining work is finite by family: identify recipe target and immutable binary identity; determine reached guest BOP/host owner and real execution evidence; compare other candidate bytes and source-policy; retain unknowns without new guest builds. No product, original guest, package or shared file changed.
