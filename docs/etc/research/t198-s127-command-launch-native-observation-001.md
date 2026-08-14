# M0 T198 S127: One Native COMMAND Launch Observation

S127 rebuilt and executed exactly one provenance-locked native fixture at
`artifacts/build/t198-s127-command-launch-native-r1`.  The retained
`t198-s97-post-bootstrap-native-probe.json` says `runs: 1`, uses the locked
S93 COMMAND/SHARE inputs and the current composition manifest, and records
guest exit code `2`: the fixture's bounded observation stop, not a normal
target exit.

`run.log` SHA-256 is
`A366B233C33819B28091AE1DA687D461383B7B313771D67A7CE0D8660FA987D9`.

## Result

The prior S124 path stopped at `54:01` at `95AB:03C3`.  This run instead
reaches its next terminal BOP at `54:0E`, `95AB:05DB`.  Therefore the
assembled `54:01` transaction was accepted and resumed past its four-byte
instruction; it did not decline, fall through to the invalid-opcode path, or
claim target completion.  The direct evidence is the later canonical BOP and
the new terminal address, together with the same retained DEM lifecycle
through AUTOEXEC EOF and close.

`54:0E` is `SVC_GETKBDLAYOUT`, as defined in the pinned
`base/mvdm/inc/cmdsvc.h`; `dos/command/cmddisp.c` maps it to
`cmdGetKbdLayout`.  It is a COMMAND console/layout boundary, not a Bochs CPU,
device, DOS-kernel, or DEM filesystem requirement.  The existing source audit
records its bounded en-US no-install (`DX=0`) outcome, but S127 neither
reuses nor changes that helper.

## Non-claims and next action

This is one native observation only.  It does not prove that the selected
target executed, that a host command queue exists, or that `54:0E` is already
present in the new boot-namespace composition.  The next admitted static task
must decide whether the existing source-derived keyboard-layout provider can
be reused through the composed COMMAND plane with its exact original ABI and
without reintroducing console/VDD/Win32 dependencies.  No second native run
is authorized by this evidence.
