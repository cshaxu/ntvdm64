# M0 T457 S3 — hooks and timers acceptance

## Result

S3 closes with one source-proven repair. The retained original timer and hook
providers are preserved; the repair restores original worker-shared
foreground-idle intent without reviving an NT4 global hook or adding another
hook registry.

## Root cause and ownership

Original `wkman.c::WK32WowSetIdleHook` stores a live task's registration in
that task's original TD. `taskman.c` calls the foreground-idle hook while the
*currently scheduled* task is idle, then `WowOriginalForegroundIdleHook`
performs `CallBack16(RET_FOREGROUNDIDLE)`. The Win16 INT 2F:1689 vector is
therefore worker-shared: task A can install it and task B can receive it while
B is scheduled.

The adapter had narrowed that intent to the selected native thread. In one
shared WOW worker, a B idle event ignored A's live TD registration. The new
zero-thread query means “any live original TD in this worker registered”, while
a nonzero query still returns the observer for its exact native thread. The
callback is still entered only through the current scheduled task's existing
scope. The original TD list remains the sole authority for registration; native
USER owns per-thread observer handles; the adapter owns neither a new global
hook nor duplicated registration state.

This intentionally retains process/worker-local behavior. It does not claim
cross-worker or cross-process foreground-idle priority, consistent with the
approved thread/process boundary.

## Retained original behavior

- `wutmr.c` owns the 27 callback slots, 55 ms clamp, duplicate/Excel matching,
  queued-after-kill records and `DestroyTimers16` task-loss order.
- `wuhook.c` and `wowhooks.c` own hook records, legacy handle convention,
  conversion, `CallNextHookEx`, and `W32FreeOwnedHooks` retirement.
- The existing adapters only establish task/borrow scope around callback entry
  and use the original TD registration as above.

## Evidence

| Check | Result |
| --- | --- |
| Timer callback scope | `WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1 exception=1 denied=2` |
| Hook creation callback | `WOW_HOOK_CREATION_GUARD errors=0 enrollment=6 accepted=1 rollback=4` and callback scope assertions pass |
| Native hook registration seam | `WOW_HOOK_NATIVE_BOUNDARY errors=0 calls=24 local=17 global=1 invalid=1 null=1 mismatch=1 callback=1 dead=1 foreign=1 handles=77/77` |
| Idle pure boundary | `WOW_IDLE_HOOK_GUARD errors=0 calls=5 ... worker_shared=1` |
| Separate real WOW workers | `PASS two real separate WOW workers, own idle callbacks, retirement and surviving peer idle` |
| Shared real WOW worker | `PASS shared worker 32736, A vector ran on unregistered B task, both requests exited normally`; A recorded four callbacks on B's task identity |
| Build and delivery | formal x86 build followed by `ninja: no work to do`; all ten candidate/release/installed hashes match in `build/t457-s3/publication-r1.json` |

The native desktop idle-chain subcase could not create/switch an input desktop
in this execution environment and is an explicit non-pass before WOW behavior.
The real isolated guest worker witnesses above supply the required registration,
delivery, retirement and surviving-peer proof without treating that unavailable
desktop operation as success.

## S4 handoff

S4 owns integrated UI acceptance across normal, error and reentry paths. It
does not reopen timer/hook implementation unless a real integration failure
identifies a different source boundary. Guest media remains unchanged.
