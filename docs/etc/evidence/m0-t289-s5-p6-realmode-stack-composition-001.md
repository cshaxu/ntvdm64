# M0 T289 S5 P6 — Original real-mode DPMI stack composition

## Question

Can the two original real-mode `stack.c` entrypoints be composed without
reviving `Sim32GetVDMPointer` as an unbounded native alias?

## Inputs

- `mvdm-host/dpmi32/stack.c`: `DpmiPushRmInt` and
  `DpmiSimulateIretCF`.
- Existing session-owned `guest_memory` lease and typed
  `adapter-bochs` copied real-mode frame operation.

## Procedure

1. Kept both original MVDM function entries, assertions, scalar operands and
   no-return-value failure direction.
2. Replaced only their raw stack/IVT pointer body with the same-shaped SoftPC
   facade. It takes fresh bounded session leases, retains the original
   little-endian BOP/FLAGS/far-frame layout, and commits CS/SS/SP/IP/FLAGS as
   one selector-blind CPU frame.
3. Ran the original-body fixture on x64 and x86:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File \
     tools/build/New-T289S5RealModeStackNinja.ps1 -Architecture x64
   ninja -C build/M0-T289/S5/realmode-stack-x64
   build/M0-T289/S5/realmode-stack-x64/bin/t289-s5-realmode-stack-fixture.exe

   powershell.exe -ExecutionPolicy Bypass -File \
     tools/build/New-T289S5RealModeStackNinja.ps1 -Architecture x86
   ninja -C build/M0-T289/S5/realmode-stack-x86
   build/M0-T289/S5/realmode-stack-x86/bin/t289-s5-realmode-stack-fixture.exe
   ```

## Observations

- Both fixtures exit zero.
- Original `DpmiPushRmInt(21h)` wrote `RmBopFe` low word, high word and saved
  FLAGS at `SS:SP-6`, decremented SP and loaded the exact IVT target.
- Original `DpmiSimulateIretCF` restored the BOP far target, restored SP and
  merged only current CF into the saved low FLAGS word.
- A source invocation with `FFFFh` did not mutate CPU or guest bytes.

## Interpretation

The real-mode stack/IVT subset is now source-derived, bounded and locally
proved on both host widths. The selected adapter body contains no guest
pointer, host pointer, new mapping manager, DPMI selector or Bochs type.

## Remaining work

`DpmiSwitchToDosxStack`/`DpmiSwitchFromDosxStack` still require the
monitor-owned guest-visible VDM-TIB/DOSX projection. Protected dispatch,
IRET and fault paths still need descriptor publication and checked protected
stack-frame composition. Neither has been enabled through a BOP route.
