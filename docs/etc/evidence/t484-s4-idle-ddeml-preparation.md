# T484 S4 idle and DDEML preparation

P14 gives test input copying/renaming one CMD owner. prepare-test-inputs.cmd
accepts explicit source/leaf pairs into build-owned output, refuses existing
leaves and invalid names, and never deletes source or output. Failure is
nonzero; callers stop before execution. Its generated CALL-label script uses
CRLF while the authored template remains LF. No new compiled helper is added.

Idle retains its build-only output names, hash-bound prebuilt input selection,
shared/separate producer selections and existing guest lifecycle assertions.
DDEML keeps its identity-bound original app-local DLL, authored probe variants,
private desktop and fresh runtime package identity. Both delegate final test
file placement to the existing CMD physical stage. Remaining Copy-Item calls
copy result receipts, not runtime inputs. No original guest or production
source/image changes; winnt.zip is untouched.

## Actual verification

build/t484-s4-idle-ddeml records PID26964,
2026-11-24T17:39:26Z-17:43:06Z,exit0, under the existing modern pwsh/Userenv
runner. Cached full build passes. Separate real WOW workers show first process
exit and surviving peer idle. SharedTasks with LifecycleOnly shows two distinct
overlapping tasks, independent retirement and surviving registrar. This is the
approved lifecycle slice, not a new claim for the stricter cross-task vector
identity contract.

DDEML init, register and direct-self-callback cases all pass. Actual loaded DLL
SHA256 is4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443.
Register produces2 callbacks (80a2h/80d2h); direct control produces3. Release,
unregister, free and uninit assertions remain. WindowWordControl is not a new
runtime claim in this batch; it uses the same verified preparation route.

PID27768,17:43:42Z-17:44:21Z,exit0, verifies binary rename and refusal to overwrite,
traverse, copy missing/directory inputs or write outside build. Separate/shared
prebuilt routes both pass build-only and produce the same three executable
hashes as their real-run generated inputs. This is identity/provisioning proof,
not a second guest execution claim.

Final11 raw product inputs and22 dual-published hashes are unchanged. No
changed-image publication or new Full regression is claimed. P14 closes two
frozen entries:343/355 disposed,12 pending;24 overlapping supplemental chains
remain pending. S4 and S5 remain open.
