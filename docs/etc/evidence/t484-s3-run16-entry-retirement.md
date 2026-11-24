# T484 S3 P211: retire historical Run16 entry builder

Verify-Run16Entry.mjs hardcodes build/M0-T412/S3/product/run16.exe, requires
an x86 image and the old opennt-base-client classifier map member, then creates
its own compile.cmd for test/app/run16_native_child.c. No current caller uses
this script or the child. Retained T412 evidence is historical, not an active
acceptance entry. Both obsolete files are removed; no new replacement harness.

Current src/run16-exe/main.c::launch_native captures command, environment,
directory and standard streams into run16_native_start, calls
run16_frontend_scope_launch_native and waits on the request as appropriate.
It does not own the former direct-child execution model or old map layout.
This is why retaining the T412 producer unchanged would test the wrong package.

Current coverage remains in test/app/run16_launch_options_test.c (exact tail
pointer/quoting and options), test/lib-win32/process-launch/command_line.c
(argument boundaries), test/observation/verify-base-classifier-path.ps1
(selected classifier objects and native layouts), and
 test/observation/verify-native-launcher-abi.ps1 (capture/environment/scope/search).
Actual native worker requests, exit values and streams are covered separately
by test/integration/base_client_rpc_first_test.c and
 test/integration/verify-broker-io-handoff.ps1. These are scope mappings, not a
claim that those runtime integrations were rerun in this cleanup batch or that
all old assertions are identical to a new end-to-end acceptance.

Owned PID22692 started2026-11-24T14:11:41.0951285Z, running
build/t484-s3-run16-entry-retirement/run.ps1. It runs the existing x86/x64
classifier-path test, then cached full host build and11 raw/22 dual-published
hash checks. result.json and session evidence record actual results; no fresh
Full runtime pass is claimed. The final documentation/tracker checks precede
commit and push. No production, guest, mirror, ABI or release-byte change.

All30 P199 direct-command candidates are now classified, including the retained
read-only audit. This does not close S3: frozen316/355,23 additional staging
chains, the simultaneous-output failure and S4/S5 obligations remain explicit.
