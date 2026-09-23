# T422 S1 WOW32 autonomous-binding inventory

## Question

How much project-authored code is presently required to compose `WOW32.DLL`,
what does each file actually own, and which files are candidate source-first
recovery or removal work rather than evidence of a completed WOW32 runtime?

## Inputs and procedure

This inventory reads the current `src/wow32-dll/sources` manifest and every
selected `src/wow32-dll/source/*.c` file.  Line counts are physical current
source lines; they do not count generated build files, headers, or the 77
original `src/mvdm/wow32` bodies.  The original DLL link inventory is checked
by `tests/observation/verify-wow32-s1-formal-graph.ps1`.

The result is deliberately an ownership inventory, not a functional-pass
claim.  A file can be a necessary standalone boundary and still be incomplete
until its original caller, production wiring, guest route, failure and
teardown contracts have passed in its receiving S packet.

## Observation

The selected provider currently contains:

| Composition class | Files | Lines | Meaning |
| --- | ---: | ---: | --- |
| Original selected WOW32 bodies | 77 | build-selected original bodies | `src/mvdm/wow32`; original algorithms and dispatch remain here. |
| Imported original non-MVDM USER slices | 16 source files / 19 link objects | 4,674 | `src/opennt-host/windows/core/ntuser/{client,kernel,rtl}`; repeated `client.c` and `class.c` objects are deliberately compiled under distinct original subsets. |
| Target-local standalone bindings | 21 | 2,977 | `src/wow32-dll/source`; this is the audited autonomous/boundary surface. |
| Worker-local WOW runtime binding | 2 | 180 | `src/ntvdm-exe/wow`; one worker TLS/session domain, not a DLL-owned second runtime. |

The 21 target-local files total 116,476 bytes.  They divide into finite
responsibilities as follows:

| Binding file | Lines | Current responsibility | Original owner / receiving closure |
| --- | ---: | --- | --- |
| `ntuser_rtl_compat.c` | 23 | Allocation and ANSI-code-page carrier for imported `chartran.c`. | Original `ntuser/rtl/chartran.c`; S6 conversion/ownership audit. |
| `wow_bitmap_bindings.c` | 65 | Synchronous DC/capability scope for original bitmap conversion. | Original WOW bitmap/resource callers; S3/S4. |
| `wow_callback_frame_lease.c` | 73 | Checked temporary guest-frame access for `CallBack16`. | Original `wcall16.c`; S2 callback/reentry/teardown. |
| `wow_class_client_bindings.c` | 101 | Scope and native registration handoff around recovered original class client. | Original `client.c` class routines; S2. |
| `wow_class_words_binding.c` | 244 | Native property-backed counterpart for original `CLS.adwWOW` words. | Original class/cleanup owners; S2. |
| `wow_cleanup_bindings.c` | 74 | Derived native tree used only while original cleanup traverses enrolled windows. | Original `cleanup.c`; S2 teardown. |
| `wow_dialog_creation_binding.c` | 242 | Public USER dialog creation and callback scope. | Original dialog/WOW registration owner; S2. |
| `wow_private_user_compat.c` | 216 | Finite public USER/GDI replacements for named private calls (paint, menu/resource and related queries). | Original individual client/kernel owners; split S2/S4/S7 by entry. |
| `wow_public_user_facade.c` | 230 | Public USER/GDI forms for named WOW32 calls such as dialog/font/DC operations. | Original WOW32 callers; S3/S4/S7 by operation. |
| `wow_sim32_pointer_compat.c` | 24 | CPU40's original no-op `Sim32FlushVDMPointer` form. | Original CCPU40 macro contract; retain only if current formal x86 rebuild proves it is still required. |
| `wow_task_event_binding.c` | 39 | Retained native event handle replacing an NT kernel object reference. | Original task registration; S2 task lifecycle. |
| `wow_task_profile_binding.c` | 28 | Read-only PMAP_COMPAT key entry for original profile reader. | Original `profile.c`; S2 init and S7 shared registry audit. |
| `wow_user_borrow_scope.c` | 116 | Explicit borrow lifetime for private WND/CLS aliases during a thunk. | Original USER lock lifetime; S2. |
| `wow_user_message_bridge.c` | 79 | Public USER queue wake/receive binding. | Original `taskman.c`/`queue.c`; S2 scheduler/message closure. |
| `wow_user_object_bindings.c` | 42 | Native-window enrollment and attempted guest HANDLEENTRY publication. | Original `handtabl.c`; explicitly incomplete S2 `USER-HANDLE-01`. |
| `wow_user_private_access.c` | 87 | Private WW/WC access over scoped native bindings. | Original private class/window access; S2. |
| `wow_user_registration_bridge.c` | 197 | Local 21-input/20-output USER registration transport. | Original `wow32.c::W32Init`; S2/S4/S5/S6/S7 slot owners. |
| `wow_user_task_lifecycle.c` | 504 | Task, queue, callback and cleanup carrier around recovered original task owners. | Original `taskman.c`, `queue.c`, `cleanup.c`; S2. |
| `wow_user_thunk_scope.c` | 75 | Enter/leave scope for original WOW thunk dispatch. | Original `W32Dispatch`; S2 reentry. |
| `wow_window_creation_binding.c` | 225 | Public window creation plus CBT/callback metadata scope. | Original create/class flow; S2. |
| `wow_window_words_binding.c` | 293 | Native property storage and lifetime for original WW words. | Original WND/WW consumers; S2. |

