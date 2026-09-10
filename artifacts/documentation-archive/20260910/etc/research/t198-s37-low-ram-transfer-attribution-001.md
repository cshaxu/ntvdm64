# T198 S37 low-RAM transfer attribution 001

## Question

After the current source-built `50:0F` drive service returns, why does the
same CPU5 x64 fixture later execute `0000:0A82 -> 0000:0A84`, where `0A84`
raises the first non-BOP `#UD` on an all-`FF` window?

## Current dynamic facts

S26/S36 establish that the current composition handles and resumes the reached
DEM service family, including `50:0F`; S36's bridge-time history records the
consecutive final positions:

```text
predecessor=0000:0A82 seq=AAD7
fault=0000:0A84 seq=AAD8
```

S30/S31 establish that physical `0x0A84` lies inside the original NTIO load
aperture `0x0700..0x8AFF`, but its runtime bytes differ from the source-loaded
image.  The state at the fault is real mode `CS=0000`, not the original NTIO
entry `0070:0000`.

## OpenNT source map

`inc/bop.h` defines `BOP_DOS=50h`; `inc/dossvc.h` defines an SVC as
`C4 C4 50 <service>`, so successful SVC resume is a guest continuation, not a
host interrupt or a device transaction.  `SVC_DEMGETDRIVES=0Fh` maps through
`dos/dem/demdisp.c` to `demGetDrives` in `demgset.c`; `msinit.asm` consumes
its `AL` result as `numio` immediately.

The same `msinit.asm` path subsequently records DTA, hard-error and COMMAND
information, then performs guest-side DOS initialization.  In particular it:

- moves DOS data to a low-memory segment selected from the BIOS communication
  data;
- changes `SS:SP` to the DOS data stack;
- writes IVT entries, including the `addr_int_ibm` entry; and
- later initializes additional DOS interrupt-vector targets.

`demSetHardErrorInfo` itself only stores host-side decoded `DS:DX` and `DS:BX`
locators.  `demGetDPBList` (`50:46`) can write a DPB list at guest `ES:BP`, but
the current evidence does not establish that it is the write to `0x0A82`, or
even that its output range overlaps that address.

## Correction of retained historical interpretation

`r20-real-ntio-next-boundary-triage.md` accurately described an earlier state
where an unhandled `50:0F` caused native exception delivery.  It is not a
current root-cause claim: the T198 composition now source-builds and resumes
`50:0F`, then reaches later source-derived services before the same low-RAM
frontier.  It remains historical evidence only.

## Classification

The exact transfer that first changed CS to zero and the writer that replaced
the `0x0A82..0A84` source bytes remain **unproven**.  Static source establishes
several legitimate guest low-memory/IVT/stack operations, but does not bind a
linked-image offset or a runtime address to this particular transition.

Consequently no Bochs device, VGA/firmware mapping, BOP provider, adapter
result, or guest-image patch is authorized.  The next admissible action is a
selector-blind, fixed-width observation that identifies the first `CS` change
in the existing execution stream (or otherwise retains enough position
history to include it), with no instruction bytes, guest reads, BOP knowledge
or CPU mutation.
