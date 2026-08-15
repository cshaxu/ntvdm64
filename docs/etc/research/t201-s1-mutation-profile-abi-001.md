# T201 S1 Mutation-Profile ABI Evidence

## Scope

S1 introduces the common bx-vdm policy record. It performs no host I/O,
guest-memory access, BOP decode, provider dispatch or overlay storage.

`bx_ntvdm_mutation_profile_v1` contains one VDM-session mode: `direct`,
`readonly`, `overlay`, or `virtual`. Owner registration pairs a package owner
with one mutation class and a permitted-mode mask. Authorization accepts only
the already-selected profile mode; no API permits an endpoint to supply a
different mode.

The mutation-class enum includes `none/mechanical`, namespace/content, file
metadata, session context, host-global state, device/raw-media, and
IPC/network. The records contain no host handle, pointer, callback, Bochs
type, selector, service number or guest address.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T201S1MutationProfileProbe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot artifacts\build\t201-s1-mutation-profile-r1
```

Observed result: MSVC x64 `/MT` compiled and ran with exit code zero.

The fixture proves all four session modes are selectable; an unregistered
owner/class is rejected; duplicate registration is rejected; invalid profile
mode is rejected; and an owner whose registration does not permit the selected
mode is rejected. It is deliberately not a claim that direct, readonly,
overlay or virtual host behavior is implemented.