## Immediate conclusions

1. The 2,977 lines are not eligible for a bulk move into `mvdm` or
   `opennt-host`: no file is an original OpenNT mirror file.  Moving them
   would disguise autonomous implementation as source recovery and violate
   canonical-mirror policy.
2. Nor may they be treated as accepted merely because they link.  The known
   `wow_user_object_bindings.c` path explicitly fails active guest
   HANDLEENTRY publication, and the registration bridge deliberately exposes
   `pfnGetFullUserHandle` as `ERROR_CALL_NOT_IMPLEMENTED`.
3. The only present, high-confidence direct deletion candidate is
   `wow_sim32_pointer_compat.c` **if and only if** a fresh x86 provider link
   proves that the original CPU40 macro form is visible at every selected
   caller without a concrete DLL symbol.  It is not removed on a source-name
   guess.
4. S2 is the primary reduction gate: the class/window/object/borrow/cleanup/
   lifecycle/registration set is one coupled guest identity and callback
   contract.  It must first recover usable original-layout desktop/WND/
   HANDLEENTRY publication, then delete only bindings demonstrably superseded
   by an admitted original owner.  S3--S7 own the narrower bitmap, resource,
   profile and peripheral facade rows.

## A03 disposition

The earlier per-operation gap is now closed by the
[binding removal matrix](m0-t422-s1-binding-removal-matrix.md#privatepublic-user-facade-operation-audit): it enumerates every operation in
`wow_private_user_compat.c` and `wow_public_user_facade.c`, names its original
caller/owner, finite binding or source-recovery disposition, receiver and
deletion criterion.  The other 19 files each retain the single responsibility
listed above and are likewise assigned in that matrix.  This is a complete S1
design disposition, not a claim that the bindings' runtime routes have passed.
The required desktop producer/update/withdrawal and guest callback tests are
S2's already named implementation obligations; no adapter is deemed necessary
solely from this inventory.

## Reproduction

```powershell
$files = Get-ChildItem src/wow32-dll/source -Filter *.c -File
$files.Count
($files | ForEach-Object { (Get-Content $_.FullName).Count } |
    Measure-Object -Sum).Sum
& .\tests\observation\verify-wow32-s1-formal-graph.ps1 `
    -RepositoryRoot (Get-Location).Path `
    -BuildRoot 'build/M0-T421/S4/wow32-ntvdd-r1'
```

Expected counts are 21 files, 2,977 lines, and the static
`WOW32_S1_FORMAL_GRAPH_OK` marker.  These checks do not execute WOW16.
