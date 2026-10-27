# T473 S9 — generic public-USER wrapper identity audit

## T-wide DOS16/WOW32 reconciliation started

The owner broadened T473 acceptance to the read-only DOS16 and WOW32 audit
inventories and requires per-file full/partial/excluded dispositions. The
current source-and-build inventory was regenerated as
`build/t473-s9/whole-audit-r1/mvdm-union.csv` (SHA-256
`5C086CE961D7CFC3792D10CAA6949A06C4F1EE1C896F99521E6FC0C7E642BCB0`).
`tools/audit/ReconcileMvdmAuditInventories.py` joined it to both audit source
catalogs in `build/t473-s9/cross-audit-r3`. The 4,684 union paths contain
3,520 paths referenced by the DOS catalog and 3,671 by the WOW catalog.
Exactly 205 current mirror paths differ in raw bytes from both available
original identities and are marked partial under the owner's reporting rule;
4,479 rows remain pending, not silently fully restored or excluded. The WOW
audit also contains 42,963 entries outside this MVDM path union. They are
retained in a separate external-rows ledger for relevance and owner review,
not assumed to be either required or irrelevant.

The initial file ledger SHA-256 is
`3A244BDC5ED89951CED9C88ABEF81E0D9311F47AFB1E1557656AD5746A184040`;
the external-row ledger is
`949D696E96F088A69EA437A41F15A09D50C8218BA29983A15A7B31E25AF95623`.
For 204 partial text files it records a reproducible matching-original-lines
heuristic; this is textual similarity only, not a restored-code percentage.
The requested restored-code estimates remain pending semantic, selection and
runtime review. The extra non-text/build-recipe difference has no line estimate.
These are initial inventory results, not completion or runtime evidence.

In `cross-audit-r5`, 35 of those pending rows are now explicitly classified as
excluded from the product: 33 unmirrored `v86/monitor` and `v86/scaffold`
inputs and two retained `v86/util` inputs. The existing source policy rejects
the retired CPU_30_STYLE kernel-VDM executor while selecting original CCPU40;
the product-link inventory also shows these exact rows are not selected. This
is an authority-backed role exclusion, not a claim that their functions were
restored by another implementation. The remaining counts are 205 partial and
4,444 pending. The revised per-file ledger SHA-256 is
`F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`.

## First source result

S8 deliberately did not change the generic wrappers in
`wow_user_task_lifecycle.c`: their contracts mix null, pseudo, foreign and
multiple HWND parameters.  The first S9 source sweep proves that a bounded
subcohort nevertheless needs separate review.  Original `wuwind.c` calls
`MoveWindow`, `SetActiveWindow`, `SetParent`, `SetWindowPos` and `ShowWindow`
with `HWND32`/`HWNDIA32`; original `wuser.c` calls `SetFocus` and `SetCapture`
with `HWND32`.  `walias.h` defines both forms as signed 16-bit carriers.

The current `WOW_NATIVE_CALL` macro forwards its `HWND` parameters unchanged
to modern USER.  It therefore cannot distinguish such a carrier from a full
host HWND.  This is the same numerical-width hazard resolved for menu/dialog
operations in S8, but must not be fixed by converting the entire macro family.

## Parameter contract matrix

| Original operation | Required target treatment | Other HWND/result treatment | Reason not to use a blanket resolver |
| --- | --- | --- | --- |
| `MoveWindow`, `ShowWindow`, `SetActiveWindow`, `SetFocus`, `SetCapture` | non-null current-worker owned target | Returned prior HWND is converted only by the existing original return thunk | Calls can synchronously change focus/activation/capture; result lifetime is not the input lifetime. |
| `SetParent` | non-null owned child | parent may be null; non-null parent requires a separately proved owned/public rule; returned old parent has independent conversion | Child and parent have different null and cross-owner meanings. |
| `SetWindowPos` | non-null owned target | `hwndInsertAfter` preserves native pseudo handles (`TOP`, `BOTTOM`, `TOPMOST`, `NOTOPMOST`) and only otherwise follows a proven mapping rule | A pseudo handle is an API instruction, not a guest object identity. |
| Message/default/MDI wrappers | retain their original `FULLHWND32` conversion sites | target can be null, broadcast, foreign or paired with a client HWND; message payload may encode an HWND | Their receiver and return contracts are not reducible to this subcohort. |

## Required proof before repair

1. Trace every selected call into the wrapper and classify each `HWND`
   parameter as owned, null-permitted, pseudo, external or returned.
