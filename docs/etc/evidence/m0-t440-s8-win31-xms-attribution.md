# M0 T440 S8 — Win3.1 XMS-path attribution

## Scope and safety

This is a bounded diagnostic record only.  It changes neither the product
image nor guest media.  The temporary S7 `ntvdm.exe` used only to obtain a
matching link map was restored immediately afterwards; the deployed
`O:\winnt\system32\ntvdm.exe` SHA-256 is again
`5F7AF08DE717DB2CA54184D2C3E70F17540EAE6301944ADD6FBFA6889EF90269`.

## Measured ownership

With the installed Win3.1 Standard launch profile started and allowed to warm, a five-second
thread sample found one NTVDM thread consuming `4.984375` CPU seconds.  The
remaining ten NTVDM threads consumed at most `0.015625` seconds each.  This
excludes NTCON, NTSRV, worker transport, frame presentation, and the timer
thread as the immediate owner of that interval's cost.

A later read-only sample of the already-retained project NTVDM session gave
the same result independently: one CCPU execution thread used `4.91` CPU
seconds in five wall seconds; its six companion threads used no more than
`0.02` seconds each.  Its mapped active stack was:

```text
c_cpu_simulate
  <- ccpu / CALLF / spush16
  <- vir_write_word / spr_write_word / phy_w16
  <- mvdm_softpc_report_wow_source_descriptor_store
  <- KernelBase / ntdll
```

This removes the earlier ambiguity between startup-only XMS work and an
already-retained Win3.1 session: the disabled tracing hook is also on the
actual hot CCPU execution path after startup.

The standard Windows CPU ETW profile could not be enabled on this host
(`CPU.Verbose.File`, `0xc5585011`), so the record instead uses the existing
read-only x86 instruction-pointer/stack observer.  Twenty bounded snapshots
of the hot NTVDM thread contain `xmsMoveBlock` in 16 stacks.  Representative
mapped frames are:

```text
xmsMoveBlock
  <- mvdm_softpc_guest_memory_copy_forward
  <- mvdm_softpc_guest_memory_copy_to
  <- session_guest_memory_release
  <- guest_memory_lease_release
  <- mvdm_softpc_guest_memory_write
  <- c_sas_stores -> c_sas_store
  <- mvdm_softpc_report_wow_source_descriptor_store
```

The existing bounded XMS witness, enabled only for this run, observed thirty
forward moves totalling `15,682,640` bytes.  The first 28 moves are contiguous
copies of `0x8b810` bytes.  During the five-second witness window before the
writer completed its capped record, 23 starts totalling `13,142,384` bytes
were already recorded.  These figures describe startup/transition work; they
are not a claim that Win3.1 was retained at a user-visible desktop for the
whole window.

## Source-backed mechanism

`xms.486/xmsblock.c` correctly retains original ownership of the XMS move
descriptor and forward-copy rule (T407 S2).  Its standalone transport calls
`mvdm_softpc_guest_memory_copy_forward()`.  The descriptor itself remains a
checked twelve-byte lease; that recovery boundary is not, by itself, a
justification to restore a raw host pointer.

There is, however, an independent project-added diagnostic hook in
`softpc.new/base/ccpu386/ccpusas4.c`: commit `806598b9b` inserted
`mvdm_softpc_report_wow_source_descriptor_store()` into every byte/word/dword
SAS store.  A string store calls `c_sas_store()` once per byte.  Consequently
the observed 15.0 MiB XMS transfer invokes that hook at least 15,682,640
times.

The hook is meant to be default-off, but its first action on every invocation
is `GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", ...)`.  The
trace variable and selector were absent in the measured process.  The stack
samples place this disabled diagnostic call directly below `c_sas_store` and
above the active XMS transfer.  It is therefore a project-owned, source-backed
candidate for the next bounded repair, independent of graphics cadence or
guest behavior.

