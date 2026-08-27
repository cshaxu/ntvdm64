# M0 T291 S3 P1 — WOW bounded pointer-scope recovery

## Question

Can the original WOW32 pointer macro trio be represented without extending
the Redirector scope, exposing a native alias, or adding another mapping
manager?

## Implementation

`adapter-mvdm-host-out/softpc` now owns the WOW-specific source-shaped scope.
It resolves a real-mode or active protected selector/offset numeric span,
obtains exactly one `session.guest_memory` lease, and returns its synchronous
bounce bytes.  `FLUSH`-equivalent release commits only an explicitly writable
lease; plain release discards it.  End rejects an unreleased entry after
discarding it, so no bounce pointer can survive the source call epoch.

The existing `adapter-bochs` facade gained a selector-blind protected-range
preflight used through the existing SoftPC protected-span seam.  It performs
the native Bochs access/limit check before a copied numeric linear address is
passed to the session lease.  No Bochs object, raw guest alias, OpenNT service
name or BOP selector crosses that boundary.

## Verification

The formal Ninja fixture is generated and executed for both host widths:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T291S3WowPointerScopeNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T291\S3\wow-pointer-scope-x86
build\M0-T291\S3\wow-pointer-scope-x86\bin\t291-s3-wow-pointer-scope-fixture.exe

powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T291S3WowPointerScopeNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build\M0-T291\S3\wow-pointer-scope-x64
build\M0-T291\S3\wow-pointer-scope-x64\bin\t291-s3-wow-pointer-scope-fixture.exe
```

Both passed. The fixture proves reject-before-begin, nested-scope rejection,
exact write/flush commit, nonflush discard, double-release rejection and
post-end rejection. It deliberately exercises no original WOW body, callback,
selector, guest load or BOP ingress.

## Remaining S3 work

The source-shaped monitor task/frame projection and the one-session
`CallBack16` transaction remain. Their frame/worker inputs must not be
approximated by this pointer scope.
