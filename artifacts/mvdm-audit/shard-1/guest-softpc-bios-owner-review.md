# Shard 1: pending historical SoftPC guest BIOS sources

Seven `softpc.new/bios` paths were `待核验` in the current shard ledger and match both frozen OpenNT editions. The observer verifies the original `makefile`/`msbio.lnk` chain to guest `NTIO.SYS` and searches normalized VALID dependencies of four checked x86 worker/WOW32 and x64 frontend/service host graphs. No exact scoped source appears in those checked host dependency records. Source identity and recipe membership do not establish installed-image provenance or guest behavior.

| File | Original role and actual recipe evidence |
| --- | --- |
| `biosbop.inc` | BIOS BOP macro and printer operation constants; `mschar.asm` directly includes it and `mschar.obj` is in the original NTIO response. |
| `msend.asm` | End-of-segment markers for IO.SYS data/code and device sections; `msend` is in the original NTIO link response. |
| `msint13.asm` | Despite its filename, the original source explicitly identifies an INT 2F handler and says INT 13 support was removed; `msint13` is linked by the recipe. Do not classify it as the disk BIOS INT 13 implementation by name. |
| `spcemm.asm` | Guest EMM identification device stub with strategy/interrupt/device header, conditional on EMM availability according to source; linked by the original response. |
| `spckbd.asm` | Historical guest keyboard BIOS/driver source; `spckbd` is linked by the original response. The separate `dos/v86/doskrnl/bios/spckbd.asm` has different bytes; a no-index textual comparison reports 221 insertions/19 deletions relative to this copy. Do not substitute one for the other or claim equivalent active behavior. |
| `spcmse.asm` | Historical guest mouse device source describing resident and initialization sections; `spcmse` is linked by the response. The same basename was not present in the other checked BIOS directory. |
| `sysinit2.asm` | Guest system initialization, device and config support; `sysinit2` is linked by the response. |

The original makefile builds `ntio.sys` by linking `msbio.lnk` and relocating its `ntio.exe`. The retained `dos/v86/doskrnl/bios/NTIO.SYS`, exact installed `O:/winnt/system32/NTIO.SYS`, and ZIP member `WINNT/SYSTEM32/NTIO.SYS` match SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`. Because the binary is in the **other** BIOS tree and the keyboard sources differ, this equality cannot identify which source tree or response generated it. Per-file source and alternative-tree hashes are recorded in JSON.

These are candidate guest BIOS source/input roles only relative to the checked host graphs and original recipe; do not exclude guest boot, mouse, keyboard, EMM, IRQ or disk behavior. Replay `python artifacts/mvdm-audit/shard-1/guest-softpc-bios-owner-observer.py`. No guest, compiler, product or provider ran; no guest, product, build tree or shared ledger was modified. Runtime service behavior, BOP wiring, conditional variants and binary lineage remain unverified.