The permitted SoftPC comparison tree's corresponding `ccpusas4.c` contains
the original `c_sas_store*` and byte-loop `c_sas_stores` bodies without this
report call or a per-store environment query.  The difference is therefore
not an unavoidable CCPU40/XMS cost and does not require importing or deriving
new comparison-project code.

## Disposition and limit

The next repair should initialize the optional descriptor-trace configuration
once before CCPU execution, then bypass all five instrumentation calls in the
ordinary disabled case.  When the two trace environment variables are set
before worker startup, the existing descriptor filtering and emitted content
must remain unchanged.  It must not change `xmsMoveBlock` ownership,
descriptor parsing, copy direction, guest memory leases, frame publishing, or
the 50 Hz policy.

This record establishes a concrete, high-frequency project diagnostic cost in
both the XMS startup work and an already-retained Win3.1 session.  Re-measure
that retained session after the default-off instrumentation repair before
attributing any remaining mouse latency to CCPU or guest Windows.

## S8 repair and focused verification

The repair resolves the two trace environment values exactly once in
`mvdm_standalone_worker_begin()` before CCPU starts.  The five existing SAS
hook sites now make only a process-local active-flag branch; disabled workers
do not enter the recorder.  The recorder retains its existing selector parse,
address filter, line format and append behavior when active.  This is recorded
as `MVDM-HOST-DIV-329`.

The current x86 graph rebuilt and linked `ntvdm.exe` successfully at
`build/M0-T440/S7/r001/ntvdm.exe` (SHA-256
`D34704655AD431D5D6A0DFBAC019F909C10830EA5B3E2D1888CF82C6F816AB89`), including
the CCPU archive and the standalone worker.  Its existing focused
`ccpu-halt-reset-test.exe` passed: the original reset vector was reached with
`AX=BEEF`, and all existing CCPU fault/interrupt assertions passed.

A temporary exact candidate replacement in `O:\winnt\system32` successfully
launched and closed one `run16 WINMINE.EXE` WOW session; the original released
`ntvdm.exe` was restored immediately afterwards and again hashes to
`5F7AF08DE717DB2CA54184D2C3E70F17540EAE6301944ADD6FBFA6889EF90269`.

The candidate also launched the installed Win3.1 Standard launch profile, warmed for twelve
seconds, then used `9.546875` NTVDM CPU seconds in the following ten wall
seconds.  This is only a `0.125` second (about 1.3%) reduction against S7's
`9.671875` ten-second retained-desktop reference.  Thus the eliminated
environment lookup is real and correctly bounded, but it is not the remaining
desktop responsiveness bottleneck.  The benchmark session was stopped and
the same published hash restored after the sample.

Twelve further read-only instruction-pointer snapshots of that candidate
desktop found the one active CCPU thread repeatedly executing the following
mapped stack, with no descriptor-trace helper frame:

```text
c_cpu_simulate
  <- MS_bop_1
  <- cmdExpandEnvironmentStrings
  <- xmsMoveBlock
  <- mvdm_softpc_guest_memory_copy_forward
  <- mvdm_softpc_guest_memory_copy_to
  <- session_guest_memory_release / guest_memory_lease_release
  <- c_sas_stores / c_sas_store
```

The snapshot identifies the remaining cost as real original-XMS forward-copy
execution through the previously approved lease boundary.  It does not yet
prove why this guest path repeats after the visible desktop: the sampled
return-frame chain crosses original BOP/COMMAND code, so the next diagnostic
must count and classify the XMS move requests before proposing any lease or
CCPU change.  In particular, it must not infer that
`cmdExpandEnvironmentStrings` itself directly calls the XMS provider merely
from this asynchronous stack chain.

