# M0 T371 S1 — Original XMS, DPMI and DOS-extender execution topology

## Question

Before changing a selected XMS or DPMI body, what complete original execution
topology is already present in the CPU40/x86 product, where is its first
whole-source recovery cohort, and which boundaries remain outside it?

## Inputs

- The selected original `mvdm-host` XMS (`xms.486`), DPMI32 (`dpmi32`) and
  SoftPC host sources.
- The CPU40/x86 formal Ninja graph produced for M0 T370 S3.
- The earlier XMS owner ledger
  (`m0-t310-s15-xms-owner-contract-ledger-001.tsv`) and the original DPMI
  package plan (`m0-t332-dpmi-owner-package-recovery-plan-001.md`).
- The immutable staged guest media and T370 fixed observation, used only to
  establish that no `52:xx` or `53:xx` operation has yet been reached.

## Original source topology

### Shared startup order

The original startup order is not a trace-selected BOP path:

1. `softpc.new/host/src/config.c` sets `xmsMemorySize`, initializes SAS with
   the selected VDM memory size, and initializes the original UMB list.
2. `softpc.new/host/src/nt_msscs.c:InitialiseDosEmulation` resets and installs
   the original SoftPC host environment, then calls `scs_init`.
3. `scs_init` calls `GetNextVDMCommand`, `CMDInit`, `DemInit`, **`XMSInit`**,
   and `DBGInit`, in that original order.  `XMSInit` failure terminates the
   current VDM through the original error path.

Therefore the first recovery cohort is original XMS initialization, not an
isolated `52:xx` service.  Its immediate prerequisites are selected CPU40/SAS
memory geometry and the original UMB initialization; its immediate consumers
are the original XMS dispatch table and later INT 15/A20 users.

### XMS ingress and services

`softpc.new/host/src/nt_bop.c:MS_bop_2` reads one service byte through the
original in-machine `Sim32GetVDMPointer`, calls original `XMSDispatch`, then
increments IP by one.  `xms.486/xmsdisp.c` owns the original dispatch table
for services 0--11:

- A20 control;
- move block;
- allocate, free and reallocate extended-memory blocks;
- page size and free-memory query;
- UMB initialize, request and release;
- INT 15 hook notification.

The original bodies own register results and failure directions.  No adapter
may become an XMS policy dispatcher.

`xms.486/xms.c:XMSInit` first reserves free UMBs, computes the original XMS
size, invokes `VdmAllocateVirtualMemory`, and initializes original `suballoc`
through one of two original callback sets:

- a VDM virtual-memory commit/decommit pair after successful reservation; or
- `xmsCommitBlock`/`xmsDecommitBlock`/`xmsMoveMemory` when the original
  allocator reports `STATUS_NOT_IMPLEMENTED`.

The latter callbacks already have a bounded source-shaped binding in
`adapter-mvdm-host-out/softpc`; that binding translates only checked
session-owned guest-memory ranges to the selected CPU40/SAS mechanism.  It
does not expose a host pointer to XMS, guest state or asynchronous work.

### DPMI and DOS-extender ingress

`softpc.new/host/src/nt_bop.c:MS_bop_3` directly calls original
`DpmiDispatch`.  `dpmi32/dpmi32.c` reads the subfunction byte with the same
in-machine `Sim32GetVDMPointer` lifetime, advances IP by one, bounds-checks
the index and enters its original 25-entry dispatch table.  The table owns
the selected original descriptor, protected-mode, interrupt, XMEM,
stack/fault and DOS-extender initialization control flow.

The `mvdm-host/dpmi` DOSX package is guest carrier media/source.  It is not a
host static library and must remain immutable guest input.  A DOSX workload
can reach the host DPMI table only after the existing DOS/COMMAND execution
continuity reaches guest DOSX handoff.

## Current formal graph and runtime boundary

The selected CPU40/x86 formal product already links both original owner
archives:

- `original-mvdm-xms.lib` includes the selected XMS, A20, block, dispatch,
  miscellaneous, UMB and XMS A20-state sources.
- `original-mvdm-dpmi32.lib` includes the selected DPMI32 dispatcher,
  descriptor, memory, stack, fault, DOSX initialization and associated
  original provider sources.

This proves source selection and link ownership only.  The one fixed T370
observation reached COMMAND `54:01`, `54:0E` and `54:04`, then a later
original `54:01` continuation; it did not reach `52:xx`, `53:xx`, XMS
initialization evidence or a DOSX transition.  It cannot be used as evidence
that either family runs.

## ABI and boundary findings

- The selected original `xms.h` declaration spells `xmsMoveMemory` arguments
  as `(Source, Destination, Count)`, while both selected original direct
  definitions (`xmsmemr.c` and `i386/xmsmem86.c`) spell them as
  `(Destination, Source, Count)`.  `SAInitialize` receives the callback as a
  function pointer.  This is an original source-contract ambiguity, not a
  license to invent a wrapper or reorder a call.  S2 must establish the
  `suballoc` callback typedef and every selected caller before accepting an
  executable XMS cohort.
- `52:xx` and `53:xx` use in-machine `Sim32GetVDMPointer` in the original
  synchronous SoftPC frame.  This is not a durable cross-component pointer
  and must not be replaced by a per-expression opaque token.
- Only identities that cross the host boundary use the session mapping
  manager.  In particular, the XMS fallback memory callbacks use the
  session-owned guest-memory mapping manager; numeric XMS addresses, sizes,
  offsets, selectors and service bytes retain their original numeric form.
- DPMI entries that require the historical kernel-VDM monitor, VDD/VCD,
  BaseSrv/CSR or a later guest DOSX handoff retain their source-shaped failure
  direction until their named owner packages are recovered.  S1 does not
  mark them successful merely because their translation units link.

## S2 admission decision

S2 is limited to the earliest complete original XMS startup cohort:

`config memory/UMB setup -> XMSInit -> original Vdm/suballoc callback
selection -> ExtMemSA allocation state`.

It must first settle the original `SAInitialize` move-callback ABI across its
definition, initialization and call sites.  It may then add a focused
source-shaped XMS initialization proof and rebuild the CPU40/x86 final
product.  It must not implement an individual `52:xx` service, alter guest
media, create a second XMS dispatcher, or claim guest runtime reach.

S3 remains the complete original DPMI/DOSX cohort and begins only after S2
has recorded the XMS startup result.

## Confidence and follow-up

High confidence for the source order, ingress and formal archive ownership:
each is directly visible in selected original sources and the formal graph.
Medium confidence for the runtime callback selection until S2 completes the
original `suballoc` ABI audit and focused initialization proof.  No runtime
claim is made for XMS, DPMI or DOSX.
