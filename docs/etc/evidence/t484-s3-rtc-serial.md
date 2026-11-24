# T484 S3 P91 retire RTC investigation and serial wrapper

B0150 was a historical investigation, not a passing product gate. The first
native candidate reproduced67 checks/13 failures in owned PID10520. Existing
[RTC disposition](m0-t478-s35-rtc-investigation.md) and TODO already identify
this exact original DOUBLE_TIME word-layout defect in alternate host BOP70;
normal observed IRQ8 uses original ROM. There is no current test/tool caller
of the old Python investigation. Retire that script and withdraw the new
candidate C source/target instead of migrating a completed investigation.
Do not change the expected result to bless faulty arithmetic. Keep historical
failure receipts and the explicit reopening trigger in TODO. Actual RtcEntry
and RtcAlarm native probes and integration workloads remain unchanged; they
were inspected, not rerun or newly claimed passed in this batch.

B0151's12 baud cases remain a useful current contract of HOST_COM_BAUD:
forwarded values including signed limits, exactly-once call, failure report
before saved state update and final state. Keep serial_baud_test.c unchanged.
Use the existing native source-projection helper with unique start/end markers
and original line mapping. Native serial-baud x86 target owns compilation;
remove the unused Python wrapper rather than keep a trivial build/run script.
No physical serial endpoint is opened. Test assertions remain enabled.

Owned PID24924 built and ran the native target, exit0 with12-case marker.
No product, original guest or mirror bytes changed. The two retired wrappers
remove140 lines; the only new implementation is a10-line declarative target.

[Receipts](../../../artifacts/build-migration/t484/s3-rtc-serial/result.json)
retain the failed RTC investigation and passing serial run. Raw logs remain
build/t484-s3-rtc-serial. This resolves two entries:186 complete/169 remaining
of355. Retention review and final S4/S5 work remain separate and incomplete.

Final gate PID31044 exited0: cached full host build, unchanged serial fixture
mtime/hash, x64 rejection2 and11 raw/22 publication hashes passed. Existing
product publication remains byte-identical; no replacement package produced.
