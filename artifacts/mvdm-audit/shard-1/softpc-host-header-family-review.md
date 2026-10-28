# S10 shard 1: SoftPC host declarations and keyboard build artifact

Eleven pending `softpc.new/host/inc` files were checked together. All match both reference editions by SHA-256. Per-file full hashes, direct textual includes and normalized x86 worker VALID dependency objects are in `softpc-host-header-family-evidence.json`; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/softpc-host-header-family-observer.py`. A textual basename include may resolve differently or be preprocessor-disabled. A VALID object dependency proves inclusion in that object build, not every declared function's linked or runtime behavior.

| Path | Role and evidence; boundary |
| --- | --- |
| host_rrr.h | Host error definitions; 20 direct references and 21 VALID objects, including host/config and adapter objects. Retain selected declaration carrier; individual error routing unverified. |
| host_trc.h | Host trace declarations; 4 direct references and 3 objects, nt_com/nt_ntfun/nt_wcom. Trace enablement and production link not established. |
| hosttyps.h | Historic host-type carrier; zero direct include and zero deps in scope. Pending other profiles; do not infer host types are absent. |
| nt_conf.h | Host configuration definitions; 0/0. Pending profile and actual config owner. |
| nt_eoi.h | PIC EOI host definitions; 17 direct references and 17 objects including keyboard reset. Current keyboard, timer and mouse implementations have distinct EOI state machines; header inclusion is not proof PIC cancellation, deferred EOI or device interactions work. Refer to existing PIC/keyboard functional tests separately. |
| nt_getxx.h | CPU register accessor declarations; one textual include from nt_yoda.c, zero deps. Conditions/profile and actual CPU accessor binding pending. |
| nt_paint.h | Historical console graphics globals; 0/0. Do not conclude current presentation is absent; trace current rendering owner separately. |
| nt_pif.h | PIF configuration structures; 7 direct references and 9 deps including config and command objects. Actual paths and generated config lifetimes need separate execution evidence. |
| sdos_ddi.h | Historical platform DDI declarations; 0/0, pending supported profile attribution. |
| winsigni.h | Host/Insignia declaration carrier; 0/0, pending profile attribution. |
| kybdcpu.gi | Zero-byte file, exact identity in both references; no textual include and no VALID dep found. Candidate exclusion only for this empty artifact in the observed graph; cannot infer keyboard behavior, generator inputs or alternate-profile status. |

Counts are bounded to C/H/INC/CPP direct scan in current softpc.new and VALID deps in one x86 worker tree; no exhaustive other-platform, path resolution, stale dependency, binary/image binding or runtime/device test was done. Product, guest, build tree and shared ledger remain untouched.
