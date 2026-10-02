# T423 S36 visible Console title in NTKVM Window

## Question and inputs

The owner closed S35 and inserted S36: replace the fixed `NTVDM` Window
caption with the title of the **user-visible Console to which NTKVM is
attached**. This explicitly excludes NTCON's hidden Console and any Windows
Terminal tab-level override. The input baseline is delivered S35
`96a5e0bad` and its [evidence](m0-t423-s35-native-text-window.md).

## Procedure and observations

- NTKVM's presentation thread previously created its Window controller with
  the literal `NTVDM`. The project-local Window API already had a copied,
  asynchronous title-control operation; no library, guest or cross-EXE wire
  change was required.
- NTKVM now reads `GetConsoleTitleA` from its attached Console on presentation
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
- MSVC x86 linked `ntkvm.exe` and both focused tests in
  `build/M0-T423/S36/candidate1/`. The copied S35 x86 graph was executed
  with the build-local sequential command runner because the ordinary Ninja
  runner stalled without compiler output in this environment. The final
  native-frontend object and `ntkvm.exe` were relinked after the root-loss
  safety review.
- On the final staged hash, the existing product matrix passed **17/17
  Console** (`s36-final-console17-summary.json`) and **17/17 Window**
  (`s36-final-window17-summary.json`) in an isolated package via temporary
  `Z:`. This includes CMD, COMMAND, MEM, EDIT, native/DOS return and nesting.
  The `Z:` alias was removed after testing.
- Only `ntkvm.exe` changed in the coherent eight-file package. The final
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
its own tab title, which is not the Console title returned to NTKVM. Static
external title changes with no NTKVM wake and titles beyond the local
128-byte control capacity remain explicit limits of this bounded S, not
claimed as fully observed. T423 stays open for owner acceptance. The
read-only task-trace S is now S37 and was not admitted here.
