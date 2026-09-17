# M0 T419 — Terminal task-manager closure

T419 delivered the owner-requested native Terminal task manager for product
workers. `dtmgr.exe` displays the authenticated BaseSrv worker projection,
continues with an empty unavailable-broker view when BaseSrv is absent, and
requests selected-worker termination only through BaseSrv's server-owned
capability. It never enumerates or directly terminates arbitrary Windows
processes.

The final S5 repair aligned the client RPC caller identity with the existing
real-handle contract, retained the launcher-prepared worker capability for
BaseSrv-owned termination, and restored generic compact first-option launch
normalization such as `run16 command/c ver`. The product now uses explicit
executable component roots:

```text
src/run16-exe/   -> run16.exe
src/basesrv-exe/ -> basesrv.exe
src/ntvdm-exe/   -> ntvdm.exe
src/dtmgr-exe/   -> dtmgr.exe
```

The former `dtaskmgr.exe` deployment name was retired after `dtmgr.exe` was
verified in `O:\winnt`. The x86 build, BaseSrv lifecycle fixtures, clean-Console
launch matrix, and established COMMAND/MEM/EDIT regressions passed. Exact
hashes, commands and the deliberately bounded worker-termination evidence are
in the [T419 S5 evidence](../etc/evidence/m0-t419-s5-dtaskmgr-rpc-and-run16-compact-entry.md).

T419 does not certify idle-broker lifetime policy, full WOW16 execution, or
the completeness of any original MVDM package. Those are owned by subsequent
queued package-completion work.

## Post-closure correction

The owner accepted a focused DTMgr/BaseSrv correction on 2026-09-17 after
interactive nesting exposed a non-original display stack: five nested COMMAND
instances could appear as depth seven.  The correction removes that parallel
state machine and projects the existing original `DOSRECORD` chain instead.
It is recorded in the [post-closure evidence](../etc/evidence/m0-t419-post-closure-dtmgr-record-projection-repair.md).
T420 S3 remains the separate `video` package unit; this correction does not
change its package scope.
