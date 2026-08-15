# T215 S5 — startup-image direct-provider overlay

## Result

T204 already built and owned two source-derived startup images, but T202's
complete direct-host DEM provider took the pathname and handle routes first.
This S closes that composition seam without creating a virtual drive or a
second filesystem: the direct provider borrows the ready namespace solely for
root-relative `CONFIG.SYS` and `AUTOEXEC.BAT` on its declared boot drive.
All other names remain routed to the existing admitted host namespace.

## Owner and contract

`cmdconf.c` is the source contract: the historical COMMAND host creates two
replacement images, publishes their paths through `54:0C/0D`, and the DOS
side subsequently uses the ordinary DEM file service family. The modern
provider substitutes session-owned immutable bytes for historical temporary
files; it does not replace DEM, DOS, COMMAND, or Bochs semantics.

| DEM operation | Startup image disposition | Other paths |
| --- | --- | --- |
| `50:12` open | read-only typed token and size | direct host backend |
| `50:00`, `50:16`, `50:02`, `50:08` | existing copied readonly seek/read/close/time contracts | direct host backend |
| `50:1E`, namespace create/delete/rename/metadata write | access denied; no host side effect | existing mutation-profile decision |

The borrowed namespace is explicitly restricted to ready entries 1 and 2.
Its command/target profile entries cannot be selected through this route.
Tokens remain copied adapter values; no Win32 handle, guest pointer, or Bochs
semantic crosses the boundary.

## Evidence

`Invoke-T202S4DemWholeProviderProbe.ps1` built and ran under MSVC x64 `/MT`
in `artifacts/build/t215-s4-startup-overlay-provider-r5`. The regression
opened generated `CONFIG.SYS`, read `FILES=20\r\n` through a checked
guest-write action, rejected `50:1E` with `AX=5`/CF, and closed the token.
The same probe retains direct-host file, FCB, search, mutation and profile
coverage.

`Invoke-T214S3CompositionOwnerFixture.ps1` built the full current source
closure under MSVC x64 `/MT` with paired generated CONFIG/AUTOEXEC inputs in
`artifacts/build/t215-s4-startup-overlay-r2`. Its bounded native observation
still stopped at `8DC8:0984`, bytes `C4 C4 50 3D`. This does not claim a guest
run, normal terminal, or that the native path reached either startup image.
The unchanged stop is a next-causal-observation question, not a reason to add
a `50:3D` handler.

## Boundaries retained

- No host `CONFIG.SYS`, `AUTOEXEC.BAT`, temporary file, or registry entry is
  created, changed, or removed.
- No virtual boot C: is introduced; ordinary host-drive policy remains intact.
- No Bochs, bx-core, or bx-mantle source changes are made.
- This is a provider-boundary contract for two declared startup images, not a
  claim that the whole COMMAND package or startup sequence is closed.
