# T425 S10 repeated shared-WOW WINMINE investigation

## Question and scope

Owner closes S9 at 817fe636b and reports that a second `run16 winmine` does
not create another WINMINE while the first task and WOW worker remain alive.
Investigate whether this is a broken second submission or original guest
instance policy. This is a test/research delivery, not production repair or
general WOW capability closure. T425 remains open.

## Source findings

- Original `src/opennt-host/base/win32/server/srvvdm.c` BaseSrvCheckWOW,
  lines 805-884, allocates another WOW record, copies the command, creates its
  independent parent wait and notifies authenticated WOWEXEC with
  WM_WOWEXECSTARTAPP. Shared worker residency does not suppress a request.
- `src/run16-exe/main.c` lines 319-335 handles VDM_PRESENT_AND_READY and uses
  wait_wow_startup for the ordinary Win16 startup-only contract. Explicit
  --wait instead observes that task's original completion.
- Read-only original reference:
  `O:/repos.external/OpenNT-4.5/nt/private/windows/ep/winmine/winmine.c`,
  MMain WIN16 branch lines 124-133. A nonzero hPrevInstance leads to FindWindow,
  GetLastActivePopup, BringWindowToTop, optional SC_RESTORE and return fFalse.
  It never reaches the new-window path in this branch. Source SHA256:
  83796E8EC74E2710B39D906EDD290F12FBB02A5D63F78CCFB7C7F5D6F533C1F7.
- This agrees with the retained
  [T423 shared-WOW investigation](m0-t423-s8-gui-launch-wait.md), which traced
  second InitTask, normal task-2 completion and source-defined single-instance
  activation. The current test already encodes this original contract.

General Win16 multi-task support does not guarantee multiple windows for an
application that intentionally chooses single-instance behavior. No guest
binary, original mirror or production adapter is changed to bypass this rule.

## Inputs and procedure

Use unchanged accepted x86 CCPU40 eight-file package
build/M0-T425/S9/r022/runtime. All eight SHA256 values match O:/winnt. Published
WINMINE.EXE SHA256:
A9D2AFBD1AA98E38F3679F3A1A5DC0463367DD47C9AB1792E5AB8BBDC3441C0C.
No profile/environment reduction or guest patch is introduced.

Reuse tests/observation/wow_shared_launch_test.c. Add only --async-second,
which makes its second invocation ordinary startup-only too. Existing
--async, --wait-first, shared worker/window identity, redundant WOWEXEC and
independent completion assertions remain intact. The probe owns a private
unswitched desktop and its process handles; it never interacts with the user's
desktop. Its WM_CLOSE addresses only the test-owned first window at cleanup.

Build in build/M0-T425/S10/r001 using the retained S9/r033/msvc-x86.cmd and:

```text
cl /nologo /TC /MT /W4 /D_CRT_SECURE_NO_WARNINGS
  tests/observation/wow_shared_launch_test.c
  /Fobuild/M0-T425/S10/r001/wow-shared-launch-test.obj
  /Febuild/M0-T425/S10/r001/wow-shared-launch-test.exe /link user32.lib
```

Compiler MSVC14.43, SDK22621, Win32/x86 /MT. Initial /WX attempt failed on an
existing signed/unsigned fault-code assertion (C4389); it is retained as a
failed build, not misreported as warning-clean. Existing W4 probe recipe
builds with that warning visible. No production build or code changed.

Map only Z: to the physical package, then invoke:

```text
wow-shared-launch-test.exe Z:\ <physical-r022-runtime> --async
wow-shared-launch-test.exe Z:\ <physical-r022-runtime> --async-second
wow-shared-launch-test.exe Z:\ <physical-r022-runtime> --wait-first
```

Run serially against the global BaseSrv endpoint. Each owns its broker, two
launchers and same worker; wrapper uses identity-checked package cleanup and
always subst Z: /d. Reports remain in S10/r001, not the runtime root.

## Actual observations

| Run | Results |
| --- | --- |
| shared-async.txt | First default launcher returns 0, worker25104 stays live. Second explicit-wait invocation returns 0 with the same first HWND and worker. Redundant WOWEXEC returns 0 without closing WINMINE. WOW-SHARED-LAUNCH-PASS. |
| async-second.txt | Both default startup-only launchers return 0. First HWND remains the only visible WINMINE in worker21520. Redundant WOWEXEC succeeds. WOW-SHARED-LAUNCH-PASS. |
| wait-first.txt | First explicit-wait launcher remains pending in worker24428 after second invocation returns 0; only closing its own first window completes it with 0. WOW-SHARED-LAUNCH-PASS. |

Final probe source SHA256:
29B565B184AAA902AF86302212A24DACA21E8441D507166102453BD410EBD84A.
Final probe EXE SHA256:
94AE7A8188B174A5AF07FA0CC2FFA7CA6536F4D5F36CD0C3F0DA067B9953C2EF.
shared-async used the preceding unmodified test; the other two use the final
test. Product inputs are identical throughout. Z: is released and exact owned
package cleanup passed. Full product regression is not rerun for this
test-only addition; S9/r035-r036 retains those unchanged-product passes.

## Interpretation and limitations

The reported absence of a second window is consistent with original Win16
WINMINE's deliberate single-instance behavior, not a demonstrated worker
reentry regression. The tested second request does execute/complete successfully,
and cannot complete or close the first task accidentally. There is no basis
for forcing another worker or patching the guest to create a second window.

The source requests activation of the existing window; the private-desktop
test does not establish physical foreground activation on the owner's desktop.
If the actual second launcher hangs, reports nonzero, or fails to activate the
first visible window, that is a different symptom requiring its own retained
state. Concurrent different-image WOW applications and all possible Win16
application instance policies are not proved by these WINMINE cases.
