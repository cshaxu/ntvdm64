# Reusable published-window retirement witness

The earlier [controlled experiment](../terminal-schedule-owned/review.md) is
now an opt-in test in the existing selected-object runner:

```powershell
pwsh -NoProfile -File tests/observation/verify-wow-task-lifecycle.ps1 `
  -ProviderBuildRoot build/.ninja/msvc-x86-release/wow32 `
  -WorkerBuildRoot build/.ninja/msvc-x86-release/worker `
  -BuildRoot build/t473-s11/terminal-schedule-formal-final `
  -Case window-borrow -TerminalSchedule
```

The runner makes a disposable instrumented copy of the current production
terminal provider, retains the real CCPU page-domain provider and checks the
scheduled witness marker and actual worker thread exit code. It also runs the
existing registered-owner dialog lifecycle checks. This run exited zero; its
fixture output and hashes are retained here. The ordinary
`-RealDialogLifecycleOnly` control without instrumentation also exited zero,
under `build/t473-s11/terminal-schedule-formal-control/`.

The worker is a bound resource thread, not a second Win16 guest task; no
guest callback is forced during this overlap. This test is not proof of the
earlier SOL 1400 cause or all possible retirement interleavings. The provider
copy contains test-only hooks and is never released; no guest or product image
was changed.
