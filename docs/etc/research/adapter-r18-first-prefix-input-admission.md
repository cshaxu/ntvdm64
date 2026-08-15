# r18 First-NTIO-Prefix Input Admission

## Decision

The pinned r18 CPU5 stock-ROM/floppy profile is admitted only as the provider
of the source-identified inputs consumed by the first NTIO `init` prefix,
through its first `C4 C4 50 11` host boundary.  It is not admitted as a
general BIOS, device, DOS, DEM, BOP or COMMAND compatibility claim.

## Evidence and Decode

The input record is the atomically exported r18 snapshot in
`adapter-r18-startup-snapshot-evidence-export.md`, decoded by
`tools/probe/Read-StartupSnapshotEvidence.ps1`:

| NTIO source input | r18 value | First-prefix disposition |
| --- | --- | --- |
| IVT 10h | `c000:014a` | saved only |
| IVT 15h | `f000:f859` | saved, then guest replaces IVT 15h |
| IVT 19h | `f000:e6f2` | saved, then guest replaces IVT 19h |
| IVT 1Bh | `f000:ff53` | saved only |
| BDA `0040:0096` | `0x10` | selects NTIO's extended-keyboard branch |
| `ntvdmstate` `0040:0314` | `0xc0000000` | first FCLI/FSTI normal x86 path |

`msinit.asm:96-137` copies the four vectors before replacing 15h and 19h.
It invokes neither saved handler in this prefix.  `msinit.asm:188-200` reads
the BDA byte and branches locally.  `vint.h` defines FCLI/FSTI tests only on
the low word: `MIPS_BIT_MASK|RI_BIT_MASK` is `0x2400` and
`INTERRUPT_PENDING_BIT` is `0x0003`; all are clear in r18's low word `0x0000`.
FCLI therefore clears virtual-IF bit `0x0200` and FSTI restores it without a
real `CLI`/`STI` fallback.

## Exact Admission Boundary

The admission allows an eventual generic startup transaction to preserve the
observed `0x714` bytes, preload the verified NTIO bytes at `0070:0000`, and
execute original guest code until the first decoded `50:11` boundary.  The
adapter may transport that boundary to the host-service plane; it may not
execute an IVT target, manufacture a BDA or virtual-interrupt value, or treat
the result as a DOS boot.

## Still Rejected

- execution of the saved INT 10h/1Bh targets;
- any use of original INT 15h/19h targets after NTIO mutation;
- device/IRQ behavior, BIOS 12h, and arbitrary firmware calls;
- NTDOS initialization, filesystem and COMMAND launch;
- all DEM/BOP services except the separately admitted `50:11` transaction.

## Required Next Implementation Gate

The startup transaction must be one atomic adapter plan, not a boot shim:

```text
after original Bochs hardware reset, before its first cpu_loop
  validate profile/resource identity and full entry record
  preflight every guest-RAM read/write
  capture 0x714..0x717 before the overlapping NTIO write
  copy the verified NTIO payload to 0x700 only after preflight succeeds
  restore precisely those captured four bytes at 0x714
  apply the declared real-mode entry register record
  begin the native cpu_loop exactly once
```

The sole Bochs touchpoint may be a generic, default-off execution-plan
consumer immediately after `bx_init_hardware()` in `main.cc`; it receives a
pointer-free adapter record and must not name NTIO, DOS, BOP, DEM, IVT, BDA or
firmware fields.  Its memory and register application must be recorded as a
new intrusion before implementation.  Any failure must leave the CPU loop
unentered rather than partially starting guest code.
