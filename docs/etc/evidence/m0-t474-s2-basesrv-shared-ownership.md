# T474 S2 — BaseSrv shared source ownership

## Ownership and dependency decisions

The shared BaseSrv client core and launch extension, local scope, classifier
binding, copied message/payload mechanics, Base command marshalling and
public binding declarations are owned by `src/common/base_srv`. Client core
retains one process-local authenticated connection; launch borrows it. Scope
captures the calling process's logon and package identity. Server authorization,
peer validation, receipt storage, delivery journals, tasks and lifecycle remain
in NTSRV. Both use the unchanged copied service protocol. The scope token
helper retains its prior logic and is shared by capture and service authorization.

The former mixed binding and transport archives are split: product clients
select `common-base-srv-bindings.lib` and `common-base-srv-transport.lib`;
NTSRV also selects its private `opennt-base-bindings.lib` and
`broker-transport.lib`. Run16, NTCON, NTVDM, NTVWM, NTMON and both Hooks
have no NTSRV-private production source edge in the inspected build graphs.
WOW32 uses its original NTVDM provider ABI; that runtime ABI is not a source
ownership edge. `worker-base` now uses the common client declaration.

The original non-MVDM VDM source changes only its project-added client-binding
include path, registered as OPENNT-HOST-243. Original execution policy, guest
bytes, and wire formats are unchanged. The original baseline has 94 WOW32
root headers; the stale 104-header generator assertion was corrected to match
both the committed tree and the checked main baseline.

## Local verification

Fresh x86 NTVDM, VDMREDIR, WOW32, Hook32 and AMD64 NTSRV, Run16, NTCON,
NTVWM, NTMON and Hook64 built from this worktree. The source-ownership graph
gate and injected-private-source negative control passed. Native Base RPC
core/launch source and link-map checks passed with five negative controls.
The VDM message/payload fixture, service reservation and lifecycle fixtures,
idle policy fixture, package-identity mixed/missing negatives and 118-assertion
Hook installation/propagation fixture passed. NTVDM export ordinals and 63
immutable guest files passed. The locally staged ten-image package matches all
fresh build input hashes; exact publication identity is recorded in Status.

The isolated Control suite passed 11 of 12 gates, including real RPC,
version negatives, native GUI, DOS/terminal return, relaunch, isolation and
retirement. The nested-window handoff did not pass: scripted input was not
delivered and the observer timed out. A separate candidate recheck and an
S1 baseline control reproduced the same input-delivery timeout; the baseline
comparison does not prove a root cause or turn the result into a pass. The
window-owned-console variant also timed out. Reports are in
`build/t474-s2/control-log4`, `handoff-recheck`, `handoff-baseline` and
`handoff-owned`. The ten-image candidate e34caccf-13d8-4999-9be2-183ba247b6ee
is locally published to assets/release with matching manifest hashes. S2 remains
open because this gate did not pass.
This local-only work does not publish to the main worktree runtime or push to a
remote.
