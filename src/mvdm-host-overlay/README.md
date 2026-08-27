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
| MVDM-HOST-DIV-013 | Read and normalize a protected guest LDT-entry array through `Sim32GetVDMPointer`, then install it into the host x86 LDT. | A raw guest pointer is not durable on x64, and modern user mode cannot own the historical host LDT. | The private buffer acquires/releases one checked selector-bounded byte span around the unchanged source loop.  The original final `DpmiSetX86Descriptor` import returns its source failure direction until a later Bochs-native descriptor-publication package is admitted. | `dpmi32/dpmi_descriptor_buffer.[ch]`, `dpmi32/dpmi_descriptor_unavailable.c`; mirror call site `../mvdm-host/dpmi32/dpmiselr.c` |
| MVDM-HOST-DIV-014 | Fill the original DPMI memory-info response through a protected `Sim32GetVDMPointer` alias. | The original PVOID is an unbounded native alias of guest memory. | The private generic DPMI buffer performs one checked copy-in/copy-out around the unchanged original field sequence. | `dpmi32/dpmi_protected_buffer.[ch]`; mirror call site `../mvdm-host/dpmi32/dpmimemr.c` |
| MVDM-HOST-DIV-015 | Persist DPMI DOSX shared-data, DTA and selector-table raw pointers in process globals. | Those pointers are invalid across bounded leases and cannot represent a modern x64 Bochs guest. | A private per-session numeric state record preserves the original decoded values and field order.  It is not a mapper, guest pointer, host pointer or second CPU state. | `dpmi32/dpmi_session_state.[ch]`; mirror call sites `../mvdm-host/dpmi32/dpmi32.c` |
