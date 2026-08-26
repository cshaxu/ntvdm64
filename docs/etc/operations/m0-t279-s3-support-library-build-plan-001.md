# M0 T279 S3 — support-library build plan

## Architecture

`oemuni.lib` remains composed from its two original selected bodies and the
adapter-owned RTL binding. On x86 both original OEM bodies retain their
historical exports. On x64 the original bodies are compiled with only four
unsafe definitions renamed; `opennt-mvdm-support-overlay` exports the original
four names with source-derived, pointer-width-safe implementations.

`suballoc.lib` always remains the original `suballoc.c` body. Its callbacks
remain unresolved at static-library level by design: they are caller-provided
and must later be bound through `adapter-softpc` to the session guest-memory
mapping instance.

## x64 overlay scope

Exactly these original APIs are overlaid:

- `FindFirstFileOem` and `FindNextFileOem`: use `offsetof(WIN32_FIND_DATAW,
  cFileName)` rather than truncating two pointers to `ULONG`.
- `GetFullPathNameOem` and `SearchPathOem`: reconstruct `lpFilePart` by
  converting the verified wide buffer prefix and adding its byte count to the
  caller buffer, never by narrowing `lpBuffer`.

The overlay preserves public names, arguments, return values, LastError
handling and call order. It has no guest, session or BOP dependency.

## Build rules

- A T279-specific generator produces `build/M0-T279/<arch>/build.ninja`.
- Both graphs use MSVC `/std:c11 /MT /W4`, `deps = msvc`, original include
  order, and no old component input.
- The original sources may warn under `/W4`; no `/WX` or warning suppression
  is used. Adapter and overlay sources must compile warning-free.
- The artifact target is static libraries only. A focused fixture links the
  OEM library and adapter binding directly for x64 wrapper tests.
