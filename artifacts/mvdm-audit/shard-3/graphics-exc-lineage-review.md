# S10 shard 3: GRAPHICS.EXC to installed COM byte relationship

This is a read-only refinement of `graphics-input-role-review.md` covering the six pending GRAPHICS guest-build inputs and retained intermediate. No guest rebuilding or execution.

`graphics/makefile` builds the linked `GRAPHICS.EXC` from the graphics/control/install/loader modules and declares `GRAPHICS.COM` dependent on it. `graphics-exc-lineage-observer.py` independently finds that retained `GRAPHICS.EXC` SHA-256 `40B380874875AC10E16BEDB04C5C05E7894A3F27A6EE1CB892948810BBC4D7B4` has a 512-byte MZ header followed by 256 zero bytes, then bytes **exactly equal** to installed `O:/winnt/system32/GRAPHICS.COM`, SHA-256 `989B38806EC1E1BDD30DA3DC6B41CC85BDC81DDA74F870FCC7DF399876EB2756`. The COM also matches approved guest manifest as established in the earlier report. This establishes an exact retained EXC payload-to-COM relationship, not that the current source files produced that historical EXC, nor execution or printer output correctness.

| Pending file | Original consumer/source role | File-level recommendation |
|---|---|---|
| `grctrl.ext` | Control exports referenced by installation and print-control modules; graphics makefile dependencies include `grinst.obj`, `grint2fh.obj`. | Retained guest build interface; don't exclude print-screen capability. |
| `grctrl.str` | Control structures included by monochrome/color and control ASM; dependencies include `grbwprt.obj`, `grctrl.obj`, `grcolprt.obj`. | Retained guest build interface; structure semantics need guest proof. |
| `grinst.ext` | Installer declarations included by `graphics.asm` and loader modules; makefile lists them as dependencies. | Retained guest build interface; install semantics remain separate. |
| `grload3.ext` | Loader declarations; `grload.asm` and makefile reference it. | Retained guest build interface; dynamic loader/error behavior untested. |
| `grmsg.equ` | Message equates included by installer and loaders, listed in makefile dependencies. | Retained guest build metadata; actual locale/error strings untested. |
| `grparms.ext` | Parameter declaration included in `grinst.asm` and referenced by its makefile dependency. | Retained guest build interface; command parsing remains untested. |

All six are byte-identical to both original inventories as recorded in earlier report/ledger. Source inclusion and exact EXC/COM byte correspondence do not prove source-to-image production or functionality; retain runtime print screen, palette, printer and error paths as separate obligations. The earlier report's statement that the EXC→COM relation was unproved is now superseded by the exact payload observation, with these limits.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'
python artifacts/mvdm-audit/shard-3/graphics-exc-lineage-observer.py
Get-Content src/mvdm/dos/v86/cmd/graphics/makefile
rg -n -i 'grctrl\.ext|grctrl\.str|grinst\.ext|grload3\.ext|grmsg\.equ|grparms\.ext' src/mvdm/dos/v86/cmd/graphics
```