Enabled-trace acceptance remains open.  The existing
`verify-wow-user-destruction.ps1` preflight hard-rejects the now-native x64
`run16.exe`, and its own PowerShell window probe shadows read-only `$PID`.
The direct one-cycle replacement therefore did not reach that script's
specific callback-selector witness (`83B7`).  These test-harness defects are
not treated as a product failure or as proof of unchanged enabled output.
Before S8 closes, the trace observer needs a small architecture-neutral
preflight/probe repair or an equivalent focused descriptor-write fixture; it
must demonstrate at least one filtered record with the pre-existing line
shape, then repeat the retained-desktop CPU sample.

## Continued XMS-bridge accounting

The retained witness's 30 forward moves total `15,682,640` bytes.  The
current `copy_forward` implementation divides that into 3,829 4-KiB-or-less
chunks.  For each chunk it takes one readable lease for the source, then one
writable lease for the destination.  The generic writable lease deliberately
hydrates its bounce buffer before a later commit, because other callers may
perform a read-modify-write through a `WRITE` view.  Thus this particular
whole-span overwrite currently performs 7,658 lease acquires, 7,658 provider
reads, 3,829 provider writes, and 7,658 allocate/free pairs.  At the CCPU
boundary this means at least three byte access operations for every moved
byte: source read, destination hydration read, and destination store —
47,047,920 accesses for the recorded total.

The destination hydration is not safe to remove globally: current lease tests
and callers intentionally use a `WRITE` acquisition as a readable
read-modify-write view.  Conversely, the original XMS forward-copy contract
does overwrite every destination byte.  S8 therefore changes only the
project-owned `mvdm_softpc_guest_memory_copy_forward()` bridge.  For a
non-zero in-range active SoftPC session, it now calls original
`c_sas_move_bytes_forward(source, destination, length)`: numeric addresses
are resolved by the selected CCPU one byte at a time, in original forward
order, without a host alias, bounce allocation, or destination hydration.
The generic `copy_from`, `copy_to`, and overlap-aware `move` APIs remain
unchanged.

The focused mapped-XMS fixture now proves one direct forward CCPU call, no
provider reads/writes, forward-overlap contents, and a rejected out-of-range
span.  The real original CCPU external-memory fixture separately passes its
reversed adjacent guest-page load/store/forward-move case.  The regular CCPU
HALT/reset and fault test also passes.  Thus the new bridge retains numeric
mapping semantics rather than relying on a flat or native alias.

The source-built x86 candidate (`1C3575CD18324FC47064226D8562AFF3B6476180C33AA44017793D06140853F8`)
was sampled twice after the same twelve-second `WINSTD.CMD` warm-up.  It used
`9.90625` and `9.671875` CPU seconds in the following ten wall seconds.  The
second equals the published S7 baseline; the first is worse.  This proves the
bridge repair is not the dominant cost of this retained-desktop sample, though
it removes the documented 7,658 lease acquires and 47,047,920 minimum CCPU
byte accesses from the separately observed 15.0 MiB XMS witness.  After
focused proof the identical candidate was published to both
`assets/release/ntvdm.exe` and `O:\winnt\system32\ntvdm.exe`; both hash to
`1C3575CD18324FC47064226D8562AFF3B6476180C33AA44017793D06140853F8` and the
release manifest was checked against all ten images.  A bounded published
`run16 command /c ver` smoke returned zero.  The temporary sampling sessions
were stopped before publication.

A bounded non-interactive `WINSTD.CMD` diagnostic with the existing XMS trace
environment variable captured 13 early XMS BOP records but no move service
before the session ended.  It cannot substitute for the earlier retained
desktop witness, and it was used only to verify that this launch form did not
silently supply a new move-count result.  The temporary candidate was removed
and the published `ntvdm.exe` hash above restored after the session.

## Continued desktop saturation attribution

