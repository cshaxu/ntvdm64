# M0 T280 S6 — whole DEM compile-surface evidence

## Procedure

The exact sixteen entries in original `dos/dem/sources` were each parsed using
MSVC syntax-only mode on both host architectures:

```text
VsDevCmd -arch=x64 -host_arch=x64
cl /nologo /std:c11 /MT /W4 /Zs /FI src/adapter-win32/include/nt.h ... <body>

VsDevCmd -arch=x86 -host_arch=x64
cl /nologo /std:c11 /MT /W4 /Zs /FI src/adapter-win32/include/nt.h ... <body>
```

The include list was limited to the existing adapter-win32 facade, session
headers, exact DEM/support/platform mirrors, and original SoftPC declaration
roots.  Raw logs were placed only under the disposable `build/M0-T280/s6/`
run root.

## Observed outcome

- x64: eight bodies are syntax-clean apart from warnings; eight stop in four
  shared prerequisite families.
- x86: `demmsg.c` is syntax-clean apart from warnings.  The same eight bodies
  stop in the same shared families, while seven otherwise-clean bodies stop at
  the same historical `GetDiskSpaceInformationA` declaration colliding with
  the modern SDK declaration.
- This is not a reason to edit seven original files.  It is one DEM x86
  binding-form compatibility decision, captured once in the ledger.
- `demsrch.c` confirms the prior S5 conclusion: `VDMQUERYDIRINFO` is a
  host-local directory-control record.  It must never enter the mapping
  manager.  Its later `GetVDMAddr` operations, like all reached guest numeric
  spans, require a direct synchronous `guest_memory` lease owned by
  `adapter-softpc`.
- `demdasd.c`, `demioctl.c`, and the DASD portions of `demgset.c` separate
  Win32 storage declarations from machine transfer mechanics.  Geometry and
  host storage structs are copied host-local data; only the associated guest
  transfer spans use a mapping-manager lease.

## Dual-role review

Implementation review confirms the ledger has all sixteen source selections,
does not infer runtime closure from syntax success, and assigns no BOP route.
Independent review of the source/form distinction confirms that all five
families are package-shared: DEM declaration form, x86 declaration collision,
directory/NT record ABI, DASD public storage ABI, and SoftPC guest mechanics.
The correct next S is therefore a single shared declaration/compatibility
design, not a BOP-by-BOP patch sequence.
