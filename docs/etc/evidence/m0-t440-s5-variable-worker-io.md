# T440 S5 — bounded variable worker I/O

## Scope

S5 replaces the direct worker/NTCON protocol's fixed 16 KiB embedded payload
array with a fixed wire header followed by the record's actual payload. The
maximum actual payload is 1 MiB. A larger video frame is an ordered sequence
of bounded data records and receives one reply on its final data record.

The record header deliberately excludes the local payload pointer. A sender
borrows its request bytes, the NTCON receiver owns one reusable bounded
allocation for the record it is receiving, and a worker client owns reusable
reply storage. Ordinary records neither allocate nor transmit a MiB.

## Regression found during publication

The first publication caused palette-indexed DOS graphics programs (the owner
reported `WINSTD.CMD` and `mysmb16.exe`) to report `ERROR_NOT_READY` as “The
device is not ready”. This was not a Win3.1 configuration fault.

The faster S5 transfer exposed an existing ordering: a graphics surface can
become active before its guest palette is installed. `capture()` called
`ntvdm_console_bitmap_copy()`, received `ERROR_NOT_READY` for the unresolved
palette, and the shared publication thread treated it as terminal. The repair
keeps the graphics surface dirty and returns an empty successful capture for
that one not-yet-publishable state. The already-existing palette-install path
marks it dirty and signals the publisher again. Allocation, VGA state and
publication ownership remain unchanged.

## Verification

| Check | Result |
| --- | --- |
| x86 `product-programs`, `console-client-test.exe`, `console-video-test.exe` | Pass |
| `console-client-test.exe --variable-video` | Pass: 307,200 B is `BEGIN` plus one payload record; 1,310,720 B is `BEGIN` plus two payload records and one final reply |
| `console-video-test.exe` | Pass: production frame staging/dispatcher contracts |
| Dedicated `verify-console-graphics-publication.ps1` | Pass: 200 invalidations coalesce; a palette-late `DIB_PAL_COLORS` surface defers rather than poisons publication and publishes after palette installation |
| AMD64 `ntcon.exe` and `ntvwm.exe` builds | Pass |
| Attached-console `run16 mysmb16.exe` smoke | Returned `EXITCODE:0`; no device-not-ready termination |
| Published/release identities | All ten `assets/release` files hash-equal their `O:/winnt/system32` counterparts; changed `ntvdm.exe` and `VDMREDIR.DLL` are hash-equal to their x86 build outputs |

The background-process smoke is intentionally not used as a product witness:
it lacks the caller Console required by the normal frontend route. The
attached-console invocation above is the relevant automated regression smoke.
The owner retains interactive Win3.1/Window acceptance, especially Window
mode.
