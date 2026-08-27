# M0 T289 S6 P1 — Original DPMI VDD/misc composition

## Result

The x86-selected original DPMI VDD/misc branches now have formal local
evidence on both supported host architectures.  This adds no BOP ingress and
does not claim a protected DPMI guest can yet execute continuously.

- `dpmi32/i386/dpmimisc.c:DpmiDpmiInUse` and
  `DpmiDpmiNoLongerInUse` are deliberate original x86 no-ops.  The fixture
  invokes both against a stopped protected Bochs frame and proves that neither
  alters the existing register state.
- `dpmi32/vxd.c:DpmiVcdPmSvcCall32` preserves its original public Win32
  registry query.  `DX=0` returns the original VCD version `030Ah`; `DX=1`
  returns a nine-bit COM-port bitmap from
  `HKLM\\HARDWARE\\DEVICEMAP\\SERIALCOMM`.  A missing key is the original
  zero bitmap outcome.  The fixture never selects its assert/failure-only
  unimplemented VCD request branches.
- The source warning in `vxd.c` is an upstream `CHAR[16]` to `LPBYTE`
  declaration mismatch.  No source alteration is made for it.

`New-T289S6DpmiVddMiscNinja.ps1` produces a `/MT` graph with exact original
`dpmimisc.c` and `vxd.c`, the same-shaped SoftPC register facade and the
selector-blind `adapter-bochs` machine.  It rejects `src.old`, BOP ingress,
host-LDT implementations, a VDD broker and prebuilt product archives.

- `build/M0-T289/S6/vdd-misc-x64/bin/dpmi-vdd-misc-fixture.exe`: exit `0`.
- `build/M0-T289/S6/vdd-misc-x86/bin/dpmi-vdd-misc-fixture.exe`: exit `0`.

The complete current `53:00..18` disposition is recorded in the
[S6 matrix](../operations/m0-t289-s6-dpmi-family-disposition-matrix.tsv).
The matrix includes the revised S4 fixture evidence for `53:10` and `53:13`:
original owner-specific and whole-list XMEM release now both execute on x86
and x64, and retired opaque IDs take the original carry failure path.

## Boundaries

The public registry read is source-native host capability, not a VDD broker.
Acquiring/stealing ports and the remaining VCD services stay for the VDD owner
package.  `53:0E` also remains unavailable: the original
`ThreadSetDebugContext` programs monitor host threads, while a modern product
must eventually target the Bochs CPU through an explicit selector-blind
debug-register operation.  No such operation is introduced here.
