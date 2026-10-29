# S10 follow-up: DOS v86 role ownership

Read-only review of the 38 C/H rows under `dos/v86/` marked `role-pending` in `host-ch-scope/inventory.csv`; inventory snapshot SHA-256: `d873f70ce701f2f6a487a649acdd13edfad0330c18faca9f1da9b12544210bc5`. Three tool rows already have ledger status `excluded`; scope and ledger are distinct. This report does not change either. Source paths below are relative to `src/mvdm/`. SHA-256 prefixes identify the actual inspected files; complete hashes and both OpenNT comparison hashes are recorded in `frozen-file-reconciliation.csv`. All 38 match both recorded originals.

## Basis and limitations

- Historical `dos/v86/make.inc` selects `cl16`, `link16` and `-I. -I..\..\h`; `cmd/mem/makefile` builds MEM.EXE using that environment and explicitly names `version.h` and `copyrigh.h`. `cmd/mem/mem.c` includes the eight cited common names plus its local `parse.h`; `memex.c` also includes `versionc.h`. Those common names have no same-named file in `cmd/mem`. This demonstrates a historical guest build relationship, not that a presently installed guest binary contains these sources.
- The build-helper programs below are historical producer tools; a tool being executed by a host to produce a guest image does **not** make it part of the NTVDM runtime, nor does it make its source a guest runtime module. Recorded historical executables are not proof of their provenance. Guest builds and modifications were not performed.
- The existing guest-tree name-collision dependency evidence contains retained x86 worker, x86 WOW32, x64 worker and package x86 worker graph snapshots. None has an exact path dependency on any of these 38 files. This is a bounded historical observation, not proof for every configuration; graphs were not rebuilt in this review. Host include-spelling counts below are from that existing analysis, not resolved identity. For example `bios.h` host dependencies there resolve to `softpc.new/base/inc/bios.h`, and the `version.h` dependency resolves to `src/common/version.h`; basename alone never proves dependency.
- Other legacy headers lack sufficient exact-path selection evidence in the inspected build graphs. Keep their host/shared ownership **pending**; 16-bit-looking syntax or absent captured dependencies alone is not proof of global exclusion. Locale header selection likewise remains unproven. The DOS and WOW source inventories label these paths guest context/trace but are filename inventories, not independent semantic or dependency proof.

## Per-file results

`host spellings` is the captured host lexical include count; it is not a resolved dependency. `pending` means further exact dependency evidence is needed before clearing host audit scope.

