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

## Retained v2 host stack: hard-error dialog

The already retained `ddeml-register-w31-staged-v2.observer.txt` reports
NTVDM PID 14100 with executable base 0x00f00000 and thread 29892 frame 4
PC 0x010761ca. Its product image is the r10 NTVDM, identical in hash to the
msvc-x86-release image paired with the map (SHA256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`).
RVA 0x1761ca lies in `_mvdm_wow_hard_error_present`, whose map interval is
0x175f90 through before 0x176290. Disassembly of the exact mapped image shows
that address immediately follows an indirect CALL through IAT RVA 0x18b46c;
the image import table identifies this slot as `DialogBoxIndirectParamW`.
Source `src/ntvdm-exe/win32/wow_hard_error_dialog.c` constructs and invokes
that modal dialog for recognized WOW hard errors. This is strong retained
stack evidence that the sampled worker thread was within the dialog call when
the observer captured it, rather than proof that DdeNameService itself was
blocked in its implementation. No UI interaction was used.

The retained observer has frames but not the hard-error arguments or dialog
text; earlier guest CCPU snapshots do not expose those native arguments.
We therefore cannot identify the error code, offending module, call-site,
original trigger, or whether the independent probe or product caused it.
Host stack unwinding itself has ordinary limitations, and the saved trace is
one snapshot from v2; do not infer all runs had the same error. A future
owned isolated run needs read-only same-desktop window enumeration or a
bounded native stack/context reader for the exact worker, preserve dialog
text and process provenance, and distinguish an independently authored probe
fault from a genuine original guest DDE path. Do not dismiss the dialog or
invent a result.

## UUID-isolated GPF, direct thunk control, and captured fault fields

With the published `build/package-uuid/package-c` copied via
`New-PhysicalRuntimeStage -FreshPackageIdentity`, the separately authored test
loaded unchanged W31 DDEML (`6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B`).
Before registration its receipt logged MakeProcInstance callback 17af:00ba.
The private-desktop timeout observer reported General Protection Fault in
DDEML 0001:58ef. That NE segment-1 address maps to file offset 0x60ef; bytes
`ff 5f 04` decode as `call far [bx+4]`. The argument-push neighborhood is
consistent with a DDE callback but does not prove source lineage or the
contents of the pointer operand.

The independent `NO_REGISTER_CONTROL` plus `SELF_CALLBACK_CONTROL` binary
SHA256 `058077E3B9B803E508E57356DDD15843442ED94B2F0CF781038260AB5841274F`
called the test-created MakeProcInstance thunk with a full 28-byte argument
frame; callbackCount=1, server initialization and clean teardown succeeded.
Receipt SHA256
`691F40726D185987BF05E285951DB5C241F3FA7562896F9918D96AF92B4EEAE3`.
This is a direct-call control, not proof that DDEML saved the same pointer or
that cross-module callback and nested message delivery work.

A second UUID-isolated registration run explicitly enabled the existing
opt-in `MVDM_WOW_CALLBACK_CPU_TRACE_PATH` and selector filter `17af`.
The trace SHA256
`93217B371BA32B72D81F79EA1FE27A09D9012139357235DA7F6AEF69CE2D3079`
records vector 0d, error 0271, source 177f:58ef and bytes `ff5f04...`;
the matching private-desktop GPF observer SHA256 is
`E3146CCD70FD47B0E513E9BFBEB07BB3445199D07718DFD04E2F8600BCED0EC6`.
The pre-call module/thunk receipt SHA256 is
`AE52F6DA74D228D9A6F17D6B6FDD0559485D11F43CB38EC9103CF96125BC4D32`.
There is no `far-call` record for selector 17af; this witness filters after
operand decoding, so absence does not distinguish a fault before decoding,
a different target, or other trace limitations.

`ccpu386/c_xcptn.c:252` constructs an ordinary GP error code as
`(selector & 0xfffc) | EXT`. Under that ordinary-selector interpretation,
0271 means selector bits 0270 plus EXT=1, TI=0 (GDT). `GP_INT` has a separate
IDT construction path; the recorded dispatch does not record which GP branch
or xcode produced this error. It is therefore not proven that 0270 was the
far-call target or an invalid descriptor rather than some other selected
segment. The DPMI saved frame records CS:EIP, SS:ESP, flags and error, not the
fault-time DS, BX or `[DS:BX+4]`. The later CPU snapshot is already inside
exception/host-dialog handling and is not the fault context. The available
read-only tools and this log cannot reconstruct those missing values; a
fault-time breakpoint/diagnostic or an independently authored exception
handler with a validated control would be required. No such result is claimed.
No guest files or product source were modified, and only processes belonging
to the independent stage were cleaned up.
