# T199 S37 DEM DASD and IOCTL Family Regression 001

OpenNT `demioctl.c` assigns `AL=08` and `AL=09` to one
`demIoctlChangeable` provider.  Its invalid-drive branch returns
`ERROR_INVALID_DRIVE` with CF set.  The adapter previously recognized only
admitted metadata and allowed an invalid 08/09 request to fall through to the
unrelated `demIoctlInvalid` AX=1 result.

The metadata provider now owns the complete 08/09 result slice: fixed versus
removable response, local versus remote response, and AX=15/CF invalid drive.
It consumes only the immutable CLI drive snapshot.  Unknown functions remain
the explicit AX=1/CF invalid-function terminal result.  `demdasd.c` absolute
read/write retain AX=21/CF because the CLI profile has no BDS lifecycle, raw
block-media capability, INT13 path, or host device handle.

`artifacts/build/t199-s37-dem-package-r73-dasd-ioctl/` source-builds, links,
and runs the full DEM x64 `/MT` fixture.  It proves all listed results without
a native trace, bx-core/bx-mantle input, Bochs device, or ambient host-device
access.  This is a provider-family correction, not a raw-media admission.
