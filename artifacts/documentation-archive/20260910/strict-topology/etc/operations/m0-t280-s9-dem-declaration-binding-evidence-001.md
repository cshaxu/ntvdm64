# M0 T280 S9 — DEM storage declaration-binding evidence

The focused fixture includes the existing adapter-win32 `nt.h` facade followed by the exact original `dem.h`. It does not include a DEM C body or call an API.

## Formal Ninja results — 2026-08-26

```text
VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T280/x64-dem-storage-declarations
[1/1] SYNTAX .../tests/adapter-win32/t280_s9_dem_declaration_fixture.c

VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T280/x86-dem-storage-declarations
[1/1] SYNTAX .../tests/adapter-win32/t280_s9_dem_declaration_fixture.c
```

`/showIncludes` confirmed both architectures selected these project-owned source forms in order: adapter-win32 `nt.h`, platform-ABI `devioctl.h`, original DEM `dem.h`, platform-ABI `ntdddisk.h`, then platform-ABI `ntddstor.h`.

The one `ADAPTER-WIN32-004` divergence is limited to making the original storage declaration carrier visible and undefining the modern unrelated `GetDiskSpaceInformation` convenience alias.
The complete `ntioapi.h` surface remains outside this facade because direct inclusion with the modern `winternl.h` produces unbound private NT I/O declarations and redefinitions.
It transfers as a whole to the next source-shaped ABI package; it is not silently replaced by a modern SDK type or a hand-written provider.

No mapping-manager operation occurs in the fixture: storage geometry and `DEVICE_TYPE` are host-local declarations.
Future guest buffer spans remain solely `adapter-softpc -> session.guest_memory -> adapter-bochs` work.
