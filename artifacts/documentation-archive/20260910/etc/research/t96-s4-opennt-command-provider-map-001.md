# M0 T96 S4 OpenNT COMMAND Provider Map 001

## Question and method

Which complete original COMMAND BOP services form composable CLI provider
families, and which are bound to the historical NTVDM host? The review uses
the pinned `cmddisp.c` table, all handler definition modules, and guest
`CMDSVC` call sites. It is a source map, not a claim that a historical source
file can be linked into the modern adapter.

Pinned inputs (SHA-256): `cmddisp.c`
`2A906E7100DA9DB6F16BAB91C767B149F28E63A0DB2803297812A8E86D2B19D6`;
`cmdmisc.c` `1CA036932368A0D0BF1848B078CC80CD66E67A4CC46AD26A0C1B236FE40F4281`;
`cmdconf.c` `32892C8C489BED9E4BE8FAED12E1BAFBC6C580BA81A509425E8493DE1E6FFC0A`;
`cmdexec.c` `98A941095D47AE73EF9731D389A433203C7184B0A016B62E24B202BB89F829D0`;
`cmdredir.c` `A403F15BA9EC33AF85AB38AAA7C4FEA301016C4BB09A1CCBD3ADEE71A07DCC49`;
`cmdenv.c` `AFC71D306491FE6A7F8CD7356B8682176A33A05F3652BAE735BE079B3CD0A32B`;
`cmdkeyb.c` `B945CD4A072B592211F6427EE526B2CA47BE05AC6BEF9A33956105749D899EBA`;
and `cmdexit.c` `3FB4616667B2B73F00ED56D5944A786BE741F5DB511E6F0D5F6A751593A418C9`.

## Complete dispatcher disposition

| Svc | Original handler/module | Component and required contract | Current disposition |
| ---: | --- | --- | --- |
| 00 | `cmdExitVDM`, `cmdexit.c` | VDM lifecycle / historical `TerminateVDM`. | CLI controlled-stop candidate; not yet migrated. |
| 01 | `cmdGetNextCmd`, `cmdmisc.c` | Launch envelope, `CMDINFO` guest structure, environment, PIF, redirection and console lifecycle. | Deferred as one launch-composition component. |
| 02 | `cmdComSpec`, `cmdmisc.c` | First-call COMSPEC guest string and host command state. | Deferred with startup envelope. |
| 03 | `cmdSaveWorld`, `cmdmisc.c` | Compile-disabled `CHECK_IT_LATER` body. | Original no-op candidate. |
| 04 | `cmdGetCurrentDir`, `cmdmisc.c` | Guest output buffer plus per-drive current-directory environment and host drive query. | Deferred; requires contained namespace CWD ABI. |
| 05 | `cmdSetInfo`, `cmdmisc.c` | Installs guest `SCSINFO`/DOSDATA pointers into host globals. | Deferred; no raw guest-pointer retention in adapter. |
| 06 | `cmdGetStdHandle`, `cmdredir.c` | Inherited Win32 handles, pipe threads/temp files and 32-bit handle return. | Deferred; host handles may not cross the ABI. |
| 07 | `cmdCheckBinary`, `cmdexec.c` | Non-DOS executable classification and process launch preparation. | Deferred with execution component. |
| 08 | `cmdExec`, `cmdexec.c` | Native process execution, environment and NT console/process state. | Explicitly unavailable in first CLI DOS profile. |
| 09 | `cmdInitConsole`, `cmdmisc.c` | Video VDD/event-thread activation. | Deferred; no VDD/GUI in adapter. |
| 10 | `cmdExecComspec32`, `cmdexec.c` | 32-bit COMSPEC process execution. | Explicitly unavailable in first CLI DOS profile. |
| 11 | `cmdReturnExitCode`, `cmdexec.c` | Return from child execution, redirection and CWD synchronization. | Deferred with execution/redirection component. |
| 12 | `cmdGetConfigSys`, `cmdconf.c` | CONFIG materialization into temp host file and guest path output. | Source-derived CLI capability candidate after bounded guest-write and namespace-materialization admission. |
| 13 | `cmdGetAutoexecBat`, `cmdconf.c` | AUTOEXEC materialization into temp host file and guest path output. | Same paired configuration component as 12; no one-off provider. |
| 14 | `cmdGetKbdLayout`, `cmdkeyb.c` | Keyboard-layout DLL/VDD and guest structures. | Deferred; keyboard device/VDD is outside current profile. |
| 15 | `cmdGetInitEnvironment`, `cmdenv.c` | Initial environment transformation and guest buffer output. | Deferred pending bounded environment snapshot/write ABI. |
| 16 | `cmdGetStartInfo`, `cmdmisc.c` | AL reflects historical `DosSessionId`. | Source-derived CLI session-policy candidate; exact bounded contract recorded, not yet migrated. |
| 17 | `SVC_CMDLASTSVC` | Dispatcher boundary. | Not callable. |

## Interpretation

`CmdDispatch` is a simple service table but every nontrivial handler includes
`softpc.h` and many use raw `GetVDMAddr` pointers. It must remain reference
structure only. The adapter's future COMMAND plane preserves this table order
and component ownership through copied ABI records; it does not execute
`cmddisp.c` or retain `CMDINFO`/Win32 handle pointers.

The existing isolated adapter COMMAND endpoint evidence (boot files, COMSPEC,
current directory, get-next, keyboard layout and set-info) is not provider
admission. In particular, services 12 and 13 are a paired config-material
component, while service 01 is the whole launch protocol—not a convenient
single-command callback.

The paired boot-file replacement itself is reconciled in
`t96-s4-command-boot-file-reconciliation-001.md`: retain its contained
namespace semantics, but defer legacy runtime-chain migration to S8.

Confidence is high for table/handler/module mapping and host-dependency
classification, all directly observed in the pinned source. CLI replacement
semantics are intentionally only candidates until their copied guest-memory
and profile contracts are separately admitted.
