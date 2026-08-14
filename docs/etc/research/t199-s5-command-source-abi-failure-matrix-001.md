# M0 T199 S5 COMMAND Source, ABI, and Failure Matrix

## Question

What is the complete original OpenNT COMMAND BOP package (`54:00..10`), its fixed/copyable ABI boundary, and the justified disposition before package implementation?

## Inputs

The pinned authority is `src/opennt/base/mvdm/inc/cmdsvc.h` plus `src/opennt/base/mvdm/dos/command/{cmddisp,cmdexit,cmdmisc,cmdredir,cmdexec,cmdconf,cmdkeyb,cmdenv}.c`. `cmddisp.c:15-33` is the ordered seventeen-entry `apfnSVCCmd` table. Current hashes: `cmddisp.c` `2A906E7100DA9DB6F16BAB91C767B149F28E63A0DB2803297812A8E86D2B19D6`; `cmdmisc.c` `1CA036932368A0D0BF1848B078CC80CD66E67A4CC46AD26A0C1B236FE40F4281`; `cmdexit.c` `3FB4616667B2B73F00ED56D5944A786BE741F5DB511E6F0D5F6A751593A418C9`; `cmdredir.c` `A403F15BA9EC33AF85AB38AAA7C4FEA301016C4BB09A1CCBD3ADEE71A07DCC49`; `cmdexec.c` `98A941095D47AE73EF9731D389A433203C7184B0A016B62E24B202BB89F829D0`; `cmdconf.c` `32892C8C489BED9E4BE8FAED12E1BAFBC6C580BA81A509425E8493DE1E6FFC0A`; `cmdkeyb.c` `B945CD4A072B592211F6427EE526B2CA47BE05AC6BEF9A33956105749D899EBA`; and `cmdenv.c` `AFC71D306491FE6A7F8CD7356B8682176A33A05F3652BAE735BE079B3CD0A32B`.

`CMDINFO` is two-byte packed and includes 16-bit guest segment/offsets plus a 32-bit redirector pointer. It is evidence, not a portable ABI: modern providers gather copied validated fields and never retain `GetVDMAddr` pointers.

## Complete callable table

| Svc | Original handler/source | Historical ABI and host binding | Current evidence | Disposition |
| ---: | --- | --- | --- | --- |
| `00` | `cmdExitVDM`, `cmdexit.c:27` | `TerminateVDM` lifecycle/process termination. | None. | CLI terminal-policy deferred. |
| `01` | `cmdGetNextCmd`, `cmdmisc.c:25` | `CMDINFO`, command/environment/path, PIF/redirection. | Finite launch transaction only. | Launch candidate. |
| `02` | `cmdComSpec`, `cmdmisc.c:635` | First-call COMSPEC state and guest string. | Bootstrap fragment. | Launch/environment candidate. |
| `03` | `cmdSaveWorld`, `cmdmisc.c:654` | `CHECK_IT_LATER` body is inactive. | No-state `RIP+4` provider. | Original no-op. |
| `04` | `cmdGetCurrentDir`, `cmdmisc.c:530` | Drive/output buffer and host CWD. | Adapter-only immutable root. | Namespace/CWD deferred. |
| `05` | `cmdSetInfo`, `cmdmisc.c:591` | `DS:DX` `SCSINFO`/DOSDATA pointers retained as globals. | Copied registration fragment. | Launch-state deferred. |
| `06` | `cmdGetStdHandle`, `cmdredir.c:235` | Win32 handles, pipes, temp files. | None. | Explicitly unavailable. |
| `07` | `cmdCheckBinary`, `cmdexec.c:137` | Host executable/path classification. | None. | Execution deferred. |
| `08` | `cmdExec`, `cmdexec.c:538` | Native process, console, environment. | None. | Explicitly unavailable. |
| `09` | `cmdInitConsole`, `cmdmisc.c:729` | Video VDD/console activation. | None. | Console/VDD deferred. |
| `0A` | `cmdExecComspec32`, `cmdexec.c:501` | 32-bit COMSPEC execution. | None. | Explicitly unavailable. |
| `0B` | `cmdReturnExitCode`, `cmdexec.c:602` | Child result, redirection and CWD sync. | None. | Execution/redirection deferred. |
| `0C` | `cmdGetConfigSys`, `cmdconf.c:60` | Temp-file materialization and guest path. | Immutable boot-file fragment. | Paired config candidate. |
| `0D` | `cmdGetAutoexecBat`, `cmdconf.c:97` | Same paired materialization contract. | Immutable boot-file fragment. | Paired config candidate. |
| `0E` | `cmdGetKbdLayout`, `cmdkeyb.c:56` | `DX` KEYB state; keyboard DLL/VDD/guest data. | Fixed `DX=0`, four-byte adapter-only evidence; one native continuation. | Console/keyboard deferred. |
| `0F` | `cmdGetInitEnvironment`, `cmdenv.c:179` | Environment transform, guest buffer, `BX` retry. | Related COMSPEC fragment only. | Environment deferred. |
| `10` | `cmdGetStartInfo`, `cmdmisc.c:893` | `AL=(DosSessionId ? 1 : 0)`. | None. | CLI session-policy deferred. |

## Sentinel and failure rule

`SVC_CMDLASTSVC` is `17`, so `54:11` is not callable. Historical `CmdDispatch` checks the bound, sets CF and returns `FALSE` only under `DBG`; its non-debug form indexes the table unchecked. That unsafe release implementation is not a provider contract. Modern common ingress rejects `54:11` before provider selection; no facade may reproduce an out-of-bounds call.

All nontrivial handlers are host-bound through `softpc.h`, CCPU/SAS pointers, Win32 process/console/VDD APIs, or historical globals. `cmddisp.c` is therefore a source-order and failure reference only, never a direct link target.

## Current adapter cross-check

`bx_ntvdm_command_plane_v1` already classifies the entire family: lifecycle (`00`); launch (`01,02,04,05`); config (`0C,0D`); execution/redirection (`06,07,08,0A,0B`); console (`09,0E`); environment (`0F`); session (`10`); original no-op (`03`). It rejects the sentinel and marks `08`/`0A` explicitly unavailable.

Current composition has fragments for `01`, `02`, `04`, `05`, `0C`, `0D`, and `0E`; none closes its component. `54:0E` has only fixed `DX=0`, four-byte adapter-only evidence and one native continuation. `54:04` has only adapter-only immutable-root success/failure and no accepted native integration.

## Follow-up

The implementation unit remains one COMMAND package with lifecycle, launch, paired config, execution/redirection, console/keyboard, environment, and session subproviders. S6 must choose package-wide capability and failure dispositions before provider expansion. No trace is admitted by this map.
