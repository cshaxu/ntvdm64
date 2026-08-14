# T199 S46 — Redirector whole-package revalidation

## Question

Does the current CLI composition still route every original OpenNT Redirector
BOP through one source-derived, non-invasive unavailable provider rather than
through service-specific adapter code or an ambient host capability?

## Inputs and procedure

The authoritative contiguous service list is
`src/opennt/base/mvdm/inc/rdrsvc.h`: `SVC_RDRINITIALIZE` through
`SVC_NETBIOSCHECK`, inclusive, defines exactly `57:00..31` (50 members).
`src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` documents the historical
`MS_bop_7` fallback: after `VDMREDIR` cannot be loaded it sets
`ERROR_INVALID_FUNCTION` in AX, sets CF, and advances over the service byte.

The retained historical dispatch is not linked: it requires CCPU/SAS register
access, dynamic `VDMREDIR` loading, Win32 handles and unadopted named-pipe,
mailslot, network, DLC and VDD providers.  The selected CLI profile therefore
keeps the complete package explicitly unavailable.

Fresh verification used:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t199-s46-redir-package-r1 `
  -Fixture boot-namespace -HostArchitecture x64
```

## Observations

The command source-built, linked and ran the x64 `/MT` bound boot-namespace
composition with exit code zero.  Its Redirector family loop dispatches each
of the 50 four-byte BOPs through the generic-UD bridge and verifies:

| Input | Result |
| --- | --- |
| `C4 C4 57 00..31` | typed resume at fault RIP + 4, CF set, AX=1 |
| `C4 C4 57 32` | rejected; no provider outcome |

`bx_ntvdm_redir_package_facade_v1` performs no service-number switch.  It
accepts only the system plane's original-owner Redirector classification and
applies the one source-derived failure result.  The bound composition calls it
after DEM and COMMAND package sessions; it is not a Bochs callback and adds no
selector knowledge to bx-core or bx-mantle.

## Interpretation and follow-up

The full Redirector provider package remains closed for the declared CLI
profile as explicit unavailability.  This proves neither Redirector operation
nor any historical host service; named pipes, mailslots, NetAPI, NetBIOS, DLC,
VDM windows, async I/O and Redirector lifecycle remain unavailable by design.
No native guest trace ran.  The next owner package is Debugger, not a
Redirector service discovered from trace.
