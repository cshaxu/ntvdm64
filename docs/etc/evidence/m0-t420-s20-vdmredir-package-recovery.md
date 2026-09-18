# T420 S20 VDMREDIR package recovery

## Question

Can the selected original VDMREDIR DLL remain source-shaped in the x86 worker
while retaining only finite standalone guest-memory, ABI and unavailable-RAP
boundaries?

## Result

Yes for source composition and DLL linkage. `vrdlcpst.c`, `vrinit.c` and
`vrmslot.c` now match pinned OpenNT after newline-insensitive comparison.
`vrnmpipe.c` restores the original empty `VrTerminateNamedPipes` body and
removes the autonomous all-request worker stop path. The remaining mirror
differences are bounded async guest staging (`+30/-52`), synchronous NetAPI
guest-copy (`+60/-68`) and unavailable NT4 RAP conversion (`+15/-6`).

## Verification

- All 15 selected original C units compiled with the generated formal x86
  flags. The VDMREDIR target now force-includes the existing local
  cdecl-to-WINAPI thread facade at build scope, preserving original source
  spelling.
- The rebuilt asynchronous redirector contract fixture returned zero.
- `VDMREDIR.dll` linked successfully at
  `build/M0-T420/S20/formal-x86-001/VDMREDIR.dll` (241,152 bytes).
- The exact DLL was published to `O:\winnt\VDMREDIR.dll`, SHA-256
  `9B29F5C91703CF1931BC1A47EA76704E34179042FA03023056200FA18FF37EE`.

## Final acceptance and approved exclusions

The current matching formal package is
`build/M0-T420/S20/formal-x86-013/VDMREDIR.dll` (SHA-256
`7e4d189e5372a45e0b334bc003bb2c075b8ef74ae327640552fd56ac1b5d6a69`).
The complete DOS-guest family sweep, including UNC named-pipe open/read,
wait/state/peek/transact/call, asynchronous read/write and timeout, mailslot,
local NetAPI, synchronous and asynchronous NetBIOS, BOP 7 dispatch and
multi-task cleanup, is recorded in the linked
[operation-family matrix](m0-t420-s20-vdmredir-operation-matrix.md).  The
established `COMMAND -> MEM -> EDIT -> MEM` regression passed against the
same deployment.

Remote RAP transport remains excluded.  The owner also explicitly approved
exclusion of DLC support on 2026-09-18: the real `VDMDLC.COM` guest probe
reaches unchanged `VrDlc5cHandler` and its original `07` unavailable result;
Windows 11 supplies no DLC provider.  No replacement provider, historic
binary or fake-success shim is introduced.  Modern SMB is not excluded: the
UNC named-pipe witness uses the host SMB/UNC provider successfully.

## Superseded preliminary limitation

The auxiliary Ninja invocation stalled in this host although direct formal
MSVC invocations succeeded; this is an execution-wrapper issue, not a source
failure. Owner runtime acceptance of an actual BOP 7 redirector workload
remains pending. The separately queued COMMAND stream/pipe package owns
`>`, `>>` and `|`; VDMREDIR does not.
