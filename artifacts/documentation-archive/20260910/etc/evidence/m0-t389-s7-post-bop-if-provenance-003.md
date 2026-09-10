# M0 T389 S7 — post-BOP IF provenance observation

## Question

Does the selected BOP ingress/return binding clear IF after the normal-child
`COMMAND.COM` record, making the original CPU40 keyboard IRQ1 path unavailable?

## Method

The selected original `MS_bop_4` control flow in
`src/mvdm-host/softpc.new/host/src/nt_bop.c` was left unchanged:

```text
sas_load(selector) -> CmdDispatch(Command) -> setIP(IP + 1)
```

The existing default-off post-return observation copied the already-existing
CPU40 IF scalar together with CS:IP, AX and CF.  It neither reads guest memory
nor changes flags, registers, controller state, BOP routing or IRQ delivery.
The formal CPU40/x86 product was rebuilt and staged at `O:\ntvdm64`.  The
fixed Console-owning, non-debug observation executed the explicit
`command.com` product row for 30 seconds and offered the normal `ver` key
sequence only after the existing source-owned `AH=1` idle-status gate.

## Result

The product reported:

```text
MVDM-BOP-RETURN 54:01 cs=95EB ip=03C5 ax=0002 cf=0 if=0
MVDM-BOP-RETURN 54:0E cs=95EB ip=05DD ax=AD80 cf=1 if=1
MVDM-BOP-RETURN 54:04 cs=FE2E ip=6778 ax=0E0E cf=0 if=0
```

After `54:04`, the same fixed run continued through original NTDOS file/path
and read work, including the previously classified `$READ+83` offset.  The
Console key was accepted by the original worker and reached the unchanged
8042/ICA request prefix, but the CPU40 hardware-interrupt predicate still
reported `if=0`; no port-60 drain was observed.

## Source-backed disposition

`MS_bop_4` has no flag mutation between `CmdDispatch` and the observation.
The fact that `54:01` already returns with IF clear also predates the later
`54:04` return.  Thus this result does not support a repair that forces IF,
synthesizes IRQ1, drains port 60 from the host, or changes BOP return state.

It proves instead that the probe offers its key while the original normal-child
load/NTDOS continuation is still executing with interrupts disabled.  The
first remaining prerequisite is a source-defined transition from that
continuation into the second `COMMAND.COM` DOS `CON` wait.  The current
`AH=1` idle-status marker is not such a transition and must not be used as a
prompt gate.

## Limits

This bounded observation does not prove that IF remains clear forever, does
not identify a fault in the original NTDOS code, and does not prove an
interactive prompt, `ver`, line editing, Ctrl+C or child return.  It solely
rules out the adapter/BOP-return and keyboard-controller explanations at this
point in the original control flow.
