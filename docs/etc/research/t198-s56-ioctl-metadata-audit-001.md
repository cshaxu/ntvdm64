# T198 S56: reached `50:21` IOCTL metadata audit

## Observed request

The clean r1 run records the first reached DEM IOCTL request as:

```text
state=9346:443c eax=00004408 ebx=0000f002 ecx=00000000 edx=00000000
```

This is `SVC_DEMIOCTL` / `demIOCTL`, function `AX=4408h`; `BL=02h` selects
drive C:.

## Existing contained provider

`bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch` already accepts exactly
this raw-media plane request.  It consumes the immutable host-drive snapshot,
not a device or host filesystem.  The fixture's snapshot admits C: (index 2)
with type 3, so the provider returns `AX=1`, clears CF, and resumes after the
four BOP bytes.  Its sibling AX=4409 behavior remains separately guarded.

The active finite composition does not yet call this provider.  Its existing
gset snapshot and normal ingress/selection/DEM-plane records are sufficient;
the missing work is a narrow composition call, not an IOCTL implementation.

## Next admission

Add the existing metadata provider to the explicit finite manifest and call
it only after the active composition has produced the validated DEM
RAW_MEDIA record.  Do not expose raw media, disks, host paths, or any other
AX=44xx service.
