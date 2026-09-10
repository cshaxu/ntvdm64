# T198 S55: boot-drive finite composition witness

## Result

The active finite composition now invokes the existing, exact `50:0D`
source-derived provider after generic DEM ingress and before the detached
legacy paths.  The provider retains its strict real-mode `C4 C4 50 0D`
validation, preserves AH, returns the source-defined contained `AL=3` (C:)
fallback, and resumes after four bytes.

No registry, host-drive scan, device, or Bochs behavior is involved.

## Clean witness

The current-source x64 `/MT` build
`artifacts/build/t198-s55-boot-drive-composition-r1` advances beyond 50:0D
and reaches the next original boundary:

```text
next=50:21 terminal=1:9346:443e
```

The expected next service is OpenNT `SVC_DEMIOCTL` (`demIOCTL`), rather than a
failure at the boot-drive provider.

## Next admission

`50:21` is a raw-media/IOCTL family, not a generic drive or namespace query.
The next task must audit its actual AX/AL request and existing profile metadata
provider before deciding whether any contained subfunction is composable.
