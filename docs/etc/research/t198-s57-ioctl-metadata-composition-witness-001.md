# T198 S57: IOCTL metadata finite-composition witness

## Result

The active composition now calls the existing `50:21` metadata provider only
after normal BOP ingress, original-provider selection and DEM RAW_MEDIA plane
classification.  The provider consumes only the immutable gset drive snapshot.

The clean source-built r1 witness advances past the observed AX=4408/BL=02
request and reaches:

```text
next=50:42 terminal=1:9346:6353
```

No raw device, disk image, host path, or generic IOCTL behavior was enabled.

## Next frontier

`50:42` is the DEM fast-read family.  It must be classified from its actual
register/request shape and the existing finite namespace provider before any
new behavior is admitted.
