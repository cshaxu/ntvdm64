# T423 S36 visible Console title in NTCON Window

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and inputs

The owner closed S35 and inserted S36: replace the fixed `NTVDM` Window
caption with the title of the **user-visible Console to which NTCON is
attached**. This explicitly excludes NTW32's hidden Console and any Windows
Terminal tab-level override. The input baseline is delivered S35
`96a5e0bad` and its [evidence](m0-t423-s35-native-text-window.md).

## Procedure and observations

- NTCON's presentation thread previously created its Window controller with
  the literal `NTVDM`. The project-local Window API already had a copied,
  asynchronous title-control operation; no library, guest or cross-EXE wire
  change was required.
- NTCON now reads `GetConsoleTitleA` from its attached Console on presentation
  wake, caches a changed title in its controller and updates an existing Window
  through the local title control. A successful original DOS Console-title
  operation wakes that presentation thread. Display switches and worker
  handoffs already wake it. A failed title read during root loss is cosmetic
  and cannot prevent a worker handoff or normal frontend retirement.
- The existing fixed 128-byte Window title carrier bounds copied text;
  overlong Console titles are safely terminated at that bound. No task/image
  title is substituted. A title changed externally without any frontend event
  is observed on the next normal presentation wake, not by a new timer.
- `frontend-window-controller-test.exe` passed on an automatically created
  private desktop: same-HWND title update, empty title and capacity guard.
  `console-frontend-test.exe` passed in a private Console, including one
  notification on a successful DOS title call. Both tests avoid the user's
  active desktop.
- MSVC x86 linked `ntcon.exe` and both focused tests in
  `build/M0-T423/S36/candidate1/`. The copied S35 x86 graph was executed
  with the build-local sequential command runner because the ordinary Ninja
  runner stalled without compiler output in this environment. The final
  native-frontend object and `ntcon.exe` were relinked after the root-loss
  safety review.
- On the final staged hash, the existing product matrix passed **17/17
  Console** (`s36-final-console17-summary.json`) and **17/17 Window**
  (`s36-final-window17-summary.json`) in an isolated package via temporary
  `Z:`. This includes CMD, COMMAND, MEM, EDIT, native/DOS return and nesting.
  The `Z:` alias was removed after testing.
- Only `ntcon.exe` changed in the coherent eight-file package. The final
  binary was published to `O:/winnt`; its SHA-256 is
  `5E18A7F8A67880B59F0B0D91D647C5D685C58BC872004C79DB951A33872E129A`.
  All eight published files match the staged package. Published native CMD
  (`s36-final-published-native`) and DOS MEM (`s36-final-published-mem`)
  smoke tests passed.

## Interpretation, limits and follow-up

The title source, controller update, and DOS title-change signal are verified
separately; the full product matrices prove no observed execution/display
regression. They do not visually assert the caption in the owner's live
Windows Terminal session. The owner can check `run16 command`, Ctrl+Alt+F,
and a DOS title change on the deployed package. Windows Terminal may display
its own tab title, which is not the Console title returned to NTCON. Static
external title changes with no NTCON wake and titles beyond the local
128-byte control capacity remain explicit limits of this bounded S, not
claimed as fully observed. T423 stays open for owner acceptance. The
read-only task-trace S is now S37 and was not admitted here.

## Reopened S36: active-worker title and nested CMD

The first delivery observed only NTCON's attached visible Console title. A
real nested CMD changes its own Console title without changing that visible
root, so the Window caption stayed stale. This P adds one copied, bounded
`CONSOLE_IO_PUBLISH_TITLE_A` operation (Console I/O 21, application/RPC 27):
NTVDM publishes after a successful original Console-title call; NTW32 reads
its own hidden Console title during its existing capture and publishes changes.
The common transport is in `worker-base`. NTCON accepts the publication only
from the active channel, caches it for that channel, and rechecks on handoff;
no guest, original mirror, shared KVM library, process tree or new timer was
changed. An inactive channel cannot rename the current Window. Native CMD's
own title changes remain owned by Windows; the copied notification merely
projects the observed title to NTCON's Window.

On the old published package, the real private-desktop probe observed the
Console-title sequence `<S36-OUTER>` → `<S36-OUTER - cmd>` → `<S36-INNER>` →
`<S36-OUTER>` while the Window remained at the original launch title in each
stage. On the new package, the same four Console titles and all four Window
captions matched; this includes CMD's **automatic** nested ` - cmd` change and
automatic restoration, not just explicit `title` commands. The new candidate
and the published `O:/winnt` package both returned
`s36-title outer=1 nested=1 inner=1 after=1 window=1/1/1/1`.

The x86 graph compiled and linked all seven changed product targets; the
unchanged `wow32.dll` was carried in the coherent eight-file package.
`console-frontend-test.exe` passed, including invalid old/future Console-I/O
versions, generation/sequence errors, malformed title payload and successful
copied-title delivery. The full product matrix passed 17/17 Console and 17/17
Window cases, including direct and nested COMMAND, MEM, EDIT and native/DOS
return. The `O:/winnt` eight files were hash-matched to the staged package;
the published nested-CMD title probe passed. The build/test drive `Z:` was
removed afterwards. Evidence is under `build/M0-T423/S36/reopen1`:
`baseline-nested-title-escalated.raw`, `candidate-nested-title-r2.raw`,
`s36-reopen-console17-summary.json`, `s36-reopen-window17-summary.json`, and
`published-nested-title.raw`.

The older `console-channel-lifetime-test.exe` reports the same line-181
`ERROR_PIPE_CONNECTED` failure on the previous and new packages in this host
environment, so it is not counted as a new passing test. The
`console-client-test.exe` title transport assertions ran through successfully,
but its later desktop-window fixture fails at `GetConsoleWindow()` in the
automated private desktop. `ntw32-presentation-test.exe` did not complete and
was stopped; no pass is claimed for it. The 17+17 product and real title probes
are the runtime acceptance evidence; these focused-fixture limits remain
recorded, not silently promoted to passes. WOW visual frontiers remain subject
to the owner's prior headless-only allowance, with no new WOW code in this P.

Published SHA-256 hashes:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `3C888B501164C379F7B2227F09AC50608C7CB769F9083518996DE89A4DD57710` |
| `ntsrv.exe` | `72E8A6F266A2BED6075024E18BE789A28A6205ED0CD7AFC8CB7316D43577BBE1` |
| `ntvdm.exe` | `2FF54F07890D071687D519A2BF7BE4CC82C0D8D88F6E255896D6AA94AF58F780` |
| `ntw32.exe` | `101E0807BF27CAE4554006D18F5479E514BA06CBD2262FD2766395013A9660A7` |
| `ntcon.exe` | `ABBA80C8B666420A00D6C775DD82FBB0F6B2BEC3DD67750F41FFFD92B94170E1` |
| `ntmon.exe` | `BE493183DCEC33F3CDEE8B10875FD53FC3F6B1D862232617959149EC5A76AFEF` |
| `VDMREDIR.dll` | `1A2418FE667348EF3C6764A85C375A53B40C00D3ECAF2FBC8F74B2D1DF4D881F` |
| `wow32.dll` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
