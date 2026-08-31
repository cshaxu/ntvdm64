# M0 T332 S1 — Original DPMI/DPMI32 source, ABI and failure admission

## Result

S1 admits one complete owner package, rather than a trace-selected `53:xx`
repair: the original host provider in `src/mvdm-host/dpmi32` and the distinct
DOSX guest carrier in `src/mvdm-host/dpmi`.  The 25 entries in the original
`DpmiDispatchTable` keep their original order and bodies.  DOSX remains guest
source/media and is never a host library.

The current CPU40 graph selects all thirteen common provider translation
units plus the original portable `dpmimemr.c` and `dpmimscr.c` fallback pair
under `LINKED_INTO_MONITOR`.  The manifest's `i386/dpmi386.c`,
`i386/dpmimem.c` and `i386/dpmimisc.c` are retained as original evidence, but
are not silently treated as present runtime behavior.  They contain the
historical CPU/monitor-specific route and need an explicit S4 disposition.

The detailed source/table/import result is in
[`m0-t332-s1-dpmi-source-abi-admission-ledger.tsv`](../operations/m0-t332-s1-dpmi-source-abi-admission-ledger.tsv).

## Shared contracts discovered

- `Sim32GetVDMPointer` is used by the dispatcher and the selected DPMI
  providers for BOP-byte fetch, DOSX shared tables, stacks, LDT/IntelBase
  state and response buffers.  It cannot be replaced by a DPMI-local mapper.
  S2 owns one session guest-memory mapping-manager lease/copy contract for
  these uses.
- `dpmiint.c` has the source-shaped `VdmTib` and
  `NtVdmControl(VdmSetInt21Handler, ...)` dependencies.  The current monitor
  adapter supplies TLS layout and only `VdmQueryDir`; it correctly returns
  `STATUS_NOT_IMPLEMENTED` for `VdmSetInt21Handler`.  S2 must record the
  monitor disposition before S4 can exercise interrupt registration.
- `mvdm_host_identity_*` is not a second mapping manager.  Its implementation
  delegates to `session_host_resource_mappings(instance)`, the session's
  host-resource mapping-manager instance.  It preserves original 32-bit
  numeric carriers for XMEM and PM stack-information publication without
  placing native pointers in guest registers.  S2 must verify its allocation,
  resolution and teardown lifetime alongside the separate guest-memory
  instance.

## Mirror differences

The current original-mirror differences are limited to source bindings already
marked in the affected files: `MVDM-HOST-DIV-018` (PM-stack information host
identity), `MVDM-HOST-DIV-012` (XMEM host identity), `MVDM-HOST-DIV-139` and
the related pointer-width/include binding differences.  They remain pending
contract verification; S1 does not convert a successful static link into a
runtime claim.

## S2 admission

The next work is a shared-contract recovery, not an isolated DPMI service:
bounded guest-span access, host-resource identity lifetime and the
monitor/TIB/NtVdmControl failure boundary.  No new DPMI dispatcher, raw host
pointer, kernel VDM/CSRSS reconstruction, second executor or source-derived
success result is admitted.