| File | SHA-256 prefix | Evidence and conservative ownership | Host spellings |
|---|---|---|---:|
| `dos/v86/cmd/command/buildmsg.c` | `4b9fe27d3c5f` | Historical COMMAND build helper: `cmd/command/makefile` builds `command.ctl` with buildmsg.exe; source uses DOS `intdos` and segmented pointers. | — |
| `dos/v86/cmd/command/usa-ms/bldmsg.h` | `618b85a24adc` | Locale build-helper header candidate: `buildmsg.c` includes `bldmsg.h`; exact locale include selection not established; pending. | — |
| `dos/v86/cmd/mem/parse.h` | `aa0354eadad2` | MEM local header: `mem.c` includes `parse.h`; historical 16-bit guest build; no captured host exact dependency. | — |
| `dos/v86/h/assert.h` | `6a32b3d4b878` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 4 |
| `dos/v86/h/bios.h` | `e90adf1f14f8` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 48 |
| `dos/v86/h/conio.h` | `4c58132772ff` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 4 |
| `dos/v86/h/copyrigh.h` | `0f4bda1c3e68` | Historical MEM makefile explicitly names `$(hinc)\copyrigh.h` as prerequisite; no captured host exact dependency. | 0 |
| `dos/v86/h/ctype.h` | `c96d2c704a0e` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 17 |
| `dos/v86/h/direct.h` | `687ecc848473` | Historical SETVER source includes this basename (`parse.c` / `setver.c`); exact selected file not proven; pending. | 1 |
| `dos/v86/h/dos.h` | `dd9f790b37b7` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 0 |
| `dos/v86/h/errno.h` | `55e3ced19f97` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 3 |
| `dos/v86/h/fcntl.h` | `79915ee9a0e6` | Historical SETVER source includes this basename (`parse.c` / `setver.c`); exact selected file not proven; pending. | 5 |
| `dos/v86/h/float.h` | `386d2dd80397` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 1 |
| `dos/v86/h/graph.h` | `3a2d85ce58b3` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 0 |
| `dos/v86/h/io.h` | `41a51f1eab9e` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 9 |
| `dos/v86/h/limits.h` | `afe7f9b89ded` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 1 |
| `dos/v86/h/malloc.h` | `aa0cb86f9d6b` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 24 |
| `dos/v86/h/math.h` | `4df668bf2766` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 3 |
| `dos/v86/h/memory.h` | `ad17bb794def` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 7 |
| `dos/v86/h/process.h` | `0d005443e278` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 6 |
| `dos/v86/h/search.h` | `2d476b5f3629` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 0 |
| `dos/v86/h/setjmp.h` | `1a3265e579c8` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 4 |
| `dos/v86/h/share.h` | `f72afe01ccaf` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 0 |
| `dos/v86/h/signal.h` | `8ade415619a7` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 5 |
| `dos/v86/h/stdarg.h` | `1375a4706d03` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 9 |
| `dos/v86/h/stddef.h` | `ef005074dbc2` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 6 |
| `dos/v86/h/stdio.h` | `5f33039b1dee` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 151 |
| `dos/v86/h/stdlib.h` | `9f2969d0ff20` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 91 |
| `dos/v86/h/string.h` | `b1643c577511` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 41 |
| `dos/v86/h/time.h` | `9b26227e1f2e` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 4 |
| `dos/v86/h/varargs.h` | `9d4d6f8357b2` | Legacy v86 header; no exact selected current-host dependency established from captured graphs; ownership pending. | 0 |
| `dos/v86/h/version.h` | `0acb18fc1a4d` | Historical MEM 16-bit build: `mem.c` includes this name. No MEM-local namesake; see include-path qualification above. | 0 |
| `dos/v86/h/versionc.h` | `6782153411f3` | Historical MEM 16-bit build: `memex.c` includes this name. No MEM-local namesake; see include-path qualification above. | 0 |
| `dos/v86/tools/src/buildidx/buildidx.c` | `791f0acc67d4` | Historical build helper: messages/makefile:14 invokes buildidx; tools/src/buildidx/sources defines BUILDTOOL /AS /G2s. | — |
| `dos/v86/tools/src/getsize/getsize.c` | `39d35e722f5f` | Historical build helper: doskrnl/dos/makedos.cmd invokes getsize; its sources defines BUILDTOOL /AS /G2s. | — |
| `dos/v86/tools/src/h2inc/h2inc.c` | `d41ccc8bb473` | Historical build helper: redir/makefile:96 invokes h2inc; h2inc.mak uses -AS and bind. | — |
| `dos/v86/tools/src/stripdd/stripdd.c` | `7a5e4536964d` | Historical build helper: dev/himem/makefile:61 invokes stripdd; its sources defines BUILDTOOL /AS /G2s. | — |
| `dos/v86/tools/src/stripz/stripz.c` | `3b3dd85fbef2` | Historical build helper: doskrnl/dos/makefile:323 invokes stripz; its sources defines BUILDTOOL /AS /G2s. | — |

## Handoff

Do not automatically mark the whole shard guest-only or exclude by path. The exact MEM dependencies above support guest-build classification for those specific files; helper sources are build tooling, and the remaining legacy headers remain unresolved for host/shared ownership until exact build-resolution evidence exists. No product code, guest source, guest image, inventory, ledger or build state was changed; no builds or tests were run.
