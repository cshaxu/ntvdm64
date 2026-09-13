# M0 T409 closure — DOS file-service restoration

T409 restores both admitted DOS file-service families without retaining a
second standalone file-service algorithm.

- S1 audited original DEM/VDD ownership and its shadow replacement.
- S2 restored original D31--D33 VDD lifecycle and deleted the SFT/JFT shadow.
- S3 removed the kernel-fast-I/O lookalike and selected the original DOS CF
  fallback to normal DEM read/write.

The formal Win32/x86 product links and is staged at `O:\ntvdm64\ntvdm32.exe`.
The automatic MEM watchdog times out identically before and after T409; it is
a Console-observation limitation, not a fast-I/O workload pass. See the
[S1 audit](../etc/evidence/m0-t409-s1-vdd-handle-lifecycle-audit.md),
[S2 restoration](../etc/evidence/m0-t409-s2-original-vdd-handle-restoration.md),
and [S3 fallback](../etc/evidence/m0-t409-s3-fastio-fallback-restoration.md).
