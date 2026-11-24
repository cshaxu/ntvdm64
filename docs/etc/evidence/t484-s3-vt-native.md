# T484 S3 P175 native VT fidelity producer

B0036 retained a useful independent Console conversion/pixel oracle, but its
build recipe pinned fb7650ce0 and hand-wrote CL/LINK commands against a caller's
historical frontend output. Replace that recipe with the current CMD/native
frontend graph and a shared x86/x64 producer fixture. Reuse the production
frontend session, Console, frame, window and codec objects; the existing capture
and launch sources are explicitly compiled into the verifier. No test-body,
product-source, original guest or mirror change is made. FrontendBuild/VsDevCmd
remain compatibility arguments but no longer choose build inputs.

Run: build/t484-s3-vt-native, normal-user-environment supervisor26176,
runner20568, 2026-11-24T10:06:32Z to10:07:30Z, exit0. Both x86 and x64 writers
report RESULT checks=230987 failures=0. The unchanged oracle covers VT standard,
256/RGB/default/custom palette behavior, output chunking, Console conversion,
production capture/frame/pixels and file/pipe bytes. This is an unswitched
private-desktop native test, not guest or physical-input acceptance.

Cached full delivery verification runs separately as PID23796; inspect
full-build.log, delivery.log and delivery-result.json. The final publication
identity and governance results are recorded below after completion. Only B0036
is closed by this batch; DLL-entry and staging callers remain pending.

PID23796 completed in61.7seconds, exit0. All11 raw inputs and22 published
hashes match; no redundant publication is required. No runtime package copy
was created by this native-only test. Ledger now314/355 disposed,41 remain.
Governance evidence is closure-result.json in the same batch.
