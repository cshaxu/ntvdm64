# T176 S24 Illegal-Operation Runtime Observation 001

## Question

Does the S23 narrow derivative reach the newly linked, unchanged OpenNT
`illegal_op_int` machine-handler island during the already observed normal
return execution path?

## Inputs

- `artifacts/build/current/t176-s23-illegal-op-link-r1/ntdos64-t98-current-adapter.exe`;
- the unchanged S19 BYOB profile and Bochs configuration; and
- one fresh direct-PID controller observation under
  `artifacts/analysis/t176-s24-illegal-op-observation-001/`.

## Procedure

The existing direct controller launched exactly that binary with the unchanged
profile, a 10-million instruction benchmark and a 30-second watchdog. It
created a fresh evidence directory and did not retry the run.

## Observations

- The controller started successfully and the watchdog terminated the process
  after 33,031 ms; its immutable result is `observation.json`.
- The log records startup transaction completion and multiple existing adapter
  resumes and data transfers.
- It records zero `selector=06`, `machine composition report`, and `machine
  composition outcome` markers.
- The final native fault is `stackPrefetch(2): access [0x0000ffff] >
  SS.limit [0x0000ffff]`, followed by a third exception at real-mode
  `CS:EIP=0000:019d`.

## Interpretation

This run neither proves nor disproves the `illegal_op_int` handler's runtime
behavior: its entry point was not reached. It does prove that the linked
derivative still reaches the separately known native stack-fault predecessor
before an observed BOP `06h`. The failure is not evidence for an adapter
service defect, an `illegal_op_int` semantic defect, or a missing Bochs
device. T176 must not add a retry, a synthetic selector, a device, or an
adapter workaround to force reachability.

## Follow-up

T176's normal-continuation result is blocked by the pre-existing native
stack/control-flow predecessor. A later package may reconsider that owner only
with independent source/control-flow evidence; it must not be created merely
because this observation did not reach selector `06h`.
