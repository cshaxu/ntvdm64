# DOS GRAPHICS historical input and packaging — shard 3

Read-only review of eight pending files under `dos/v86/cmd/graphics`. `graphics-input-observer.py` and captured JSON compare full SHA-256 with both original trees, and inspect package and installed GRAPHICS.COM/GRAPHICS.PRO by exact hash without extracting or modifying images. All eight inputs plus makefile and graphics.lnk are identical to both references. No guest was executed or rebuilt.

Original `makefile:10-15,29-78` names GRAPHICS.COM and GRAPHICS.PRO as distribution outputs, compiles the listed ASM modules using the six declaration/structure/equate inputs, links `GRAPHICS.EXC` from their objects and makes GRAPHICS.COM depend on it. `graphics.lnk:14` names GRAPHICS.EXC as linked output. The retained GRAPHICS.EXC is an MZ-format 20,462-byte intermediate; there is no exact named EXC member in the archive. The archive does contain GRAPHICS.COM (19,694 bytes, SHA-256 `989b38806ec1e1bdd30da3dc6b41cc85bdc81dda74f870fcc7df399876eb2756`) and GRAPHICS.PRO (21,232 bytes, `b734f750a473f83d33aa773fe3d6c7bcdd209d97339f37b03b8c5f3f8eed890b`); both installed system32 files match their archive identities. Different names/sizes and a build dependency do not prove the retained EXC generated the shipped COM, nor prove printer/graphics behavior.

| shard path | proposed disposition | evidence / boundary |
|---|---|---|
| `GRAPHICS.EXC` | excluded, historical intermediate role only | MZ linker target and dependency of COM; never conflate exclusion of this intermediate with absence or completion of shipped GRAPHICS.COM. Source-to-COM provenance and behavior not established. |
| `graphics.tag` | excluded, change-log document role only | Historical text change notes; not executable data. Does not exclude the behaviors it describes. |
| `grctrl.ext`, `grctrl.str` | pending | Historical graphics printer-control external declarations and structures included by grbwprt/grcolprt/grctrl (source lines and makefile); source-to-shipped guest not established. |
| `grinst.ext` | pending | Included by graphics.asm and installation modules; dependency alone does not prove use in shipped guest. |
| `grload3.ext`, `grmsg.equ`, `grparms.ext` | pending | Loader, messages and parameter interfaces consumed by grinst/grload/grparms per makefile and source includes; behavior and source-to-image association not established. |

No aggregate runtime coverage, printer/device availability, screen-print result or original guest equivalence is inferred from archive identity. The guest executable and its configuration are not modified. The `.ext`/`.str`/`.equ` files remain pending rather than being excluded merely because the modern host build does not compile them.
