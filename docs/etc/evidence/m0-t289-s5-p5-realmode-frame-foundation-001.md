# M0 T289 S5 P5 — Selector-blind real-mode frame foundation

## Question

Can the project provide the atomic real-mode CPU-frame operation required by
the original DPMI stack/interrupt paths without giving Bochs any DPMI, BOP,
interrupt-vector, guest-memory or OpenNT meaning?

## Inputs

- Original `mvdm-host/dpmi32/stack.c` uses a 16-bit `SS:SP` stack and changes
  `CS:IP` after its stack/IVT work.
- The P4 dependency audit established that its raw-pointer memory work is a
  separate bounded guest-memory requirement.
- `adapter-bochs` owns the sole public mechanical access to `bochs-core`.

## Procedure

1. Added a copied real-mode frame record to `adapter-bochs`; it represents
   only mode/CR0, CS, SS, SP, IP and FLAGS.
2. Added the smallest private `bochs-core-overlay` member operation. It
   verifies real mode before any mutation, then applies only the supplied
   real-mode register image.
3. Ran the formal `/MT` MSVC/Ninja fixture on both host architectures:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File \
     tools/build/New-T289S5RealModeFrameNinja.ps1 -Architecture x64
   ninja -C build/M0-T289/S5/realmode-frame-x64
   build/M0-T289/S5/realmode-frame-x64/bin/t289-s5-realmode-frame-fixture.exe

   powershell.exe -ExecutionPolicy Bypass -File \
     tools/build/New-T289S5RealModeFrameNinja.ps1 -Architecture x86
   ninja -C build/M0-T289/S5/realmode-frame-x86
   build/M0-T289/S5/realmode-frame-x86/bin/t289-s5-realmode-frame-fixture.exe
   ```

## Observations

- Both x64 and x86 fixtures exited zero.
- A candidate copied frame changed CS/SS/SP/IP and CF only after an exact
  current-frame comparison.
- A reserved FLAGS-bit change was rejected without architectural mutation.
- A frame based on the earlier snapshot was rejected as stale.

## Interpretation

The required CPU-only half of the historical real-mode stack/interrupt path
is now available as a selector-blind adapter-bochs primitive.  It is not an
interrupt emulator and does not read an IVT, resolve a pointer, write a guest
stack or enable a DPMI selector.

## Remaining boundary

`DpmiPushRmInt`, `DpmiSimulateIretCF` and the real-mode branches of
`dpmiint.c` still need source-shaped bounded guest-stack/IVT copy operations.
Those operations remain blocked on the P4 monitor/descriptor and checked
memory composition, so no source route is enabled by this part.
