# M0 T259 S4 P1 — DEM Hard-Error NTDLL Facade Recovery

## Original source contract

OpenNT `demerror.c` resolves a host DOS drive letter for a hard-error report:

1. append a trailing slash with `RtlAppendUnicodeToString`;
2. open each historical DOS-device symbolic link with
   `NtOpenSymbolicLinkObject`;
3. query its target through `NtQuerySymbolicLinkObject`;
4. compare case-insensitively with `RtlEqualUnicodeString`.

The imported body already retains that sequence.  The old bx-vdm shim instead
returned `STATUS_NOT_SUPPORTED` for all three status interfaces and `FALSE`
for every Unicode comparison, making the complete source branch unreachable.

## Recovered implementation

`demerror_lock_shim.c` now resolves the same four exports from the loaded
`ntdll.dll` by their original spellings and calls them with the original
OpenNT-shaped signatures.  This is a host-only facade:

- counted-string buffers and object handles never cross a guest ABI;
- no OpenNT `demerror.c` statement or call ordering changes;
- no Bochs/bx-mantle change or product-shell recreation;
- an absent export produces `STATUS_NOT_IMPLEMENTED` (or `FALSE` for the
  Boolean comparator), preserving a truthful source-visible failure rather
  than fabricating a drive match.

## Verification

Fresh external formal Ninja root:

```text
build/M0-T259-S4/formal-r6
```

The complete 286-object graph linked and the direct import fixture passed:

```text
T230 S8 direct OpenNT demerror/demlock import: NTDLL Unicode, hard-error setup, dispatcher retry and file lock contracts verified
```

The fixture proves the same-named Unicode append and case-sensitive/
case-insensitive comparison results, then retains the existing hard-error
guest layout, dispatcher retry, opaque handle, and Win32 file-lock checks.
The actual DOS-device symbolic-link target remains host-dependent integration
input; its source body is now reachable rather than hard-coded unavailable.
