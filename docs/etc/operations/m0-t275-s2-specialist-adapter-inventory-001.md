# M0 T275 S2: missing-interface and specialist-adapter inventory

## Decision

Import selected original MVDM translation units first. A compile or link failure
at an external historical boundary is not permission to crop, rewrite, or
replace the reached OpenNT function body. It is an interface-debt record:

```text
original caller + original declaration + observable contract
    -> one named adapter component
    -> same-shaped modern implementation or explicit original-style failure
```

Only include-path, import-library, and statement-local call binding changes are
admissible in `opennt-mvdm-host` until this record has selected an adapter.

## Complete external-boundary inventory

This inventory is authoritative for the selected MVDM union. A later reached
interface is added here before it changes a mirror source. No generic
compatibility layer is permitted.

The machine-readable debt ledger is
[`m0-t275-s2-missing-interface-ledger.tsv`](m0-t275-s2-missing-interface-ledger.tsv).
It freezes twelve current boundary families. Before a package is linked, its
family row is expanded into one record per actually reached declaration.

### Existing adapters

- **`adapter-bochs` — Bochs mechanics.** It is the only production caller of
  `bochs-core` and owns construction, bounded run and opaque machine lifecycle.
  OpenNT-facing callers reach it only through `adapter-softpc`.
- **`adapter-bop` — copied BOP frame transport.** Evidence:
  `softpc.new/host/src/nt_bop.c` and `softpc.new/host/inc/host_bop.h`. It
  transports fixed-width frames and typed completion; selector/service meaning
  remains in imported MVDM dispatch code.
- **`adapter-softpc` — SoftPC/CCPU/SAS.** Evidence: MVDM `softpc.h`, `sas.h`,
  CCPU and `softpc.new/host` callers. It preserves reached call shapes and
  outcomes while translating only to typed `adapter-bochs` mechanics and
  session checked leases; it never includes a Bochs type.
- **`adapter-win32` — historical host APIs.** Evidence: DEM, COMMAND, OEM and
  utility Win32/NTDLL imports. It uses supported public Win32 APIs where they
  can retain the original call shape and failure result; otherwise it returns a
  source-recorded unavailable result, never a replacement provider.
- **`adapter-vdm-monitor` — VDM monitor APIs.** Evidence: `vdmtib.h`, SIM32/V86
  monitor declarations and callers. It owns `NtVdmControl`, `VDM_TIB`, events
  and handler-interface disposition with session/thread binding; private
  kernel/CSRSS operations remain explicitly unavailable.

### Specialist adapters established before source recovery

- **`adapter-redir` — VDMREDIR/Redirector.** Evidence: `vdmredir` packages
  including `vrdisp`, `vrinit`, `vrnmpipe`, `vrremote`, `vrnetapi`, plus
  `vdmredir.h`/`rdrsvc.h`. It owns redirector control, transport and named-pipe
  interface deficits. It may use `adapter-win32`, `session` and public IPC but
  may not reimplement Redirector provider logic.
- **`adapter-wow` — WOW32/WOWEXEC.** Evidence: WOW32, WOWEXEC and `wow*.h`.
  It owns historical host broker, callback and launch-interface deficits. It
  contains no guest NE/WOW provider or Bochs object.
- **`adapter-vdd` — VDD.** Evidence: `vdd` packages and `softpc.new/host` VDD
  callbacks. It owns registration/callback/host-bridge deficits. Device
  emulation stays Bochs-owned and VDD provider logic stays in the original
  MVDM package.
- **`adapter-debugger` — BDE/DBG/VDMDBG/VDMEXTS.** Evidence: those packages,
  their declarations and reached diagnostic hooks. It owns debugger product
  registration, notification and control-interface deficits, never debugger
  policy or a replacement debugger.

## Per-interface recovery record

Before an imported MVDM unit is enabled, its ledger row records:

1. selected source-union path and original caller symbol;
2. original declaration path, name, parameters, calling convention and layout;
3. the dependency preventing direct linkage;
4. exactly one owner adapter from this inventory;
5. exact reuse, same-shaped public-API implementation, or explicit failure;
6. permitted MVDM edits, normally include/import binding only, with
   `DIVERGENCE:` and README registration when source text changes; and
7. focused x86/x64 compile-link and positive/negative contract evidence.

If no adapter owner is valid, recovery stops at the debt record and the source
unit stays out of the enabled link graph. No local helper, generic shim,
altered provider algorithm, or direct Bochs call may bypass the record.

## S2 exit evidence

- All nine adapter components have a declared owner boundary and README.
- The S1 ledger assigns every known historical external interface family to
  this inventory and future symbol records link to it.
- Architecture, coding rules, current proposal and status require the
  import-first/interface-debt workflow.
- S2 adds no adapter ABI and makes no mirror-source change.
