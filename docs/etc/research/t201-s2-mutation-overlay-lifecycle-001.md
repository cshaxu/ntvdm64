# T201 S2 Mutation-Overlay Lifecycle Evidence

The overlay is a bounded, opaque, per-VDM bx-vdm record store. It copies the
already-validated overlay profile into session state and accepts a record only
after common profile authorization for its owner and mutation class.

It has no filesystem, Registry, device, network, guest-memory, Bochs or BOP
dependency. Its key and value are opaque copied data; they are not paths,
handles, pointers, selectors, service IDs or guest addresses.

Reproduce with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T201S2MutationOverlayProbe.ps1 `
  -RepositoryRoot (Get-Location).Path `
  -BuildRoot artifacts\build\t201-s2-mutation-overlay-r1
```

The MSVC x64 `/MT` fixture passed with exit code zero. It proves authorized
record/lookup, duplicate and unauthorized-owner rejection, rejection of
direct/readonly/virtual initialization, and zeroing teardown. This is an
overlay lifecycle primitive, not an implemented profile backend or BOP
provider.
