# opennt-bop

Minimal-change mirrors of original OpenNT BOP providers, selector/service
routing and source-owned provider control flow. Preserve original names,
parameters, structures, ordering and observable failures.

It may call `opennt-host`, `adapter-win32` and `adapter-softpc` through their
declared interfaces; it must not touch Bochs objects, `bx-mantle` or `bx-core`
directly.

## Local intrusion register

Every edit to an imported BOP mirror is registered here with original path and
identity, exception identifier, necessity and focused verification. Each edited
source expression additionally carries a `DIVERGENCE:` comment naming the
replacement and reason. The future static library name is `opennt-bop`; its
public include surface is the BOP ingress/provider ABI consumed by `app`.

## Production layout

| Directory | Contents | Modification rule |
| --- | --- | --- |
| `command/`, `dem/`, `xms/`, `dpmi/`, `softpc/` | Compiled, minimal-change OpenNT provider bodies, retained in their original owner-family grouping. | Every changed statement, include or definition is marked `DIVERGENCE:` and registered below. |
| `ingress/` | Compiled project-owned BOP ingress and selector routing. | May adapt component boundaries, but may not reimplement an imported provider without a source-derived exception. |

Immutable originals, uncompiled mirror fragments, pre-T260 overlays and import
manifests are evidence rather than production code. They are retained under
`docs/etc/legacy_code/opennt-bop/`; the authoritative upstream comparison root
remains `refs/opennt/`.

## Local divergence register

Each row is a required nonzero difference from an identified OpenNT source.
The cited production file also contains the corresponding `DIVERGENCE:`
comment. Historic overlays are evidence only and are recorded in
`docs/etc/legacy_code/opennt-bop/`, not as a second provider.

