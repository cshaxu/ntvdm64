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

## Call-boundary follow-up

The next isolated run used probe SHA256
`73126159486E3322C162A60A342CB60C1B4191DBEB47BFAF734291675D2DC04C`.
It wrote marker 09 after all `DdeNameService` arguments were pushed and
immediately before the NE far call instruction. A callback-entry marker 10
and return marker 05 were added; neither was observed. The timeout observer
is `ddeml-register-w31.observer.txt` and the copied stage byte is in
`ddeml-register-w31.stage.bin`. This proves the probe reached the call site,
not that the callee or callback actually executed: control transfer,
MakeProcInstance callback execution, and stage-writing behavior remain
unverified. The callback marker itself uses DOS I/O and could perturb
reentrancy; absence is not conclusive. No final receipt proves which DLL
was loaded. There is still no matched server-mode no-register or real peer
control, so this does not isolate the problem to a product component.

## Server-mode control and callback-I/O control

The `NO_REGISTER_CONTROL` build (SHA256
`46170F839B4226BEA9C7F57E1E56512DD00DCA7553A25A4E9C740679944DD63F`)
uses the same server initialization, callback thunk and string operations but
skips registration and unregistration. `ddeml-control-w31.result.json` records
normal exit, the loaded W31 module path and matching hash, init=0, nonzero
instance and service, free=1, uninitialize=1; registration fields remain
0xffffffff sentinels. The initial runner assertion mistakenly interpreted
0xffffffff as signed -1 and falsely reported failure after saving the receipt;
its comparison was corrected to `[uint32]::MaxValue` and the saved receipt
was rechecked against the corrected condition. It was not rerun under the
corrected script. This control does not prove registration or callback behavior.

The separately retained registration pre-call receipt records the actual
loaded app-local DLL SHA256 `6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B`,
init=0, nonzero instance and service. W31 binary NE names resolve ordinals
2/3/21/22/27 respectively to DDEINITIALIZE, DDEUNINITIALIZE,
DDECREATESTRINGHANDLE, DDEFREESTRINGHANDLE and DDENAMESERVICE. Header
`wow16/inc/ddeml.h` defines the four handles as 32-bit and FAR PASCAL;
the probe pushes parameters left-to-right with each DWORD high word first,
uses 14 argument bytes for DdeNameService, and its eight-argument callback
uses `retf 28`. These are source/NE checks, not a dynamic stack validation.

A further registration probe removed all DOS I/O from the callback and retained
only in-memory counting. Its SHA256 was
`77D8880018E5B00775FF0949F1D38529BC91820F25630C2B8B6FB0B21CB67C87`;
`ddeml-register-w31.pre.json` confirms the module and pre-call inputs and
`ddeml-register-w31.stage.bin` again records 09 before the far call. The
observer again timed out. No post-call receipt was written, so the memory
callback count was not observable and the call cannot be claimed entered.
This removes callback DOS I/O as a necessary condition for the observed
probe timeout, but does not isolate a product bug or validate the callback
thunk. All test stages and processes were cleaned up; no guest was modified.
