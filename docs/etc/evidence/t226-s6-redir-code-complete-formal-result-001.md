# M0 T226 S6 — Redirector package code-complete formal result

## Scope

This result closes the local code-complete gate for the OpenNT Redirector
`57:00..31` owner package under the current Direct/Readonly CLI profile. It
does not claim network, IPC, VDMREDIR, NetBIOS, DLC, mailslot, or named-pipe
capability.

## Source and failure disposition

The retained [Redirector source/ABI/failure matrix](../research/t199-s12-redirector-source-abi-failure-matrix-001.md)
binds all 50 contiguous `rdrsvc.h` identities to `nt_bop.c`'s `MS_bop_7`
unavailable branch. The historical `VDMREDIR` dispatcher is not composable in
the CLI product. The source-derived whole-package facade therefore resumes at
`RIP+4` with `CF=1` and `AX=ERROR_INVALID_FUNCTION (1)`. It consumes no host
path, handle, network API, device, callback, or Bochs mechanism.

## Formal build and local result

The pinned MSVC x64 `/MT` Ninja graph was generated in
`build/M0-T226-S6/redir-package-20260817a`. The formal
`t226-s6-redir-package-facade-fixture.exe` built and returned zero. It invokes
every `57:00..31` identity and verifies the exact copied result; it also
proves that out-of-range `57:32` cannot be dispatched.

## Code-complete disposition

All 50 T226 S1 Redirector rows have one source-proven unavailable result and
formal local assertion under Direct/Readonly. Overlay remains ABI-only and
native guest trace is not part of this result.