2. Prove that native full handles already held by internal adapters remain
   accepted, while a fabricated wider handle and a foreign clipboard
   observation are rejected from the owned route.
3. Verify `SetParent` and `SetWindowPos` independently, including a null
   parent and the four documented Z-order sentinel values.
4. Retain original algorithms, `GetLastError`, native-call unwind and
   published-return behavior.  Do not change guest source or reconstruct
   private USER state.

## Repair and evidence

The selected narrow-carrier wrappers now call the existing
`wow_user_window_native_handle` projection before modern USER.  That function
maps an original narrow carrier through the published object table and leaves
an already-full host HWND alone, which is required by retained native-only
adapter callers.  It does not create a second handle table.

`SetParent` separately maps its nullable parent; `SetWindowPos` preserves the
four Z-order pseudo handles before conversion. The selected generic wrapper
cohort covers activation/focus/capture, destruction, dialog end, placement,
icon/pop-up, scroll/bar, paint/caret, invalidation/redraw, class and window
text operations all resolve their direct owned carrier before calling modern
USER. Other original `HWND32` consumers outside this wrapper family require
their own per-entry trace; this record does not assert they are all mapped.
`SetWindowLongA(GWL_HWNDPARENT)` also resolves its nullable value
separately and still publishes the prior parent result through the original
return owner.  `DestroyWindow` resolves before destruction but uses the
existing no-publication completion path after native USER has consumed its
identity.

The earlier T463 `EnableWindow` binding accepted only a short carrier. The
same binding is also the `NtUserEnableWindow` leaf for recovered
`dlgmgr.c` and `combo.c`, which pass `HWq(...)` or an owner HWND from native
window state. S9 uses the existing original full-handle projection for short
inputs and preserves already-full native inputs; it does not widen the guest's
16-bit argument. The T463 modal-owner path and the recovered control clients
remain separate acceptance requirements, rather than assuming a low-word-only
test proves both.

The x86 lifecycle fixture passes the affected short-carrier group: movement,
visibility, every Z-order sentinel, capture/release, owned pop-ups, child
scrolling, frame and control scroll bars, paint/erase, caret, icon state,
class style, ANSI window text, and null-window `HideCaret`/`ShowCaret` as
required by original `wucaret.c`. It also retains menu, callback,
retirement and full-HWND regressions. The focused run after the scroll and
update-region wiring is `build/t473-s9/generic-wrapper-r25` and ends with
`WOW_FIXTURE_OK case=menu-resource`.

An object-relocation audit also checked the real entrypoints, not just direct
fixture calls. `wuwind.obj` already calls the mapped window wrappers through
`precomp.h`; `wuser.obj` still imported native `BeginPaint`, `EndPaint`,
`InvalidateRect` and `DragDetect`, and `wucaret.obj` still imported native
caret operations. The existing private redirect header now connects those
original thunk calls to the same wrapper owner. After rebuilding, the object
relocations name `wow_native_*` for each selected entry. The original thunk
bodies remain unchanged. The same source and relocation audit subsequently
identified direct narrow carriers in `GetScrollPos`, `GetScrollRange`,
`GetUpdateRect`, `GetUpdateRgn`, `InvalidateRgn`, `ValidateRect` and
`ValidateRgn`; these now route through the existing mapping/return owner as
well, with `wuser.obj` relocations confirming the connection. The focused
fixture asserts actual scroll values and update-region results. Other direct
native APIs still require separate source/contract audit; no blanket redirect
is claimed. `wucaret.obj` retains direct caret-position and blink-time calls
that do not take an HWND.

