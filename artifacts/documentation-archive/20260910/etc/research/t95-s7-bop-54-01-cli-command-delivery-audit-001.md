# T95 S7 BOP 54:01 CLI Command Delivery Audit

## Reachability Fact

The retained r42 trace reaches `C4 C4 54 01` at `95AB:03C1`.  In the pinned
OpenNT definitions this is `BOP_CMD:SVC_CMDGETNEXTCMD`.  SoftPC's original
`MS_bop_4` reads the fourth byte at current `CS:IP`, calls `CmdDispatch`, then
advances IP by that service byte.  `CmdDispatch` selects `cmdGetNextCmd` for
service `01`.

No Bochs device, BIOS or CPU facility is implicated.  When the current
identity-only listener falls through, CPU #UD uses the guest INT6 vector.  The
observed `C4 C4 06` loop is the historical SoftPC `BIOS[]` `illegal_op_int`
table entry, and is evidence of the missing `54:01` delivery—not an endpoint
to implement.

## Original Guest Contract

COMMAND.COM calls this service in `dos/v86/cmd/command/tcode.asm` with
`DS:DX` naming a guest-resident packed `CMDINFO` request.  On normal return it
tests Carry: clear continues to `run_cmd`; set means the supplied environment
buffer was too small and `AX` carries the requested size for its retry path.

The relevant `CMDINFO` fields from `inc/cmdsvc.h` are all 16-bit guest values:

| Field | Direction | CLI-contained meaning |
| --- | --- | --- |
| `EnvSeg`, `EnvSize` | input / conditional response | guest environment buffer and capacity; required only when an explicitly selected environment is delivered. |
| `CurDrive`, `NumDrives` | response | frozen CLI/session drive policy, never a guest-triggered enumeration. |
| `CmdLineSeg`, `CmdLineOff` | input | guest UCOMBUF destination for the command tail. |
| `CmdLineSize` | input | destination capacity bound. |
| `ReturnCode` | input | prior command result on repeat call. |
| `ExecPathSeg`, `ExecPathOff`, `ExecPathSize`, `ExecExtType` | response | guest-visible selected executable pathname, size and extension classification. |
| `CodePage`, `fTSRExit`, `pRdrInfo`, standard-handle fields | response / historical integration | not admitted until their owning coherent capability is selected; no host pointers or native handles may cross. |

`cmdGetNextCmd` calls the invasive NT `GetNextVDMCommand` API, conditionally
merges environment, applies PIF policy, changes console/current-directory
state, processes standard handles, and may terminate or block the VDM.  Those
branches are historical semantic evidence only.  The non-invasive CLI product
must replace them with one immutable command-envelope selected before session
creation; it must not emulate ambient NT process policy.

## Required Replacement Shape

The next implementation is a **single-use, immutable command envelope**
owned by CLI and copied into the adapter session before Bochs starts.  It must
contain only bounded DOS-visible data: canonical guest executable path,
already-normalized command tail, declared DOS extension type, optional bounded
environment payload, and frozen current-drive policy.  It must not contain a
host path, Windows handle, callback, pointer, or a request to inspect host
state.

At `54:01` the adapter will eventually receive fixed-width CPU/window records
and a checked guest-range request for the `CMDINFO` structure.  It may return
only a checked generic response composed of bounded ordinary-RAM writes plus a
fixed-width CPU delta.  The response must either:

1. deliver the immutable envelope once and clear Carry; or
2. return the source-shaped environment-capacity retry (`CF=1`, bounded AX
   size) without writing a partial envelope; or
3. fail closed with a documented DOS-visible error for an absent/exhausted
   envelope.

There is no direct handler in Bochs and no exception recovery for selector
`06`.  The BOP decoding/dispatch decision belongs to the adapter composition
layer only after request decoding, write layout, retry state and CLI ownership
are independently tested.

## Follow-up Gate

Before implementation, close a versioned copied `CMDINFO` guest-layout ABI
and a CLI envelope profile contract, then test first delivery, capacity retry,
second delivery/exhaustion, malformed ranges and no-pointer/no-host-path
invariants.  Only then admit the generic BOP-to-adapter result routing needed
to consume that contract.
