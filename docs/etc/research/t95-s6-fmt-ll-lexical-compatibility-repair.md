# T95 S6 `FMT_LL` lexical compatibility repair

## Result

`BX-ABI-028` is implemented as a source-token-only repair. All 14 full-tree
`FMT_LL` string-literal adjacencies were separated by lexical whitespace across
exactly seven registered imported files. No macro definition, literal text,
format conversion, argument, numeric behavior, feature flag, adapter/DOS
term, or unrelated formatting changed.

## Affected files and counts

| File | Adjacent occurrences repaired |
| --- | ---: |
| `refs/bochs/gui/paramtree.cc` | 2 |
| `refs/bochs/gui/siminterface.cc` | 2 |
| `refs/bochs/iodev/hdimage/hdimage.cc` | 1 |
| `refs/bochs/iodev/usb/scsi_device.cc` | 3 |
| `refs/bochs/iodev/harddrv.cc` | 2 |
| `refs/bochs/iodev/pit.cc` | 3 |
| `refs/bochs/iodev/virt_timer.cc` | 1 |

The read-only inventory checked every `refs/bochs` `*.cc`/`*.h` source for
either direct form (`"..."FMT_LL` or `FMT_LL"..."`) before the patch. It found
exactly these 14 occurrences. The repair changes, for example,
`"0x"FMT_LL"x"` to `"0x" FMT_LL "x"`; it does not alter the three adjacent
literal/macro values.

## Verification and non-claims

`Test-BochsFmtLlLexicalBoundary.ps1` now scans the complete imported C/C++
tree, rejects any residual direct adjacency, requires all expected spaced forms
in the seven registered files, and asserts the five original `osdep.h`
`FMT_LL` definitions are unchanged. It passed with the recorded 14/7 count.
The retained #UD default-off/interception tests and the projection-tool test
also passed.

Conditional review strengthened that proof: the test now scans `*.c`, `*.cc`,
and `*.h`; derives the observed repair set from exact registered forms; rejects
an unexpected/missing file; and requires per-file macro-occurrence counts of
`2/2/1/3/2/3/1` in the table order. The strengthened test passed, so its
14-count is now an asserted invariant rather than a status message.

No config projection was rerun. No `/Zs`, compiler, object, link, executable,
configure, VS project, `bochs.exe`, or runtime operation occurred in this
patch package. A later syntax observation requires separate coordinator
admission.