The retained Standard desktop was sampled again after the trace and XMS
repairs.  Over ten wall seconds its CCPU execution thread consumed `9.75`
CPU seconds; the ten other NTVDM threads together were effectively idle, and
NTCON consumed about `0.17` CPU seconds.  Twenty-five read-only CCPU state
snapshots at 100 ms intervals showed changing protected-mode instruction
pointers, `IdleNoActivity=1`, and interrupts enabled.  This is not the former
ring-0 `HLT` spin and is not one fixed guest instruction loop.  It is ordinary
active CCPU execution that the current worker lets run without a host-rate
governor.

The original OpenNT idle path is present and enabled for the Standard PIF:
the generated `WINSTD.PIF` has polling detection enabled and carries
foreground/background priority values `100/50`.  In the retained original
host, `host_timer_event()` calls `PrioWaitIfIdle()` only when the active PIF
priority is below 100.  Thus the current foreground value deliberately
requests no timer-driven host wait, even though `IdleNoActivity` is true.
Keyboard-poll and explicit wait-I/O calls can still enter the separate
original idle path; they do not impose a continuous instruction-rate limit on
an active graphical desktop.

As a bounded configuration experiment only, a byte-for-byte copy of that PIF
with the existing W386 priority fields changed to `50/25` was run with the
same hidden Standard desktop launch.  The default `100/50` PIF used `5.844`
and `5.828` CPU seconds in two six-second samples.  One valid `50/25` sample
used `4.188` CPU seconds in six seconds.  This proves the retained OpenNT PIF
priority/wait mechanism is functioning and materially reduces host CPU use.
It does **not** prove a better user-visible latency, and it must not silently
change the launcher profile: that would trade guest throughput for scheduler
yield as a product-policy decision.

The comparison SoftPC runtime has a separate project-owned executor pace at
its CCPU instruction boundary (one million instructions per second, checked
every 1,024 instructions and bypassed for pending events).  It is not an
OpenNT mechanism and is not present in this product.  Its presence explains
why the comparison can look smoother despite lower raw execution throughput:
the guest cannot monopolize a host core between device events.  Copying it
would be a new timing policy, outside S8's admitted scope; it needs a new S
packet with an independently authored design, explicit cap/enable policy and
input-latency as well as throughput validation.

## Follow-up: why the PIF priority experiment did not change the desktop

The owner tested the existing `50/25` copy of `WINSTD.PIF` and found no
visible responsiveness improvement.  That outcome is consistent with the
reached original host branching and must not be interpreted as a failed
graphics or mouse transport repair.

`host_timer_event()` in `mvdm/softpc.new/host/src/nt_timer.c` runs
`IDLE_tick()` and the PIF-driven `PrioWaitIfIdle()` only inside
`if (!VDMForWOW)`.  Win3.1 Standard is a WOW worker, so its normal desktop
execution does not use that timer branch as a frame or input pacing mechanism.
The documented `50/25` CPU sample therefore remains evidence that an
original PIF-related wait can be reached during the broader session; it is not
evidence that the setting regulates retained WOW desktop scheduling.

The active desktop still has one CCPU thread using approximately a host core
while the frontend is nearly idle.  The source-selected provider for
`ActivityCheckAfterTimeSlice()` is absent; the project-owned fallback in
`ntvdm-exe/softpc/mvdm_softpc_ccpu_fallback.c` is deliberately a no-op.
However, its reached original callers are after idle waits in `nt_unix.c` and
cannot be repurposed as a general executor throttle without changing their
meaning.  The actual responsiveness gap is therefore not a fixed 20/30/50 ms
publisher delay and not yet a proven mouse-IRQ debt: it is the lack of a
modern replacement for the original NT kernel-VDM/WOW scheduling carrier when
the CCPU executes an active GUI desktop continuously.

SoftPC's smoother comparison behavior comes from a separate project-owned
executor pace at its CCPU instruction boundary.  It is not a missing body that
can be copied into `ActivityCheckAfterTimeSlice()`, and importing it would be
a new product timing policy.  Any delivery must first admit a separate task
that defines its wake sources, maximum uninterrupted guest work, timer
semantics, throughput contract, and input-latency measurement.  S8 retains no
such production change.

