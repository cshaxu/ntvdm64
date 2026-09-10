# M0 T289 S5 P4 — DPMI stack-frame dependency audit

## Scope

This audit covers every still-uncomposed original `dpmi32/stack.c` and
`dpmi32/dpmiint.c` stack, interrupt, IRET and fault path after P1--P3. It is
an implementation gate, not a runtime-success claim.

## Proven source dependencies

- `DpmiSwitchToDosxStack` and `DpmiSwitchFromDosxStack` consume
  `DosxStackFramePointer`. The selected original C package only initializes
  that historical process pointer to `NULL`; its live backing relationship is
  established by DOSX/monitor descriptor setup, not by an admitted C source
  body. Replacing it with a native pointer or arbitrary allocation would
  change source ownership. It waits for the monitor-owned descriptor-backed
  VDM-TIB/DOSX projection.
- `DpmiPassPmStackInfo` is now safe at the host boundary: its original
  `CX:DX` pointer shape is a session identity. The original DOSX assembly
  immediately uses it to create `SEL_VDMTIB`; that descriptor must expose a
  copied, session-owned guest projection, not resolve the token into a host
  pointer. This is the same monitor projection dependency.
- `DpmiPushRmInt` and `DpmiSimulateIretCF` write a real-mode stack and read
  IVT entry 0. Their original `Sim32GetVDMPointer` aliases need a distinct
  selector-blind real-mode checked frame operation. The existing P2 primitive
  correctly refuses to pretend that a protected same-CPL transaction covers
  real-mode state.
- `DpmiSwIntHandler`, `DpmiHwIntHandler`, both `DpmiIntHandlerIret*`, and
  the fault/exception paths require both checked protected stack-frame
  copy-in/copy-out and source `Ldt[]` segment-type/limit queries. Current
  descriptor publication deliberately retains the original host-LDT failure;
  creating a second host LDT or making unchecked Bochs reads would violate the
  package boundary.
- The non-`i386` executor/monitor callback branches (`VdmInstall*Handler`,
  CCPU instruction emulation and host LDT paths) remain excluded by their
  original target profile. They are not silently replaced by a second CPU.

## Disposition

P1 registration, P3 locked-stack switching and the PM-stack identity are
locally composed on x86/x64. All remaining source functions remain compiled
but selector-disabled until their named mechanical/monitor dependencies
exist. They must report their original unavailable direction when admitted;
they may not be reached through a partial live-register setter.

## Required next owners

1. A monitor/descriptor package supplies the guest-visible, session-owned
   VDM-TIB/DOSX projection and source-shaped descriptor lifecycle.
2. A selector-blind adapter-bochs package supplies an atomic checked real-mode
   stack/IVT frame primitive if the original real-mode paths are admitted.
3. The DPMI descriptor-publication package replaces the recorded host-LDT
   unavailable outcome with a Bochs-owned descriptor mechanism before
   protected interrupt, IRET or fault dispatch can run.
