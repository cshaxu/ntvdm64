# VDD port conflict and rollback: original contract versus selected IOS

This review only reads source and the existing test result; no new execution tool, product or guest operation was made for this pass. Current `nt_vdd.c` and `ios.c` are byte-identical to `O:/repos.external/OpenNT/base/mvdm/softpc.new` in this checkout. The conflict is therefore present in the compared original code as well as the selected implementation; this review does not attribute it to a local mirror change.

## What the source says and does

`host/src/nt_vdd.c:233–266` documents first-hook ownership, failure of subsequent conflicting requests, and `ERROR_ACCESS_DENIED` for an already hooked port, with an explicit caveat about video ports. `VDDInstallIOHook:306–365` allocates one adapter, installs handler pointers, then calls `io_connect_port` for each inclusive port range. If it returns FALSE, the original caller walks already-connected ranges and the earlier portion of the current range, calls `io_disconnect_port`, frees its adapter, sets `ERROR_ACCESS_DENIED`, and returns FALSE.

The actually selected NTVDM branch of `base/support/ios.c:868–878` directly assigns the requested adapter to each selected input/output port table and unconditionally returns TRUE. It contains no occupied-port check. The original compared tree contains the same implementation. Thus, in this particular composition, the caller's failure/rollback branch cannot be reached by an ordinary conflict from that function; the documented first-owner result does not follow from this code. Other causes, injected failures, alternate profiles, and port aliasing need separate analysis.

## Bounded existing object evidence

The previously executed test-owned `vdd-port-range-fixture.c` linked the actual cached `nt_vdd.obj`, `ios.obj` and CCPU I/O objects and used only the in-memory IOS table. VDD A registered `0x2e0`; VDD B then registered `0x2e0–0x2e1`. B returned success, reads at both ports called B's callback (`0x91`), and both owners remained in the adapter table. This concretely confirms replacement of A on overlap in the fixture. It does not prove a real VDD ever makes that request, nor execute the failure/rollback path. There was no physical device or guest.

The same existing probe observed a reversed range succeeding while retaining a slot and a duplicate range within one request succeeding. Those are observations, not yet proof of the intended invalid/duplicate-range contract; do not infer a required rejection code. The `0xffff` wrap witness is separately recorded in `vdd-port-ffff-loop-review.md` and is not evidence for conflict rollback.

The [test-owned fixture](vdd-port-range-fixture.c) was independently rebuilt with the same MSVC x86 driver, flags and original objects as the [slot-reuse fixture](vdd-slot-reuse-interleave-review.md), and executed with `--ranges`. It reproduced `RANGE_CONFLICT success=1 old=91 next=91 owners=1/1` and five baseline checks; these are assertions of existing behavior, not a corrected contract. Source SHA-256 `3A8AA58B08C7AD00D3FB6527AEDE4EBB20EE08328863F219C440E7E45989DFBF`; independently rebuilt EXE `9242D514022ABEFE96D3D1244C05D4A5179A64C5B6CD3E4DD225479C449AA795`; cached `nt_vdd.obj` `75C48554F12D179F4DC3FD95051BB788E9D879C13F7792901F4EB4FDA2E08F5B`; `ios.obj` `00F21423007F39DDB26312B4EB41A41911456B062EA5B035D394577C498D4DF9`. The x86 link also uses the original `in.obj` and `out.obj`; compilation produced an existing `NTVDM` macro-redefinition warning. No physical port is used.

## S13 boundary

Preserve first-owner semantics where the original contract applies, including both input/output ownership, masked addresses and the documented video-port exception. Establish whether ownership checks belong in VDD registration or the shared IOS provider without changing other devices' intentional registrations. Ensure failed multi-range requests release only newly acquired mappings, preserve prior owners, retire the allocated slot and return the original error; overlapping ranges, existing partial input/output ownership, full 16-bit endpoint and install/uninstall concurrency require explicit tests. Do not assume a new preflight alone eliminates races. The exact validation and duplicate-range semantics remain to be traced before selecting a source change. Register any original mirror difference and do not replace the conflict with a success stub.

No product, guest or shared ledger was changed. The probe described above remains evidence of one controlled successful-overlap path, not a tested repair or rollback.
