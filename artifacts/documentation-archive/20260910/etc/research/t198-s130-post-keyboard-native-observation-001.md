# M0 T198 S130: One Native Observation after COMMAND `54:0E`

S130 rebuilt and executed exactly one locked native fixture at
`artifacts/build/t198-s130-post-keyboard-native-r1`.  Its retained
`t198-s97-post-bootstrap-native-probe.json` declares `runs: 1` and the S93
COMMAND/SHARE exact input contract.  The bounded observer's exit code is `2`,
not a normal target exit.

`run.log` SHA-256 is
`AD208330A4F6189D8425AF15FB3719B84B520033809FE8AEA10267C470132074`.

The S127 run stopped at `54:0E` / `95AB:05DB`.  This run continues beyond
that location and stops at `54:04` / `0032:67A3`.  It therefore proves that
the composed fixed `DX=0` no-install result for `54:0E` was accepted and
resumed through real guest control flow.  It does not establish target
execution, console initialization, or general keyboard support.

`54:04` is `SVC_CMDGETCURDIR`.  It is a COMMAND current-directory boundary;
the next action is a static original-provider/reuse audit, not a Bochs,
device, or filesystem expansion and not a second S130 run.
