# M0 T194 S42: Paired Metadata Provider Focused Test

The existing isolated S40 MinGW build root rebuilt only
`bx-ntvdm-dem-ioctl-metadata-provider-v1-test`. WinLibs MinGW GCC 16.1.0
linked the target and one execution exited zero with:

```text
bx-ntvdm DEM IOCTL metadata provider: paired snapshot contracts verified
```

The executable is 71,785 bytes with SHA-256
`8DE2A9C99A0ED02B585DEBCD7A7D74830F3D85D336526F13E6FE4A533365E9A7`.
No Bochs, CLI, adapter runtime, engine derivative, bundle, or guest process
was built or run. This proves only the focused common-plane/snapshot provider
contracts; it is not runtime or normal-return evidence.
