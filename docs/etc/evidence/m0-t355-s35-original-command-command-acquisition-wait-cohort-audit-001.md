# M0 T355 S35 — Original COMMAND command-acquisition/wait cohort audit

## Question

What is the complete original owner chain behind COMMAND service `54:01`, and
which part of that chain can be recovered in the selected one-session CPU40
product without fabricating a command or recreating CSRSS?

## Inputs

- Original selected MVDM COMMAND sources:
  `src/mvdm-host/dos/command/{cmddisp.c,cmdmisc.c,cmdexec.c,cmddata.c}`.
- Byte-identical non-MVDM OpenNT evidence mirrors:
  `src/opennt-host/base/win32/{client/vdm.c,server/srvvdm.c}`.
- Current source-shaped Base VDM binding:
  `src/adapter-mvdm-host-out/basesrv/{include/base_vdm_local.h,source/base_vdm_client.c,source/base_vdm_local.c,source/base_vdm_broker.c}`.
- Current app composition input:
  `src/app/{entry.c,launch_declaration.c,launch_declaration.h}`.
- Local contract fixture:
  `tests/adapter-mvdm-host-out/basesrv/base_vdm_local_fixture.c`.

## Original owner ledger

| Edge | Original source contract | Current disposition |
| --- | --- | --- |
| `54:01 -> cmdGetNextCmd` | `cmddisp.c` selects the original `cmdGetNextCmd` table entry.  `cmdmisc.c` forms `VDMINFO`, preserves first/repeat-call state, requests a DOS binary and consumes the returned command, directory and optional environment records. | Original body remains selected, with only registered guest-location and host-resource mapping divergences. |
| `cmdGetNextCmd -> GetNextVDMCommand` | Original COMMAND calls the BaseClient spelling; it does not own a private command callback. | Same public spelling is supplied by `base_vdm_client.c`. |
| BaseClient capture/wait/retry | Original `base/win32/client/vdm.c` captures `VDMINFO`, invokes CSR `BasepGetNextVDMCommand`, waits on the returned DOS wait object, sets `ASKING_FOR_SECOND_TIME`, then retries without retaining the caller buffer. | `base_vdm_client.c` copies the request, waits only on a session-owned event, reconstructs the capture, and retries.  No guest pointer, caller pointer or native handle crosses the wait. |
| BaseSrv DOS dispatch | Original `base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand` distinguishes DOS console records, returns an empty success for a WOW request, performs capacity checks/copies, and publishes a wait object only for an empty DOS queue. | `base_vdm_local.c` preserves these reached DOS/WOW/capacity/copy/wait distinctions for one session.  It deliberately rejects PIF, separate-WOW, child and cross-process paths. |
| Normal command production | Original `BaseSrvUpdateVDMEntry` constructs DOS records, transfers command metadata and standard-handle ownership, then changes a record to `VDM_TO_TAKE_A_COMMAND` and signals a waiting VDM. | `base_vdm_broker.c` has only the copied record bridge.  `app/launch_declaration.c` currently creates one fixed `/C EXIT` record only when `--ordinary-child` is present.  It is an observation composition input, not a complete normal BaseSrv producer. |
| WOW notification | Original shared WOW uses BaseSrv's WOW queue and a `WM_WOWEXECSTARTAPP` notification rather than blocking a WOW worker in `GetNextVDMCommand`. | The local consumer preserves only empty-WOW nonblocking behavior.  WOW producer/notification remains outside this COMMAND cohort. |
| Re-entry accounting | Original `cmdExec32` increments/decrements the BaseSrv console re-entry count through `GetNextVDMCommand`. | `base_vdm_local_dispatch` preserves bounded increment/decrement accounting.  Child/parent execution remains a later COMMAND/DOS lifecycle owner. |

## Findings

1. The current implementation does **not** make `54:01` a leaf shim: the
   original COMMAND caller, BaseClient function name, request/capture order,
   DOS pending/wake/retry behavior, buffer-capacity outcomes, and re-entry
   counter all remain represented by their original owner boundaries.
2. The original BaseSrv server is not directly composable.  Its DOS records,
   duplicated wait handles, CSRSS process/thread data and shared WOW window
   notification are a private NT4 product shell.  Importing `srvvdm.c` as a
   runtime library would not recover the contract; it would import a missing
   CSR transport and server state.
3. A one-session public-API replacement is feasible only at the already
   selected adapter boundary: copied command record, explicit session binding,
   event-backed DOS wait, and source-shaped retry.  The existing implementation
   is a valid consumer-side recovery of that bounded contract.
4. The remaining local product gap is the **normal DOS command producer**.
   `app_launch_declaration_publish` is intentionally limited to the fixed
   observation declaration (`--ordinary-child`, `/C EXIT`).  It is neither an
   interactive COMMAND input loop nor an original BaseSrvUpdateVDMEntry
   replacement capable of receiving a declared normal product command.
5. No correction to `50:42`, DEM, the guest image, CCPU, BIOS, or a BOP result
   can solve this producer boundary.  Those owners must remain untouched.

## Selected successor

The next bounded recovery cohort is **normal DOS command declaration to the
existing Base VDM producer**.  It must:

- introduce one explicit app-owned launch declaration (not an ambient console
  read and not a fabricated COMMAND result);
- translate it to the existing copied `base_vdm_command` record with the
  original BaseClient command, application, current-directory and environment
  shape;
- preserve the existing DOS wait/retry/second-request handling and leave WOW,
  PIF, child/parent EXEC, standard-handle duplication and CSRSS broker paths
  unavailable;
- validate the producer through the existing local Base VDM contract fixture
  plus a focused product composition fixture before one later integrated
  observation.

## Confidence and limits

High confidence for the selected local COMMAND/BaseClient/BaseSrv source path:
the exact client and server source files are mirrored and the current adapter
documents each deliberate private-product divergence.  This record does not
claim interactive COMMAND execution, a CSRSS replacement, cross-process
broker completion, WOW wake notification, child lifecycle or a runtime guest
continuity result.

