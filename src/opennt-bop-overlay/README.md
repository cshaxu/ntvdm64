# opennt-bop-overlay

Private source bodies for registered OpenNT BOP mirror divergences. This root
has no public ABI and is included only by the matching `opennt-bop` mirror
translation unit; no adapter, host, session or app source may include it.

| ID | Original definition purpose | Private divergence | Mirror boundary |
| --- | --- | --- | --- |
| `BOP-DIV-102` | `cmdexec.c:cmdExec32` owns the historical CCPU worker, BaseSrv re-entry and process-wide standard-handle swap. | The bounded one-session public-Win32 worker is a same-signature replacement too large for the COMMAND mirror. | `command/cmdexec.c` includes this one private `cmdExec32` body; no other component may include it. |
| `BOP-DIV-103` | Reached COMMAND providers share active-call, bounded pointer materialization/writeback and terminal glue that OpenNT obtained from its product-global CCPU/SAS/CSR composition. | The source-derived body has no standalone original translation unit and exceeds the mirror threshold. | Original `command/cmd.c` includes this one private shared body; no other component may include it. |
| `BOP-DIV-104` | `cmdexec.c` consumes an SCS address projection supplied by historical product-global CCPU/SAS identity. | The bounded source-derived projection is not an OpenNT mirror body. | Original `command/cmdmisc.c` includes this private body only. |
| `BOP-DIV-105` | `cmdconf.c` consumes historical product configuration and bootstrap inputs. | The one-session copied configuration binding is not an OpenNT mirror body. | Original `command/cmdconf.c` includes this private body only. |
| `BOP-DIV-106` | `cmdenv.c` consumes process environment/UI product state. | The one-session snapshot/conversion binding is not an OpenNT mirror body. | Original `command/cmdenv.c` includes this private body only. |
| `BOP-DIV-107` | XMS package configuration, copied-call validation and scoped CCPU/SAS composition surround the original XMS services. | OpenNT has no independently composable source unit for that NT4 product lifecycle. | Original `xms/xms.c` includes this private body only. |
| `BOP-DIV-032` | `softpc.new/host/src/nt_pif.c` parses PIF data for original COMMAND `cmdpif.c`. | Its legacy parser record and process-global state are layout-incompatible with the COMMAND caller record, and extracting it to `opennt-host` would create a host-to-BOP dependency. | The exact parser remains in the COMMAND mirror; its modern binding and same-process layout bridge are private here. | Original `command/cmdpif.c` includes this private body only. |

| ID | Original definition purpose | Private divergence | Mirror boundary |
| --- | --- | --- | --- |
| `BOP-DIV-023` | `memory_size` reads the mutable BIOS memory-size word. | The reached one-function source subset is private because the complete SoftPC file is not admitted and the subset exceeds the mirror threshold. | `softpc/mem_size.c` |
| `BOP-DIV-080` | `cassette_io` and `emm_init` are historical SoftPC BIOS entry points. | The admitted real-mode AH=88h and no-EMS source subsets are private because their full product files are not admitted and the subsets exceed the mirror threshold. | `softpc/tape_io.c`, `softpc/emm_fncs.c` |
| `BOP-DIV-056` | `vrdisp.c` dispatches the VDMREDIR service table through the historical CCPU frame. | The copied-frame dispatcher is a source-derived rehost, not a retained OpenNT mirror body. | `vdmredir/vrdisp.c` |
| `BOP-DIV-090` | `demdisp.c` owns the original DEM service table and shared dispatcher state. | The reached subset exceeds the mirror threshold, but its shared globals cannot be duplicated or renamed without changing DEM behavior. | `dem/demdisp.c` includes the private body in its original linkage unit. |
| `BOP-DIV-091` | `vrmslot.c` owns the VDMREDIR mailslot record/list state and source-shaped handlers. | The retained source-derived rehost exceeds the mirror threshold; splitting or wrapping individual entries would duplicate the private list lifecycle. | `vdmredir/vrmslot.c` includes the private body in its original linkage unit. |
| `BOP-DIV-108` | `nt_mouse.c:host_mouse_install1` begins the NT4 mouse product shell. | The admitted headless source fragment publishes only the resident INT 33h vector. | `softpc/nt_mouse.c` includes this private body only. |
| `BOP-DIV-109` | `printer_.c:printer_io` includes device and monitor product paths. | The admitted fragment keeps only source-ordered LPT open/close lifecycle. | `softpc/printer_.c` includes this private body only. |
| `BOP-DIV-110` | `keybd_io.c:UpdateKbdInt15` and `nt_bop.c:MS_bop_F` require keyboard/PPI/timer/ICA composition. | The admitted fragments keep only checked IVT/vector publication. | `softpc/keybd_io.c` and `softpc/nt_bop.c` include these private bodies only. |
