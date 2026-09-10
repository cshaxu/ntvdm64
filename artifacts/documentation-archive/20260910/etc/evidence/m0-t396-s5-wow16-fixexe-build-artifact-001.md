# M0 T396 S5 WOW16 `fixexe` build-artifact recovery

## Question

Can the selected retail `KRNL386.EXE` receive the original
`kernel31/fixexe.c` DOS-header postprocessing without mutating the selected
WOW16 media, and does that correction move the first original Win16 bootstrap
to a different owner frontier?

## Inputs

- Original owner/source: `src/mvdm-guest/wow16/kernel31/fixexe.c` and its
  `kernel31` build invocation.
- Immutable source media:
  `src/mvdm-guest/wow16/kernel31/retail3/krnl386.exe`.
- Read-only comparison: the independently maintained ntvdmx64 WOW16 patch
  narrows the same three `fixexe.c` header variables from host-width `int` to
  16-bit `short`; it is comparison evidence, not imported code.
- Formal x86 parent:
  `build/M0-T396/S3-formal/original-softpc-process.exe`; complete S2
  `WOW32.DLL`; and the existing console-owning startup observer.

## Four-rung recovery audit

| Rung | Disposition |
| --- | --- |
| Original source reuse | `fixexe.c` is the direct original owner. It writes, in order, the MZ `e_cblp` word at offset 2, `e_cp` word at offset 4, and zero `e_maxalloc` word at offset 12 after producing `KRNL386.EXE`. Its historical `int` is 16-bit in the original toolchain. |
| Smallest source-shaped shim | Admitted. `tools/build/Apply-Wow16Fixexe.mjs` preserves those three ordered 16-bit writes; `Stage-OriginalSoftpcRuntime.mjs` invokes the same transform only while creating `system32/KRNL386.EXE`. It retains source bytes unchanged and records both source and output hashes. |
| External-code intrusion | Rejected. The read-only comparator corroborates the width issue, but no ntvdmx64 patch or source is copied. |
| Newly authored behavior | Rejected. The shim has no loader policy, executable selection, BOP result, or guest behavior; it only expresses the original build-tool width contract on a modern host. |

## Procedure

1. Ran the fixed-width shim over the selected source input and verified its
   source file was not opened for write.
2. Rebuilt the disposable x86 runtime package with `WOW32.DLL`. The stage
   manifest is format 8 and identifies the kernel entry as
   `original-kernel31-fixexe-16bit`, with `derivedFrom` and output `sha256`.
3. Ran the staged parent through the console-owning observer from the short
   runtime root `O:\ntvdm64\s5fixexe`, declaring
   `system32\krnl386.exe`; this avoids the already-known 64-byte `SHELL=`
   staging-root limitation.
4. Compared the original source map and emitted CPU/BOP observations at the
   next transfer boundary.

## Observations

| Artifact | `e_cblp` | `e_cp` | `e_maxalloc` | SHA-256 |
| --- | ---: | ---: | ---: | --- |
| Immutable source `retail3/krnl386.exe` | 320 | 0 | 0 | `bced7650a5b5d7ac25955ab2a21bc53ff06de1c35b25fad0690390135b093a03` |
| Derived staged `system32/KRNL386.EXE` | 320 | 168 | 0 | `63168b7ddf6008da030a61d5e80e62de58816c5491068002ea990274e0d2ff78` |

The source and derived files are both 85,824 bytes. `168 = ceil(85824 / 512)`
and `320 = 85824 mod 512`; the source stays byte-identical.

The staged bootstrap reached the original path below. The observer timed out
after 15 seconds, with `loaded-wow32=no` and no `51h` dispatch; that is not a
WOW32 loader-success claim.

```text
MVDM-CPU-RETF source=FE2E:5A70 ... target=00A7:10F5
MVDM-BOP-DISPATCH 50:36
MVDM-DEM-ENTRY-DOS-APP ... entry=9FFF:0000 stack=9FFF:0080
MVDM-CPU-RETF source=00A7:1113 stack=9FFF:0000007C ... target=FFFF:FFFF
```

`NTDOS.MAP` places `disa20_xfer` at original offset `0794:10F5`. The trace
reaches that stub and progresses through its `mov ss,ax; mov sp,di; sti; push
ds; push si; ...; retf` sequence; the final `retf` instruction is at
`00A7:1113`. `disa20_iret` is a separate interrupt-return stub and is not
reached here. This is the HMA/A20 low-memory child-transfer path, not the
normal `$Exec:Xfer_To_User` `retf`, so the remaining failure is not an MZ
page-count layout or a missing `50:36` BOP.

## Interpretation and confidence

High confidence: S5 restores the actual original build-artifact contract and
does not alter source media. It changes the malformed zero-page MZ header to
the original 16-bit page layout, but the observed bootstrap still reaches the
same HMA/A20 low-memory transfer before any WOW32 dispatch. Therefore S5 is
closed; a separate CPU/machine compatibility-profile audit owns the remaining
stack-write/low-memory-transfer frontier. No CPU instruction, BOP, guest
source, or loader semantic change is justified by this artifact recovery.

## Follow-up

S6 audits one bounded profile: the original HMA `Disa20_Xfer` bridge, A20
wrapping, real-mode segment/stack writes, and the associated control-transfer
fetches. `disa20_iret` remains a separately excluded return path unless a
later trace reaches it. S6 must first classify the full affected mechanical
groups and prove the source/current difference before any repair is admitted.
