# T202 S2: DEM current-directory/default-drive family service

## Question

Can `50:13`, `50:18`, and `50:1A` use one DEM-owned CWD context and one
checked guest-memory transaction path, replacing the retained root-only
workaround without making a trace-selected leaf implementation?

## Source and ABI map

| Service | OpenNT owner | Copied input | Result |
| --- | --- | --- | --- |
| `50:13` | `demdir.c:demQueryCurrentDir` | DS:SI CDS, AL drive | validate admitted drive; reconcile `CurDir_Text` and `CurDir_End` with session CWD; CF clear |
| `50:18` | `demdir.c:demSetCurrentDir` | DS:DX NUL path | source-shaped drive/path check; update DEM session CWD or return typed error |
| `50:1A` | `demgset.c:demSetDefaultDrive` | DS:SI NUL path, DL drive | retain DL/path-letter guard; update session CWD and default drive or return typed error |

The guest string window is bounded to the historical CDS text capacity. Paths
are copied, ASCII/OEM-safe, DOS 8.3 root-relative values only. No host pointer,
absolute host path, or process current-directory state crosses the boundary.

## Profile disposition

- direct validates under the selected real root through the private namespace
  handle; missing paths receive `ERROR_PATH_NOT_FOUND`;
- readonly returns `ERROR_ACCESS_DENIED` for state changes;
- overlay and virtual update their shared/session state; neither creates an
  implicit boot C:.

The original `50:1A` path-letter mismatch remains `AX=1, CF=1`. The original
`50:18` malformed first-letter path retains the no-synthetic-AX failure form.

## Evidence

`Invoke-T202S2DemCwdServiceProbe.ps1` at
`artifacts/build/t202-s2-dem-cwd-service-r4` passed under MSVC x64 `/MT`.
It verifies direct root set/query CDS writeback, valid and mismatched default
drive, readonly refusal, overlay success, and an explicit virtual-profile
CWD set/query round trip. The direct case reads the admitted C: root only;
the virtual case has no host namespace lookup. The fixture performs no guest
execution or host configuration mutation.

The complete DEM package fixture also compiled, linked and ran at
`artifacts/build/t202-s2-dem-cwd-family-compile-r1`; it includes the new
service in the manifest closure.

The bound `dem-package` fixture at
`artifacts/build/t202-s2-dem-cwd-family-r3` installs the shared overlay
profile into the DEM session before dispatch. Its generated
`t198-s16-bx-vdm-dem-package.exe` returned zero after exercising the new
service through the package session and boot-plane checked read/write bridge.

## Limits and follow-up

This replaces only the current-directory/default-drive three-service family.
It does not close DEM file/handle/FCB/search behavior, and it does not claim a
native trace. The legacy root-only CWD/default-drive fallback was removed
after this family became session-owned: a boot namespace plane without the
installed DEM package context does not claim to implement these services.
