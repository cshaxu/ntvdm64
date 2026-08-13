# T191 S1 post-bootstrap COMMAND-family inventory 001

Date: 2026-08-12  
Packet: M0 T191 S1  
Disposition: reached-family source map complete; no source/build/run change.

The sole T190 trace reaches COMMAND services `01`, `02`, `04`, `05`, `0C`,
`0D`, `0E`, and `0F`.  All are table entries in original `cmddisp.c`, entered
by `MS_bop_4` in `nt_bop.c`.  The table below records the actual current
owner/disposition, rather than treating each observed byte as a new task.

| Service | Original owner | Current grouped disposition | Boundary conclusion |
| --- | --- | --- | --- |
| `01` | `cmdGetNextCmd` (`cmdmisc.c`) | finite BYOB command-launch provider | checked CMDINFO gather and finite target/quit state; not a general CreateProcess broker |
| `02` + `0F` | `cmdComSpec` / `cmdGetInitEnvironment` (`cmdmisc.c` / `cmdenv.c`) | T190 source-derived bootstrap pair | checked 64-byte COMSPEC gather, cached CLI-local value, bounded BX retry/write; ambient Win32 environment remains excluded |
| `04` | `cmdGetCurrentDir` (`cmdmisc.c`) | contained drive-projection provider | admitted drive yields root-only path; unadmitted drive returns original-shaped CF failure; no host current directory |
| `05` | `cmdSetInfo` (`cmdmisc.c`) | registration-only provider | retains bounded SCS pointers for the existing launch state; no host pointer crosses ABI |
| `0C` + `0D` | `cmdGetConfigSys` / `cmdGetAutoexecBat` (`cmdconf.c`) | finite boot namespace provider | returns profile-materialized guest names; no NT temporary file or host filesystem exposure |
| `0E` | `cmdGetKbdLayout` (`cmdkeyb.c`) | explicit no-install result | historical code reads console/registry and launches KEYB; CLI profile returns DX=0 and does not emulate keyboard/registry policy |

No reached COMMAND-family endpoint is unowned.  The only paired provider with
a historical host composition gap was `02`/`0F`, now closed by T190.  The
remaining source-derived entries are deliberately finite CLI-profile
capabilities or source-backed unavailable results.  Therefore T191 rejects a
new COMMAND provider slice: the next holistic question is the non-COMMAND BOP
families reached after bootstrap, not another `54:xx` patch.