| ID | Original definition purpose | Divergence reason | Replacement implementation | Production file(s) |
| --- | --- | --- | --- | --- |
| `BOP-DIV-001` | `cmdconf.c` finds NT4's installed `command.com`. | A standalone bundle has no NT4 system installation contract. | Ask the COMMAND host provider for the admitted source-built image. | `command/cmdconf.c:514` |
| `BOP-DIV-002` | `cmddisp.c` imports the NT4 CCPU/SAS header closure. | That product-host closure is not independently linkable. | Use the fixed-width COMMAND shim while retaining the service table. | `command/cmddisp.c:9` |
| `BOP-DIV-003` | `cmdenv.c` snapshots the process environment. | Environment ownership is per CLI session. | Materialize an equivalent session-owned Unicode multisz. | `command/cmdenv.c:48` |
| `BOP-DIV-004` | `cmdenv.c` uses historical environment byte counts. | Host width differs on x64. | Make the original `DWORD` count conversion explicit. | `command/cmdenv.c:440` |
| `BOP-DIV-005` | `cmdenv.c` grows by fixed 512-byte units. | Modern bounded input may exceed one legacy increment. | Grow by the actual bounded requirement, retaining the original result contract. | `command/cmdenv.c:457` |
| `BOP-DIV-006` | `cmdenv.c` relies on source-flow initialization. | Modern `/W4 /WX` rejects the unproven local state. | Initialize only the temporary; preserve short-circuit ordering. | `command/cmdenv.c:554` |
| `BOP-DIV-007` | `cmdenv.c` counts values through historical pointer assumptions. | Counts must be width-stable on x86/x64. | Keep `DWORD` count fields explicit. | `command/cmdenv.c:567` |
| `BOP-DIV-008` | `cmdenv.c` assigns `p1` through a short circuit. | Modern flow analysis cannot prove that assignment. | Initialize `p1` without changing source traversal. | `command/cmdenv.c:725` |
| `BOP-DIV-009` | `cmdenv.c` computes an environment name length. | The OpenNT count ABI is `DWORD`, not host `size_t`. | Explicitly retain the `DWORD` conversion. | `command/cmdenv.c:734` |
| `BOP-DIV-010` | `cmdexec.c` uses the historical global `VDMInfo`. | The imported global conflicts with the admitted `cmdmisc` body. | Retain a function-local return record with the same lifecycle order. | `command/cmdexec.c:697` |
| `BOP-DIV-011` | `cmdexec.c` treats `BX:CX` redirection data as host-addressable. | A guest token cannot be a host pointer on x64. | Resolve it through the session redirection map. | `command/cmdexec.c:724` |
| `BOP-DIV-012` | `cmdexit.c` imports CCPU/SoftPC product headers. | Only the typed stop seam is needed in this composition. | Replace the include closure, preserving the imported body. | `command/cmdexit.c:11` |
| `BOP-DIV-013` | `cmdmisc.c` assigns a title length to `ULONG`. | `strlen` is native-width. | Use an explicit historical-width conversion. | `command/cmdmisc.c:122` |
| `BOP-DIV-014` | `cmdmisc.c` stores redirection information in a 32-bit CMDINFO field. | Raw pointers cannot cross that field on x64. | Use the session's opaque guest token. | `command/cmdmisc.c:152` |
| `BOP-DIV-015` | `cmdmisc.c` computes command-line length. | The original observable type is `ULONG`. | Explicitly preserve that conversion. | `command/cmdmisc.c:318` |
| `BOP-DIV-016` | `cmdmisc.c` publishes `pRdrInfo` in CMDINFO. | Host pointer width is not guest ABI width. | Publish a fixed guest token in the original slot. | `command/cmdmisc.c:371` |
| `BOP-DIV-017` | `cmdmisc.c` writes the historical `0xff` sync marker. | Plain signed conversion is warning-prone on x64. | Make the same 8-bit value explicit. | `command/cmdmisc.c:376` |
| `BOP-DIV-018` | `cmdmisc.c` walks a multisz with pointer-to-`ULONG` casts. | Such casts truncate x64 pointers. | Keep bounded source traversal using native-width arithmetic. | `command/cmdmisc.c:649` |
| `BOP-DIV-019` | `cmdpif.c` declares ForceDos-only locals. | Their owning branches are absent. | Retain declaration order but silence unused-local diagnostics. | `command/cmdpif.c:31` |
| `BOP-DIV-020` | `cmdpif.c` assigns a `MAX_PATH` length to a 16-bit CMDINFO field. | Modern compilation requires a visible narrowing boundary. | Explicitly convert the bounded original value. | `command/cmdpif.c:231` |
| `BOP-DIV-021` | `cmdpif.c` assigns a bounded command path size. | The destination retains a 16-bit original ABI. | Explicitly convert the bounded original value. | `command/cmdpif.c:264` |
| `BOP-DIV-022` | `cmdredir.c` uses K&R forms and diagnostic locals. | `/W4 /WX` diagnoses them. | Preserve semantic body while applying only warning-clean declarations. | `command/cmdredir.c:15` |
| `BOP-DIV-023` | `mem_size.c` includes Insignia/CPU/SAS product headers. | They are not a standalone modern closure. | Use the narrow SoftPC memory-size facade. | `softpc/mem_size.c:4` |
| `BOP-DIV-024` | `suballcp.h` imports private NT headers. | They are unavailable outside NT4 composition. | Use `xms_shim.h` only. | `xms/suballcp.h:2` |
| `BOP-DIV-025` | `suballoc.c` imports private NT headers. | They are unavailable outside NT4 composition. | Use `xms_shim.h` only. | `xms/suballoc.c:2` |
| `BOP-DIV-026` | `xms.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xms.c:2` |
| `BOP-DIV-027` | `xmsa20.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xmsa20.c:2` |
| `BOP-DIV-028` | `xmsblock.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xmsblock.c:2` |
| `BOP-DIV-029` | `xmsdisp.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xmsdisp.c:2` |
| `BOP-DIV-030` | `xmsmisc.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xmsmisc.c:2` |
| `BOP-DIV-031` | `xmsumb.c` imports XMS/SoftPC/SAS/private NT headers. | The historical product closure is absent. | Use the fixed XMS shim boundary. | `xms/xmsumb.c:2` |
| `BOP-DIV-032` | `nt_pif.c` and COMMAND's `cmdpif.h` expose different historical `PIF_DATA` definitions. | Direct linking would cross incompatible layouts. | Keep the host parser source unchanged and translate only its compatible result through a separately named BOP composition unit. | `command/opennt_pif_composition.c:33`, `command/opennt_pif_composition.h` |
| `BOP-DIV-034` | COMMAND flushes historical LPT open state. | No LPT controller model is admitted. | Preserve a no-controller outcome rather than claiming a flush. | `command/opennt_command_composition.c:297` |
| `BOP-DIV-035` | COMMAND publishes console bytes through BaseSrv/CSR. | The NT4 product broker is absent. | Retain the session publication boundary. | `command/opennt_command_composition.c:307` |
| `BOP-DIV-036` | COMMAND probes an old 1 KiB DOS environment span. | That limit is not the source provider contract. | Read the exact bounded multisz extent. | `command/opennt_command_composition.c:866` |
| `BOP-DIV-037` | COMMAND materializes historical SAS pointers. | Raw aliased host access is unsafe. | Use bounded copied spans with alias rejection. | `command/opennt_command_composition.c:908` |
| `BOP-DIV-038` | `demdir.c` imports the historical DEM/SoftPC/OEM product-header closure. | That closure cannot compose independently with the modern component graph. | Split only the include boundary between BOP composition and same-shaped adapter-win32 OEM calls. | `dem/demdir.c:13` |

The migration evidence is
[`m0-t260-s5-opennt-bop-layout-migration-001.md`](../../docs/etc/evidence/m0-t260-s5-opennt-bop-layout-migration-001.md).
