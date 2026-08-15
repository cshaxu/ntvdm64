# T215 S4: Startup-Configuration Attachment

## Question

Can the existing T204 startup-configuration package be reused by the current
full bx-vdm composition, and does that attachment remove the source-backed
fatal configuration path?

## Inputs And Source Contract

OpenNT `cmdconf.c` is the configuration transform owner and `cmdenv.c` owns
the resulting AUTOEXEC environment merge.  T204 already recovered their
bounded, source-derived CLI composition as one package:

```text
CLI paired sources -> copied input -> T204 transform/provider
                   -> provider-owned CONFIG/AUTOEXEC images and merge state
                   -> COMMAND paths and boot namespace
```

`ntdos64-run` already transports the paired source paths in its child
environment. `bx_ntvdm_startup_configuration_source_v1_from_environment`
copies both files once, closes them, and has no retained path/handle.  The
current full `composition_runtime`, however, previously did neither compile
that reader nor call its existing
`bx_ntvdm_boot_namespace_composition_v1_set_startup_configuration` seam.

## Change

The full composition now reuses the existing reader and invokes the existing
composition setter after COMMAND context bind and before the direct DEM
namespace or composition binds.  A new explicit installation diagnostic stage
names this attachment.  The current-source build manifest also includes the
already owned reader translation unit.  The bounded native fixture creates
paired OEM test inputs only under its generated build directory and exports
them solely to the child run.

No CONFIG transformation, BOP handler, Bochs source, host temporary file, or
host source mutation was added.

## Reproduction And Observation

The full current-source x64 `/MT` build and bounded run completed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T214S3CompositionOwnerFixture.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t215-s4-startup-attachment-r1
```

The reader, provider, and full composition all compiled and linked. The run
still reached the previously classified `C4 C4 50 3D` typed STOP.

## Current Composition Finding

This unchanged terminal is not evidence that the attachment failed. It
reveals the remaining whole-package composition gap:

- T204 binds generated images into `plane.provider.readonly_namespace`.
- The current composition then installs the T202 whole direct-host DEM
  provider.
- `bx_ntvdm_dem_package_session_v1_dispatch` gives that provider's namespace,
  handle, FCB and search partitions priority before the boot-namespace plane.
- Consequently a guest request for `C:\CONFIG.SYS` reaches the admitted real
  host namespace first; the generated T204 image is not an overlay member of
  that direct provider.

The missing piece is therefore not a new `50:3D` implementation and not a
configuration-parser rewrite. It is an explicit, provider-owned startup-image
overlay at the DEM direct-host namespace boundary, with source-shaped
precedence and lifetime for CONFIG/AUTOEXEC, plus a matching COMMAND merge
handoff. This must be designed and implemented as the remainder of the whole
T204 package integration; it cannot be supplied by a trace leaf patch.

## Confidence And Next Work

High confidence: the source call chain and provider priority are direct
current-source facts, and the complete rebuilt run proves the attachment is
linkable. The cause of the guest's subsequent fatal path is not yet proven
solely by this run; no normal guest-run claim follows.

The next S4 work is to map the bounded overlay contract across DEM open/read/
seek/close/search/FCB paths and COMMAND environment merge before modifying the
direct provider. It must retain the real host C: projection for ordinary
paths; only provider-owned generated startup images may take the declared
precedence.
