# M0 T279 S2 — adapter-win32 binding audit

## Result

The original support bodies are unchanged. `adapter-win32` now provides the
complete reached NT4 source-form surface identified by T279/S1:

- adapter-private `NtCurrentTeb()->StaticUnicodeString` scratch storage;
- adapter-private `NtCurrentPeb()->ProcessParameters->CurrentDirectory`
  snapshot plus matching PEB lock calls;
- RTL counted-string initialisation, ANSI/OEM/Unicode conversion and matching
  allocation/free contracts;
- RTL heap, full-path and current-directory entry points; and
- NTSTATUS-to-Win32 error conversion used by the original `BaseSetLastNTError`
  macro.

Every implementation uses only public Win32 APIs and adapter-local memory.
No native pointer crosses an MVDM, guest, mapping-manager or historical
`ULONG` boundary.

## Compile admission evidence

Both forms compile the byte-exact source bodies with `/MT /W4`:

```text
VsDevCmd -arch=x64 -host_arch=x64
cl /std:c11 /MT /W4 ... opennt_support_rtl.c file.c process.c

VsDevCmd -arch=x86 -host_arch=x64
cl /std:c11 /MT /W4 ... opennt_support_rtl.c file.c process.c
```

The original source warnings remain visible. They are not adapter warnings and
are not waived. The build-island packet must decide its warning policy without
editing or suppressing those original source diagnostics.

## Required later overlay

Four helpers compile on x64 but contain 32-bit pointer arithmetic that is not
safe to execute there:

- `file.c:414` — `FindFirstFileOem` derives a copy length by casting two
  pointers to `ULONG`;
- `file.c:467` — `FindNextFileOem` repeats the same calculation;
- `file.c:563` — `GetFullPathNameOem` reconstructs `lpFilePart` through an
  `ULONG`; and
- `process.c:224` — `SearchPathOem` reconstructs `lpFilePart` through a
  `DWORD`.

Before this library enters an x64 product, `opennt-mvdm-support-overlay` must
export the same four contracts using `offsetof` or a verified buffer-relative
offset. This is a source-shaped overlay requirement, not permission to
truncate a pointer or alter the original mirror.
