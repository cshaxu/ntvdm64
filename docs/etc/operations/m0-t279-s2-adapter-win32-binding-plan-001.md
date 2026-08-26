# M0 T279 S2 — same-shaped Win32/RTL binding plan

## Scope

This packet binds the byte-exact original `oemuni/file.c` and
`oemuni/process.c` bodies to their reached historical NT4 user-mode interface
surface. It does not enable a provider route or modify the original sources.

## Binding rules

- Public `Create*W`, path, directory, environment, and process APIs retain
  their original direct call shape.
- `NtCurrentTeb`, `NtCurrentPeb`, `Rtl*String`, `Rtl*Heap`, current-directory
  and PEB lock interfaces are declared with their original source spelling in
  `adapter-win32` headers and implemented there from public Win32 APIs.
- The adapter owns thread-local counted-string scratch storage and its private
  current-directory snapshot. Native pointers never enter guest/session ABI
  or an historical `ULONG` field.
- `suballoc.c` keeps original callbacks; later callers alone bind those to
  `adapter-softpc` bounded guest-memory operations.

## x64 exception decision

`SearchPathOem` reconstructs `lpFilePart` by narrowing a buffer pointer
through `DWORD`. That is valid only in the original x86 product. If formal
x64 compilation proves this expression cannot be retained safely, an
`opennt-mvdm-support-overlay` helper will preserve the exported result without
truncating a host pointer. No native pointer is narrowed merely to preserve
source text.

## Verification boundary

S2 proves declaration and implementation binding plus source-form compile
admission. S3 alone creates and runs the dual-architecture Ninja islands.

## Binding evidence

On 2026-08-26 the exact original `file.c` and `process.c` bodies compiled
against these adapter headers with MSVC `/std:c11 /MT /W4` under both x64 and
x86 developer environments. The compiler reports pre-existing original-body
uninitialised-local and 16-bit-length conversion warnings; no warning is
suppressed and the mirror sources remain byte-exact.

The compile proves declaration closure, not safe enablement of every exported
routine. Four original OEM helpers use x86 pointer truncation and therefore
require explicit overlay decisions before an x64 static library can enter the
product build graph: `FindFirstFileOem`, `FindNextFileOem`,
`GetFullPathNameOem`, and `SearchPathOem`.
