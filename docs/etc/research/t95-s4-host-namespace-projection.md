# M0 T95 S4: Read-Only Host Namespace Projection

## Implemented Boundary

`bx_ntvdm_host_namespace_v1` consumes only the immutable S2 drive snapshot.
It opens admitted drive roots once and retains their handles inside the
adapter. Its public adapter-local operation accepts a drive index plus a
DOS-relative directory component sequence, and returns only DOS-visible entry
metadata: 8.3 name, DOS attributes, low-size-compatible byte count and DOS
time/date. It returns no host path and no host handle.

Traversal uses `NtCreateFile` relative to the current opened directory with
`OBJ_DONT_REPARSE` and `FILE_OPEN_REPARSE_POINT`. Enumeration uses
`NtQueryDirectoryFile(FileIdBothDirectoryInformation)` directly on that
handle with an explicit first-query restart and subsequent continuation. This
replaces the rejected `GetFileInformationByHandleEx` approach, whose implicit
enumeration behavior produced an empty nested directory in the fixture.

## Projection Rules

- Excluded/unavailable snapshot drives cannot be enumerated.
- `..`, empty components, absolute paths, and non-8.3 traversal components
  fail closed.
- Reparse entries are excluded; reparse traversal is rejected by native open.
- Existing Win32 short names are used when present; otherwise only an exact
  ASCII DOS-8.3 host name is exposed. Ambiguous projected names fail closed.
- Results are sorted by projected DOS name. The implementation has no file
  open, mutation, BOP, DEM, Bochs, or guest-memory operation.

## Reproduction

```powershell
cmake -S . -B artifacts\build\cli-component-layout-check
cmake --build artifacts\build\cli-component-layout-check --target `
  bx-ntvdm-host-namespace-test bx-ntvdm-host-drive-policy-test `
  bx-ntvdm-adapter-runtime-test
.\artifacts\build\cli-component-layout-check\bx-ntvdm-host-namespace-test.exe
```

The fixture creates an explicit temporary `ALPHA.TXT`, `ZETA.TXT`, and
`SUBDIR\CHILD.TXT` tree. It proves ordered root enumeration, nested traversal,
and `..` rejection. The observed marker is:

```text
bx-ntvdm-host-namespace-test: contained DOS projection and deterministic enumeration verified
```

## Deferred Work

S4 does not consume guest paths or perform wildcard matching. S5 must supply
the generic 128-byte guest-read transport, request/response transaction,
opaque continuation table, and atomic result write before any `50:09..0C`
service becomes resumable.
