# M0 T473 S2 — DOS, COMMAND and XMS source reconciliation

## Reconciled ownership

The selected DOS, DEM, COMMAND and XMS bodies remain the policy owners.  The
current product does not replace `COMMAND.COM` parsing, COMSPEC construction,
XMS descriptor interpretation, allocation policy or DOS return flow with a
host shell implementation.  The relevant project bodies exist because the
original worker passes durable `GetVDMAddr` aliases and native process inputs
across boundaries that are no longer valid in a session-owned CPU40 worker.

| Boundary | Original policy owner | Necessary current adaptation | Disposition |
| --- | --- | --- | --- |
| XMS move descriptor and forward-copy semantics | `xms.486/xmsblock.c::xmsMoveBlock` | A 12-byte descriptor is copied from its original `SS:BP` coordinate, then the original parser and forward-copy decision run against numeric CCPU addresses. | **Retain original body plus thin lease call.** The old XMS adapter parser has already been removed; `mvdm_xms_memory.c` is now only transfer/cancellation plumbing. |
| XMS/DOS guest addresses | selected XMS, DEM and COMMAND callers | `mvdm_guest_location` and session guest-memory leases replace durable process aliases with short numeric access leases. | **Retain.** The location contains only 16:16/numeric values; CPU40/SAS remains byte authority. |
| COMMAND SCS/DOSDATA scalars | selected COMMAND/DEM/SoftPC code | `mvdm_command_guest_state` records source guest positions and obtains a fresh lease for each scalar operation. | **Retain.** It cannot be a second DOSDATA: it stores no copied command data and its values expire with the worker session. |
| Native child command/environment/standard handles | selected COMMAND BOP/cmdExec path and `command_process_compat` | `mvdm_command_native_child` snapshots bounded inputs before asynchronous host-CUI handoff and retires them through the owning session. | **Retain.** It has no command parser or process-routing policy; original COMMAND selects the service and retains return semantics. |
| Product DOS/firmware roots | original DEM/SoftPC readers | session-configured immutable media root, OEM path conversion and the named ROM resource loader. | **Retain.** Original search/order/read behavior remains in the original callers; binding chooses only the admitted immutable resource. |

## Earlier duplication finding is closed by the current source

An older T407 audit correctly identified a then-existing duplicate XMS parser
and chunk-copy policy.  Current `xmsblock.c` is materially different: it
retains the original descriptor parsing and forward-copy contract, and calls
only `mvdm_softpc_guest_memory_copy_forward` for the unavailable raw-address
transport.  The current XMS adapter neither parses the frame nor picks copy
direction.  It therefore does not reproduce the historical duplication and
must not be deleted on the basis of that superseded finding.

The source also retains two defensive results that the original raw-pointer
path could not represent safely: reject a descriptor byte count that overflows
its word-to-byte conversion, and request cancellation when the bounded lease
cannot be acquired or copied.  These are failure transport rules, not a new
XMS success policy.  Removing them would permit the original caller to
continue after a session/memory failure with no valid native alias.

## Single-authority check

`COMMAND.COM` owns shell semantics; the selected original `cmdExec` route
chooses whether to construct `COMSPEC /c` and invokes the existing native
child service.  The child snapshot is the only cross-worker ownership object,
and session teardown is its sole retirement owner.  XMS owns move, allocator
and result semantics; CPU40 SAS owns data.  The lease layer owns only the
validity interval for native access.  No adapter maintains an alternate shell,
environment, XMS allocation map or guest byte store.

Existing focused contracts are the XMS mapped-memory fixture, COMMAND native
child/guest-state fixtures, and the established nested COMMAND/native-CUI
normal/return paths.  This review changes no source; those retained fixtures
are evidence of the adapter contracts, not a claim that all DOS runtime
packages are now complete.  S3 owns real end-to-end behavior revalidation.

## Result

No additional original file can replace these boundaries without restoring
invalid raw aliases or a second process model.  No current duplicate policy
remains in this owner group.  The next S2 review therefore moves to SoftPC
device boundaries, rather than reopening the fixed XMS parser issue.
