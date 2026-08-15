# T202 S5 COMMAND host-capability source/ABI/mutation map

## Scope and method

This is the complete admission map for `cmddisp.c`'s seventeen callable
entries, `54:00..10`; `54:11` is only `SVC_CMDLASTSVC`. It supersedes neither
the original sources nor the retained T199 contained-profile ledger. Its
purpose is to identify which former “unavailable” outcomes were merely a
policy choice, and which require replacement of an NT4 composition dependency.

The audit uses the user-approved definition of non-invasive CLI integration:
ordinary user-mode Win32/NT APIs may use the caller's permitted host state,
but the product may not require a modified kernel, restored NTVDM API,
installed host component, or system/registry mutation.

## Source and capability findings

| BOP | Original owner and guest ABI | Historical dependency | Mutation/profile class | S5 disposition and next provider path |
| --- | --- | --- | --- | --- |
| `54:00` | `cmdexit.c:cmdExitVDM`; no guest input | `TerminateVDM` lifecycle | none | Keep source-derived controlled stop. It is CLI engine lifecycle, not a host process exit. |
| `54:01` | `cmdmisc.c:cmdGetNextCmd`; CMDINFO copied from guest | SCS/VDM launch state and continuation | session state | Source-derived CLI launch/session provider. Existing checked CMDINFO ABI is reusable; audit its registration/return sequence as one package. |
| `54:02` | `cmdmisc.c:cmdComSpec`; copied COMSPEC/environment layout | command-session globals and VDM memory access | profile read | Source-derived profile provider; ordinary selected-host environment may be an input, but no ambient path or raw guest pointer may cross. |
| `54:03` | `cmdmisc.c:cmdSaveWorld` | body disabled by source | none | Retain original no-op/four-byte resume. |
| `54:04` | `cmdmisc.c:cmdGetCurrentDir`; guest output buffer | process current-directory/session drive state | direct or readonly context | Ordinary `GetCurrentDirectory`/`SetCurrentDirectory` are usable. Rehost through one copied CLI host-context provider, with direct policy allowed and readonly/overlay/virtual dispositions explicit. |
| `54:05` | `cmdmisc.c:cmdSetInfo`; SCS/launch registration records | historical SCS/CCPU pointer retention | session state | Source-derived fixed-width registration provider. Preserve field order and failure form; no host mutation or pointer retention. |
| `54:06` | `cmdredir.c:cmdGetStdHandle`; `CX` selects stdin/out/err, result is handle/size | raw inherited `HANDLE`, pipe threads, temporary files | stream/handle capability | `GetStdHandle`, pipes, files and threads are available, but the raw-handle guest ABI is forbidden. Rehost only after one CLI-owned opaque standard-stream/pipe capability exists; until then explicit package-deferred failure, not stop-by-default. |
| `54:07` | `cmdexec.c:cmdCheckBinary`; `DS:DX` pathname | `GetBinaryType`/path classification plus VDM launch policy | readonly host inspection | Ordinary binary inspection exists. Admit a bounded copied path plus root policy only with the execution/lifecycle package; preserve DOS/WOW/32-bit classification and source failure mapping. |
| `54:08` | `cmdexec.c:cmdExec`; `DS:SI` command tail, `ES:0` environment | `CreateProcess`, thread and VDM re-entry broker | explicit process mutation/lifecycle | `CreateProcess` is available. The missing part is historical child/VDM re-entry composition, not an unavailable API. Defer to a whole CLI process/lifecycle provider with typed child result and no raw handles. |
| `54:09` | `cmdmisc.c:cmdInitConsole` | Video VDD notification/console state | console capability | Console APIs are available; VDD activation is not a modern CLI dependency. Source-derived CLI console provider may own explicit stream/terminal state; absent that provider retains an observable no-console disposition. |
| `54:0A` | `cmdexec.c:cmdExecComspec32`; `ES` environment, `AL` drive | same process/VDM re-entry broker as `54:08` | explicit process mutation/lifecycle | Same provider and admission as `54:08`; no separate endpoint implementation. |
| `54:0B` | `cmdexec.c:cmdReturnExitCode`; `AH` result | historical queued command/VDM lifecycle | session lifecycle | Source-derived CLI return/next provider. It must use a typed engine terminal/child-result contract, never the host process exit code. |
| `54:0C` | `cmdconf.c:cmdGetConfigSys`; `DS:DX` 64-byte path output | `%SystemRoot%`, PIF config, temp file and VDM error UI | profile read / optional overlay materialization | Normal file/temp APIs exist. The old temporary-file and system-file behavior is not automatic admission: retain selected guest boot input as the default profile path; any host-backed CONFIG source requires a separate explicit profile policy. |
| `54:0D` | `cmdconf.c:cmdGetAutoexecBat`; `DS:DX` 64-byte path output | same as `54:0C` plus environment extraction | profile read / optional overlay materialization | Same provider family as `54:0C`; keep current copied path contract, do not create an ambient temporary/system file. |
| `54:0E` | `cmdkeyb.c:cmdGetKbdLayout`; `DX`, guest buffers at `DS:SI`/`DS:CX` | console keyboard layout, registry, KEYB and VDD memory contract | readonly console/registry capability | `GetConsoleKeyboardLayoutName`, `GetConsoleCP` and registry reads are ordinary APIs. The unavailable part is the historical KEYB/VDD guest installation path; rehost one complete keyboard/console profile or retain DX=0 source branch. |
| `54:0F` | `cmdenv.c:cmdGetInitEnvironment`; `ES:0` buffer, `BX` paragraphs | process environment, Rtl environment routines, VDM pointer write | readonly host context / session copy | Environment APIs are available. Rehost only through a copied, selected environment snapshot with the original paragraph-retry form; never mutate or expose the ambient process environment by default. |
| `54:10` | `cmdmisc.c:cmdGetStartInfo`; `AL` output | `DosSessionId` / console-session state | CLI lifecycle/console state | Source-derived CLI lifecycle result. No missing Win32 API; it needs an explicit session-state owner. |

