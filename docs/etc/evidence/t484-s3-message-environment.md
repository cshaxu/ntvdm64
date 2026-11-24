# T484 S3 P164 Win16 message environment consolidation

The live message integration runner dynamically compiled its own C# logon
reader. Remove that duplicate and reuse the existing CMD/native
logon-environment target and shared reader. Factor the existing owned CMD
producer launch so both message and environment targets use it. Record the
native image and reader hashes. No message assertions, probe algorithms,
production code or original guest bytes change.

Background PID15188 in build/t484-s3-message-environment runs the default
and StructuredPayloads/ThreadClientView actual USER16 two-task groups. Both
report PASS for send/nesting/reply/post/filter/quit/string copyback, native
peer directions and normal retirement. Optional fault, DDE, cross-worker and
lifetime variants were not rerun; this batch does not claim their acceptance.
The existing CMD message producer retains feature-mask selection.

The same owned run executes cached full and verifies the 11 raw build-input
hashes and 22 published hashes. See result.json, full-build.log, per-case
result.txt, exit.txt, process records, guest records and input hashes. Package
copies are disposable; no new product publication is needed when these hashes
remain unchanged. Documentation/tracker validation is recorded separately in
closure-result.json. S3 remains active; S4 and S5 remain outstanding.
