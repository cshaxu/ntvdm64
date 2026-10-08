# M0 T437 S3 — PIF and hash utilities

## Delivered tools

- `tools\\pif\\PIF.EXE` is an AMD64, format-only PIF utility. It supports
  `inspect`, `verify`, `create`, and `set`; it validates the standard-record
  checksum, linked header bounds/cycles, and recognised Windows 386/NT 3.1
  record sizes before reading or editing. Unknown extension records are
  preserved by `set`.
- `tools\\hash\\HASH.EXE` is an AMD64, one-purpose SHA-256 printer. Its only
  accepted argument is one file path; on success it prints one uppercase
  64-character digest followed by a newline. It neither compares digests nor
  reads manifests or installation configuration.

The independently authored sources are respectively under `src/addon/pif/`
and `src/addon/hash/`. Both `BUILD.CMD` files select the AMD64 MSVC environment
and link with `/MACHINE:X64`.

## Focused verification

`tests/component-integration/pif_hash_tool_test.ps1` passed against the
shipped tool copies. It verifies:

- PIF create → verify → inspect → set round trip, including standard, Windows
  386, and NT 3.1 fields;
- rejection of both malformed input and a fixed-record checksum mutation;
- HASH output equality with PowerShell's independent SHA-256 result, exact
  uppercase format, and rejection of a nonexistent file.

The delivered `PIF.EXE` and `HASH.EXE` PE headers both report machine
`0x8664` (AMD64).

## Boundary retained

These utilities deliberately do not migrate the Win1.01 installer. A future
admitted S may make `SETUP.CMD` orchestrate them while retaining the original
Setup-owned `MOUSE.DRV` replacement/`WIN100.BIN` assembly path.
