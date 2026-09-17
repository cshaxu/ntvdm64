# M0 T420 S18 DPMI32 Package Recovery

## Complete selected unit

The selected `src/mvdm/dpmi32/sources` unit has fifteen production bodies:
`debug`, `dpmi32`, `dpmiint`, `dpmiselr`, `buffer`, `data`, `int21map`,
`modesw`, `register`, `savestat`, `stack`, `vxd`, `xmem`, `dpmimemr`, and
`dpmimscr`.  Its reached public surface is `dpmi32p.h`, `dpmidata.h` and
`dpmiint.h`.  There is no DPMI32 overlay and no adapter-owned duplicate DPMI
provider in the formal product graph.

Nine selected bodies now match the pinned OpenNT body after normalizing line
endings: `buffer`, `int21map`, `debug`, `register`, `savestat`, `stack`,
`vxd`, `xmem`, and `dpmimscr`.  `i386/dpmi386.c` and `i386/dpmimem.c` are
original x86 kernel-VDM providers, not omitted portable bodies: their
passive-`CONTEXT`/kernel-`VDM_TIB` mechanism cannot execute inside CPU40.
The selected portable `modesw.c` and `dpmimemr.c` are therefore the correct
source owners for the standalone profile.

## Source-first disposition

The S18 sweep removed all reached source drift that has no CPU40 or
standalone meaning:

- Restored the original low-memory arithmetic in `buffer.c` and `int21map.c`.
- Restored the original interrupt-vector expression in `dpmiint.c` and removed
  its unused fault-CS/fault-IP reads; no fault behavior, dispatch, or return
  value changes.
- Removed the inert `MVDM-HOST-DIV-164` observation comment from `dpmi32.c`.

The remaining differences are finite, non-duplicated execution boundaries:

| Boundary | Why the original body alone cannot run | Current owner/disposition |
| --- | --- | --- |
| GDT/LDT publication | Original `NtSetLdtEntries` writes a process LDT while DOSX also supplies a GDT image. CPU40 needs two active guest descriptor images. | `dpmi32.c`, `dpmiselr.c`, `data.c`; retain GDT/LDT shadows and selector-domain fixture. |
| 53:01/FD mode transitions | Original `i386/dpmi386.c` writes passive kernel `CONTEXT` fields and NTVDM state bits. CPU40 has no trap return; it must load the active register and hidden segment caches at the same source-defined transition. | `modesw.c`; retain CPU40 transition carrier, not a second DPMI policy. |
| Final IDT | DOSX publishes `SEL_IDT` after `MoveDscrTables`; native VDM installs it through kernel state. | `data.c` resolves the final source-published descriptor; fixture proves absence, final publication, replacement and limit rejection. |
| PM stack information | Original x86 gives DOSX a kernel-owned `VDM_TIB` address. A host address is invalid guest data. | `dpmiint.c` projects the original `VDM_DPMIINFO` layout into leased guest memory. |
| DPMI allocations | CPU40 XMS owns the reserved extended range before DOSX reaches allocation services; generic `VdmAllocateVirtualMemory` therefore has no free extent. | `dpmimemr.c` calls the original shared-XMS `SA*` allocator; no autonomous allocator is introduced. |
| Interrupt/emulation hooks | Original monitor callbacks and `VDM_TIB.Ldt` are kernel carriers. | `dpmiint.[ch]` chooses the original non-monitor path and CPU40 descriptor carrier, preserving original handler policy. |

The kernel sources confirm the distinction: `base/ntos/vdm/x86/strtexec.c` and
`vdmints.c` require a trap frame and per-thread `VDM_TIB`; `psldt.c` under
`base/ntos/ps/x86` owns `NtSetLdtEntries`. They are not user-mode MVDM functions that
can be copied into the worker.  No new kernel mirror or substitute was added.
Worker exit remains the teardown boundary: its address space releases the
three CPU40 guest projections together.  There is no worker reuse contract,
so adding a speculative in-process reset state machine would diverge from the
single-worker product lifecycle.

## Footprint

- This S changes four existing mirror bodies by **+3/-18 lines** (net
  **-15**): three source-expression restorations and fifteen removed inert or
  dead lines.
- DPMI32 overlay: **0 added, 0 removed**. Adapter/autonomous product code:
  **0 added, 0 removed**.
- Against the pinned source with line endings ignored, the remaining DPMI32
  mirror delta is **+428/-27 lines** across the nine finite CPU40 boundary
  files above. It is a classified residual, not unreviewed local policy.

## Verification

- Fresh formal graph: `build/M0-T420/S18/formal-x86-001`; its original-candidate
  compile completed 448 nodes, and explicit `product-programs` linking produced
  `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe` and `VDMREDIR.dll`.
- The fresh `cpu40-descriptor-domain-fixture.exe` passed. It covers distinct
  GDT/LDT selection, absent descriptors, the final DOSX IDT descriptor,
  replacement publication and invalid IDT limits.
- The four new EXEs were deployed to `O:\winnt`; SHA-256 values are
  `03720e46a4cb58af7ca9d114a3e1400d31bf343345d4e05f6c45b40feb8848c1`
  (`run16`), `59d65e152346abbdba6a27bd3df3f472d4289fcd8723665a8a8fc36c32ae34b4`
  (`basesrv`),
  `cb7fa8b6fae1be893948eeec235771014d31f14bd4f16d987cd7310651284887`
  (`ntvdm`) and `a15e6fe44c3d37c747661f9657de8852b16a770df92b760994549c6513dd3cd7`
  (`dtmgr`).
- Real Console regressions passed for `run16.exe MEM.EXE` and
  `run16.exe COMMAND.COM /C MEM.EXE`; both emitted normal DOS MEM output and
  returned. The owner-accepted interactive COMMAND → COMMAND/MEM/EDIT → MEM
  route remains the established baseline and is not altered by this
  DPMI32-only source recovery.

S19 owns the load-only guest `dpmi` manifest and DOSX activation/return
contract. It must not re-open S18 host descriptors without new evidence.
