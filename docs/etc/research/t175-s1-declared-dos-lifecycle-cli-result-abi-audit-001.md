# T175 S1 declared DOS lifecycle CLI-result ABI audit 001

## Question

Can the CLI expose the T174 copied 16-bit DOS terminal status now, without changing guest state or treating a native process exit code as DOS status?

## Inputs and procedure

The audit traces ntdos64-run.c (run_process), ntdos64_bochs_engine.c (run_bochs), the adapter runtime process-local cmd_get_next state, the Bochs exception seam, and the T169/T174 controlled-stop and return-lifecycle evidence.

## Observations

| Boundary | Present behavior | Meaning |
| --- | --- | --- |
| Runner to engine | Runner creates one child, handles cancellation by terminating it, then returns the engine Win32 exit code unchanged. | A runner code denotes child process outcome; it has no DOS-result object. |
| Engine to Bochs | Engine creates one private run directory and one Bochs child, then returns Bochs Win32 exit code unchanged. | The engine has no pipe, result file, shared object, callback or result parser. |
| Bochs and adapter | They are linked in the Bochs process. The exception seam carries only copied boundary state and CPU result (PASS_THROUGH, RESUME, STOP). | T174 terminal_dos_exit_code is process-local adapter state. It is not a Bochs exit status. |
| Accepted FE | T168 registered accepted STOP calls SIM quit with zero; T169 observed exit code zero. | This proves only the exact controlled-stop target completed, not normal DOS return or DOS status. |
| T174 54:11 | On the source-derived no-next branch, adapter copies DX, clears CF and resumes the guest. | It neither stops Bochs nor produces an engine-visible record. It is not yet runtime-reached. |
| Cancellation/watchdog | Runner cancellation terminates its child; observer watchdog separately terminates a timed-out process tree. | Neither is guest completion or a DOS result. |

The original COMMAND handler reinforces the separation: it receives guest DOS status in DX but only writes AL from historical dwExitCode32, whose meaningful values come from the old host CreateProcess path. There is no source basis for reusing either field as a new CLI process exit code.

## Result

No CLI result implementation is admitted now. The smallest honest result vocabulary is conceptual, not yet a wire ABI:

| Disposition | DOS status present? | Process exit-code interpretation |
| --- | --- | --- |
| exact controlled stop observed | no | native success only; T169 proves this one case |
| normal declared-target return observed | yes, copied DX | unavailable to CLI until a new transport is admitted |
| startup/profile/engine failure | no | existing nonzero runner/engine code |
| user cancellation | no | runner-owned termination, not guest result |
| watchdog/forced termination | no | observer infrastructure outcome, not guest result |
| unobserved/ongoing guest execution | no | no terminal claim |

A correct future wire ABI must be fixed-width, versioned, and engine-owned at the process boundary. It may carry a disposition plus a validity-tagged 16-bit DOS status, but must never encode guest pointers, CPU state, Bochs objects, a service selector, host handles, or a command queue. It must not overload Bochs/engine Win32 exit codes. Implementing that channel requires separate admission because the current Bochs exception seam exposes no terminal-result transport and adding one touches the Bochs/adapter process composition boundary.

## Interpretation and next priority

T175 rejects premature transport work, not CLI result reporting as a product feature. The gating fact is still missing: no runtime trace proves a guest reaches the normal 54:11 return path and its guest continuation. The next highest-ROI task is therefore a source/evidence audit for one admissible normal-return guest fixture and its expected continuation. Only after that path is reached may a separately designed engine-owned result channel be justified.

## Confidence

High for the current-path rejection: both process launches return only GetExitCodeProcess, and no adapter result accessor is consumed outside the Bochs process. High for the FE distinction because T169 records exact FE and native exit zero. No claim is made about arbitrary DOS program completion.
