# DIV-147 host close-cohort source and lifecycle audit

## Question and comparison

Do `nt_reset.c::HostApplicationStarted/HostApplicationClosed` duplicate the
NTSRV/worker-base close authority and can they safely be deleted? This is a
source and retained-evidence audit only: no source, guest, product image or
runtime state was changed, and no new fault-injection test was run.

The pinned original `softpc.new/host/src/nt_reset.c` has neither static flag
nor guard: `host_applClose` directly performs event-thread, sound, heartbeat,
video, printer, COM, mouse-menu and configuration cleanup. Its
`host_terminate` calls `ExitVDM` and `ExitProcess`.

The current source adds these separate things:

- `HostApplicationClosed` and `HostApplicationStarted` are plain static BOOLs;
  `host_applInit` sets them FALSE and TRUE at the **beginning** of its body.
  `host_applClose` first asks `ntvdm_worker_lifecycle_authorize_exit`, then
  returns if not started or already closed, otherwise marks closed **before**
  executing original cleanup. These flags neither signal NTSRV nor identify
  the cleanup thread. The Started flag is an entered-init indicator, not proof
  that every original resource initialized.
- `host_terminate` independently obtains that authorization and still executes
  original `ExitVDM` then `ExitProcess`. T412 S5 restored this terminal exit
  from a prior session-escape change. The current `src/mvdm/README.md` DIV-147
  table says the former nt_reset entries are restored original, but its wording
  omits the surviving DIV-147 flags and close guard. The same ID is also used
  for distinct xt.c and sim32.c adaptations. Registration needs a precise
  source/hunk disposition, not a claim the complete file is byte-original.
- DIV-356's service authorization and DIV-147's once/entered guard are distinct
  mechanisms; replacing or deleting one based on the other's name alone is
  unsupported.

## Actual entry and exit paths

1. Original `obj.vdm/ntvdm.c` now calls the project standalone begin, arms an
   execution escape, calls original `TimerInit`/`host_main`, and on escape
   branches to `mvdm_standalone_worker_finish`; it does not return through
   `host_main`'s normal tail after a longjmp. Original `host_main` enters
   `host_applInit`, later calls `host_start_cpu`, then calls `host_applClose`
   only if that normal CPU call returns.
2. Original `TerminateVDM -> terminate -> host_applClose -> host_terminate`
   performs device shutdown before host cleanup and original `ExitVDM`/exit.
   Original `nt_event.c::CntrlHandler` has a separate Console-close branch
   that calls `host_applClose -> ExitVDM -> ExitProcess`. Original reset.c can
   call `cmdRebootVDM -> host_terminate` directly; **that direct call does not
   itself call host_applClose**. An EGA trace-only source also names the
   close function; active-profile selection must be verified separately.
3. `mvdm_softpc_terminate_current_session` explicitly calls `host_applClose`
   and then `session_terminate_current`, which longjmps only when the current
   bound session has an armed termination escape. Its callers include selected
   sim32 allocation failure and source-shaped unavailable service adapters.
   In the standalone production main the jump lands at finish; no second
   host_main-tail close is executed by that jump.
4. `mvdm_softpc_execution_run_until_return` is a separate bounded-execution
   fixture bridge: after a termination escape it calls `host_applClose` again.
   Thus a path entering the project termination helper then escaping to that
   fixture bridge has two close-function entries; the static guard suppresses
   the second. This establishes the source path, not that a real production
   guest has exercised it. The current `ccpu_bounded_execution_fixture` uses
   a host_applClose counter stub and normally returns from host_start_cpu; its
   one-call assertion does not prove the real cleanup cohort's duplicate or
   failure behavior.

## Authority and concurrency

NTSRV authorizes worker-wide shutdown. `worker_lifecycle.c` uses an interlocked
`closing` owner-thread ID: a natural terminal caller claims it, requests the
service via worker-base, and waits for the actual worker/Console close event
or service death. A competing thread waits for process exit; the same cleanup
thread is allowed to reenter without another request. Management instruction
callbacks claim the same field. This serializes distinct terminal executors in
the bound standalone worker, but **does not by itself prevent the authorized
thread from entering host_applClose twice**. The two plain BOOLs in nt_reset
are a local cleanup guard, not the service's lifecycle authority. Standalone
watch activation is delayed until original initialization reaches
`nt_cpu.c::host_start_cpu`; pre-activation failure and fixture-without-owner
paths must be considered separately. The fixture mode deliberately bypasses
service ownership and does not establish multithread safety for these BOOLs.

T448 S3 and T472 S2 retain evidence for authorization-before-cleanup,
service death and same-owner recursive authorization. T472's `natural_exit`
fixture exercises the authorization function, not the actual nt_reset close
body. These receipts therefore cannot prove the guard is redundant or that its
real event/timer/device cleanup executes exactly once.

## Failure and initialization boundaries requiring proof

`host_applClose` sets Closed before `nt_remove_event_thread`. The selected
`nt_event.c` calls `mvdm_softpc_event_thread_alert_and_join`, but ignores its
return. That adapter returns failure on thread-handle duplication, alert or
wait failure. Static control flow can therefore continue to sound, heartbeat,
video and device cleanup after a failed join, and subsequent close entries
will not retry the cohort. No failing production run, unsafe access or required
retry mechanism has been demonstrated. Inject each named failure against the
actual selected functions and establish what remains alive before proposing a
repair; do not replace the service protocol with a local retry/poll loop.

The Started flag is set before host_applInit completes. Conversely `TimerInit`
occurs in the original outer entry before `host_main`/host_applInit; a failure
there can reach original termination with Started still false. The flag does
not model the readiness of individual heartbeat, console event, video or
printer resources. Audit the real partial-initialization branches and their
safe cleanup/exit outcomes; do not relabel Started as an initialization-complete
state or invent one without a demonstrated need.

## Recommendation and finite acceptance

**Retain the current guard provisionally.** The already approved centralized
service authority is preserved, and there is concrete duplicate-entry source
in the fixture escape path. Correct the DIV-147 registration to describe the
actual current guard and distinguish xt/sim32 meanings; its existence alone is
not final proof that the guard is needed in production. If a later complete
caller audit and actual-body tests prove a single-entry original close contract,
remove duplicate fixture/adapter entry first and only then remove a redundant
guard as one cohesive change. If same-thread recursion is genuinely required,
keep one clearly owned idempotence mechanism with failure behavior proven.
No new independent lifecycle registry or third control transport is justified.

Before either retain-as-final or delete, require actual-body fixtures and
appropriate real-worker regression for: normal CPU return; one and repeated
host-close calls; terminate->host close->host terminate; Console-close and
worker-close race; NTSRV disconnect; startup failures before/within
host_applInit and after event/heartbeat acquisition; helper longjmp and the
fixture bridge; direct reboot host_terminate; and event join/alert/duplication
failure with proof of the event thread's lifetime. Check exactly-once cleanup,
return/error and process/guest state, no use after thread retirement and no
false successful release. Include prior real DOS/WOW and complete product
regressions if runtime source changes. No guest modification/rebuild.
