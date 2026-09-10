# M0 T194 S44: Metadata Provider Derivative Generation

`New-T98S1CurrentAdapterEngineDerivative.ps1 -BopRegisterObservation` created
the fresh uncompiled root
`artifacts/build/current/t194-s44-metadata-provider-generator-r1`.

Its manifest SHA-256 is
`D7911F5D918CE85C306472FF74B7F3BF8C98D154CE8D89307409093528920678`; the
generated makefile SHA-256 is
`93B86FE72B5FA25834BB3309B43338A18B8E75E895E0CEC2795EC051F2F1FBC8`.

The manifest has exactly 68 adapter sources, five CLI sources, and one Bochs
replacement: `cpu\exception.o`. It contains
`bx_ntvdm_dem_ioctl_metadata_provider_v1.c`, contains no removed legacy IOCTL
source, and its makefile has no `main.o:` rule. No compiler, linker, archive,
engine, CLI, or guest was invoked.
