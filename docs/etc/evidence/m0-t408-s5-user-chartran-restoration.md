# M0 T408 S5 — USER character conversion restoration

## Result

Restored the directly reached OpenNT USER slice
`windows/core/ntuser/rtl/chartran.c:WCSToMBEx,MBToWCSEx` under its original
relative path.  The prior 44-line adapter `MBToWCSEx` body and a 259-line
unselected duplicate carrier were removed.

The source slice is lines 1--311 of
`O:\repos.external\OpenNT\windows\core\ntuser\rtl\chartran.c` (external
SHA-256 `0f9f0b7fde3aaa475492a50f333745508698228c38fdbf9a21cbd4bfbead383b`).
The only source seam replaces the
NT4 imported `NlsAnsiCodePage` and USER allocator/free endpoints with bounded
modern ACP and `LocalAlloc`/`LocalFree` bindings.  It adds no conversion,
allocation policy or guest-memory publication.

## Verification

- Formal x86 WOW32 provider linked at
  `build/M0-T408/S5/r001-chartran-provider/wow32.dll`.
- Independent x86 fixture built and passed at
  `build/M0-T408/S5/r002-chartran-fixture/ntuser-chartran-fixture.exe`.
  It exercises ACP and UTF-8 branches, automatic allocation, terminator,
  invalid length, short-buffer prefix and reverse conversion.
- The ACP short-buffer row returned its original partial prefix (`ab`, count
  2), proving a semantic distinction from the removed generic façade.
