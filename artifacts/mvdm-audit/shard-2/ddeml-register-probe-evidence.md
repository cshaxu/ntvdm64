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

## Read-only CCPU snapshot

A further isolated registration run used the same `77D888...67C87` probe;
`ddeml-register-ccpu-snapshot.json` (SHA256
`96C17861D08C8555D9711DA9837833AC3FA414BE1A76F74E6F6BDC0519E30CF1`)
was collected with `tests/observation/read-ccpu-paged.py` without suspension
or process writes. The isolated worker image and the image paired with the
link map were identical SHA256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`.
The snapshot reported CS 021f, IP aeb7, code beginning EB 03 C4 C4 51;
the 24-byte instruction prefix occurs in deployed `KRNL386.EXE` at file offset
0xc897, and the matching repository retail3 kernel map places
`WOW16SLOWVECTOR` at 0001:aeb9 and `WOW16CALLCONTINUE` at 0001:aebc.
This supports that the sampled guest was at the original WOW16 call/return
bridge; one non-atomic snapshot does not prove it was stuck there or identify
which call it was servicing. The run again timed out and did not establish
callback delivery or a product root cause. The registration pre-call identity
receipt SHA256 is `3642EB0F159ACE39300CBBBF8F38CBD27087204DDA4630D13DE3AECCDBAC4F43`;
the no-registration control receipt SHA256 is
`B9418ABC8CE9E9B3E5BF9B4B568DFC10476B0CF201FEDBEBF730B80EB926B5AA`.

## NE mapping and repeated non-atomic snapshots

The r10 package KRNL386.EXE SHA256 is
`D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A7AF4`.
Its NE segment-1 file offset is 0x19e0, length 0xd8b4; segment offset aeb7
therefore maps to file offset 0xc897 and its bytes match the snapshot.
Although the retail3 comparison executable has a different full-file SHA256
`BCED7650A5B5D7AC25955AB2A21BC53FF06DE1C35B25FAD0690390135B093A03`,
the entire segment 1 is byte-identical (segment SHA256
`79B3C4B7A6179AA247F59265A253DADE4489A4CDC90F5C983689B5A1E7F709CD`).
Its map labels 0001:aeb9 WOW16SLOWVECTOR and 0001:aebc WOW16CALLCONTINUE.
`kernel31/wow16cal.asm:331-351` identifies the instruction at aeb7 as the
jump following the FastWOW host call, before the common continuation; slow
vector is an alternate path. This mapping is well supported for the code
segment, but does not independently prove which guest API called the bridge.

In the repeat run, three read-only snapshots were taken at local timestamps
23:34:13.123, 23:34:13.851 and 23:34:14.491 (individual capture durations
not atomic). All show CS:IP 021f:aeb7 and SP 0xe82, and identical reported
registers, segment structures and first 80 stack bytes. Full captured JSON
files `ddeml-ccpu-repeat-{1,2,3}.json` each have SHA256
`4E0CF401A364531042D0AB719E32271BD4E1CC525C7478750C60159463EC5163`.
This repeated state is consistent with the guest CCPU not advancing over
this interval; the host may be executing or waiting while the guest CPU state
is held. It does not distinguish nested callback, host wait, infinite loop,
or any particular API and does not prove reentrancy. The test worker and
temporary probe were cleaned up; the original guest was not modified.
