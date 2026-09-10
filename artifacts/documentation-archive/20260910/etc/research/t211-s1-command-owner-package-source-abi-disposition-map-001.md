# T211 S1 — OpenNT COMMAND owner-package source/ABI/disposition map

## Question

What is the complete original ownership, ABI/failure boundary and modern CLI
provider disposition for every `54:00..10` COMMAND service, before another
COMMAND implementation is admitted?

## Inputs and procedure

- Original dispatcher: `src/opennt/base/mvdm/dos/command/cmddisp.c`.
- Service constants and `CMDINFO`: `src/opennt/base/mvdm/inc/cmdsvc.h`.
- Original owners: `cmdexit.c`, `cmdmisc.c`, `cmdexec.c`, `cmdredir.c`,
  `cmdenv.c`, `cmdconf.c`, and `cmdkeyb.c`.
- The complete all-BOP admission ledger, T202--T207 evidence, and completed
  T210 host-context package.

Read the original dispatch order first, then grouped handlers by shared
historical state, ABI and failure path. Compared each group against current
`bx_ntvdm_command_package_session_v1` routes and the T201 mutation profile.
No source or fixture was changed by this audit.

## Original dispatch boundary

`cmddisp.c` has exactly 17 function pointers in `apfnSVCCmd`, in the order
`SVC_CMDEXITVDM` through `SVC_GETSTARTINFO`; `SVC_CMDLASTSVC` is 17 and is a
debug-only range sentinel, not a callable normal BOP. `CmdDispatch` has no
per-service fallback: it calls the selected owner directly. The table below
therefore preserves service identity while assigning every row to one
component package.

| BOP | Original owner and bounded ABI/failure contract | Component | Modern dependency verdict | Provider disposition | Current action |
| --- | --- | --- | --- | --- | --- |
| `54:00` | `cmdExitVDM` / `cmdexit.c`; no input, private `TerminateVDM` terminal | lifecycle | `TerminateVDM` is not public process-exit semantics | source-derived typed engine terminal, deferred until engine owner admits it | migrate existing controlled terminal boundary |
| `54:01` | `cmdGetNextCmd` / `cmdmisc.c`; DS:DX `CMDINFO`, `VDMINFO` broker/reentry and current-directory update | bootstrap/launch | `GetNextVDMCommand` is a private NT broker with no x64 user-mode replacement | source-derived declared CLI command/session transaction | migrate existing declared-plan helper; no arbitrary child queue |
| `54:02` | `cmdComSpec`; DS:DX command path, builds `COMSPEC=` and AL console/WOW result | bootstrap/environment | checked guest gather plus copied host context are available | source-derived provider | retain/migrate T210 context consumer into package |
| `54:03` | `cmdSaveWorld`; compiled-out historical full guest-image save | lifecycle | fixed historical save path is not a CLI product dependency | original no-op in shipped path | retain no-op, reject save-world feature expansion |
| `54:04` | `cmdGetCurrentDir`; DS:SI output, AL drive, `=X:` / drive validation, CF+AX failure | bootstrap/environment | public drive/context APIs; T210 owns shared state | original+CLI seam | retain T210 provider consumer; delete synthetic fallback |
| `54:05` | `cmdSetInfo`; DS:DX/BX/CX guest `SCSINFO` registrations | bootstrap | private guest pointers only | source-derived checked registration | migrate existing copied registration into package |
| `54:06` | `cmdGetStdHandle`; redirection records, temp files/pipes and host handles | launch/console | Win32 handles/pipes exist, but raw handles cannot cross guest boundary | deferred until complete DOS handle/redirection profile | retain explicit unavailable/deferred route |
| `54:07` | `cmdCheckBinary`; DS:DX path and ES:BX parameter block, file type/WOW branch | launch | `GetBinaryTypeW` and registry reads are public; WOW launch is separate | source-derived classifier candidate | map with launch policy; no endpoint implementation |
| `54:08` | `cmdExec`; copied tail/env/standard-handle record, host child and reentry | launch | `CreateProcessW` exists but broker/reentry and handle model are package state | source-derived bounded terminal child only; broader form deferred | migrate current typed-child evidence into launch component |
| `54:09` | `cmdInitConsole`; enables console/event-thread output | console | public console APIs exist; historical VDD/event thread is private | CLI console capability candidate | replace endpoint helper only with whole console disposition |
| `54:0A` | `cmdExecComspec32`; ES env/AL drive, COMSPEC child/reentry | launch | same `CreateProcess` and broker limitation as `08` | deferred with `08` except already admitted terminal form | migrate alongside launch component |
| `54:0B` | `cmdReturnExitCode`; DX code, AL drive, BX:CX redirection, broker return | lifecycle/launch | private broker unavailable | source-derived terminal-result transport | retain package-local record; no general lifecycle claim |
| `54:0C` | `cmdGetConfigSys`; DS:DX 64-byte result, transform/temp failure historically terminates | environment/config | T209 owns bounded guest configuration images | source-derived startup provider | retain/migrate T209 consumer |
| `54:0D` | `cmdGetAutoexecBat`; DS:DX 64-byte result, same transform/fatal path | environment/config | same as `0C` | source-derived startup provider | retain/migrate T209 consumer |
| `54:0E` | `cmdGetKbdLayout`; DX mode, DS:SI/CX output, console layout/registry/KB16 assets | console | public console/registry APIs exist; KB16 assets are not guaranteed | source-derived no-install/failure when assets absent | migrate endpoint evidence into console component |
| `54:0F` | `cmdGetInitEnvironment`; ES output/paragraph sizing, merged environment/fatal failure | bootstrap/environment | copied T210 environment available; private terminal remains unavailable | source-derived environment provider | retain/migrate T210 projection consumer |
| `54:10` | `cmdGetStartInfo`; AL from `DosSessionId`/broker session | bootstrap/lifecycle | historical broker session has no public equivalent | source-derived declared-session answer or deferred | migrate CLI-profile helper only after bootstrap contract |
| `54:11` | `SVC_CMDLASTSVC` sentinel | dispatcher | not callable | non-callable fence | retain as range rejection |