The earlier `wuser.obj`/`wuwind.obj` relocation inventory retained
direct public imports including `GetDC`, `GetWindowDC`, `ExcludeUpdateRgn`,
`GetPropA`, `SetPropA`, `RemovePropA`, `DeferWindowPos`, `FlashWindow`,
`GetClassNameA`, `GetWindowRect`, `GetParent`, and window-word accessors.
Their original caller operands, null and handle-return contracts and existing
adapter ownership must be traced before any further mapping. In particular,
DC acquisition/release and deferred-position multi-window identity must be
reviewed as paired lifetimes; object imports alone do not prove a defect.
The source trace establishes that `WU32DeferWindowPos` passes distinct
`HWND32` and `HWNDIA32` operands to native `DeferWindowPos`, while the already
mapped `EndDeferWindowPos` receives only the HDWP. The insert-after operand's
pseudo values and `SWP_NOZORDER` ignored-operand semantics require explicit
handling. Both operands now use the existing identity conversion. With
`SWP_NOZORDER`, immediate and deferred position calls pass `HWND_TOP` as an
ignored placeholder: passing an invalid caller value through made the host
deferred call fail with error 1400. The `generic-wrapper-r28` real-CCPU
fixture passes with a short target, invalid ignored operand and actual final
geometry; the preceding failed `r27` remains evidence. `WU32GetDC` and
`WU32GetWindowDC`, and `WU32GetDCEx` call original `StoreDC`, whose cache
saves `HWND32(hwnd16)` for later `ReleaseDC`. `WU32ReleaseDC` deliberately
defers release; `ReleaseCachedDCs` executes it on later acquisition, dispatch
or message processing, and `FreeCachedDCs` handles task exit. Any adaptation
must cover all three acquisition paths, preserve that original cache policy,
and keep acquisition and deferred-release identities coherent through handle
reuse, not resolve a recycled low word at release time. Candidate
`MVDM-HOST-DIV-455` carries the acquired HWND into the original cache;
its failure and lifecycle verification remains in progress.

The subsequent source and relocation trace identifies direct narrow HWND
operands for original `WU32GetProp`, `WU32SetProp`, `WU32RemoveProp`,
`WU32ExcludeUpdateRgn`, `WU32FlashWindow`, and the `wusercli.c` class-name,
client/window rectangle and parent queries. The original property payload is
untyped and must not be converted as a handle; the update-region HDC remains
the existing GDI alias. The candidate routes each HWND through the existing
public-window mapping and preserves the native result, including a separately
published returned parent. The cached x86 provider build succeeds, and the
actual original `wuser.obj`, `wusercli.obj`, and `wuwind.obj` relocations now
resolve to the corresponding `wow_native_*` entries. The
`property-geometry-r1` CCPU-host fixture passes short-HWND parent, client and
window rectangle, class name, untyped property round-trip, flashing and
ExcludeUpdateRgn. `property-group-r1` passes all 19 supplemental CCPU/native
cases, including atom properties and invalid-window rectangle preservation.
The identical r9 package passes `property-product-r1` and its independent
Win16 `property-guest-r1`: real DC acquisition/release, property round-trip,
class name and client/window geometry. The Product WOW observations are
startup witnesses, not gameplay acceptance. Control remains in progress.

OpenNT `windows/core/ntuser/client/client.c::GetDCEx` calls
`NtUserGetDCEx`, whose `kernel/ntstubs.c` entry calls `_GetDCEx` in
`kernel/dc.c`. The original `_GetDC` and `_GetWindowDC` also enter that same
kernel operation. Its body contains DCHook reentry comments and a separate
client-cleanup-on-window-destruction note, but no direct guest window-message
call was established by this trace. These observations do not prove that a
modern host can never call back or that all helper functions are non-reentrant;
the candidate retains the existing native call/return boundary.

