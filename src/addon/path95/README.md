# Win95 offline path utility

path95.c produces the AMD64 path95.exe. It discovers SystemRoot from a
supported CREG hive or prepares a fresh candidate with bounded path
substitutions and updated metadata. It uses public format documentation;
no libcreg implementation is linked or copied.

Build with `tools/build-addon.cmd path95` and publish with
`tools/publish-addons.cmd`. The
tools/win95-path scripts consume assets/release/path95.exe and own backup,
installation and recovery. This helper never joins a Run16 or worker runtime.

The explicit `replace-equal INPUT OUTPUT FROM TO` operation accepts only
CREG hives and nonempty, equal-length roots. It preserves allocation extents,
entry padding and free-list metadata; only matched REG_SZ bytes and enabled
RGDB checksums change. Both replacement operations require a fresh output.

`replace-text INPUT OUTPUT FROM TO` performs literal case-insensitive byte
replacement without a trailing path boundary, matching copied INI/CMD/NT
configuration preparation. It preserves other bytes and rejects CREG input.
Ordinary `replace` retains its existing path-boundary rules.
