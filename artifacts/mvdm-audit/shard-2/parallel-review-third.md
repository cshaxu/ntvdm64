# Shard 2: original WOW GDI font, text and metafile dispatch

This group advances the existing [GDI source review](wow-gdi-text-font-metafile-review.md).
The [observer](parallel-review-third-observer.py) checks both original source
editions, current source, the exact x86 Ninja graph and valid object dependency
records, and the **currently released** WOW32 image. The [receipt](parallel-review-third-evidence.json)
contains exact SHA-256 identities, object/map input hashes, each resolved GDI
ordinal, its pointer in `aw32WOW`, the selected object and relocation-masked
function-body comparison. The selected image and build code sections match;
package metadata is not interpreted as identical whole-image bytes.

Retail `W32` entries hold a single pointer. The original ordered kernel and
USER table sources contain 310 and 539 entries, with two separators, so the
GDI table starts at index 851 in `aw32WOW`. Each of the 28 rows below has its
actual `wowtbl.obj` relocation checked against the original `wgtbl2.h` ordinal,
the released image pointer and its selected original object. This is stronger
than a matching function name in a map. It proves initial linkage, not a
guest call, dynamic table-offset setup or runtime callback behavior.

| File | Proposed role | Matched original GDI ordinals | Remaining evidence |
| --- | --- | --- | --- |
| `wow32/wgfont.c` | partial candidate | 56, 57, 70, 119, 136, 350, 353, 411 | Font creation and resource errors, enumeration callback conversion/reentry and retirement, per-ordinal guest dispatch and task cleanup. |
| `wow32/wgtext.c` | partial candidate | 8, 9, 10, 33, 89, 90, 91, 92, 93, 345, 346, 351 | Guest strings/counts, encoding, clipping and alignment, output copyback, per-ordinal negative results and reentry. |
| `wow32/wgmeta.c` | partial candidate | 123–127, 151, 175, 176 | Metafile conversion, handle/record ownership, callback reentry, validation and teardown, and failures. |

`GetTextExtentPoint` is a separate selected `wgdi31.obj` owner, not an
additional `wgtext.c` ordinal. `wgfont.c`'s `WG32EnumFonts` calls the original
`W32EnumFontHandler`, which maps the guest name and invokes host `EnumFonts`
with `W32EnumFontFunc`; this handoff does not establish the callback's full
guest-memory or task lifetime. `wgmeta.c::WG32EnumMetaFileCallBack` copies
records and handle tables, calls `CallBack16`, then copies them back; source
order does not prove correct cleanup during nested termination or failure.

All three files have 100% original source-text retention against both source
editions. The restored **behavioral** percentage is unmeasurable from linkage
and source identity; they must not become `full`. Historical T456 public-host
seam tests and bounded product workloads remain useful but are not tests of
every original thunk, callback and failure path. Their identities and limits
remain in the earlier review.

No guest, product image, installed package or shared ledger was changed; no
guest build or executable runtime test was performed in this batch. Main
review must replay the observer and independently check the per-file scope
before applying these partial candidates to the shared ledger.
