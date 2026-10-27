# Registration probe: bounded Win3.1 observation

The independently authored NE probe `ddeml-register-probe.asm` is built via
`nasm -f bin ... -o ddeml-register-probe.tmp.exe`; runner is
`ddeml-register-run.ps1 w31`. The 2-segment NE check found KERNEL/DDEML
imports, initial CS=1, SS=2. Probe SHA256 for the staged version:
`09A3FF763DAA9D49206058F7E9FCE9BB79383F13DCAC40E8BEACDA736F517457`.
The input app-local W31 DDEML DLL was checked against SHA256
`6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B`.
This is an independently built test program; original guest was not rebuilt.

First run (probe SHA256 `962874D70FB571A3FEFB2C3655BA74A0ADA2EE8A35F4A8C1A7589D670560C737`)
was supervised for 40 seconds and timed out; saved as
`ddeml-register-w31-first.observer.txt`. It had no milestones and did not
produce a valid result receipt. Stage-instrumented rerun likewise timed out;
`ddeml-register-w31.stage.bin` is byte `04`. It is written after return of
DdeCreateStringHandle and before attempting DdeNameService(DNS_REGISTER).
Therefore the last recorded point is immediately before the register call;
no subsequent return marker was written. This does **not** prove that the
register call is the ultimate cause: callback/thunk errors, message handling,
probe defects, and receipt/observer failures remain possible. There was no
register callback log or final receipt and no loaded-module path receipt from
this run; copied input hash is not proof of actual loaded module identity.

Control: the previously captured `ddeml-init-w31.result.json` used the same
app-local DLL in client-only mode and successfully initialized/reinitialized
and uninitialized. That is a launch/loader/init control, not server-mode or
registration control. No new debug-DLL run or transaction test was performed.
The runner's finally block completed its isolated process/stage cleanup on both
runs. No original guest or product source was modified. Retain pending status.
