# M0 T407 S5 descriptor-domain restoration

## Result

S5 removes the selector-cache policy and the `0040h` CPU40 bypass.  CCPU now
has separate guest-backed GDT and LDT images, while its IDTR remains bound to
the DOSX-published IDT source.  This is a table-register compatibility seam,
not a new descriptor decoder or CPU rule.

## Source-first ledger

| Rung | Evidence and disposition |
| --- | --- |
| Original source | `base/mvdm/dpmi32/i386/dpmi386.c::DpmiSetX86Descriptor` publishes 53:00 descriptor records through `NtSetLdtEntries` / `ProcessLdtInformation`. `base/mvdm/dpmi/486/dxboot.asm` establishes DOSX GDT and adjacent IDT and populates it through `NSetSegmentDscr`. Original CCPU resolves protected selectors through GDTR/LDTR; its original `c_seg.c` contains neither the local cache policy nor a `0040h` exception. |
| Smallest composition seam | Current Windows cannot install the historical process LDT or kernel VDM tables for CCPU. `dpmi32` therefore retains two bounded guest-backed images and binds them only through CCPU's original GDTR/LDTR setters. A TI=0 53:00 publication updates its DOSX GDT image and the process-LDT image, matching the original LDT publish; TI=1 updates only LDT. |
| External intrusion | None. |
| New behavior | None. The table storage/binding is the minimum unavailable-kernel carrier; it exposes no host pointer to the guest. |

## Changes and review

- `DpmiPassTableAddress` allocates adjacent but distinct GDT/LDT images; it
  preserves DOSX's source table and LDT register binding.
- `DpmiSetDescriptorEntry` follows the original LDT publication and updates
  the GDT image only when the published selector has TI clear.
- The effective-address adapter no longer chooses a segment cache by register
  order. Protected lookup reaches the CCPU descriptor-table rule.
- `c_seg.c` is restored to its original validation path: the 48-line low-CS
  observer and `0040h` fabricated descriptor are deleted.
- A first attempt which wrote a record to only the TI-selected image caused
  `MEM.EXE` to remain resident. The correction above was derived from the
  original `DpmiSetX86Descriptor` LDT publication, then rebuilt and retested.

## Verification

| Check | Result |
| --- | --- |
| `cpu40-descriptor-domain-fixture.exe` | Exit 0. It proves TI=0 selects GDT and TI=1 selects LDT at the table-binding boundary. |
| Fresh formal x86 build | `build/M0-T407/S5/r001-descriptor-domain-x86`; fixture and `original-softpc-process.exe` linked successfully. |
| Initial integration result | First build exposed the publication mistake: `MEM.EXE` did not exit within 20 seconds. That result is retained as failed evidence, not accepted. |
| Corrected integration | Rebuilt the same formal root; deployed `build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe`, both SHA-256 `40d9eb0a00415a6081378e9b69c084ed057b909ed4288e308ab9c4ab93f2167e`; `MEM.EXE` exited 0 within 20 seconds. Runtime captures remain in `O:\ntvdm64\logs\M0-T407-S5-mem-runtime*.log`. |
| Diff hygiene | `git diff --check` passed before the final review. |

## Residual receiver

S6 owns the still-unresolved platform carriers: CPU40 TSS construction,
`VDM_DPMIINFO`/protected-stack projection, FastWOW TEB/TD prefix lifecycle,
and IDT reset/republication. S5 does not claim WOW/WRITE acceptance.
