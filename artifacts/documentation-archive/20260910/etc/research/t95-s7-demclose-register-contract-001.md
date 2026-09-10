# T95 S7 DEM Close Register Contract 001

## Question

Does the contained adapter's reached `50:02` response have authority to write
general registers on successful close?

## Source and trace evidence

`refs/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:137-152` enters
`SVC_DEMCLOSE` with `AX:BP` as the handle and `CX:DX` as the optional
position. On return it restores its saved `BP`, clears the SFT reference
count, and branches only on CF. The historical host handler,
`refs/opennt/base/mvdm/dos/dem/demhndl.c:28-85`, documents successful
`demClose` as CF clear and failure as CF set with AX status; its success paths
call only `setCF(0)`.

Fresh source-built trace r23 (`t95-s7-runtime-trace-022-20260811-001`) puts
the final close at `CS=0032`, `RIP=61A3`, with `AX=4005`, `BP=4E53`,
`CX=DX=FFFF`. NTDOS's `STARTCODE=2250h` means the corresponding on-disk code
is `0x3F53`: `C4 C4 50 02`, then `JNC`, `POP BP`, SFT cleanup and the normal
DOS return path. Thus it is the ordinary final close path, not an unknown BOP
or a Bochs device event.

## Correction

The prior adapter implementation reused a generic result constructor and
wrote AX, BX, CX, DX and BP as zero on successful close. That contradicted the
source contract. `bx_ntvdm_dem_readonly_file_v1_close` now returns only the
generic `RIP+4` resume plus CF on success; no GPR delta is emitted. Its
invalid-handle failure writes only source-defined AX=6 and CF.

This is an adapter-only ABI correction. It adds no file capability, BOP
selector, Bochs change, guest-memory operation, or host integration.

## Runtime result

Fresh r24 (`artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r24`,
binary SHA-256
`CB8C595AA7C0F1AA1EC68827E37B6B53AFD249BE1E12D38B4E6D1BE5E9AB5C7E`)
confirms every reached successful `50:02` has `delta=00`. The last one retains
`AX=4005`, `BX=0005`, `CX=DX=FFFF`, `BP=4E53` and resumes `61A3 -> 61A7`.

The subsequent `0000:019D` stack underflow still occurs. The correction is
therefore source-required but **not** the proven root cause of that later
fault. No additional BOP, device, or adapter behavior is admitted from this
result; the next evidence must be an ordinary-instruction control-flow trace
from this correct close continuation to the fault.
