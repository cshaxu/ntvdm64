# S10 shard 2: original WOW Clipboard ownership and format boundaries

`wow32/wuclip.c` is selected, extracted (`WU32SetClipboardData`,
`WU32GetClipboardData`, cache access and close entrypoints occur in the
current x86 link map) and differs from the frozen original: OpenNT SHA256
`C386AF38376945CB4FD141D878BCA8EF5140B15DCCC9695A0ADCB0941FBB0083`,
current `3B30B2AF4F1BBB588C051C15D03FBF9C7E1E299FFEA02BB4BCC74A74DF6C10FA`.
It remains partial. The original format conversion, guest cache and
`W32EmptyClipboard` own behavior; modern USER owns the actual clipboard.
The page-domain adapter translates only published live native/guest window
identities (DIV-454), and the generation projection guards stale original
cache use (DIV-452). NULL clipboard HWND remains different from rejected
nonzero foreign/retired identity. DIV-453 gates success and copy ownership
for the common HGLOBAL branch on native acceptance.

T460 S4 reports actual immutable-guest/native peer tests for CF_TEXT and
CF_DIB: 18 transfer, 13 callback and 7 task-exit checkpoints each for its
sealed S3 R5 package, with rejected ownership, replacement and delayed render.
This is strong bounded historical evidence for those two formats and listed
lifecycle paths, not a proof of all formats, arbitrary current-package build,
DDE conversations, or all allocator failure cases.

## Specific unverified failure branches

- `WU32SetClipboardData` common HGLOBAL branch conditionally caches the guest
  handle only after nonzero native return and frees the newly allocated copy
  on rejection. In contrast, `CF_HDROP`, `CF_METAFILEPICT` /
  `CF_DSPMETAFILEPICT`, and OLE object descriptor paths call
  `WU32ICBStoreHandle` unconditionally after the native call. Source inspection
  does not show corresponding rejection cleanup in those branches. For
  CF_HDROP `CopyDropFilesFrom16` allocates a new native global block; for
  metafile-pict both a global structure and a native metafile can be created.
  Thus a rejected transfer is a concrete candidate for retained guest cache
  or leaked native resources. Visibility of stale cache, exact format-specific
  ownership and each nested allocation's retirement remain unverified; do not
  apply a blanket GlobalFree that could mishandle delayed rendering, GDI
  objects or successful ownership transfer.
- The original dependency `wshell.c::CopyDropFilesFrom16` declares `h32`
  without initialization and returns it after the `if (GlobalLock16(...))`
  block. A failed initial lock therefore takes an uninitialized-result source
  path. This is an inherited source finding; no runtime repro or authority to
  edit the original is inferred. Source also computes a size from the locked
  16-bit block minus the 16-bit DROPFILE header; validate undersized data and
  provider behavior before claiming an exploitable overflow or selecting a
  minimal fix. `wshell.c` belongs to a different shard; do not change it here.
- The metafile-pict branch allocates HGLOBAL then dereferences the result of
  `GlobalLock` without an explicit NULL check. A lock-failure fixture should
  establish actual behavior and error/cleanup contract before treating this
  source observation as a reproduced defect.

A finite original-call fixture and independent guest/native peer should test
per-format no-open/rejected native transfer, initial guest lock failure, short
DROPFILE data, conversion/GlobalAlloc/GlobalLock failure, delayed NULL rendering,
accepted transfer, replacement and cache retirement. Instrument guest and host
handles separately, including nested HMETAFILE ownership, and ensure a failed
transfer cannot later free a still-owned guest handle or return a stale cache.
Decide source/host provenance and user-approved original-defect policy only
after those cases are evidenced. Do not infer other formats from CF_TEXT/DIB.
No test was run and no guest/product source was modified in this review.
