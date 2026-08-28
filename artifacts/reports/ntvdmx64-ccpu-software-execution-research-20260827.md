# NTVDMx64 CCPU Software Execution Research

Date: 2026-08-27  
Scope: source-reading research only; no source, build, configuration, driver, or host-system change.

## Question

How can NTVDMx64 run NTVDM-related code on an x64 host without Windows Virtual-8086 execution, and is its CCPU route a credible future path for replacing the Bochs machine backend with restored SoftPC?

## Short conclusion

NTVDMx64's CCPU route avoids native V86 execution by running a software x86 interpreter in an ordinary user-mode process.  It does **not** restore V86 support to 64-bit Windows.  The promising reusable idea is the original OpenNT SoftPC/CCPU execution model; the NTVDMx64-specific host modifications, injection model and HAXM option are not suitable product dependencies for NTVDM64.

CCPU is therefore a serious *candidate backend*, not a small V86 patch.  Replacing Bochs requires a whole-closure feasibility audit for CCPU/SAS/interrupt/device/BIOS/monitor contracts before any backend switch.

## Software CPU path

The relevant original-shaped execution flow is:

```text
guest CPU state + SAS-provided guest memory
  -> c_cpu_simulate()
  -> CCPU software fetch / decode / execute loop
  -> BOP, I/O, interrupt, fault or controlled return
  -> BIOS[] / MVDM host provider
  -> c_cpu_continue() or monitor return
```

`c_cpu_simulate()` owns the interpreter entry.  The guest's execution mode, registers, segments and instruction effects are software state; the x64 host processor never needs to execute guest code in its VM/V86 hardware mode.  The address/segment/memory path belongs to the CCPU/SAS contract, not to a cast from an OpenNT `ULONG` guest address to a native host pointer.

The CCPU BOP convention is particularly relevant: its decoder recognizes the SoftPC BOP form (`C4 C4` plus selector), advances the guest instruction pointer according to the historical contract, then enters the selected `BIOS[]` service.  This is different from the current Bochs route, where a mechanical CPU event must be bridged outward before BOP dispatch.

## What NTVDMx64 adds

NTVDMx64 also has a separate optional HAXM route.  It opens `\\.\\HAX`, creates a VM/VCPU through private driver IOCTLs, transfers CPU state and converts HAX exits (I/O, memory, BOP, IRQ and related exits) into old VDM events.  That is hardware-assisted virtualization, not the CCPU software path.

The project also relies on product-specific host measures: injection/patching of old system components, private console/CSRSS compatibility handling and, for HAXM, kernel-driver installation and boot/signing configuration.  Those measures conflict with this project's portable, non-invasive host-installation goal and are evidence only.

## What is reusable for NTVDM64

- Original CCPU control-flow shape: initialization, bounded interpreter entry, continuation and controlled return to the monitor.
- Original CCPU BOP decode and exact instruction-pointer advancement rules.
- Original ownership boundary: CPU execution, address translation and machine events remain below MVDM BOP/DEM/COMMAND business logic.
- A source-shaped SAS facade whose guest-address side remains 16/32-bit while the backend uses the session-owned guest-memory mapping manager and checked Bochs memory access.
- The distinction between an asynchronous BaseSrv/command control plane and synchronous `host_simulate`/WOW callback re-entry.

## What must not be copied

- AppInit/system-DLL injection, binary patching, registry/boot-policy changes, or an installed kernel driver.
- HAXM private IOCTL ABI or its driver-dependent execution model.
- Raw guest-address-to-host-pointer casts, process-global NT4 VDM state, or unmanaged TEB/VDM_TIB assumptions.
- NTVDMx64's known added CCPU/SAS placeholder vectors such as `c_sas_touch` or `c_VirtualiseInstruction`; available evidence shows no original selected OpenNT provider for them.  Treating placeholders as recovered semantics would hide a missing contract.

## Why this is not merely “add V86 support to SoftPC”

CCPU removes the need for Windows to provide V86 execution, but a usable CCPU machine still needs a coherent closure for:

- SAS guest-memory, segment, descriptor and paging behavior;
- PIC/IRQ acknowledgement, I/O ports, A20 and reached device behavior;
- ROM/BIOS and BOP selector table lifecycle;
- `host_simulate` and monitor return/re-entry contracts;
- bounded session state and the mapping-manager backed guest-memory lease rules.

Moreover, the original NT4 x86 shipping product did not simply use the CCPU path as its ordinary x86 monitor.  The historical CCPU interpreter exists and is valuable source evidence, but its host-side integration has build-profile and generated-ABI gaps.  A backend decision must not mistake a successful CCPU compilation fixture for a complete x86 NTVDM product closure.

## Strategic comparison

- **Retain Bochs now:** it supplies a mature x86 CPU/FPU and machine backend while MVDM's external host boundary, Base VDM protocol and original provider packages are recovered.
- **Study CCPU as an alternative:** it can ultimately reduce the translation distance from original SoftPC/MVDM control flow, especially for BOP and `host_simulate` paths.
- **Do not dual-build prematurely:** switching to CCPU before proving its full machine closure would create a second incomplete executor and repeat the same external-interface work.

The lowest-risk sequence is to keep the present Bochs backend as the working mechanical baseline, complete the source-level CCPU/SoftPC closure audit against the same adapter/session contracts, then make a deliberate backend decision from measured gaps rather than from NTVDMx64's invasive product shell.

## Required future feasibility gate

A CCPU replacement proposal should not be admitted until it provides:

1. a complete reached CCPU/SAS/SoftPC interface ledger, including generated vectors and original provider evidence;
2. a per-interface disposition: direct original, same-shaped adapter, bounded overlay, or explicit unsupported result;
3. a Bochs-versus-CCPU capability comparison for CPU/FPU, V86/protected mode, IRQ/PIC, memory/A20, BIOS/BOP and monitor return;
4. one bounded native execution demonstration using original CCPU BOP dispatch without a raw host pointer crossing the session boundary; and
5. a migration decision showing whether Bochs can be removed without weakening existing guest, host or session contracts.

## Evidence read

- `O:\repos.hobby\ntdos64\docs\NTVDMX64-NONINVASIVE-RUNNER-ASSESSMENT.md`
- `O:\repos.hobby\ntdos64\docs\CCPU-SOFTWARE-MONITOR-ROUTE.md`
- `O:\repos.hobby\ntdos64\docs\CCPU-MISSING-INTERFACE-CONTRACT.md`
- `O:\repos.hobby\ntdos64\docs\BOP-DISPATCHER-SOURCE-RECOVERY.md`
- `O:\repos.hobby\ntdos64\artifacts\reports\legacy-root-milestones\M11-NTVDMx64-direct-source-evidence.md`
- `O:\repos.hobby\ntdos64\artifacts\reports\legacy-root-milestones\M87-NTVDMX64-HAXM-ADAPTER-BOUNDARY-AUDIT.md`

## Confidence and limitation

Confidence is high for the architectural distinction between software CCPU execution, HAXM and NTVDMx64's invasive host shell.  Confidence is medium for the final CCPU replacement cost because the required CCPU/SAS/device/monitor closure has not yet been completed as one audited, runnable package in this repository.
