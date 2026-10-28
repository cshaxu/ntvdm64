# Part 8: Win16 USER generated-thunk build chain

This reviews the ten owned `wow16/user` generated artifacts/scripts and `listing.inc` in the part-8 inventory. The five ASM and resource files under `k/daytona` are current-only against the frozen original mirror catalogs; their resemblance to the historical generation steps is not independent provenance proof. Do not execute any historical script: they invoke nmake and delete files, and guest recompilation is prohibited.

Original `user/makefile:43-65,88-91,224-251` selects `O=k/ALT_PROJECT`; only x86/non-NONX86 selects PMODE32 and wowk.obj/wowkr.obj. It gets generated `wowasm.asm` from NTUSER client and `wowrtl.asm` from NTUSER RTL client, copies them, then uses wowasmk.cmd/sed to generate USER guest assembly and assembles the corresponding object. `wowasm.sed` explicitly transforms USE32 to USE16 and near to far among other changes; this is a build-time source transformation, not a modern host object cast or an independent USER behavior engine. Original recipe by itself cannot prove which specific retained text produced the current immutable USER binary.

| Owned path | Bounded classification |
| --- | --- |
| `wow16/user/k/daytona/user.res` | Current-only resource artifact; provenance and binding to installed USER pending. |
| `wow16/user/k/daytona/wowasmk.asm` | Current-only generated-style upstream assembly; recipe copies NTUSER client output to this name. Exact origin unproved. |
| `wow16/user/k/daytona/wowk.asm` | Current-only generated-style transformed assembly; recipe target, but its output identity and actual binary selection unproved. |
| `wow16/user/k/daytona/wowrtl.asm` | Current-only copied/generated-style RTL-client assembly; source and binary provenance unproved. |
| `wow16/user/k/daytona/wowkr1.asm` | Current-only intermediate generated-style file; recipe names it as first transformation of wowrtl. |
| `wow16/user/k/daytona/wowkr.asm` | Current-only generated-style second-stage target; binary identity and actual consumer unproved. |
| `wow16/user/listing.inc` | Original assembly formatting/include input; exact resulting conditional effects remain profile-specific. |
| `wow16/user/wowasm.cmd` | Original WOW transformation helper for another output path; contains compilation/deletion, retained as historical metadata only. |
| `wow16/user/wowasmc.cmd` | Original helper targeting `c` path; alternate build profile, do not execute. |
| `wow16/user/wowasmk.cmd` | Original helper used by captured `k` recipe, including intermediate compile/error filter and deletion. |
| `wow16/user/wowasm.sed` | Original transformation rules shared by helper scripts; generated-code input, not a runtime provider. |

The actual guest/host message, handle and callback semantics are owned by the selected original guest and WOW/USER boundaries, not by the generator script. Existing T420 USER focused fixtures and T422 candidate records cannot prove these current-only outputs generated an installed image. S15 should verify immutable guest request/callback/lifecycle using the exact guest binary and selected host profile; S19 binds results to hashes. If provenance matters, compare identity against independently available original source outputs without executing scripts or rebuilding guest. Retain alternate paths and all source capability pending that proof.

Replay `rg -n "OBJ386|wowk.obj|wowkr.obj|wowasmk.cmd" src/mvdm/wow16/user/makefile`, inspect `wowasm.sed` and the named inputs; rerun part-8 inventory observer. No guest, product or shared ledger modifications and no build executed.
