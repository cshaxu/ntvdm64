# M0 T316 S1 — Monitor / `NtVdmControl` contract reconciliation

## Question

For the selected Win32/x86 CCPU40 product, which historical monitor ABI
surfaces are actually reached, which original callers own them, and what is
the smallest source-first recovery that does not reintroduce the NT4 kernel
VDM/V86 product?

## Inputs

- `src/opennt-abi/source/public/internal/base/inc/vdm.h`, the selected
  byte-for-byte OpenNT public/internal ABI mirror.
- `src/mvdm-host/dos/dem/demsrch.c`, `dpmi32/dpmiint.c`, and reached SoftPC
  host sources `nt_eoi.c`, `nt_sec.c`, and `nt_inthk.c`.
- `src/adapter-mvdm-host-out/monitor/{include,source}` and its focused
  `tests/adapter-mvdm-host-out/monitor/vdm_control_fixture.c`.
- The selected formal source graphs:
  `tools/build/New-T313CcpuLifecycleNinja.ps1` and
  `tools/build/New-T310OriginalSoftpcNinja.ps1`.

## Procedure

1. Compare the complete original `VDMSERVICECLASS`, service records and
   `VDM_TIB` definitions with the current monitor adapter declaration carrier.
2. Search every `NtVdmControl` and `VdmInstall*Handler` caller under the
   selected MVDM mirror; inspect each surrounding preprocessor condition and
   current CCPU40 graph selection.
3. Inspect every present `VDM_TIB` consumer and classify it as selected
   CCPU40, later owner package, or a non-runtime tool/product source.
4. Confirm that no selected CCPU40 graph defines `CPU_30_STYLE`, `MONITOR` or
   `V86`, and that the only formal adapter monitor source is `vdm_control.c`.

## Observations

### Original ABI and current declaration carrier

- The original `vdm.h` defines all fifteen service values in their historical
  order, including `VdmQueryDir`, `VdmDelayInterrupt`, `VdmInitialize`, and
  `VdmSetInt21Handler`; it also owns `VDMQUERYDIRINFO`, delay/INT21 records,
  interrupt/fault records and the full `VDM_TIB` layout.
- The product already mirrors that exact original header under `opennt-abi`.
  At S1 admission, the adapter's `monitor/include/vdm.h` independently
  copied a smaller subset and added session TLS state. It was therefore an
  avoidable adapter divergence, not the original ABI authority. S2 replaces
  it with the original header plus a narrow product-owned context carrier.
- The current `NtVdmControl` definition intentionally accepts only a bound,
  active-session `VdmQueryDir` callback. Its explicit unavailable result is
  `STATUS_NOT_IMPLEMENTED`; it does not enter the BaseSrv command route.

### Direct `NtVdmControl` callers

- `dos/dem/demsrch.c` calls `VdmQueryDir` after a reset-capable directory
  enumeration. This is the only current direct user-mode capability. The
  focused fixture proves unbound and unsupported service rejection, then a
  bound session callback success.
- `softpc.new/host/src/nt_eoi.c` calls `VdmDelayInterrupt` only inside
  `#ifdef MONITOR`. The selected CCPU40 build does not define `MONITOR`; its
  original non-monitor timer-count branch remains the selected body.
- `softpc.new/host/src/nt_sec.c` calls `VdmInitialize` only inside
  `#ifdef X86GFX`. That historical full-screen/ICA kernel setup is outside the
  selected non-invasive product profile and has no current runtime provider.
- `dpmi32/dpmiint.c` calls `VdmSetInt21Handler` only inside `#ifdef i386`.
  Current formal CCPU40 graphs do not define `i386`. The complementary
  non-`i386` DPMI interrupt registration calls the original
  `VdmInstall{Hardware,Software,Fault}IntHandler` functions instead.
- `softpc.new/host/src/nt_inthk.c` is already the original CCPU provider for
  those three installation forms: it stores the handlers in CCPU-owned
  variables and returns `STATUS_SUCCESS`. It is not an `NtVdmControl`
  dispatcher and must remain in the SoftPC mirror.

### `VDM_TIB` consumers

- `mvdm-host/inc/vdmtib.h` is the original declaration carrier (apart from its
  registered path-rooting include divergence). Its `VdmTib` declaration must
  use the full original `VDM_TIB` definition.
- `dpmi32` is the only selected-formal source family that needs the
  per-thread TIB carrier for its original interrupt/fault/DPMI records. DPMI
  execution is not admitted by T316; this records its ABI prerequisite only.
- `ieuvddex` (VDD/extension) and `bde`/`vdmexts` (debugging tools) include the
  header but are not selected CCPU40 runtime consumers. They retain their
  named VDD/debugger owner dispositions.
- No CCPU40 CPU/SAS/firmware selected source directly reads the monitor TIB.

## Disposition ledger

- `VdmQueryDir`: **direct user-mode binding**. Owner is DEM directory search;
  provider is the session-bound monitor callback and must retain the original
  structure and `NTSTATUS` direction.
- `VdmDelayInterrupt`: **kernel-VDM unavailable in the selected profile**.
  Its only direct caller is `MONITOR`-guarded; no generic timer dispatcher or
  monitor emulation may be introduced.
- `VdmInitialize`: **later graphics/device owner or unavailable**. It is
  `X86GFX`-guarded and must not be made successful by a partial ICA facade.
- `VdmSetInt21Handler`, LDT service classes and DPMI TIB state:
  **named later DPMI owner**. The present CCPU handler-install calls remain
  original SoftPC behavior and are not a substitute for kernel service
  success.
- Start/queue execution, feature query, printer, Adlib, PMCLI and process
  query classes: **kernel-VDM/CSRSS or named device owner unavailable**.
  The current adapter's deterministic unavailable result is correct until a
  separately admitted complete owner package proves a public user-mode path.

## Recovery plan bound by this audit

S2 shall remove the duplicate adapter `vdm.h` definitions rather than expand
them. It will include the existing original `opennt-abi` `vdm.h` through the
normal ABI include root, place only product-owned TLS `VdmTib` binding and
`pNtVDMState` declarations in a narrowly named monitor context carrier, and
retain the current one-service `VdmQueryDir` provider. It shall not add any
new service-class success path, alter CCPU40 behavior, select `MONITOR`, or
implement a generic control dispatcher.

## Confidence and follow-up

High confidence for the selected CCPU40 source and direct caller inventory:
the result comes from the original definitions, all four direct call sites,
the guard conditions, and both formal build manifests. The full runtime
behavior of future DPMI/VDD/debugger packages remains unclaimed and requires
their own owner-package admission.