The source also retains the original mouse EOI throttle, which schedules at
most one follow-on mouse interrupt after a 10 ms delay.  That can contribute
to individual mouse-motion tails, but it is original behavior and cannot
explain the measured sustained CCPU saturation.  No product change is
proposed for it under S8.

## CCPU compiler-mode attribution

The current original-SoftPC Ninja generator applies no `/O` flag to its
generic C rule.  Its selected CCPU archive therefore uses MSVC's default
unoptimized mode.  This is project-owned build selection, not an OpenNT or
guest-source difference.  The permitted SoftPC comparison project explicitly
selects a Release build when a single-config generator would otherwise leave
the interpreter unoptimized.

For a bounded same-source comparison, the generator gained a default-off
`-CcpuOptimization O2` test selector.  It changes only `obj/ccpu/*`; all
other generated flags and source inputs are identical.  Both x86 builds link
`ntvdm.exe`, and their existing CCPU reset/fault fixture passes unchanged.

A new test-only CCPU fixture executes a real-mode `INC AX / DEC ECX / JNZ`
loop through the selected original CCPU decoder, followed by its existing
direct unsimulate opcode.  On the same host, two 100,000,000-iteration
(300,000,002 guest-instruction) runs measured:

| CCPU mode | Elapsed | Throughput |
| --- | ---: | ---: |
| default compiler mode | 11.562 s; 11.266 s | 25.95; 26.63 M guest instructions/s |
| `/O2` CCPU only | 7.140 s; 7.297 s | 42.02; 41.11 M guest instructions/s |

This is a repeatable roughly 1.55–1.62× interpreter-throughput gain.  It
proves that the current unoptimized CCPU selection is a material performance
defect and makes an `/O2` candidate worth interactive acceptance testing.  It
does not prove that compilation mode is the whole visible responsiveness
problem: the retained Standard desktop still needs a fair execution/yield
contract for its continuously active WOW guest.  The `/O2` candidate is
therefore diagnostic only until both functional and user-visible acceptance
are recorded.

## Continued quick-event time-scale audit

The retained CCPU40 interface makes an explicit time-scale assumption which
is independent of the missing `GetJumpCalibrateVal` provider.  Original
`base/system/qevnt.c::add_q_event_i()` documents that its `instrs` argument is
interpreted as time at **one instruction per microsecond**.  The selected
CCPU provider, `base/ccpu386/c_main.c::c_cpu_calc_q_ev_inst_for_time()`,
returns its `time` argument unchanged.  Consequently a request to defer an
event for 10,000 us supplies a CCPU heartbeat of 10,000 decoded instructions.

This is reached original device behaviour, not a hypothetical utility path:
`host/src/nt_eoi.c::host_DelayHwInterrupt()` turns its 10 ms mouse EOI delay
into `add_q_event_i(Delay - 200, ...)`; the original keyboard path similarly
uses a 7 ms `add_q_event_t()` refill delay.  At the measured `/O2` CCPU rate
of 41.11–42.02 M guest instructions/s, 9,800 decoded instructions consume
about 0.233–0.238 ms of host execution, not the requested 9.8 ms.  The same
factor applies to the CCPU40 quick-event delays while an active guest is
running.  This provides a direct, quantified semantic mismatch between the
retained CPU40 time contract and modern-host execution rate.

The fallback `GetJumpCalibrateVal() == 0` is *not* the repair lever for this
mismatch.  In the selected NTVDM-specific `qevnt.c`, the calibration routine
which consumes that accessor is separate from `ResetCpuQevCount()` and does
not change the latter's call to `host_calc_q_ev_inst_for_time()`.  Returning a
nonzero invented calibration value would therefore be unsourced and would not
repair the active conversion path.

