# M0 T460 S1 — Clipboard owner matrix

**Status:** closed source-contract audit. This record admits a finite
format-transfer and delayed-render acceptance sequence; it makes no real
clipboard-interaction pass claim and changes neither guest media nor product
code.

## Question, inputs and method

The question was whether the selected WOW32 Clipboard path already preserves
the original owner, or whether a project-owned Clipboard provider, cache or
callback mechanism has displaced it. The review used the selected x86 source
manifest, the formal `wow32.dll` link graph, the registered USER handler ABI,
and the read-only OpenNT comparison copy. The current
`src/mvdm/wow32/wuclip.c` SHA-256 is
`C386AF38376945CB4FD141D878BCA8EF5140B15DCCC9695A0ADCB0941FBB0083`; it matches
`O:/repos.external/OpenNT/base/mvdm/wow32/wuclip.c` byte-for-byte. Line-ending
rendering makes an ordinary textual diff noisy, so the identical hash is the
comparison result.

The selected original dispatch table is `wutbl2.h`; the selected production
source manifest includes `wuclip.c`; and the formal x86 graph requires its
object in the `wow32.dll` link. `wow32.c` initializes the original format
table once and registers exactly two relevant original callbacks with USER:
`W32EmptyClipboard` and `WU32ICBStoreHandle`.

## Ownership and lifecycle matrix

| Family / entry | Original owner | Modern-host boundary | State authority and retirement | S1 disposition |
| --- | --- | --- | --- | --- |
| Open, close, enumerate, owner/viewer and registered-format queries | `wuclip.c::{WU32OpenClipboard,WU32CloseClipboard,WU32EnumClipboardFormats,WU32GetClipboardOwner,WU32GetClipboardViewer,WU32RegisterClipboardFormat}` | Public USER Clipboard APIs and public HWND conversion only. | Modern USER owns the live Clipboard lock, owner and viewer-chain state. The original thunk owns only 16/32 argument/result conversion. | Keep unchanged. No project clipboard state exists. |
| Text-like global-memory formats (`CF_TEXT`, OEM text, DIB, display text, SYLK/DIF/TIFF/PENDATA/RIFF/WAVE and owner display) | `WU32SetClipboardData` and `WU32GetClipboardData`. | Host `HGLOBAL` is copied to/from a distinct guest `HMEM16`; a raw host handle never becomes guest data. | The actual Clipboard data is host-owned after successful `SetClipboardData`; original `ClipboardFormats` holds the guest cache/ownership handle and retires it through `W32EmptyClipboard`. A failed/rejected host set must not be mistaken for an accepted transfer in S2. | Original conversion and cache remain sole owners. |
| `CF_HDROP`, bitmap/palette, metafile picture | `wuclip.c`, with original `CopyDropFiles*`, GDI handle conversion and metafile conversion helpers. | Public host handles/data are converted at the existing original GDI/VDM boundary. | `CF_HDROP` and metafile-picture guest allocations are retained in the original cache; bitmap/palette are converted through existing handle identity helpers, not a Clipboard registry. | Keep unchanged; S2 selects text and one structured format only after proving source-shaped cleanup. |
| Private/OLE descriptor formats | `wuclip.c::{W32ConvertObjDescriptor,WU32ICBStoreHandle}`. | Original lazy `OLETHK32.DLL` descriptor conversion, not a project OLE service. | Original linked-list cache owns its 16-bit handles and frees it on original empty notification. | Keep; W18's OLE alias result does not create a second Clipboard/OLE provider. |
| Delayed rendering / owner replacement | Original USER message delivery plus `WU32GetClipboardData`'s post-`GetClipboardData` cache re-read. `wmtbl32.c` explicitly selects `WM_RENDERFORMAT`, `WM_RENDERALLFORMATS`, `WM_DESTROYCLIPBOARD` and `WM_DRAWCLIPBOARD` as original no-parameter-thunk messages. | Modern USER delivers public messages to the original WOW window procedure route. | Host USER owns delayed-render request timing and Clipboard ownership. The original cache is authoritative for the guest handle once the owner has supplied it; the second cache lookup is the original guard against duplicating a just-rendered guest buffer. | S3 must prove actual deferred rendering, replacement and owner/task loss. Do not introduce a scheduler, timer or second callback registry. |
| Original USER → WOW cleanup and store callbacks | `wow32.c` registers `pfnEmptyCB=W32EmptyClipboard` and `pfnWowCBStoreHandle=WU32ICBStoreHandle`; both bodies are in `wuclip.c`. | `UserRegisterWowHandlers` is the sole existing callback table. `wow32-dll` merely exposes the already-registered `pfnEmptyCB` macro to original recovered USER control code. | Original `W32EmptyClipboard` frees every cached guest allocation and clears the only original cache; original `WU32ICBStoreHandle` replaces/free-retains exactly one guest handle per format. | Necessary binding, not a second callback/lifetime owner. |

## Failure, delayed-render and task-loss boundary

`WU32EmptyClipboard` invokes the original cache cleanup after its public
`EmptyClipboard` call, including when the host return is false. This is
byte-identical OpenNT behavior, not a project divergence; S1 neither masks it
nor claims that it is safe for every modern USER failure mode. S2 must first
separate accepted ownership transfer from rejected/abandoned data. S3 must
then verify that the original callback/cache route neither calls a retired
task nor leaks a guest handle when delayed rendering is replaced or the owner
dies. Any failure requiring a change must be fixed at the selected original
owner or its already-registered narrow host boundary, with a new documented
divergence; no new Clipboard manager or cache is admissible.

## Autonomous-substitute census

No project-owned Clipboard data store, format registry, delayed-render queue,
viewer-chain provider, task-loss cache or DDE transport was found in
`src/wow32-dll`. `wow_edit_memory_bindings.h` maps
`pfnWowEmptyClipBoard` to the one registered original callback table; it does
not implement or retain Clipboard state. The graph also finds no second
definition of `W32EmptyClipboard` or `WU32ICBStoreHandle`. Therefore S1 has
no autonomous implementation to delete and no production source repair to
publish.

## Evidence and finite handoff

S1's formal checks are source/build composition only:

- `verify-wow32-s1-formal-graph.ps1` proves the selected 77-source original
  manifest, `wuclip.obj` link inclusion and original dispatch inventory.
- `verify-wow-user-callback-abi.ps1` proves the complete registered callback
  ABI, including the single `W32EmptyClipboard@0` definition in `wuclip.obj`
  and the original caller in `wow32.obj`.

S2 owns real immutable guest/native peer exchange for a selected text format
and one structured format, including rejected and abandoned transfer
assertions. S3 owns deferred callbacks, owner replacement, cancellation/reentry
and dead-task cleanup. S4 owns repeated combined acceptance and only then may
hand the proven format/lifetime primitive to the queued DDE work. None may
modify or rebuild guest media.
