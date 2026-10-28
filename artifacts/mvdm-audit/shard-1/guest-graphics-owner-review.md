# Shard 1: pending DOS GRAPHICS image and printer-pattern inputs

At observation all three scoped paths were `待核验` in the current shard ledger. The observer rechecks each against both frozen OpenNT editions and records current ledger hash, source hashes and catalog IDs. It independently verifies original makefile/link inputs and the exact include relationships below.

| File | Established original role and relation |
| --- | --- |
| `dos/v86/cmd/graphics/GRAPHICS.COM` | Retained DOS GRAPHICS guest program. The original makefile names `graphics.com` as its target, links `graphics.exc` using `graphics.lnk`, and declares the latter as the target prerequisite. This does not prove that the retained binary came from that particular build. |
| `dos/v86/cmd/graphics/grpattrn.str` | Guest printer pattern structure declarations. Six original ASM source files include it directly (`grbwprt`, `grcolprt`, `grctrl`, `grload2`, `grload3`, `grpattrn`); the original makefile also names it as object prerequisites. |
| `dos/v86/cmd/graphics/grcommon.ext` | Guest graphics common-code external declarations, explicitly named as a `grbwprt.obj` make dependency. The current original ASM source search found no direct `INCLUDE grcommon.ext`; `grbwprt.asm` and `grcolprt.asm` instead directly include `grcommon.asm`. Thus a make dependency is proved, not actual macro expansion of this `.ext` file. |

No exact path appears in the four checked x86 worker/WOW32 and x64 frontend/service Ninja graphs or dependency logs. The two declaration files are candidates for guest historical build-input classification only relative to those checked host targets. `GRAPHICS.COM` retains its guest-program identity and must not be dismissed as an intermediate.

The source-tree `GRAPHICS.COM`, exact `O:/winnt/system32/GRAPHICS.COM` and `WINNT/SYSTEM32/GRAPHICS.COM` member of `assets/winnt.zip` are byte-identical, SHA-256 `989B38806EC1E1BDD30DA3DC6B41CC85BDC81DDA74F870FCC7DF399876EB2756`. This is package identity, not proof of execution, printer support, or source-to-image lineage.

Replay `python artifacts/mvdm-audit/shard-1/guest-graphics-owner-observer.py`. No guest/product execution or modification, printer operation, host build mutation or shared-ledger change was performed. Other build configurations and runtime print behavior remain outside this observation.
