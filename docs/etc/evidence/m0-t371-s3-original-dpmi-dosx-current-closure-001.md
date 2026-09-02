# M0 T371 S3 — Original DPMI/DOSX current closure

## Question

Does the current CPU40/x86 product still carry the original DPMI/DPMI32
provider package and its source-shaped boundaries, or did XMS recovery expose
a missing DPMI adapter or an autonomous replacement path?

## Inputs

- Selected original `mvdm-host/dpmi32` and immutable guest
  `mvdm-host/dpmi` DOSX carrier.
- T332's original DPMI source, span/identity/monitor and package-closure
  evidence.
- The freshly generated and linked `build/M0-T371/S2/formal-x86` graph.

## Current source disposition

- `MS_bop_3` remains the original direct `DpmiDispatch` ingress; no adapter
  recognizes or dispatches a `53:xx` selector.
- Original `DpmiDispatchTable` retains all 25 entries and original ordering.
  The selected common CPU40-compatible bodies own initialization, descriptor,
  memory, stack/fault, INT 21 translation and DOSX initialization behavior.
- The selected `Sim32GetVDMPointer` contract remains an in-machine CPU40
  pointer lifetime.  It is not replaced with a DPMI-local mapping manager.
- The existing `mvdm_host_identity_*` binding uses the session
  host-resource mapping-manager instance only for original XMEM/PM-stack
  host-boundary identities; it does not publish native pointers to DPMI
  registers.
- `NtVdmControl(VdmSetInt21Handler)`, kernel-V86 fast-BOP/debug paths and
  VDD/VCD services retain the previous exact unavailable/later-owner
  dispositions.  No success shortcut was added.
- DOSX remains immutable guest source/media.  It is not, and must not become,
  a host static-library input.

## Formal graph observation

The current `original-mvdm-dpmi32.lib` contains the selected original
`debug`, `dpmi32`, `dpmiint`, `dpmiselr`, `buffer`, `data`, `int21map`,
`modesw`, `register`, `savestat`, `stack`, `vxd`, `xmem`, `dpmimemr` and
`dpmimscr` objects.  The fresh `original-softpc-process.exe` directly consumes
that archive beside the original DEM, COMMAND, XMS, SubAlloc and CPU40
archives.  The final link completed without `/FORCE`.

## Interpretation

T332's source/product-route closure remains current after the XMS callback
correction.  There is no new DPMI implementation task in S3: creating a
second DPMI dispatcher, a DPMI-specific mapper or a synthetic successful
protected-mode response would reduce original fidelity.

No guest workload has reached `MS_bop_3`, protected mode or a DOSX transfer.
That lack of runtime reach is owned by the earlier COMMAND/DOS execution
continuity, not by an unlinked DPMI provider.  S4 may make one fixed
post-cohort observation and must record its first actual source-owned result
without turning it into a DPMI leaf-repair request.
