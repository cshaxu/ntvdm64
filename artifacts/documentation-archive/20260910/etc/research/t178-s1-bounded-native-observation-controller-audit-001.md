# T178 S1 bounded native observation-controller audit 001

## Evidence

T177 prepared a complete profile-pinned fixture but used cmd.exe as a wrapper in
order to set the adapter environment. Its inner watchdog was 30 seconds. The
outer execution controller interrupted the operation at 45 seconds; taskkill
then returned Access denied. No bochs.log, observation record, or live child
remained by the follow-up inspection.

This failure repeats the historical controller pattern recorded in the T95
ledger: wrapper PID tree termination can fail to return before an outer timeout,
so the outer controller can terminate the wrapper without preserving a
child/log outcome. Those records explicitly reject treating such events as
guest results.

## Owner analysis

The T177 target did not need a shell wrapper. The historical direct-process
supervisor conclusion is the applicable design: construct ProcessStartInfo for
the exact Bochs executable, set only the declared adapter environment on that
process, pass the fixed -q, -f, -benchmark and -log arguments, own the direct
PID, and use timeout-only Process.Kill on that exact PID. It avoids taskkill,
process-name/path selection, stdout/stderr pipe deadlock and a second child
whose lifecycle the observer cannot prove.

The outer execution budget is a separate controller. It must exceed the
inner watchdog plus deterministic log/result collection grace. Otherwise it
can preempt the observer and erase the difference between a watchdog result
and an infrastructure interruption.

## Result

T177 cannot be classified as a guest execution. Its fixture remains valid, but
its observation result is indeterminate due solely to controller ownership.
T179 is required to implement and statically validate a direct-PID observer
with the exact immutable T177 inputs. It must not launch the guest; a future
runtime package can be considered only after T179 proves the controller
contract and has a declared outer-timeout budget.