## Owner components and implementation order

| Component | Members | Required common state | Package order |
| --- | --- | --- | --- |
| bootstrap/environment/config | `01,02,04,05,0C,0D,0F,10` | declared CLI launch/session record, T210 host context, checked guest gathers/writes, T209 configuration images | first |
| launch | `06,07,08,0A` | explicit host-launch policy, opaque stream tokens, execution/reentry disposition, no raw handle | second |
| console | `09,0E` | explicit CLI console capability, keyboard-layout asset disposition, no VDD/event-thread dependency | third |
| lifecycle | `00,03,0B` | typed engine result/stop and cleanup boundary, never host-process termination | fourth |

Every current helper is only a candidate input to its component. In
particular `54:04` and `54:0E` have endpoint evidence, and `54:0B` has a
package-local terminal record; none closes its component.

## Unavailable/deferred register

There is no missing ordinary Win32 API blocking this map. The only unavailable
historical dependencies are `GetNextVDMCommand`/`VDMINFO` broker composition,
`TerminateVDM`, VDD/event-thread behavior, `GetVDMAddr` guest pointers and the
historical redirection/handle model. These are not to be replaced by ambient
host state, `ExitProcess`, raw `HANDLE`, or selector-specific adapters.

Keyboard layout uses still-available console and registry APIs, but modern x64
Windows does not guarantee historical `KB16.COM`/`KEYBOARD.SYS` assets. Their
absence is a source-shaped no-install/failure disposition, not a missing API
or a reason to fabricate guest files.

## Interpretation and next packet

All 17 callable identities and the sentinel now have exactly one original
owner component and disposition. S2 may recover the first whole component:
bootstrap/environment/config. It must take the existing `01/02/04/05/0C/0D/
0F/10` helpers behind one package facade, prove every member's source-shaped
failure behavior, and run one family regression before any trace observation.
