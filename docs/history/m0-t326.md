# m0-t326 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t326-softpc-pre-bop-startup-continuity-closure-20260831.md

# M0 T326 — SoftPC pre-BOP startup continuity closure

T326 closed the startup-continuity gate without admitting a trace-driven BOP
repair. A fixed, console-owning, non-debug x86 observation reached original
`MS_bop_0` service `50h:11h`; the original DEM dispatch table maps that service
to `demLoadDos`. Later BOP markers establish return to CPU40 continuation.

The same selected product graph linked on x86 and x64 after restoration of the
original `FSCTL_QUERY_FAT_BPB_BUFFER` declaration in the existing Win32
adapter. No SoftPC fixed-disk algorithm, BOP provider, guest media or
observer input changed.

The run subsequently terminated with `0xc0000005` at address zero. Because it
is observed after the original `demLoadDos` crossing, it transfers to the
minimal DOS guest execution and controlled-return package; it is not a
pre-BOP defect and does not establish guest-program execution.

Evidence: [fixed SoftPC pre-BOP startup cut](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t326-s1-fixed-softpc-pre-bop-startup-cut-001.md).

