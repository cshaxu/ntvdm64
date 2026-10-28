# Original DOS ANSI and keyboard-layout source roles

This read-only shard-2 group covers eight currently pending physical paths.
The [observer](dos-ansi-keyboard-observer.py) and [receipt](dos-ansi-keyboard-observations.json)
verify each current file against both pinned original editions and the
DOS16/WOW32 frozen catalog identities. Both recipes match primary OpenNT.
Source retention is 100% for these files; that is not evidence of guest
runtime behavior or a percentage of semantic restoration.

| Path | Proposed role | Source evidence and outstanding boundary |
| --- | --- | --- |
| `dos/v86/dev/ansi/ansi.skl` | excluded candidate, historical guest build input only | Original ANSI makefile declares `ansi.ctl` from this message skeleton and country messages, then uses that control data for `parser.obj`; no proof of source-to-installed-binary lineage or message behavior. |
| `dos/v86/dev/ansi/ansivid.inc` | excluded candidate, historical guest build input only | Original recipe declares this video mode table as an `ansiinit.obj` assembly dependency. Preserve ANSI video behavior and mode coverage as distinct guest obligations. |
| `dos/v86/dev/ansi/ANSI.SYS` | pending | Exact original guest image identity is known, but the captured ZIP has no entry named ANSI.SYS. A filename search does not exclude renamed copies or prove runtime unavailability. Determine the supported packaging and actual loaded image before disposition. |
| `dos/v86/dev/keyboard/kdfbr.asm` | pending | Original recipe comments out the KDFBR object and actively selects KDFBR275. Their exact Brazil layout/variant relationship and runtime behavior remain unverified. |
| `dos/v86/dev/keyboard/kdfit.asm` | pending | Original selected recipe names KDFIT141/142; historical layout provenance and equivalence must be traced, not inferred from the Italian label. |
| `dos/v86/dev/keyboard/kdfru092.asm` | pending | Original selected recipe names KDFRU091; Russian variant differences and support remain unresolved. |
| `dos/v86/dev/keyboard/kdfyc116.asm` | pending | Original selected recipe does not name this exact variant; locale, alternate consumers and mapping behavior need proof. |
| `dos/v86/dev/keyboard/kdfyc117.asm` | pending | Original selected recipe does not name this exact variant; locale, alternate consumers and mapping behavior need proof. |

The current ZIP, installed `O:/winnt/system32/KEYBOARD.SYS`, and source-tree
`KEYBOARD.SYS` have the same recorded SHA-256. That is an identity observation
at the captured time, not proof of which layouts are present, that the source
produced the image, or that any key conversion works. Other directory paths,
localized or alternate media were not exhaustively searched. The original
recipes are evidence only and were never run: their cleanup targets delete
files. The owner prohibits changing or rebuilding guest images.

The coordinator must replay and review before applying any candidate. No
shared ledger, guest, installation, product binary, process or Ninja tree was
modified by this review.
