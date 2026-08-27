# M0 T291 S3 P5 — WOW synchronous stack/simulation boundary

## Question

Can the original non-fast `CallBack16` transition be kept separate from
`GetNextVDMCommand` command routing while retaining its reached SoftPC call
shape?

## Original shape

`mvdm-host/wow32/wcall16.c` constructs `CBVDMFRAME`, then performs:

1. `SETVDMSTACK(ptd->vpCBStack)`;
2. `SaveIp = getIP()`;
3. `host_simulate()`;
4. `setIP(SaveIp)`;
5. `ptd->vpStack = VDMSTACK()`; and
6. reads `wAX`/`wDX` from that callback frame before restoring the task stack.

This is a synchronous CCPU/monitor guest re-entry. It is not a
`GetNextVDMCommand` / BaseSrv command-broker operation.

## Implementation

The existing SoftPC outgoing adapter now provides the reached original
`host_simulate` spelling and source-shaped numeric VDM stack forms.
`mvdm_vdm_stack_set` atomically commits the original 16:16 `SS:SP` split using
the typed real-mode `adapter-bochs` frame; `mvdm_vdm_stack_copy` reconstructs
only the original numeric `SS:SP` value; and `host_simulate` delegates the
finite resumed CPU loop to `adapter-bochs`.

`mvdm_wow_callback_transaction` invokes those forms directly. The old P3
temporary `MVDM_WOW_CALLBACK_CONTROL_OPERATION` session route is removed.
Only the callback guest frame supplies its returned AX:DX. Thus COMMAND and
future WOW `GetNextVDMCommand` callers share only the neutral command-request
operation; they do not own or invoke this callback transaction.

## Verification

- The x86 and x64 formal stack-simulation fixture passes the exact numeric
  `VDMSTACK` result, atomic `SETVDMSTACK` replacement and
  `host_simulate` resume/loop call order.
- The x86 and x64 formal callback fixture passes the complete copied callback
  frame sequence with no session control-route registration. Its simulated
  guest completion writes AX/DX only to the callback frame, which the host
  code rereads after the source-shaped interval.

The first fixture uses typed `adapter-bochs` test doubles to isolate original
SoftPC call order. Existing `adapter-bochs` real-mode frame fixtures remain
the mechanical owner evidence. A later whole-package WOW admission must add a
real loaded Win16 callback-stream integration test; no WOW provider or BOP
route is enabled by this S.

## Exclusions

Fast WOW/`CurrentMonitorTeb`, NT4 BaseSrv/CSR, WOWEXEC, a new callback worker,
and any selector/BOP enablement remain outside this S. No native pointer,
HANDLE or extra mapping-manager instance is introduced.
