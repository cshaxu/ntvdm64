# Remaining DOS-host partial classification check

The current live shard decision fields were intersected with the `host-candidate` path set. No new DOS host C-source row in the inspected `dos/` and `dpmi32/` groups had both a selected physical source/body and independent behavior evidence supporting promotion to partial: remaining `dos/dem/demmsg.c` is a DBG-only diagnostic body; `dpmi32/i386/dpmimem.c` is an unselected native-monitor allocator distinct from the selected shared-pool owner. The relevant existing decisions and evidence are [DEM diagnostics](../host-dem/demmsg-profile-role-review.md) and [DPMI allocator](../nine-way/part-2/dpmi-i386-allocator-semantic-reconciliation.md). This is not a claim that DEM, DPMI or DOS functionality is absent.

Two remaining selected header paths deserve caution:

| Path | Exact observed usage | Supported decision |
| --- | --- | --- |
| `inc/monregs.h` | `nt_thred.obj` has a VALID dependency on this exact physical header, but inspected `nt_thred.c` does not directly invoke its register/flags declarations. | Keep pending for behavior. If the ledger explicitly allows declaration-only `partial`, label the limited meaning; S11 retains CPU register/ABI and lifetime evidence. See [original owner review](../host-inc/xms-redir-export-role-review.md). |
| `vdmredir/vrdlcdbg.h` | Selected `vrdlc5c.c`, `vrdlcpst.c`, `vrdlcbuf.c` include it; inspected diagnostic constants/dump functions are tied to DBG and no active constant use has been established. | Keep pending for behavior; a physical include cannot establish DLC functionality. S17 retains real command, completion, failure and retirement, plus any applicable diagnostic profile. See [diagnostic group review](vdmredir-debug-physical-owner-review.md). |

These observations cannot justify manufacturing a behavior-backed partial classification. They also do not establish a global exclusion; unobserved profiles and actual runtime behavior remain separate. Already-classified files were not reconsidered. No previously blocked probes, product changes, guest changes, tests or shared-ledger edits were made in this pass.
