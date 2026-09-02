# M0 T363 — SoftPC conventional-device and DOS workload matrix closure

T363 closes the source-owned CPU40 `c_IOVirtualised -> inb -> printer_io`
transfer found by T362.

- S1 proved the reached path is the original non-MONITOR SoftPC INT 17/LPT
  cohort, not a missing monitor import or a printer leaf failure.
- S2 verified its original reset, BDA, IOS registration, status and timeout
  contracts through a reproducible source/graph audit.
- S3 verified the same original units are selected in the formal Win32/x86
  product link.
- S4 made the one permitted frozen workload observation. It progressed past
  the device path, completed an initial original `54:01` command retrieval,
  and timed out at a later original `54:01` continuation.

No virtual printer, direct-port access policy, BOP fabrication, guest/media
mutation, CPU change or device shim was introduced. T363 does not claim
printing, `/C EXIT` completion, DOS child execution, or interactive COMMAND
completion. Its exact successor is the original COMMAND command-delivery and
built-in-dispatch cohort.