The comparison SoftPC project independently contains a host-owned continuous
executor pace: it checks every 1,024 decoded instructions, holds the guest to
one million instructions/s against a monotonic clock, and bypasses the wait
when input or another executor event is pending.  That explains the observed
combination of lower raw throughput with a smoother interactive desktop.  It
is useful comparative evidence, not an OpenNT body to transplant.  Any
NTVDM64 delivery must be admitted as a timing-policy task and must establish
the equivalent bounded inter-instruction safe point, input/timer wake
precedence, pause/idle accounting, and latency/throughput acceptance; S8
does not introduce it.

### Candidate minimal delivery shape (not implemented by S8)

The existing project-owned `mvdm_softpc_ccpu_wait` carrier provides the
right wake source for a future pacing helper: every original
`c_cpu_interrupt()` call already signals its auto-reset event, including the
timer and hardware-interrupt paths used by keyboard and mouse input.  Thus a
future implementation does not need a polling input thread, a frontend
special case, or a new timer producer.

The narrowest shape is a process-local CCPU pacer owned by the standalone
worker.  It starts and stops beside the existing wait carrier, receives the
already-observed CCPU pending-event bitmap at the `NEXT_INST` instruction
safe point, and checks a monotonic deadline only once per bounded instruction
batch (for example 1,024 instructions).  If no CCPU event is pending and
guest execution is ahead of the 1 instruction/us contract, it waits on the
existing wake event until that deadline.  A signal returns immediately; CCPU
then consumes the original event map in its existing order.  If an event is
already pending, the helper returns without waiting.

The same helper must be paused around the original ring-0 HLT wait and resume
after it returns.  Otherwise host idle time would become execution credit and
the guest could burst after an input wake.  This is the only additional state
needed: monotonic origin/debt plus an active/paused flag, all private to the
worker.  CCPU, qevent, VGA, mouse, NTCON, worker-base and wire formats remain
unchanged; the original mirror needs only the bounded helper calls at the
existing safe point and HLT boundary.

Acceptance must demonstrate: (1) a 9.8 ms delayed mouse IRQ cannot fire
earlier than its requested interval while the guest is active; (2) a pending
input/timer/reset wakes without waiting for the pacing deadline; (3) no CPU
credit accrues across HLT; (4) Standard Win3.1 input latency improves without
regressing timer, keyboard, reset or DOS/WOW lifecycle tests; and (5) the
unchanged CCPU throughput fixture still reports its raw rate when pacing is
disabled for finite test runs.  These are a new timing contract, not S8
closure evidence.

## Closure build and publication checkpoint

The fresh `build/M0-T440/S8/r028-close` x86 graph rebuilt the affected
`ntvdm.exe` after all S7/S8 sources, including the corrected divergence ledger.
Its SHA-256 is
`B5217F3D5A245A5290D157C5E40DF1041E9C711677215681EBE17DE9982229B4`.
It passed `ccpu-halt-reset-test.exe` (original reset `AX=BEEF`), the finite
10,000,000-iteration CCPU throughput fixture (30,487,806 guest instructions/s),
the original reversed-page external-memory fixture, and the mapped-XMS fixture
(`S36_ORIGINAL_XMS_MANAGE_NO_BOUNCE_NO_ZERO_REUSE_OK` and
`S36_ORIGINAL_XMS_BOUNDED_MOVE_CANCEL_OK`). The identical image was copied to
`assets/release/ntvdm.exe` and `O:\winnt\system32\ntvdm.exe`; the prior
`1C3575CD…53F8` image is retained at
`O:\winnt\builds\M0-T440-S8-P1-20261009-ccpu-xms\ntvdm.exe`.

The normal serial product matrix was deliberately not invoked: its only
runner maps `Z:` through `subst`, while the owner has prohibited both. A direct
pipe-hosted `run16 COMMAND.COM /c ver` returned 87; this launch lacks the
observer's real Console contract and is not counted as a runtime pass or a
regression. It does not alter the focused fixture verdict. The pacing design
above therefore remains the next separately admitted scope.
