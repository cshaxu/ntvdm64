# mvdm-host-overlay

Private implementation support for material modern bindings that cannot remain
inside an original `mvdm-host` mirror file.  It is callable only by the
matching mirror; it exports no product ABI to app, session, a test, or another
component.

## Divergence register

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-010 | Persist `pHimemA20State` as a native `GetVDMAddr` pointer and write it after each A20 transition. | A native guest pointer cannot remain valid across calls or carry x64 width. | Retain the original 16:16 numeric location; the private state records the bound session and obtains a fresh one-byte lease for each state write. A stale or unavailable binding performs no write, matching the original null-pointer branch. | `xms.486/xms_a20_state.h`, `xms.486/xms_a20_state.c`; mirror call sites `../mvdm-host/xms.486/xmsa20.c`, `../mvdm-host/xms.486/xmsumb.c` |
| MVDM-HOST-DIV-011 | Read XMS move parameters and linear operands through a native guest pointer. | The source pointer is neither width-safe nor valid outside a bounded guest lease. | Read the original twelve-byte `SS:BP-12` layout under one lease, then call the same-shaped numeric XMS move seam. | `xms.486/xms_block_move.h`, `xms.486/xms_block_move.c`; mirror call site `../mvdm-host/xms.486/xmsblock.c` |
