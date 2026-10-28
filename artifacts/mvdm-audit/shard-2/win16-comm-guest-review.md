# Shard 2: Win16 COMM guest source and delivered image

This review covers five pending manifest entries. The companion observer records hashes against both frozen original trees and catalogs, source relationships, and delivered image identity. It is read-only; it does not build or run guest code. Run `python artifacts/mvdm-audit/shard-2/win16-comm-guest-observer.py` to replay its assertions.

| Entry | Evidence and proposed classification |
| --- | --- |
| `wow16/drivers/comm/bimodint.inc` | Identical to both original trees; included by original `ibmint.asm` and `ibmsetup.asm`. Candidate exclusion as a historical guest build input only. |
| `wow16/drivers/comm/ibmcom.inc` | Identical to both originals; included by original communication driver sources. Candidate exclusion as a historical guest build input only. |
| `wow16/drivers/comm/comm.rc` | Identical to both originals; includes `comm.rcv` in the historical driver build. Candidate exclusion as guest build input only. |
| `wow16/drivers/comm/ddkmake` | Identical to both originals; historical alternative DDK build recipe. Candidate exclusion as a build recipe only. Its `iclean` target deletes `comm.drv`; it was not executed. |
| `wow16/drivers/comm/comm.drv` | Original image matches the current source-tree binary, `WINNT/SYSTEM32/COMM.DRV` in `assets/winnt.zip`, and installed `O:/winnt/system32/COMM.DRV` by SHA-256. Candidate partial: delivered binary identity is established, not execution or complete behavior. |

The historical makefile describes assembly objects, `comm.def`, resources and `link16` for `comm.drv`; the definitions include `inicom @1`. The exact binary match does **not** prove that this recipe built the delivered binary, nor that Windows loads or invokes it on a particular path. Earlier shard 3 review covers other members of this driver family; these recommendations do not resolve their independent status.

No live serial connection, guest call chain, interrupt ordering, callback, multiple-port operation, reset/teardown, or physical CTS/flow-control behavior was verified. Host WOW communication thunk presence is not proof that this guest image reaches a working physical serial endpoint. Preserve those behaviors and their outstanding evidence boundaries; exclusion above applies to individual build-only artifacts, never to the functionality they describe. No product, guest, shared ledger or build tree was modified.
