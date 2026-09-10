# T221 S3 P46 — Package-session startup handle chain

## Scope

This is the first positive owner-family chain in the P44 package-session
matrix. It proves normal ingress transport rather than direct invocation of a
whole-provider partition.

For each Direct, Readonly, Overlay, and Virtual profile, the focused fixture:

1. sends `50:1B` through the normal BOP path to register a DTA, its copied
   CurrentPDB location, and its bounded SFT-head input;
2. places `C:\\COMMAND.COM` in checked guest RAM and sends `50:12` to open the
   declared immutable startup image;
3. uses the returned opaque `AX:BP` token for `50:16`, which copies the first
   command byte into checked guest RAM; and
4. sends `50:02` to close that token.

The fixture checks that the DTA registration resumes, the open result contains
both token halves, the read writes `0xf4` to the requested guest address, and
the close succeeds. No host handle or provider-private token crosses guest
state.

## Why it applies to all four profiles

`COMMAND.COM` is a declared startup image and therefore remains owned by the
immutable readonly namespace. The mutation profile still governs the
surrounding package session, but does not create a Direct fallback or a
profile-local BOP implementation for this immutable path. This gives the
matrix a common positive transport witness before it exercises each profile's
mutable namespace, handle, FCB, and search disposition.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\143-package-session-startup-chain `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The generated manifest reports MSVC x64 `/MT`, source-built compile/link/run
exit code `0`, and a closure excluding `bx-core`, `bx-mantle`, Bochs binaries,
and an OpenNT runtime.

## Remaining work

P46 covers one declared-image namespace/handle chain plus the DTA/PDB
transport prerequisite. It does not close the Direct/Readonly/Overlay/Virtual
mutable owner families, path and FCB search continuations, or the complete
29-identity matrix.