The DC follow-up trace identifies exactly three acquisition callers:
`WU32GetDC`, `WU32GetWindowDC`, and `WU32GetDCEx`. `WU32ReleaseDC` only marks
the original cache entry pending. Original `WU32GetDC` and
`WU32GetWindowDC` flush prior matching entries; `wumsg.c` flushes pending
entries for the task, `wmdisp32.c` flushes by task and window, and `wkman.c`
calls `FreeCachedDCs` on exit. `USER32` sign-extends the 16-bit HWND, while
`GDI32` and `GDI16` use the existing separate GDI handle mapping. Therefore
the cache must retain the exact full native HWND used by acquisition, not
resolve its 16-bit value during deferred release, and the HRGN argument and
native GetDCEx ownership rules must remain on the original path. An adapter
that independently resolves the HWND again after the native call cannot prove
the same generation if a callback destroys and reuses the slot. Passing the
captured HWND through the original acquisition-to-StoreDC path is the finite
candidate. The three callers and `StoreDC` now carry the explicit identity,
with the mirror exception registered; the candidate is not yet accepted.
The cached x86 provider build succeeded. `dc-candidate-r1` exercised the three
native acquisition wrappers and null screen DC; `dc-candidate-r4` additionally
executed the actual original cache's repeated ReleaseDC marking, deferred
flush and task-exit flush, with GDI alias retirement. The first cache fixture
attempt `dc-candidate-r2` crashed because it omitted the original small-heap
initialization; diagnostic `dc-diagnostic-r1` localized the fault to StoreDC.
`dc-candidate-r3` then failed to link because the fixture declared the heap
initializer with the wrong calling convention; r4 uses its actual ABI. These
fixture failures are not claimed as product failures or passes. Acquisition
callbacks, destroyed-window reuse and the three original guest thunk paths are
not yet proved by these fixtures; the earlier r7 Product/Control gates predate
this candidate and cannot qualify it for publication.
After restricting the new DC callouts to the three original thunks instead of
globally redirecting GetDC in unrelated original consumers, the provider was
rebuilt and `dc-group-r1` passed all 19 supplemental native/CCPU cases. Its
window-borrow case includes the original cache checks above. This remains a
supplemental test, not proof of real Win16 thunk execution or reused HWNDs.
`dc-reuse-r1` subsequently invokes the compiled original `WU32GetWindowDC`
and `WU32GetDCEx` thunks with source-shaped VDM frames and exercises their
actual native DC and original cache release. It also holds a cached DC across
an observed native HWND slot reuse with a different generation, verifies the
cache retains the old full HWND, and confirms the replacement window remains
alive after the old entry is retired. This is a CCPU-host fixture, not a
Win16 application; acquisition-time callback/reentry and real guest delivery
still need independent evidence.
`dc-thunk-r2` adds the compiled original `WU32GetDC` body using a disposable
zeroed task record in the project-owned WOW32Reserved TLS carrier, restoring
the prior TLS value after the call. All three original DC thunk bodies now
execute in this source-shaped CCPU-host fixture and release through the
original cache. `dc-compat-r1` additionally exercises both the original
`WOWCF_UNIQUEHDCHWND` handle-bit rule and the null-window
`WOWCFEX_FIXDCFONT4MENUSIZE` task-state rule. The fixture is not an actual
guest application and does not prove acquisition-time callback/reentry.
`dc-reentry-r2` adds actual `GetDC` acquisition/release inside both synchronous
`WM_ENABLE` callbacks produced by `wow_native_EnableWindow`, and executes the
original `WU32GetDCEx` with a GDI-aliased `DCX_INTERSECTRGN` region. The two
callback entries and region-backed acquisition pass. This proves a nested DC
call made from a native callback, not that GetDC itself generated a guest
callback, nor every region failure or reentrant destruction case.
`dc-guest-r1` runs the independently authored `win16_dc_probe.asm` against
the unchanged guest in package r8: a real Win16 task creates a window and
successfully calls GetDC, GetWindowDC, GetDCEx and screen GetDC, followed by
their ReleaseDC calls. This proves those normal guest paths, not acquisition
reentry or every compatibility flag. The exact package also passed the
`dc-product-r1` Product and `dc-control-r1` Control gates; their WOW window
observations are startup observations, not gameplay acceptance.

The later property/geometry batch was compiled and passed the 19-case
`property-group-r1` supplemental matrix. `property-product-r1` and
`property-control-r1` passed on r9, but delivery review found that package
used the older released NTVDM rather than the newer stable Ninja worker.
`package-r10` replaces that worker with the exact cached-build image and
retains the other nine independently matched images. `property-guest-r2`,
`final-product-r1` and `final-control-r1` pass on r10. The Product observations
also match the S8 and preceding r9 frontiers in
`final-product-r1/additional-frontier-comparison.json`. Independent r10
application probes pass Minesweeper flag/reset/open/exit, Solitaire
deal/draw/exit and Options modal/close, and WRITE close/restart/close.
`enable-owner-r1` separately passes short-carrier disable and full-native-HWND
enable against the same compiled provider. The r10 Product frontier also
matches both the S8 package and r9 on all three applications; this does not
replace their individual functional probes.
`final-sol-options-r1` failed to compile the x86 debugger probe with an x64
compiler; `final-sol-options-r2` uses the correct x86 driver and passes.
These results do not claim every historical application behavior or the new
T-wide DOS16/WOW32 source reconciliation is complete. No r10 publication is
claimed there. The r10 ten-image set was subsequently synchronized to both
`assets/release` and `O:\winnt\system32`, with each destination checked
against the tested manifest. The previous sets are retained separately in
`build/t473-s9/delivery-p1-backup`; T-wide audit remains open.

The same private-desktop fixture cannot be the activation/focus mapping
witness: `SetActiveWindow`/`SetFocus` retain real host thread input attachment
past the fixture's expected desktop close edge even when reset to null.  This
is a host test-container limitation, not evidence that the carrier conversion
failed.  A separate non-desktop-owning witness is required before treating
those two wrappers as runtime-accepted.  Guest media is unchanged.

