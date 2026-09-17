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

## Limitation and follow-up

The auxiliary Ninja invocation stalled in this host although direct formal
MSVC invocations succeeded; this is an execution-wrapper issue, not a source
failure. Owner runtime acceptance of an actual BOP 7 redirector workload
remains pending. The separately queued COMMAND stream/pipe package owns
`>`, `>>` and `|`; VDMREDIR does not.
