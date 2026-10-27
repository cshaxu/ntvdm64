# Native CBT creation metadata boundary observation

The current production failure can surface as `ERROR_INVALID_STATE` when
`wow_user_window_publish` cannot publish a window to the worker page domain.
That producer first queries modern USER through `GetWindowInfo` during
`HCBT_CREATEWND` enrollment. This observation tests the narrower hypothesis
that querying creation-time native metadata is intrinsically unavailable at
that point; it does not replace the production producer or diagnose SOL.

The [identity/result manifest](native-cbt-info/observations.json) binds the
independent C probe, compiler/runner, owned process and result. It reuses the
existing SOL probe's CreateDesktop/hidden child-process isolation pattern,
under desktop-capable modern PowerShell. It creates hidden native dialogs,
buttons, labels, edits, combo boxes, lists and scrollbars. A thread-local CBT
hook queries IsWindow, class atom, owner process/thread and GetWindowInfo
before forwarding the creation callback. It sends no application input,
uses no Computer Use or UI automation, and starts no product or guest.

The completed run created 3,500 explicitly requested windows across 500
rounds. Native internal children brought the actual CBT sample count to
5,000. Every sampled query succeeded, every window was valid and its native
thread belonged to the probe process. The owned window trees and private
desktop were closed by the probe.

This is a passing native-host precondition sample, not a guest/WOW acceptance
test. It supplies no proof about every class, creation hook, future host,
generation race or production state. In particular it does not prove the
original SOL failures arose elsewhere, or justify skipping native-query
failure handling. It rejects only the unsupported blanket assumption that
these metadata queries must always fail during CBT. The production creation
invariant and candidate qualification remain open.