## API availability and true gaps

The ordinary APIs identified above are available to a modern user-mode CLI and
do not require host modification: file/path and temporary-file APIs,
environment APIs, console/keyboard APIs, registry reads, standard streams,
threads and `CreateProcess`. They are therefore **not** marked unavailable.

The actual rehost gaps are historical composition mechanisms:

1. `GetVDMAddr`/register macros and CCPU/SAS pass raw guest memory into host
   providers; bx-vdm must replace these with fixed-width copied actions.
2. `TerminateVDM`, VDM re-entry and SCS bookkeeping tie command execution to
   NT4's VDM process broker; ordinary `CreateProcess` alone does not recreate
   their DOS/COMMAND return contract.
3. VDD/video notification and KEYB installation have no admitted modern
   non-invasive substitute. Console and keyboard query APIs remain usable,
   but guest installation is a separate provider decision.
4. `cmdredir.c` transmits inherited raw handles and synchronizes pipe threads.
   Its capability can be rehosted only behind an opaque CLI stream owner;
   adapter/guest register state cannot contain a host handle.

## Whole-provider implementation order

S6 should not reopen the table by trace. It should recover the following
connected provider slices in one COMMAND session, each with a shared
direct/readonly/overlay/virtual disposition and package regression:

1. bootstrap, registration, COMSPEC, selected environment and current
   directory (`01/02/04/05/0F`);
2. console/keyboard/start-state (`09/0E/10`);
3. declared execution and return lifecycle (`07/08/0A/0B`), gated by the
   typed CLI process/result contract;
4. standard streams/redirection (`06`) together with DEM pipe consumers only
   after the opaque stream capability is admitted; and
5. configuration/autoexec profile materialization (`0C/0D`) as a selected
   boot-input policy, not a synthetic default host mutation.

`00` and `03` remain their original terminal/no-op forms throughout. This map
does not implement a provider or run a trace; it is S5 admission evidence.
