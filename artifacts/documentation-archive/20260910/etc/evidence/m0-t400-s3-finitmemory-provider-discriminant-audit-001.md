# M0 T400 S3 — WRITE `FInitMemory` provider/discriminant audit

## Scope and result

S2 reduced the ordinary x86 `WRITE.EXE` failure to the eight direct `FALSE`
returns in selected original `FInitMemory`. This audit maps each return to its
immediate original provider and asks whether the existing passive Console-owner
observer can distinguish it. It cannot: its process/window census has no guest
local-heap, GDI-object, DOS-file, or API-result channel. No repair is selected.

## Original return ledger

| `FInitMemory` return | Immediate original provider | Source-visible effect before false return | Existing passive witness | Disposition |
| --- | --- | --- | --- | --- |
| `!FCreateRgbp()` | `GlobalCompact(0)` chooses a size; one or two KERNEL `LocalAlloc(LPTR, cb)` attempts provide storage | A successful allocation exists only in WRITE's local heap; failure creates no window or file | None | Retain as a KERNEL local-heap branch. |
| `vhReservedSpace == NULL` | KERNEL `LocalAlloc(LHND, cbReserve)` | A private discardable reservation only | None | Retain as a KERNEL local-heap branch. |
| FCB `HAllocate` overflow | WRITE `HAllocate`: KERNEL `LocalAlloc(LHND, cb)`, then WRITE `FTryGrow`, then one retry | A successful handle is private to WRITE; `FTryGrow` only rearranges/discards WRITE page buffers | None | Retain as a WRITE-heap/KERNEL-local-heap branch. |
| `!FSetScreenConstants()` | GDI `CreateBitmap(1,1,1,1)` or USER `GetDC(hParentWw)` | `hbmNull` may exist before `GetDC`; no child control is created in either failure case | Current census sees HWND topology, not GDI object or DC result | Retain as a GDI/USER branch. |
| `!FInitDocs()` | WRITE `HAllocate`, `HszCreate`, and `DocCreate` for private document records | Private Write heap/document state only | None | Retain as a Write-heap composite branch. |
| `!FInitProps()` | Write property initialization followed by `HAllocate` for `vhgchpFormat` | Private Write heap/property state only | None | Retain as a Write-heap branch. |
| `!FInitFiles()` | `FnCreateSz` first allocates an FCB, then `FCreateFile`; later `HAllocate` allocates two BTE handles | If `FCreateFile` succeeds it leaves an open scratch file marked `fDelete`; later allocation can still fail | Current census has no guest filesystem channel | Only candidate with an externally material side effect. |
| emergency-save `HAllocate` overflow | WRITE `HAllocate` / KERNEL `LocalAlloc` retry path | Private save reserve only | None | Retain as a Write-heap/KERNEL-local-heap branch. |

`FInitDocs` and `FInitFiles` are composite original routines, so their rows do
not pretend that one leaf call has been proven. The table records the immediate
provider selected by `FInitMemory` and the leaf family that can make it false.

## Scratch-file contract

`FInitFiles` calls `FnCreateSz("", cpNil, dtyNetwork)`. Its original chain is
`FnCreateSz -> FCreateFile -> KERNEL.97 GetTempFileName`. The selected KERNEL31
`IGetTempFileName` implementation takes its temp drive, uses guest `TEMP`
unless `TF_FORCEDRIVE` is set, otherwise calls the guest Windows directory
provider, and issues DOS create (`INT 21h`, AH `5Bh`) for a generated
`~<up-to-three-prefix-characters><hex>.TMP` name. `FCreateFile` then
normalizes the name, allocates a Write heap string and opens it.

Thus a live guest-filesystem watcher could prove the `FInitFiles` chain reached
and passed the create step. It could not, by absence alone, distinguish an
earlier `FInitMemory` return from an early `FInitFiles` failure, nor prove the
final emergency-save allocation. The existing observer does not watch that
filesystem, so its prior post-run directory listing is not causal evidence.

The active WOW32 table marks KERNEL.97 `GETTEMPFILENAME` as `LOCALAPI`, while
the active headers also declare `WK32GetTempFileName`. This does not establish
a missing host provider: the selected original KERNEL31 `IGetTempFileName` is
the local owner and a `LOCALAPI` table arrival is an error guard. T400 S5
formally verifies that distinction.

## Verification and next boundary

The ordinary diagnostic-free S2 result remains unchanged: native menu-bearing
parent present, no WRITE document children, and the original low-memory dialog.
No guest/product instrumentation, CCPU transport, selector/IP inference,
memory-size adjustment, input injection, private USER/CSRSS work, or x64 work
was used here.

A later packet may introduce only an external, read-only, live census of the
already selected guest system root while preserving the same Console owner and
no-input run. Its acceptance value is bounded to observing a scratch-file
create/delete transition; it cannot select a repair without fresh direct
evidence tying the transition to the original return path.