The complete cached component build passed with ten images and 1,519
intermediates; its second build was idle.  The first product driver stalled
before physical staging or starting a case and was stopped at the owner's
instruction. The published baseline remains package r1. Candidate package r3
predates the later lifecycle fix and must not be published as its build.

The full private-desktop group initially exposed an outdated `window-borrow`
assertion, corrected to require the fixture's `owned_handle=1` result, and an
intermittent `dialog-lifecycle` disposal failure. The actual failed assertion
was process-owner disposal, after original task cleanup and unbind succeeded.
Twelve identical diagnostic fixture runs produced six failures: a retired
window with no borrowers was still linked from the copied desktop child and a
dead window's owner field; its CLS could therefore not be reclaimed. The
diagnostic showed this is intermittent in the same launch form, so earlier
speculation that the nested PowerShell launch caused it was unsupported.

A test-only graph refresh before unbind passed twelve runs. The production
fix instead uses the existing graph refresh once after the complete terminal
native-destruction pass, before collection and while the task remains bound.
It preserves the no-publication destruction return, actual native identities,
and strict failure propagation; no new object registry is added. Twelve runs
of the actual repaired fixture passed in `dialog-diagnostics-r4`. The full
19-case group `generic-wrapper-r21` passed with zero failures. That group
predates the subsequently verified nullable-caret adjustment and thunk wiring;
the final focused fixture includes both. Package r4 predates the thunk wiring
and must not be published as the final build. Its `product-r2` Product gate reached the WINMINE observer but
failed: the process exited `0xffffffff` before a window snapshot and the
snapshot returned 6. `product-baseline-r2` used all ten pinned published r1
images through the same gate and produced the same exit and snapshot result.
This comparison attributes the observed gate failure to the shared route,
but is not a passing guest test or proof that r4 has no other regression.
Neither run reached the remaining Product cases; no candidate is published.
The independent `control-r2` run used the candidate plus separately staged
fixture executables: RPC `client` passed, while `bootstrap` returned 1 and
reported `FAIL bootstrap=87`. Its fixture/service build provenance and the
cause of this failure remain unproved; this is a non-pass, not evidence of
either a WOW32 regression or a successful complete Control suite.
The complete matrix after the scroll/update-region wiring passed all 19 cases
in `generic-wrapper-r26`; its focused behavioral witness is
`generic-wrapper-r25`. Package r6 predates the deferred-position repair and is
not the current build; neither earlier Product non-pass exercises the current
build. The complete supplemental real-CCPU/native matrix was rerun after the
deferred-position repair in `generic-wrapper-r29`: all 19 cases passed. This
is not a substitute for the real guest Product gate. No guest media changed.
The subsequent `generic-wrapper-r30` window-borrow fixture additionally
passes a deferred Z-order operation with both the target and a real sibling
supplied as 16-bit carriers, checking the actual resulting sibling order.

After the owner restored the desktop, `product-desktop-r1` passed the Product
gate using package r6: the three retained WOW window-start observations and
Console17/Window17 completed. The WOW observations are not gameplay tests.
This result resolves the earlier r6-route desktop non-pass but does not cover
the later deferred-position repair; the final candidate still requires its
own coherent-package verification.

`package-r7` contains the r29-verified WOW32 binary. On the restored desktop,
`product-desktop-r2` passed the Product gate, and `control-desktop-r3` passed
the Control gate with that same ten-image package. The Control run uses the
current x64 monitor fixture and the hash-matching service Ninja directory for
its generated RPC headers. An earlier `control-desktop-r1` passed bootstrap
but failed monitor RPC with revision mismatch using the older x86 fixture;
its temporary stage cleanup also encountered a locked DLL. The subsequent
`control-desktop-r2` passed RPC and GUI but could not compile a version
negative because its flat service cache did not contain `service.h`. Those
are recorded as failed runs, not product passes. The completed r3 run does
not establish gameplay acceptance or settle the outstanding DC cache audit.


### Visible focus fixture identity prerequisite

The real-desktop `focus-visible-r6` attempt failed before proving the product
path: its directly created host windows had not passed through the worker's
creation identity publication. Its parent was a full HWND while
the low-word lookup returned only the truncated carrier, and native return
publication rejected that invalid target. The fixture now explicitly publishes
the two direct-host-created windows before showing them and asserts that both
short carriers resolve to their full HWNDs. This is test setup, not a runtime
fix. The `focus-fixture-r10` private-desktop window-borrow group passes, and
`focus-visible-r7` on the actual desktop resolves the parent, then activates
it, focuses the child and clears focus with zero fixture errors. This does not
prove every cross-process foreground or reentrant focus scenario.
