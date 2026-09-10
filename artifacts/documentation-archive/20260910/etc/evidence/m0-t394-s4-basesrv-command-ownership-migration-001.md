# M0 T394 S4 — BaseVDM/COMMAND ownership migration

Moved the admitted `mvdm_command_guest_state.{h,c}` and
`mvdm_command_native_child.{h,c}` paths from `softpc` to `basesrv`, together
with the scalar-state fixture. All app, COMMAND and DEM consumers and the
T370/T310 graphs now resolve the final owner. No body, ABI, packed record
layout, guest lease, snapshot lifetime, child worker or error rule changed.

Fresh direct MSVC `/MT` verification passed: scalar-state fixture x86/x64
exit 0; T370 native-child lifecycle fixture x86/x64 exit 0. The latter covers
guest command/environment/standard-handle snapshot, BaseVDM re-entry and
completion cleanup. A source/build/test sweep found zero old SoftPC paths.

S4 is closed; S5 is an audit-only function-level termination/observation and
original-host ABI ownership map.
