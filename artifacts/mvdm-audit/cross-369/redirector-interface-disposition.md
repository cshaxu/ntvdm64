# S10 part 3: DOS redirector service and memory interfaces

Owned keys `inc/rdrsvc.h`, `inc/RDRSVC.INC`, and `inc/vdmredir.h` are considered. `redirector-interface-observer.py` compares edition hashes and records the normalized selected `vrdisp.obj` VALID dependencies; reproduce with `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/cross-369/redirector-interface-observer.py`. No guest, product, graph or shared ledger was modified.

All three headers match both original editions. Original guest `dos/v86/redir` uses the assembly service numbers to issue BOP 57; current `nt_bop.c::MS_bop_7` loads VDMREDIR and dispatches the encoded service; `vrdisp.c` uses the C service constants and bounded dispatch table. `shard-3/redir-service-role-review.md` records equality of 50 parsed assembly/C names and the original DLL/load/task-retirement chain. `vdmredir.h` defines shared DOS window and unaligned guest-data representations and is a VALID dependency of selected `vrdisp.obj`; it is an interface, not an owner of native network objects. Its field/macro identity does not alone establish the lifetime of the underlying guest mapping or make modern opaque data into an NT4 object.

`m0-t420-s20-vdmredir-operation-matrix.md` records a real `VDMNETA.COM` async NetBIOS request/completion/post witness and later MEM/COMMAND exit, plus other explicitly scoped local operations; a cancellation observed after completion is not a forced pending cancellation. `shard-3/remote-rap-role-review.md` separately traces the remote RAP path to an `ERROR_CALL_NOT_IMPLEMENTED` provider in its inspected map. Do not turn the real NetBIOS route into proof of all remote services, or an unavailable RAP path into a claim that all redirector operations fail.

| Owned key | Suggested role | Remaining evidence |
| --- | --- | --- |
| `inc/rdrsvc.h` | Partial: selected host service-number contract with matching original guest constants. | Bind actual DLL/worker and guest identities and prove remaining operation/failure lifecycle. |
| `inc/RDRSVC.INC` | Partial: original guest service-number interface; preserve immutable deployed guest. | The source agreement is not proof that every installed guest was built from this file. |
| `inc/vdmredir.h` | Partial: selected original host/guest data interface with a limited real async route. | Verify mapped-buffer length/unaligned access, pending I/O cancellation, task death and per-operation providers. |

S12/S13/S14 should include callback reentry, real pending cancellation, mailslot/named-pipe identity, network unavailable/error and release ordering; record the existing remote-RAP limitation and any later policy-compatible recovery separately. No new runtime test, guest rebuild or new redirector implementation was performed.
