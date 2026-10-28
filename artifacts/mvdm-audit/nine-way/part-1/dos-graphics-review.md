# Part 1 MVDM: DOS GRAPHICS profile and build-input family

Ten assigned `dos/v86/cmd/graphics` paths are listed individually below. `dos-graphics-evidence.json` has each full source and three reference hashes, four normalized VALID graph dependency checks, active textual ASM include locations, makefile mentions and ZIP/system32 basename checks. All files match both primary original trees, and all have zero exact current host VALID dependency hits. Replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/dos-graphics-observer.py`. Direct include syntax and makefile mentions are only bounded source evidence; conditional branches, include-path resolution and executable provenance are not proven.

Original `grload.asm:299,567-574` contains the GRAPHICS.PRO filename and open path. Original makefile includes the profile as a delivery target. Current source, ZIP `WINNT/SYSTEM32/GRAPHICS.PRO` and exact installed system32 profile all hash `b734f750a473f83d33aa773fe3d6c7bcdd209d97339f37b03b8c5f3f8eed890b`. Existing `shard-1/guest-graphics-owner-review.md` separately records GRAPHICS.COM identity. These observations do not prove print-screen rendering, printer compatibility, file read success or cleanup.

| Assigned file | Source role and observed inclusion | Remaining obligation |
| --- | --- | --- |
| `graphics.pro` | Runtime printer-profile data; 0 active textual ASM include sites, 1 makefile mentions. | Runtime data; validate selected profile and output. |
| `grcommon.ext` | Common graphics external declarations; 0 active textual ASM include sites, 1 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grctrl.ext` | Print-screen control externals; 6 active textual ASM include sites, 3 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grctrl.str` | Control structures and equates; 4 active textual ASM include sites, 3 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grinst.ext` | Install-code externals; 8 active textual ASM include sites, 7 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grload3.ext` | Loader externals; 2 active textual ASM include sites, 1 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grmsg.equ` | Message/error equates; 5 active textual ASM include sites, 5 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grparms.ext` | Argument-parser externals; 1 active textual ASM include sites, 1 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grpattrn.str` | Printer-pattern structures; 6 active textual ASM include sites, 6 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |
| `grshar.str` | Shared data structures/equates; 8 active textual ASM include sites, 8 makefile mentions. | Historic build carrier; exact expansion, image attribution and related behavior remain bounded. |

`grcommon.ext` has a makefile mention but zero directly active ASM INCLUDE statements in the bounded scan; grbwprt.asm instead includes grcommon.asm. Distinguish dependency listing from expansion. No guest, printer, product, build tree or shared ledger was modified or executed, and other profiles and external providers remain outside this observation.
