# M0 T423 S18 — native root restoration evidence

## Scope

S17 installed the root `retire -> restored` barrier only on the DOS completion
path.  The owner reproduced the same visible outer-CMD input/echo loss for a
native root:

```text
cmd.exe -> run16 cmd -> Ctrl+Alt+F -> exit -> outer cmd.exe
```

The cause was source-visible: `launch_native()` waited for the native target
and its NTW32 final-presentation receipt, then returned directly.  It never
called the already-existing root retirement and restoration calls.  NTKVM
therefore began restoring the original Console only after the root `run16`
process had already returned to its parent.

## Production change

`src/run16-exe/main.c::launch_native` now follows the existing DOS root shape
once the direct target handle is signalled:

```text
native target exit
  -> final-presentation receipt
  -> run16_frontend_scope_retire
  -> run16_frontend_scope_restore_parent
  -> return to outer CMD
```

An earlier completion error remains authoritative; a restoration error is
used only when no earlier completion error exists.  A live target still does
not retire the frontend.  No guest, worker scheduling, timer, redraw or input
injection behavior changed.

## Reproducible focused evidence

All runs used the current compiled `run16.exe`, the immutable package at
`O:\winnt`, and the project observer on an isolated private desktop.  No user
desktop was opened or controlled.

1. `frontend-scope-lifetime-test.exe` passed all four ownership/lifecycle
   assertions.
2. Native Console route:

   ```powershell
   $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
   console-startup-observer.exe O:\winnt\run16.exe O:\winnt <report> \
     --observation-timeout-ms 20000 --observe-console-input-text "exit`r" \
     cmd.exe /d /k
   ```

   The S18 trace records, in order:

   ```text
   run16-native-submit 0
   run16-frontend-retire 0
   run16-frontend-restored 0
   run16-exit 0
   ```

   This is the required order that was absent before the repair.
3. Window route, with `MVDM_OBSERVER_WINDOW_INPUT=1`, produced the same native
   ordering after an actual public Console CAF and Window-routed `exit`.
4. The S17 DOS `COMMAND.COM -> CAF -> exit` control route still records:

   ```text
   run16-task-completed 1
   run16-native-resume 0
   run16-frontend-retire 0
   run16-frontend-restored 0
   run16-exit 0
   ```

   Thus the native addition did not replace or weaken DOS parent resumption.

Raw records are in `O:\winnt\Logs2` under prefixes
`t423-s18-private-console`, `t423-s18-fixed-window`, and
`t423-s18-dos-window`.

## Regression-gate status

The ordinary four-case text subset passed (`empty`, `native-zero`, `missing`,
`native-seven` was not part of that subset).  A first fifteen-case attempt
stopped at `native-seven`, where guest COMMAND transiently reported its
generated `O:\winnt\tests\D7.CMD` as not recognized.  This was retained as
failure evidence rather than ignored.  It did not reproduce: the same
`native-seven` case passed first with the S17 baseline `run16.exe`, then with
the current S18 `run16.exe`.  The latter trace proves a nested native task
executes `retire -> restored` and returns exit 7 before its DOS parent resumes.

The intermittent first attempt is therefore not attributed to the S18 change,
but the remaining complete package matrix and coherent publication still have
to be rerun before S18 can close.
