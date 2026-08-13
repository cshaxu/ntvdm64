# T198 S23 P5 Native BIOS 12 Continuation 001

## Question

Does exact source-built NTIO continue past its first top-level `BOP 12h`
memory-size request under the MSVC x64 `/MT` CPU5 closure, without enabling
other BIOS-memory forms?

## Inputs And Procedure

The composition admits only a copied `C4 C4 12` window to the existing
source-derived memory provider.  Its `15h/AH=88h` contract remains retained
in that provider but is explicitly declined by this composition.  The focused
adapter fixture was rebuilt with
`Invoke-T198S16BootNamespaceVdmX64Probe.ps1` into
`artifacts/build/t198-s16-bx-vdm-x64-r13`; it is PE32+ and exits zero.  The
same x64 objects were then linked with the unchanged S23 native CPU5/mantle
object closure and the exact 0x8400-byte NTIO input.

## Observations

The focused composition fixture passes both the positive `12h` result
(`AX=027Fh`, resume `RIP+3`) and the negative `15h/AH=88h` decline.  The
native execution prints:

```
t198-s23 status=4 observed-5011=1 observed-stop=1 next=50:3b
```

Status `4` is the existing generic typed controlled-stop result.  The trace
therefore includes original NTIO entry, the existing DEM `50:11` NTDOS
publication, the admitted top-level `12h` resume, and then the next copied
unhandled window `C4 C4 50 3B`.

## Interpretation And Follow-Up

`12h` is closed only as an adapter-owned machine-composition result derived
from its documented profile value; neither Bochs nor mantle identifies a BOP.
The next work is not another BIOS addition: `50:3B` is an original DEM
host-service-plane boundary and must be recovered through the global DEM
provider structure.  This observation makes no DOS-continuity or normal boot
claim.
