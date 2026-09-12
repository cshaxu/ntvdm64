# M0 T404 S5 DEM chmod ingress correction 001

## Question

Does the repeated `50:01` marker in the positional `WRITE.EXE` startup trace
identify the WOW32 BOP boundary?

## Inputs and procedure

- Current S5 source: `src/mvdm-host/softpc.new/host/src/nt_bop.c`,
  `src/mvdm-host/dos/dem/demdisp.c`, and
  `src/mvdm-host/dos/dem/demfile.c`.
- Guest source/map: `src/mvdm-guest/dos/v86/doskrnl/dos/file.asm` and
  `src/mvdm-guest/dos/v86/doskrnl/dos/NTDOS.MAP`.
- A temporary x86 diagnostic link of the current r014 parent added a finite,
  scalar `CS:IP/DS:DX` witness to the already-decoded `50:01` ingress.  Its
  SHA-256 was `eec8766f815b596b189a27b5fcad8c6a75a6f5ae3d1ab9ce4a0e8c0b7c92a268`.
- The public positional command was run once in the existing Console observer:
  `ntvdm32.exe system32\WRITE.EXE`.  Raw output is retained at
  `O:\ntvdm64\logs\m0-t404-s5-r015-chmod-ingress-20260912-094413.*`.
  The prior product hash `27f8d7b5bb074838e9484877954a38dcbcfa0be64282a4a2d05c776867655107`
  was restored after the run.

## Observations

- `bios.c` maps BOP selectors `50` and `51` separately: `50` calls
  `MS_bop_0` (DEM), while `51` calls `MS_bop_1` (WOW).
- `demdisp.c` maps DEM service `01` to original `demChMod`.
- Every finite witness in the run reported the same caller:
  `MVDM-BOP-DISPATCH 50:01 cs=FE2E ip=6CE9 ds=00A7 dx=03D2`.
- `NTDOS.MAP` maps `$CHMOD` to `0000:6CB7`; `file.asm` shows its
  `ChModGo` branch issuing `HRDSVC SVC_DEMCHMOD`.  The observed `6CE9` lies
  in that branch after the `$CHMOD` entry.
- The run reached DOSX/DPMI registrations but did not create a WOW entry
  marker, did not load `WOW32.DLL`, and timed out after 30 seconds.

## Interpretation

The older S4 characterization of repeated `50:01` as the WOW BOP was a
selector/service confusion.  These calls are original NTDOS attribute queries
and are not evidence of a WOW32 loading loop or a `MS_bop_1` failure.
The actual frontier remains before BOP `51`: the current worker has not
reached the original KRNL386-to-WOW ingress.  Confidence is high because the
selector dispatch table, DEM service table, guest source, guest map, and live
caller location agree.

## Follow-up

Do not repair, suppress, or treat `demChMod` as the DPMI/WOW fault.  Attribute
the next investigation to the precise KRNL386/DOSX transition that must reach
BOP `51`, using only a bounded source-owned observer.
