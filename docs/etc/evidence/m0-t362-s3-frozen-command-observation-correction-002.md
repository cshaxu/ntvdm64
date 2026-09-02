# M0 T362 S3 — Frozen COMMAND observation correction

## Correction scope

This corrects the marker list in
[the original S3 record](m0-t362-s3-frozen-command-observation-001.md).  That
record read the bounded console copy only.  The same single invocation also
produced the configured child-only BOP-return sidecar, which is the complete
source-order record.  No product, package, root, observer, timeout or run was
changed; this is not a second observation.

## Complete observed progression

The sidecar proves that original `54:05` returned with `CF=0`, then records
the following continued original flow before the 8-second observer timeout:

```
54:05 return
  -> 50:46, 50:0D, 50:21
  -> 54:0C return
  -> 50:12, 50:00, 50:21, 50:42, 50:02
  -> 54:04 return
  -> repeated original DEM/configuration records
  -> 54:02 return, 54:0D return
  -> two 54:0F returns
```

`54:01` and `54:00` are absent.  Therefore the source fact is not “stopped
at `54:05`”; it is “the mapped `54:05` cohort returned normally, and the
workload later stopped before first command delivery.”

## Unchanged terminal attribution

The same observer report maps the stopped primary thread to original:

```
c_IOVirtualised -> inb -> printer_io -> CCPU40 -> host_start_cpu
```

This stack can only be the ordinary BIOS printer branch: `printer_.c` returns
from the NTVDM `SI=0/1/2` selector cases before it reaches `setIF` and `inb`.
The exact `AH=0/1/2` ordinary subcase is not observable from the frozen stack
and is deliberately not invented here.

Thus the terminal owner transfer remains the original SoftPC
conventional-device path, but it is now correctly separated from the already
successful T362 COMMAND first-call return.  The S3 run still does not prove a
printer defect, device-result failure, COMMAND built-in completion or a CPU
semantic error.
